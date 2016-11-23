#include "configresource.h"
#include <string.h>

ConfigResource::ConfigResource()
{
    SemIoTResource::uri = "/config";
    ap_ssid = "SMT_" + getIdentifier();
    basicAuthString = sha1(configUserName+configPassword);
    // begin();
}

void ConfigResource::setSleepy(bool sleepy)
{
    isSleepy = sleepy;
}

void ConfigResource::begin(bool sleepy)
{
    isSleepy = sleepy;
    readFromFS();
}

int ConfigResource::getMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    generateAnswer();
    currentCode = CoapPDU::COAP_CONTENT;
    return 0;
}

int ConfigResource::putMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    // FIXME: could crash if bad payload!
    // TODO: configure context, id and type
    StaticJsonBuffer<JS_BUF_LEN> jsonBuffer;
    unsigned char pld[payloadLen+1];
    memcpy((uint8_t*)pld,payloadValue,payloadLen);
    JsonObject& rootJson = jsonBuffer.parseObject((char*)pld);
    if (rootJson.success()) {
        setSTA(rootJson[wifiNameUri],rootJson[wifiPasswordUri]);
        setConfigAuth(rootJson[configUsernameUri],rootJson[configPasswordUri]);
        setDeviceLabel(rootJson[SemIoTResource::deviceLabelPredicate]);
        setLocationLabel(rootJson[SemIoTResource::locationLabelPredicate]);
        if (isSleepy) {
            sleepIntervalMsObject = rootJson[sleepIntervalMsPredicate];
            // observationServer
            if (rootJson[observationServerPredicate].is<JsonObject&>())
            {
                JsonObject& observationServerJson = rootJson[observationServerPredicate];
                observationHostObject = observationServerJson[observationHostPredicate].asString();
                observationPortObject = observationServerJson[observationPortPredicate].as<uint16_t>();
                observationUriObject = observationServerJson[observationUriPredicate].asString();
            }
        }
        generateAnswer();
        currentCode = CoapPDU::COAP_CHANGED;
        writeToFS();
        return 0;
    }
    return -1;
}

uint8_t *ConfigResource::getPayloadPointer()
{
    return (uint8_t*)answer.c_str(); // FIXME
}

int ConfigResource::getPayloadLength()
{
    return answer.length();
}

//char *ConfigResource::getUri()
//{
//    return uri;
//}

bool ConfigResource::getMethodIsPrivate()
{
    return true;
}

bool ConfigResource::putMethodIsPrivate()
{
    return true;
}

CoapPDU::Code ConfigResource::getCode()
{
    return currentCode;
}

const char *ConfigResource::STA_SSID()
{
    return sta_ssid.c_str();
}

const char *ConfigResource::STA_PSK()
{
    return sta_psk.c_str();
}

const char *ConfigResource::AP_SSID()
{
    return ap_ssid.c_str();
}

const char *ConfigResource::AP_PSK()
{
    return ap_psk.c_str();
}

bool ConfigResource::setSTA(String ssid, String pass)
{
    sta_ssid = ssid;
    sta_psk = pass;
    return true;
}

bool ConfigResource::setConfigAuth(String username, String pass)
{
    configUserName = username;
    configPassword = pass;
    basicAuthString = sha1(configUserName+configPassword);
    return true;
}

String ConfigResource::getBasicAuthString()
{
    return basicAuthString;
}

bool ConfigResource::setDeviceLabel(String newLabel)
{
    SemIoTResource::deviceLabelObject = newLabel;
    return true;
}

bool ConfigResource::setLocationLabel(String newLabel)
{
    SemIoTResource::locationLabelObject = newLabel;
    return true;
}

long ConfigResource::getSleepIntervalMs()
{
    FIXME:
    if (sleepIntervalMsObject<=10000) {
        sleepIntervalMsObject = 10000;
    }
    return sleepIntervalMsObject;
}

String ConfigResource::getObservationHost()
{
    return observationHostObject;
}

void ConfigResource::setObservationHost(String newHost)
{
    observationHostObject = newHost;
}

uint16_t ConfigResource::getObservationPort()
{
    return observationPortObject;
}

void ConfigResource::setObservationPort(uint16_t newPort)
{
    observationPortObject = newPort;
}

String ConfigResource::getObservationUri()
{
    return observationUriObject;
}

void ConfigResource::setObservationUri(String newUri)
{
    observationUriObject = newUri;
}

void ConfigResource::generateAnswer()
{
    answer = ""; // TODO: optimize if not changed?
    StaticJsonBuffer<JS_BUF_LEN> jsBuffer;
    JsonObject& rootJson = jsBuffer.createObject();
    rootJson[SemIoTResource::contextPredicate] = getContextObject();
    rootJson[SemIoTResource::idPredicate] = uri;
    rootJson[SemIoTResource::typePredicate] = typeObject;
    rootJson[wifiNameUri] = sta_ssid;
    rootJson[wifiPasswordUri] = sta_psk;
    rootJson[configUsernameUri] = configUserName;
    rootJson[configPasswordUri] = configPassword;
    rootJson[SemIoTResource::deviceLabelPredicate] = SemIoTResource::deviceLabelObject;
    rootJson[SemIoTResource::locationLabelPredicate] = SemIoTResource::locationLabelObject;
    if (isSleepy) {
        rootJson[sleepIntervalMsPredicate] = sleepIntervalMsObject;
        JsonObject& observationServer = rootJson.createNestedObject(observationServerPredicate);
        observationServer[observationHostPredicate] = observationHostObject;
        observationServer[observationPortPredicate] = observationPortObject;
        observationServer[observationUriPredicate] = observationUriObject;
    }
    rootJson.printTo(answer);
}

void ConfigResource::readFromFS()
{
    SPIFFS.begin();
    if (SPIFFS.exists("/configresource.conf")) {
        File f = SPIFFS.open("/configresource.conf", "r");
        if (f.available()) {
            sta_ssid = f.readStringUntil('\r');
            f.readStringUntil('\n');
            sta_psk = f.readStringUntil('\r');
            f.readStringUntil('\n');
            configUserName = f.readStringUntil('\r');
            f.readStringUntil('\n');
            configPassword = f.readStringUntil('\r');
            f.readStringUntil('\n');
            SemIoTResource::deviceLabelObject = f.readStringUntil('\r');
            f.readStringUntil('\n');
            SemIoTResource::locationLabelObject = f.readStringUntil('\r');
            if (isSleepy) {
                f.readStringUntil('\n');
                sleepIntervalMsObject = f.readStringUntil('\r').toInt();
                f.readStringUntil('\n');
                observationHostObject = f.readStringUntil('\r');
                f.readStringUntil('\n');
                observationPortObject = f.readStringUntil('\r').toInt();
                f.readStringUntil('\n');
                observationUriObject = f.readStringUntil('\r');
                // f.readStringUntil('\n');
            }
        }

        f.close();
    }
    SPIFFS.end();
}

void ConfigResource::writeToFS()
{
    SPIFFS.begin();
    // FIXME: magic strings
    File f = SPIFFS.open("/configresource.conf", "w");
    // TODO: type, id, etc
    f.println(sta_ssid);
    f.println(sta_psk);
    f.println(configUserName);
    f.println(configPassword);
    f.println(SemIoTResource::deviceLabelObject);
    f.println(SemIoTResource::locationLabelObject);
    if (isSleepy) {
        f.println(String(sleepIntervalMsObject,DEC));
        f.println(observationHostObject);
        f.println(String(observationPortObject,DEC));
        f.println(observationUriObject);
    }
    f.close();
    SPIFFS.end();
}
