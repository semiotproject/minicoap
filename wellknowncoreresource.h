#ifndef WELLKNOWNCORERESOURCE_H
#define WELLKNOWNCORERESOURCE_H

#include "coapresource.h"
#include <Arduino.h>

class WellKnownCoreResource : public CoAPResource
{
public:
    WellKnownCoreResource();
    void addResource(String resString);
    char *getUri();
    int getMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    bool getMethodIsPrivate();
    CoapPDU::Code getCode();
    uint8_t* getPayloadPointer();
    int getPayloadLength();
    CoapPDU::ContentFormat getContentFormat();
    String getSleepyAnswer();
private:
    char *wellKnownCoreUri = "/.well-known/core";
    String answer;
    String sleepyAnswer;
};

#endif // WELLKNOWNCORERESOURCE_H
