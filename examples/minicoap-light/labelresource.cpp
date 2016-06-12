#include "labelresource.h"

LabelResource::LabelResource(const char *label, MiniCoAP *coapServer):CoAPResource(coapServer)
{
    memset(&_label,0,sizeof(_label));
}

int LabelResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t *)&_label, sizeof(_label), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

int LabelResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    if (inpkt->payload.len>sizeof(_label)) {
        return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN); // FIXME
    }
    else {
        memset(&_label,0,sizeof(_label));
        strncpy(_label,(char *)inpkt->payload.p,inpkt->payload.len);
        return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
}
