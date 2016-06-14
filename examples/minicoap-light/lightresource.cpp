#include "lightresource.h"

LightResource::LightResource(const char *lightName, unsigned int connectedPin, bool isPwmSupported, MiniCoAP* coapServer):CoAPResource(coapServer)
{
    light = 0xFF;
    resourcePath = {1,{lightName}};
    pin = connectedPin;
    pwmSupported = isPwmSupported;
#ifdef WIRINGPI
    wiringPiSetup();
#endif // WIRINGPI

#ifdef LED
#if defined(WIRINGPI)
    if (pwmSupported) {
        pinMode(pin, PWM_OUTPUT);
        pwmWrite(pin,light);
    }
    else {
        pinMode(pin,OUTPUT);
        if (light>126) {
            digitalWrite(pin,HIGH);
        }
        else {
            digitalWrite(pin,LOW);
        }

    }

#endif // WIRINGPI

#if defined(ARDUINO)
    pinMode(pin, OUTPUT);
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
    return getServer()->coap_make_response(inpkt, outpkt, &light, 1, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_OCTECT_STREAM);
}

int LightResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    if (inpkt->payload.len == 1) {
        char pld = inpkt->payload.p[0];
        setLight(pld);
        return getServer()->coap_make_response(inpkt, outpkt, &light, 1, COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
}

void LightResource::setLight(unsigned char new_light_value)
{
    printf("from payload:%d\n",new_light_value);
    if (new_light_value!=light) {
#ifdef LED

#if defined(WIRINGPI)
        if (pwmSupported) {
            pwmWrite(pin, new_light_value);
            printf("pwm set:%d\n",new_light_value);
        }
        else {
            if (light>126) {
                digitalWrite(pin,HIGH);
                printf("digital set:%d\n",HIGH);
            }
            else {
                digitalWrite(pin,LOW);
                printf("digital set:%d\n",LOW);
            }
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
