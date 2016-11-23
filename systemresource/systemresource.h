#ifndef SYSTEMRESOURCE_H
#define SYSTEMRESOURCE_H

#include "semiotresource.h"

#define MAXRESOURCESCOUNT 20

class SystemResource : public SemIoTResource
{
public:
    SystemResource();
    //char *getUri();
    int getMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    int putMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    CoapPDU::Code getCode();
    uint8_t* getPayloadPointer();
    int getPayloadLength();
    int addResource(SemIoTResource *resource);
private:
    //char *uri = "/";
    char *typeObject = "SemIoTESPDevice";

    // String temperaturePredicate = "temperature";
    // String temperatureObject = "/temperatureValue";

    String labelPredicate = "label";
    String locationPredicate = "location";

    String labelValuePredicate = "@value";
    String labelValueObject = "Device ";

    String answer;
    void generateAnswer();

    SemIoTResource *resourcesList[MAXRESOURCESCOUNT];
    unsigned int resourcesCount = 0;
};

#endif // SYSTEMRESOURCE_H
