#ifndef COAPRESOURCE_H
#define COAPRESOURCE_H

#include "minicoapdefinitions.h"
#include "minicoap.h"

class CoAPResource
{
public:
    CoAPResource(MiniCoAP* coapServer);
    virtual int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt) {}
    virtual int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt) {}
    virtual int postMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt) {}
    virtual int deleteMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt) {}
    coap_endpoint_path_t resourcePath; // FIXME: was static const
    bool resourceChanged = true;
    char coreAttr[10]; // TODO: define max value
    MiniCoAP *getServer();
private:
    MiniCoAP *server;
};

#endif // COAPRESOURCE_H
