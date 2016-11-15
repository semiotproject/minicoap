#ifndef SYSTEMRESOURCE_H
#define SYSTEMRESOURCE_H

#include "semiotresource.h"

class SystemResource : public SemIoTResource
{
public:
    SystemResource();
    char *getUri();
    int getMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    int putMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    CoapPDU::Code getCode();
    uint8_t* getPayloadPointer();
    int getPayloadLength();
    void addObject(String predicate, String object);
private:
    char *uri = "/";
    char *typeObject = "TemperatureDevice";

    String temperaturePredicate = "temperature";
    String temperatureObject = "/temperatureValue";

    String labelPredicate = "label";
    String locationPredicate = "location";

    String labelValuePredicate = "@value";
    String labelValueObject = "Device ";

    String answer;
    void generateAnswer();
};

#endif // SYSTEMRESOURCE_H
