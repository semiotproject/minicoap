#ifndef SEMIOTRESOURCE_H
#define SEMIOTRESOURCE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h> // TODO: platform-agnostic
#include "coapresource.h"


#define JS_BUF_LEN 1000 // FIXME

class SemIoTResource : public CoAPResource
{
public:
    SemIoTResource();
    static String getContextObject();
    static String getHostnameUrl();
    String getIdentifier();
    virtual char *getIdObject();
    virtual char *getTypeObject();
    // TODO: get payload from string
    virtual char *getUri();
    virtual int setUri(char *uri);
    virtual char *getLinkPredicate();
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
    static String identifierObject;
    static String identifierPredicate;

    static String deviceLabelPredicate;
    static String deviceLabelObject;

    static String locationLabelPredicate;
    static String locationLabelObject;

    static String hostNameUrl;

    static String protocolPrefix;

    static String contextPredicate;
    String contextObject = "http://external/doc#";

    static String idPredicate;
    char *idObject;

    static String typePredicate;
    char *typeObject;

    char *linkPredicate;
};

#endif // SEMIOTRESOURCE_H
