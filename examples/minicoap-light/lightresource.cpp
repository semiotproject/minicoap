#include "lightresource.h"

LightResource::LightResource(MiniCoAP *coapServer):CoAPResource(coapServer)
{
    light = 0xFF;
#ifdef WIRINGPI
    wiringPiSetup();
#endif // WIRINGPI

#ifdef LED
#if defined(WIRINGPI)
    pinMode(pin, PWM_OUTPUT);
    pwmWrite(pin,light);
#endif // WIRINGPI

#if defined(ARDUINO)
    analogWrite(pin, light);
#endif // ARDUINO

#endif // pin

#ifdef BUTTON
#if defined(WIRINGPI) || defined(ARDUINO)
    pinMode(BUTTON, INPUT);
#endif // WIRINGPI || ARDUINO
#endif // BUTTON
}

int LightResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    return getServer()->coap_make_response(inpkt, outpkt, &light, sizeof(light), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_OCTECT_STREAM);
}

int LightResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    if (inpkt->payload.len == 1) {
        char pld = inpkt->payload.p[0];
        setLight(pld);
        return getServer()->coap_make_response(inpkt, outpkt, &light, sizeof(light), COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
}

void LightResource::setLight(unsigned char new_light_value)
{
    if (new_light_value!=light) {
        printf("light:%d\n",new_light_value/255);
#ifdef LED

#if defined(WIRINGPI)
        if (pwmSupported) {
            pwmWrite(pin, new_light_value);
        }
        else {
            digitalWrite(pin, new_light_value/255);
        }
#endif // WIRINGPI

#if defined(ARDUINO)
        if (pwmSupported) {
            analogWrite(pin, new_light_value);
        }
        else {
            digitalWrite(pin, new_light_value/255);
        }
#endif // ARDUINO

#endif // pin
        light = new_light_value;
        resourceChanged = true;
    }
}

void LightResource::setLightName(const char *name)
{
    resourcePath = {1,{name}};
}

void LightResource::enablePWM()
{
    pwmSupported = true;
}

void LightResource::setPin(unsigned int connectedPin)
{
    pin = connectedPin;
}
