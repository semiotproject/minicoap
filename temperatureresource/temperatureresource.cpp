#include "temperatureresource.h"

TemperatureResource::TemperatureResource()
{
    SemIoTResource::uri = "/temperatureValue";
    SemIoTResource::typeObject = "doc:TemperatureValue";
    SemIoTResource::linkPredicate = "temperature";
    SemIoTResource::deviceLabelObject = "SEMIOT TH-S #"+SemIoTResource::getIdentifier();
}

//char *TemperatureResource::getUri()
//{
//    return SemIoTResource::uri;
//}

int TemperatureResource::getMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    generateAnswer();
}

bool TemperatureResource::getMethodIsPrivate()
{
    return false;
}

CoapPDU::Code TemperatureResource::getCode()
{
    return CoapPDU::COAP_CONTENT;
}

uint8_t *TemperatureResource::getPayloadPointer()
{
    return (uint8_t*)answer.c_str(); // FIXME
}

int TemperatureResource::getPayloadLength()
{
    return answer.length();
}

void TemperatureResource::updateAnswer()
{
    generateAnswer();
}

void TemperatureResource::setValue(float v)
{
    value = v;
}

void TemperatureResource::generateAnswer()
{
    answer = "";
    // https://rawgit.com/bblanchon/ArduinoJson/master/scripts/buffer-size-calculator.html
    StaticJsonBuffer<JS_BUF_LEN> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root[SemIoTResource::contextPredicate] = getContextObject();
    root[SemIoTResource::typePredicate] = SemIoTResource::typeObject;
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
