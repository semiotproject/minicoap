#include "minicoap.h"
#include <Hash.h>

MiniCoAP::MiniCoAP()
{
    wnkRes = new WellKnownCoreResource();
    rootRes = new SystemResource();
    confRes = new ConfigResource();
    currentPacket = new CoapPDU((uint8_t *)udpRxBuf,COAPBUFLEN,COAPBUFLEN);
    resourcesList[0] = wnkRes;
    resourcesList[1] = confRes;
    // rootRes is separated with empty uri

}

int MiniCoAP::addResource(CoAPResource *resource)
{
    if (resourcesCount<resourcesListSize) {
        resourcesList[resourcesCount] = resource;
        addResToWnk(resource);
        // TODO: add resource to root
        resourcesCount++;
        return 0;
    }
    return -1;
}

int MiniCoAP::handleClient()
{
//    if (!useButton) {
//        toAP(); // FIXME: move to public
//        // sleepy only in sta
//        if (receivePacket() == 0) {
//            return handleAwake();
//        }
//    }
    if (useButton) {
        int currentState = digitalRead(buttonPin);
        if (buttonState!=currentState) {
            buttonState=currentState;
            if (buttonState == HIGH) {
                toAP();
                sending = 1;
            }
            else {
                toSTA();
                sending = 2;
            }
        }
        if (isSleepy==false)  {
            if (receivePacket() == 0) {
                return handleAwake();
            }
        }
        else {
            if (sending==1) {
                if (receivePacket() == 0) {
                    return handleAwake();
                }
            }
            if (sending==2) {
                return handleSleepy();
            }
        }

    }
    return 0;
}

bool MiniCoAP::toAP()
{
    if (WiFi.getMode()!=WIFI_AP) {
        Serial.println("to ap");
        WiFi.mode(WIFI_AP);
        delay(10);
        WiFi.softAP(confRes->AP_SSID(), confRes->AP_PSK());
        return true;
    }
    return true;
}

bool MiniCoAP::toSTA()
{
    if ((WiFi.getMode()==WIFI_STA) &&
    (WiFi.status() == WL_CONNECTED)) {
        return true;
    }
    else {
        WiFi.mode(WIFI_STA);
        delay(10);
        WiFi.begin(confRes->STA_SSID(),confRes->STA_PSK());
        char a = 0;
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            a++;
            if (a>numberOfSTAAttempts) {
                return false;
            }
        }
        Serial.println("to sta connected");
        Serial.println(WiFi.localIP());
        return true;
    }
}

int MiniCoAP::receivePacket()
{
    udpRxBufLen = 0;
    udpRxBufLen = udp.parsePacket();
    if ((udpRxBufLen>0) && (udpRxBufLen<COAPBUFLEN)) {
        Serial.println("reveived");
        udpRxBufLen = udp.read((unsigned char *)udpRxBuf,COAPBUFLEN);
        Serial.println("ok");
        currentPacket->setPDULength(udpRxBufLen);
        Serial.println("read");
        if(currentPacket->validate()==1) {
            Serial.println("validated");
            return 0;
        }
    }
    udpRxBufLen = 0;
    return -1;
}

void MiniCoAP::sendPacket(bool broadcast)
{
    Serial.println("sending packet");
    if (broadcast) {
        IPAddress brIP = ~WiFi.subnetMask() | WiFi.gatewayIP();
        udp.beginPacket(brIP, COAP_PORT);
    }
    else {
        udp.beginPacket(udp.remoteIP(), udp.remotePort());
    }
    udp.write(responsePacket->getPDUPointer(),responsePacket->getPDULength());
    udp.endPacket();
    Serial.println("sending packet is ok");
    delete responsePacket;
}

