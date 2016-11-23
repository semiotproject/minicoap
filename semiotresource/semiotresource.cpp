#include "semiotresource.h"

// some objects shared between resources
String SemIoTResource::identifierObject = "00000";
String SemIoTResource::identifierPredicate = "identifier";
String SemIoTResource::deviceLabelPredicate = "deviceLabel";
String SemIoTResource::deviceLabelObject = "";
String SemIoTResource::locationLabelPredicate = "locationLabel";
String SemIoTResource::locationLabelObject = "366";
String SemIoTResource::hostNameUrl = "";
String SemIoTResource::protocolPrefix = "coap://";
String SemIoTResource::contextPredicate = "@context";
String SemIoTResource::idPredicate = "@id";
String SemIoTResource::typePredicate = "@type";

SemIoTResource::SemIoTResource()
{
    idObject = getUri();
    SemIoTResource::identifierObject = String(ESP.getChipId(),DEC);
    SemIoTResource::deviceLabelObject = "SemIoT device #" + SemIoTResource::identifierObject;
}

String SemIoTResource::getContextObject()
{
    return getHostnameUrl()+"/doc#"; // FIXME: magic string
}

String SemIoTResource::getHostnameUrl()
{
    int currentMode = WiFi.getMode();
    // TODO: optimize
    SemIoTResource::hostNameUrl = "";
    SemIoTResource::hostNameUrl += protocolPrefix;
    switch (WiFi.getMode()) {
    case WIFI_STA:
        SemIoTResource::hostNameUrl+=WiFi.localIP().toString();
        break;
    case WIFI_AP:
        SemIoTResource::hostNameUrl+=WiFi.softAPIP().toString();
        break;
    default:
        SemIoTResource::hostNameUrl+="localhost";
        break;
    }
    return SemIoTResource::hostNameUrl;
}

String SemIoTResource::getIdentifier()
{
    return SemIoTResource::identifierObject;
}

char *SemIoTResource::getIdObject()
{
    return idObject;
}

char *SemIoTResource::getUri()
{
    return CoAPResource::uri;
}

char *SemIoTResource::getTypeObject()
{
    return typeObject;
}

int SemIoTResource::setUri(char *uri)
{
    // TODO:
}

char *SemIoTResource::getLinkPredicate()
{
    return linkPredicate;
}

CoapPDU::Code SemIoTResource::getCode()
{
    return CoapPDU::COAP_UNDEFINED_CODE;
}

uint8_t *SemIoTResource::getPayloadPointer()
{
    return nullptr;
}

int SemIoTResource::getPayloadLength()
{
    return 0;
}

void SemIoTResource::setPayload(uint8_t *value, int len)
{
    return;
}

CoapPDU::ContentFormat SemIoTResource::getContentFormat()
{
    return CoapPDU::COAP_CONTENT_FORMAT_APP_JSON;
}

bool SemIoTResource::getMethodIsPrivate()
{
    return false;
}

int SemIoTResource::getMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    return 0;
}

int SemIoTResource::putMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    return 0;
}

bool SemIoTResource::putMethodIsPrivate()
{
    return false;
}

int SemIoTResource::postMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    return 0;
}

bool SemIoTResource::postMethodIsPrivate()
{
    return false;
}

int SemIoTResource::deleteMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    return 0;
}

bool SemIoTResource::deleteMethodIsPrivate()
{
    return false;
}

bool SemIoTResource::isObservable()
{

}

bool SemIoTResource::isResourceChanged()
{

}
