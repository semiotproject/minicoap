#include "relayresource.h"

RelayResource::RelayResource()
{
    SemIoTResource::deviceLabelObject = "SEMIOT R-A #"+SemIoTResource::getIdentifier();
}

int RelayResource::getMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    _code = CoapPDU::COAP_CONTENT;
    generateAnswer();
    return 0;
}

int RelayResource::putMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    // FIXME: could crash if bad payload!
    // TODO: configure context, id and type
    StaticJsonBuffer<JS_BUF_LEN> jsonBuffer;
    unsigned char pld[payloadLen+1];
    memcpy((uint8_t*)pld,payloadValue,payloadLen);
    JsonObject& rootJson = jsonBuffer.parseObject((char*)pld);
    if (rootJson.success()) {
        String actionType = rootJson[SemIoTResource::typePredicate];
        if (actionType==turnOnActionObject) {
            if (!isOn) {
                turnOn();
                generateAnswer();
            }
        }
        else if (actionType==turnOffActionObject) {
            if (isOn) {
                turnOff();
                generateAnswer();
            }
        }
        _code = CoapPDU::COAP_CHANGED;

    }
    return -1;
}

CoapPDU::Code RelayResource::getCode()
{
    return _code;
}

uint8_t *RelayResource::getPayloadPointer()
{
    return (uint8_t*)answer.c_str(); // FIXME
}

int RelayResource::getPayloadLength()
{
    return answer.length();
}

void RelayResource::setPin(uint8_t pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin,!isOn); // inverted
}

void RelayResource::turnOn()
{
    isOn = true;
    digitalWrite(_pin,!isOn); // inverted
}

void RelayResource::turnOff()
{
    isOn = false;
    digitalWrite(_pin,!isOn); // inverted
}

char *RelayResource::getUri()
{
    return uri;
}

void RelayResource::generateAnswer()
{
    answer = "";
    // https://rawgit.com/bblanchon/ArduinoJson/master/scripts/buffer-size-calculator.html
    StaticJsonBuffer<JS_BUF_LEN> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root[SemIoTResource::contextPredicate] = getContextObject();
    if (isOn) {
        root[SemIoTResource::typePredicate] = turnOnObject;
    }
    else {
        root[SemIoTResource::typePredicate] = turnOffObject;
    }
    root[actionStatusPredicate] = actionStatusPredicate;
    root.printTo(answer);
}
