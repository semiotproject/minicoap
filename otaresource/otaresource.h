#ifndef OTARESOURCE_H
#define OTARESOURCE_H

#include "semiotresource.h"

class OTAResource : public SemIoTResource
{
public:
    OTAResource();
    CoAPResource();
    CoapPDU::Code getCode();
    CoapPDU::ContentFormat getContentFormat();

    // TODO: inputPayload, CoapPDU::ContentFormat, but we need Arguments too:
    int getMethod(uint8_t *payloadValue, int payloadLen,
                           CoapPDU::ContentFormat contentFormat);
    bool getMethodIsPrivate();
    int postMethod(uint8_t *payloadValue, int payloadLen,
                           CoapPDU::ContentFormat contentFormat);
    bool postMethodIsPrivate();
};

#endif // OTARESOURCE_H
