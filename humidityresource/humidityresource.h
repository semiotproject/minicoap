#ifndef HUMIDITYRESOURCE_H
#define HUMIDITYRESOURCE_H

#include "semiotresource.h"

class HumidityResource : public SemIoTResource
{
public:
    HumidityResource();
    char *getUri();
    int getMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    bool getMethodIsPrivate();
    CoapPDU::Code getCode();
    uint8_t *getPayloadPointer();
    int getPayloadLength();
    void updateAnswer();
    void setValue(float v);
private:
    float value = 0;
    const char *typeObject = "doc:HumidityValue";
    const char *systemInfoTypeObject = "doc:SEMIOTTHSDevice";
    const char *valuePredicate = "value";
    const char *unitCodePredicate = "unitCode";
    const char *unitCodeObject = "P1";
    const char *systemInfoPredicate = "systemInfo";
    const char *identifierPredicate = "identifier";
    const char *labelPredicate = "label";
    const char *locationPredicate = "location";
    const char *locationTypeObject = "Place";

    char *uri = "/humidityValue";
    String answer;
    void generateAnswer();
};

#endif // HUMIDITYRESOURCE_H
