#include "systemresource.h"
#include <ESP8266WiFi.h>

SystemResource::SystemResource()
{
    SemIoTResource::uri = "/";
}

//char *SystemResource::getUri()
//{
//    return uri;
//}

int SystemResource::getMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    generateAnswer();
    return 0;
}

int SystemResource::putMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    // TODO
    return -1;
}

CoapPDU::Code SystemResource::getCode()
{
    return CoapPDU::COAP_CONTENT;
}

uint8_t *SystemResource::getPayloadPointer()
{
    return (uint8_t*)answer.c_str(); // FIXME
}

int SystemResource::getPayloadLength()
{
    return answer.length();
}

int SystemResource::addResource(SemIoTResource *resource)
{
    if (resourcesCount<MAXRESOURCESCOUNT) {
        resourcesList[resourcesCount]=resource;
        return ++resourcesCount;
    }
    return -1;
}

void SystemResource::generateAnswer()
{
    answer = ""; // TODO: optimize if not changed?
    StaticJsonBuffer<JS_BUF_LEN> jsBuffer;
    JsonObject& rootJson = jsBuffer.createObject();
    rootJson[SemIoTResource::contextPredicate] = getContextObject();
    String id = SemIoTResource::getHostnameUrl()+uri;
    rootJson[SemIoTResource::idPredicate] = id;
    rootJson[SemIoTResource::typePredicate] = typeObject;
    rootJson[SemIoTResource::identifierPredicate] = SemIoTResource::identifierObject;
    JsonObject& labelJson = rootJson.createNestedObject(labelPredicate);
    labelJson[labelValuePredicate] = SemIoTResource::deviceLabelObject;
    labelJson["@language"] = "en"; // FIXME
    JsonObject& locationJson = rootJson.createNestedObject(locationPredicate);
    locationJson[locationPredicate] = SemIoTResource::locationLabelObject;
    locationJson[SemIoTResource::typePredicate] = "Place"; // FIXME
    // separated resources:
    for (int i=0; i<resourcesCount; i++) {
        rootJson[resourcesList[i]->getLinkPredicate()] = resourcesList[i]->getUri();
    }
    rootJson.printTo(answer);
}
