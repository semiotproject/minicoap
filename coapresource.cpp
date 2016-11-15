#include "coapresource.h"
#include <ESP8266WiFi.h>

CoAPResource::CoAPResource()
{
}

char *CoAPResource::getUri()
{
    return 0;
}

int CoAPResource::setUri(char *uri)
{
    return -1;
}

CoapPDU::Code CoAPResource::getCode()
{
    return CoapPDU::COAP_UNDEFINED_CODE;
}

uint8_t *CoAPResource::getPayloadPointer()
{
    return nullptr;
}

int CoAPResource::getPayloadLength()
{
    return 0;
}

void CoAPResource::setPayload(uint8_t *value, int len)
{
    return;
}

CoapPDU::ContentFormat CoAPResource::getContentFormat()
{
    return CoapPDU::COAP_CONTENT_FORMAT_TEXT_PLAIN;
}

bool CoAPResource::getMethodIsPrivate()
{
    return true;
}

int CoAPResource::getMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    return -1;
}

int CoAPResource::putMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    return -1;
}

bool CoAPResource::putMethodIsPrivate()
{
    return true;
}

int CoAPResource::postMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    return -1;
}

bool CoAPResource::postMethodIsPrivate()
{
    return true;
}

int CoAPResource::deleteMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    return -1;
}

bool CoAPResource::deleteMethodIsPrivate()
{
    return true;
}

bool CoAPResource::isObservable()
{
    return false;
}

bool CoAPResource::isResourceChanged()
{
    return true;
}
