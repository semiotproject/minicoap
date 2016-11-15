#include "wellknowncoreresource.h"

WellKnownCoreResource::WellKnownCoreResource():CoAPResource()
{
    answer = "</>;";
    answer += "rt=\"http://schema.org/EntryPoint\";";
    answer += "ct=50";

    sleepyAnswer = answer;

    answer += ",</config>;";
    answer += "rt=\"http://";
    answer += "external";
    answer += "/doc#ConfigResource\";";
    answer += "ct=50";
}

void WellKnownCoreResource::addResource(String resString)
{
    answer += resString;
    sleepyAnswer = answer;
}

char *WellKnownCoreResource::getUri()
{
    return wellKnownCoreUri;
}

int WellKnownCoreResource::getMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    return 0;
}

bool WellKnownCoreResource::getMethodIsPrivate()
{
    return false;
}

CoapPDU::Code WellKnownCoreResource::getCode()
{
    return CoapPDU::COAP_CONTENT;
}

uint8_t *WellKnownCoreResource::getPayloadPointer()
{
    return (uint8_t*)answer.c_str(); // FIXME
}

int WellKnownCoreResource::getPayloadLength()
{
    return answer.length();
}

CoapPDU::ContentFormat WellKnownCoreResource::getContentFormat()
{
    return CoapPDU::COAP_CONTENT_FORMAT_APP_LINK;
}

String WellKnownCoreResource::getSleepyAnswer()
{
    return sleepyAnswer;
}
