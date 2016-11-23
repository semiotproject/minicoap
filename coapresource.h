#ifndef COAPRESOURCE_H
#define COAPRESOURCE_H

#include <stdint.h>
#include <cantcoap.h>

class CoAPResource
{
public:
    CoAPResource();
    virtual char *getUri();
    virtual int setUri(char *uri);
    virtual CoapPDU::Code getCode();
    virtual uint8_t* getPayloadPointer();
    virtual int getPayloadLength();
    virtual void setPayload(uint8_t *value, int len);
    virtual CoapPDU::ContentFormat getContentFormat();

    // TODO: inputPayload, CoapPDU::ContentFormat, but we need Arguments too:
    virtual bool getMethodIsPrivate();
    virtual int getMethod(uint8_t *payloadValue, int payloadLen,
                          CoapPDU::ContentFormat contentFormat);

    virtual int putMethod(uint8_t *payloadValue, int payloadLen,
                          CoapPDU::ContentFormat contentFormat);
    virtual bool putMethodIsPrivate();

    virtual int postMethod(uint8_t *payloadValue, int payloadLen,
                           CoapPDU::ContentFormat contentFormat);
    virtual bool postMethodIsPrivate();

    virtual int deleteMethod(uint8_t *payloadValue, int payloadLen,
                             CoapPDU::ContentFormat contentFormat);
    virtual bool deleteMethodIsPrivate();

    virtual bool isObservable();
    virtual bool isResourceChanged();
protected:
    char *uri;
};

#endif // COAPRESOURCE_H
