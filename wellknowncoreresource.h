#ifndef WELLKNOWNCORERESOURCE_H
#define WELLKNOWNCORERESOURCE_H

#include "coapresource.h"

class WellKnownCoreResource : public CoAPResource
{
public:
    WellKnownCoreResource(const char *answer, MiniCoAP *coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
private:
    const coap_endpoint_path_t path_well_known_core = {2, {".well-known", "core"}};
    const char *_answer;
};

#endif // WELLKNOWNCORERESOURCE_H
