#include "coapresource.h"

CoAPResource::CoAPResource(MiniCoAP *coapServer)
{
    server = coapServer;
    coap_endpoint_func getMP = std::function<int(const coap_packet_t *inpkt, coap_packet_t *outpkt)>(std::bind(&CoAPResource::getMethod, this, std::placeholders::_1, std::placeholders::_2));
    coap_endpoint_func putMP = std::function<int(const coap_packet_t *inpkt, coap_packet_t *outpkt)>(std::bind(&CoAPResource::putMethod, this, std::placeholders::_1, std::placeholders::_2));
    server->addEndpoint(COAP_METHOD_GET,getMP,&resourcePath,coreAttr,&resourceChanged);
    server->addEndpoint(COAP_METHOD_PUT,putMP,&resourcePath);
}

MiniCoAP *CoAPResource::getServer()
{
    return server;
}