void MiniCoAP::addResToWnk(CoAPResource *resource)
{
    // FIXME: so hardcode
    // FIXME: external uri
    String answer = "";
    String resUri(resource->getUri());
    if (resUri=="/relay") {
        answer += ",</relay>;";
        answer += "rt=\"http://external/doc#Relay\";";
        answer += "ct=50;obs";
    }
    else if (resUri=="/temperatureValue") {
        answer += ",</temperatureValue>;";
        answer += "rt=\"http://external/doc#TemperatureValue\";";
        answer += "ct=50;obs,";
    }
    else if (resUri=="/humidityValue") {
        answer += ",</humidityValue>;";
        answer += "rt=\"http://external/doc#HumidityValue\";";
        answer += "ct=50;obs";
    }
    wnkRes->addResource(answer);
}

// https://gist.github.com/a-andreyev/9cba35e27d432cc5c9b4dbf26954e957
int MiniCoAP::handleAwake()
{
    Serial.println("handling awake");
    // get uri and compare with every res uri
    CoAPResource *resource;
    bool found = false;
    bool authIsOk = false;
    currentPacket->getURI(uriBuf,URIBUFLEN,&uriBufLen);
    Serial.println("handling awake 2");
    if (uriBufLen == 0) {
        resource = rootRes;
        found = true;
    }
    else {
        for (int r=0;r<resourcesCount;r++) {
            resource = resourcesList[r];
            Serial.println("handling awake ...");
            char *resUri = resource->getUri();
            if (strcmp(resUri, uriBuf)==0) {
                found = true;
                break;
            }
        }
    }
    if (found) {
        Serial.println("found");
        // TODO: payload
        uint8_t *payloadP;
        int payloadL = 0;
        payloadP = currentPacket->getPayloadPointer();
        payloadL = currentPacket->getPayloadLength();
    //            std::vector<uint8_t> payload;
    //            payload.assign(currentPacket->getPayloadPointer(),
    //                           currentPacket->getPayloadPointer()+
    //                           currentPacket->getPayloadLength());
        // searching for contentType
        // TODO: search for option method
        CoapPDU::ContentFormat contentType = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;
        for (int i=0;i<currentPacket->getNumOptions();i++) {
            if (currentPacket->getOptions()[i].optionNumber==CoapPDU::COAP_OPTION_CONTENT_FORMAT) {
                // TODO:
                // contentType = currentPacket->getOptions()[i].optionValuePointer;
                contentType = CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;
            }
        }
        // FIXME: send code to resource
        switch (currentPacket->getCode()) {
        case (CoapPDU::COAP_GET): {
            if (resource->getMethodIsPrivate()) {
                if (!basicAuthIsOk()) {
                    break;
                }
            }
            resource->getMethod(payloadP, payloadL, contentType);
            authIsOk = true;
            break;
        }
        case (CoapPDU::COAP_PUT): {
            if (resource->putMethodIsPrivate()) {
                if (!basicAuthIsOk()) {
                    break;
                }
            }
            resource->putMethod(payloadP, payloadL, contentType);
            authIsOk = true;
            break;
        }
        case (CoapPDU::COAP_POST): {
            if (resource->postMethodIsPrivate()) {
                if (!basicAuthIsOk()) {
                    break;
                }
            }
            resource->postMethod(payloadP, payloadL, contentType);
            authIsOk = true;
            break;
        }
        case (CoapPDU::COAP_DELETE): {
            if (resource->deleteMethodIsPrivate()) {
                if (!basicAuthIsOk()) {
                    break;
                }
            }
            resource->deleteMethod(payloadP, payloadL, contentType);
            authIsOk = true;
            break;
        }
        default:
            break;
        }
    }

    if (currentPacket->getType()==CoapPDU::COAP_CONFIRMABLE){
        Serial.println("COAP_CONFIRMABLE");
        responsePacket = new CoapPDU();
        responsePacket->setVersion(1);
        responsePacket->setMessageID(currentPacket->getMessageID());
        responsePacket->setToken(currentPacket->getTokenPointer(),currentPacket->getTokenLength());
        responsePacket->setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
        if (found && authIsOk) {
            responsePacket->setCode(resource->getCode());
            responsePacket->setContentFormat(resource->getContentFormat());
            responsePacket->setPayload(resource->getPayloadPointer(),
                                resource->getPayloadLength());
        }
        else {
            responsePacket->setCode(CoapPDU::COAP_BAD_REQUEST); // TODO: other methods errors handle
        }
        sendPacket();
    }
    return 0;
}

