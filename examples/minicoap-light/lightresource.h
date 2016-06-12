#ifndef LIGHTRESOURCE_H
#define LIGHTRESOURCE_H

#include "coapresource.h"
#ifdef WIRINGPI
// #include <wiringPi.h>
#endif // WIRINGPI


// observable by default
class LightResource: public CoAPResource
{
public:
    LightResource(const char* lightName, unsigned int connectedPin, bool isPwmSupported, MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
private:
    unsigned char light;
    void setLight(unsigned char new_light_value);
    bool pwmSupported = false;
    unsigned int pin;
};

#endif // LIGHTRESOURCE_H
