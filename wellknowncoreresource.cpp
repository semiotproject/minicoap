#include "wellknowncoreresource.h"

WellKnownCoreResource::WellKnownCoreResource(const char *answer, MiniCoAP *coapServer):CoAPResource(coapServer)
{
    resourcePath = path_well_known_core;
    _answer = answer;
}

int WellKnownCoreResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t *)_answer, 1, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}