int MiniCoAP::handleSleepy()
{

    if (sendSleepy()!=0) {
        registerSleepy();
    }
    // !!
    delay(confRes->getSleepIntervalMs()*1000);
    // ESP.deepSleep(confRes->getSleepIntervalMs()*1000); // deepSleep arg in mcs
    return 0;
}

int MiniCoAP::registerSleepy()
{
    // broadcast to: /.well-known/core?rt=core.rd-cache

    sleepyPacket = new CoapPDU();
    sleepyPacket->setVersion(1);
    uint8_t rnd1 = random(255);
    uint16_t rnd2 = random(1024);
    sleepyPacket->setMessageID(rnd2);
    sleepyPacket->setToken(&rnd1,1);
    sleepyPacket->setType(CoapPDU::COAP_CONFIRMABLE);
    sleepyPacket->setCode(CoapPDU::COAP_GET);
    sleepyPacket->setURI(rdCacheDiscoverUri);

    sendPacket(true);

    // parse core link: </rd-cache>;rt="core.rd-cache"
    // FIXME: receiving not only the first answer:

    // TODO:
    // while not parsed correctly wait for new coap answer

    if (waitForCoapAnswer()!=0) {
        return -1;
    }
    // TODO: check for uri with the rt
    // ;rt="core.rd-cache
    // and parse it to rdCacheRegUri

    // FIXME: just reset sleepyPacket
    delete sleepyPacket;
    sleepyPacket = new CoapPDU();

    uint8_t *payloadP;
    int payloadL = 0;
    payloadP = currentPacket->getPayloadPointer();
    payloadL = currentPacket->getPayloadLength();
    unsigned char pld[payloadL+1];
    memcpy((uint8_t*)pld,payloadP,payloadL);
    char *rdCacheRegUri = (char *)pld;

    // post core link: post: coap://<remote_socket>/rd?ep=identifier
    // Content-Format: 40
    // Payload: </>;ct=50, </temp>;rt="temperature";ct=50

    sleepyPacket->setVersion(1);
    // rnd1 = random(255);
    // rnd2 = random(1024);
    sleepyPacket->setMessageID(rnd2);
    sleepyPacket->setToken(&rnd1,1);
    sleepyPacket->setType(CoapPDU::COAP_CONFIRMABLE);
    sleepyPacket->setCode(CoapPDU::COAP_POST);
    sleepyPacket->setURI(rdCacheRegUri);
    sleepyPacket->setContentFormat(CoapPDU::COAP_CONTENT_FORMAT_APP_LINK);
    String rdCacheRegPayload = wnkRes->getSleepyAnswer();
    sleepyPacket->setPayload((uint8_t*)rdCacheRegPayload.c_str(),
                             rdCacheRegPayload.length());

    // TODO: send packet to

    // parse link 41 option:
    // 2.01 Created
    // Link: </rd-cache/4521>; rel="http://w3id.org/semiot/coap/rd-cache";
    if (waitForCoapAnswer()!=0) {
        return -1;
    }

    // TODO: separated search for option method

    String rdCacheUri;
    for (int i=0;i<currentPacket->getNumOptions();i++) {
        // Link custom option #41
        if (currentPacket->getOptions()[i].optionNumber==CoapPDU::Option(41)) {
            uint8_t *optP = currentPacket->getOptions()[i].optionValuePointer;
            uint16_t optL = currentPacket->getOptions()[i].optionValueLength;
            if (optL>0) {
                uint8_t linkOpt[optL+1];
                memcpy((uint8_t*)linkOpt,optP,optL);
                char rdCacheUriBuf[URIBUFLEN];
                // https://github.com/esp8266/Arduino/pull/2609
                sscanf((char*)linkOpt,"Link:%*s<%s>%*srel=\"http://w3id.org/semiot/coap/rd-cache\"",rdCacheUriBuf);
                rdCacheUri = String(rdCacheUriBuf);
                if (rdCacheRegUri!="") {
                    confRes->setObservationUri(rdCacheUri);
                    confRes->setObservationHost(udp.remoteIP().toString());
                    confRes->setObservationPort(udp.remotePort());
                    confRes->writeToFS();
                    return 0;
                }
            }
        }
    }
    return -1;
}

