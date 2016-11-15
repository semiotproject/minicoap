#ifndef RELAYRESOURCE_H
#define RELAYRESOURCE_H

#include "semiotresource.h"

class RelayResource : public SemIoTResource
{
public:
    RelayResource();
    int getMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    int putMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    CoapPDU::Code getCode();
    uint8_t *getPayloadPointer();
    int getPayloadLength();
    void updateAnswer();
    void setPin(uint8_t pin);
    void turnOn();
    void turnOff();
    char *getUri();
private:
    int _pin;
    bool isOn = true;
    CoapPDU::Code _code;
    char *uri = "/relay";
    char *actionStatusPredicate = "actionStatus";
    char *actionStatusObject = "CompletedActionStatus";
    char *turnOnObject = "TurnOn";
    char *turnOffObject = "TurnOff";
    String turnOnActionObject = "TurnOnAction";
    String turnOffActionObject = "TurnOffAction";
    String answer;
    void generateAnswer();
};

#endif // RELAYRESOURCE_H
