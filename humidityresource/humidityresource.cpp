#include "humidityresource.h"

HumidityResource::HumidityResource()
{
    SemIoTResource::uri = "/humidityValue";
    SemIoTResource::typeObject = "doc:HumidityValue";
    SemIoTResource::linkPredicate = "humidity";
}

//char *HumidityResource::getUri()
//{
//    return uri;
//}

int HumidityResource::getMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    generateAnswer();
}

bool HumidityResource::getMethodIsPrivate()
{
    return false;
}

CoapPDU::Code HumidityResource::getCode()
{
    return CoapPDU::COAP_CONTENT;
}

uint8_t *HumidityResource::getPayloadPointer()
{
    return (uint8_t*)answer.c_str(); // FIXME
}

int HumidityResource::getPayloadLength()
{
    return answer.length();
}

void HumidityResource::updateAnswer()
{
    generateAnswer();
}

void HumidityResource::setValue(float v)
{
    value = v;
}

void HumidityResource::generateAnswer()
{
    answer = "";
    // https://rawgit.com/bblanchon/ArduinoJson/master/scripts/buffer-size-calculator.html
    StaticJsonBuffer<JS_BUF_LEN> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root[SemIoTResource::contextPredicate] = getContextObject();
    root[SemIoTResource::typePredicate] = typeObject;
    root[valuePredicate] = value;
    root[unitCodePredicate] = unitCodeObject;
    /*
    if (WiFi.getMode()==WIFI_STA) {
        JsonObject& systemInfo = root.createNestedObject(systemInfoPredicate);
        systemInfo[SemIoTResource::typePredicate] = systemInfoTypeObject;
        systemInfo[SemIoTResource::identifierPredicate] = SemIoTResource::identifierObject;
        systemInfo[labelPredicate] = SemIoTResource::deviceLabelObject;
        JsonObject& locationInfo = systemInfo.createNestedObject(locationPredicate);
        locationInfo[SemIoTResource::typePredicate] = locationTypeObject;
        locationInfo[labelPredicate] = SemIoTResource::locationLabelObject;
    }
    */
    root.printTo(answer);
}
