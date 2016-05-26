#ifndef LIGHTRESOURCE_H
#define LIGHTRESOURCE_H

#include "coapresource.h"
#ifdef WIRINGPI
#include <wiringPi.h>
#endif // WIRINGPI


// observable by default
class LightResource: public CoAPResource
{
public:
    LightResource(MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    void setLightName(const char* name);
    void enablePWM();
    void setPin(unsigned int connectedPin);
private:
    unsigned char light; // fixme: was static
    void setLight(unsigned char new_light_value);
    bool pwmSupported = false;
    unsigned int pin;
};

#endif // LIGHTRESOURCE_H
