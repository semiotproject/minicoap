#ifndef TEMPERATURERESOURCE_H
#define TEMPERATURERESOURCE_H

#include "semiotresource.h"

class TemperatureResource : public SemIoTResource
{
public:
    TemperatureResource();
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
    float value;
    const char *typeObject = "doc:TemperatureValue";
    const char *systemInfoTypeObject = "doc:SEMIOTTHSDevice";
    const char *valuePredicate = "value";
    const char *unitCodePredicate = "unitCode";
    const char *unitCodeObject = "CEL";
    const char *systemInfoPredicate = "systemInfo";
    const char *identifierPredicate = "identifier";
    const char *labelPredicate = "label";
    const char *locationPredicate = "location";
    const char *locationTypeObject = "Place";

    char *uri = "/temperatureValue";
    String answer;
    void generateAnswer();
};

#endif // TEMPERATURERESOURCE_H
