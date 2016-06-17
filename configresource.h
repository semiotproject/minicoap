#ifndef CONFIGRESOURCE_H
#define CONFIGRESOURCE_H

#include "coapresource.h"

#ifdef ARDUINO
#include <ArduinoJson.h>
#endif

// <json/json.h>


class ConfigResource : public CoAPResource
{
public:
    ConfigResource(MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
private:
    char config[MAXRESPLEN];
    StaticJsonBuffer<200> jsonBuffer;
};

#endif // CONFIGRESOURCE_H
