#include "systemresource.h"
#include <ESP8266WiFi.h>

SystemResource::SystemResource()
{

}

char *SystemResource::getUri()
{
    return uri;
}

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

void SystemResource::addObject(String predicate, String object)
{
    // TODO:
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
    rootJson[temperaturePredicate] = temperatureObject;
    rootJson.printTo(answer);
}