int MiniCoAP::sendSleepy()
{
    confRes->readFromFS();
    bool regIsOk=true;
    CoAPResource *resource;
    for (int r=0;r<resourcesCount;r++) {
        if (!regIsOk) {
            break;
        }
        resource = resourcesList[r];
        if (r==0) {
            // skipping wnk and adding root res
            resource = rootRes;
        }
        if (resource->getMethodIsPrivate()) {
            continue;
        }
        sleepyPacket = new CoapPDU();
        sleepyPacket->setVersion(1); // FIXME:
        uint8_t rnd1 = random(255);
        uint16_t rnd2 = random(1024);
        sleepyPacket->setMessageID(rnd2);
        sleepyPacket->setToken(&rnd1,1);
        sleepyPacket->setType(CoapPDU::COAP_NON_CONFIRMABLE);
        sleepyPacket->setCode(CoapPDU::COAP_POST);
        // TODO concat obs uri and resource uri
        sleepyPacket->setURI((char*)confRes->getObservationUri().c_str()); // FIXME c_str()
        resource->getMethod(nullptr,0,CoapPDU::ContentFormat(0));
        sleepyPacket->setPayload(resource->getPayloadPointer(),
                            resource->getPayloadLength());
        // FIXME c_str()
        IPAddress serverIP;
        WiFi.hostByName(confRes->getObservationHost().c_str(), serverIP);
        udp.beginPacket(serverIP, confRes->getObservationPort());
        udp.write(sleepyPacket->getPDUPointer(),sleepyPacket->getPDULength());
        udp.endPacket();
        delete sleepyPacket;

        // wait for answer
        if (waitForCoapAnswer()!=0) {
            // TODO: check for answer responce code
            regIsOk = false;
        }
    }
    if (regIsOk) {
        return 0;
    }
    return -1;
}

// TODO: return coap answer code

int MiniCoAP::waitForCoapAnswer()
{
    int r = 0;
    while (receivePacket() != 0) {
        r++;
        if (r>numberOfRdAttempts) {
            return -1;
        }
        delay(10); // FIXME
    }
    return 0;
}

bool MiniCoAP::basicAuthIsOk()
{
    if (WiFi.getMode()==WIFI_AP) {
        return true;
    }
    else {
        // TODO: search for option method

        for (int i=0;i<currentPacket->getNumOptions();i++) {
            // Basic Auth custom option #40
            if (currentPacket->getOptions()[i].optionNumber==CoapPDU::Option(40)) {
                uint8_t *optP = currentPacket->getOptions()[i].optionValuePointer;
                uint16_t optL = currentPacket->getOptions()[i].optionValueLength;
                if (optL>0) {
                    uint8_t basicAuthOpt[optL];
                    memcpy((uint8_t*)basicAuthOpt,optP,optL);
                    if (strncmp((const char*)basicAuthOpt, confRes->getBasicAuthString().c_str(),optL)==0) {
                        return true;
                    }
                }
            }
        }

    }
    return false;
}

int MiniCoAP::begin(bool sleepy)
{
    Serial.begin(9600);
    isSleepy = sleepy;
    // https://github.com/esp8266/Arduino/issues/2189
    confRes->begin(isSleepy);
    int u = udp.begin(COAP_PORT);
}

int MiniCoAP::setButton(int pin)
{
    buttonPin = pin;
    useButton = true;
    pinMode(buttonPin,INPUT);
}
