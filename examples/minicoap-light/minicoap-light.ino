#include "minicoap.h"
#ifdef WIRINGPI
#include <wiringPi.h>
#endif // WIRINGPI
#ifdef ARDUINO
#include "WiFiex.h"
#endif // ARDUINO

MiniCoAP coap;

// TODO: get rid of static content argument?
static unsigned char light = 100;
bool lightChanged = true;

void setLight(unsigned char new_light_value) {
    if (new_light_value!=light) {
#ifdef LED

#if defined(WIRINGPI)
        pwmWrite(LED, new_light_value);
#endif // WIRINGPI || ARDUINO

#if defined(ARDUINO)
        analogWrite(LED, new_light_value);
#endif // ARDUINO

#endif // LED
        light = new_light_value;
        lightChanged = true;
    }
}

void turnOffLight() {
    setLight(0);
}

void turnOnLight() {
    setLight(255);
}

static const coap_endpoint_path_t pathLight = {1, {"light"}};
int getLight(const coap_packet_t *inpkt, coap_packet_t *outpkt) {
    return coap.coap_make_response(inpkt, outpkt, &light, sizeof(light), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

int putLight(const coap_packet_t *inpkt, coap_packet_t *outpkt) {
    if (inpkt->payload.len == 1) {
        char pld = inpkt->payload.p[0];
        setLight(pld);
        return coap.coap_make_response(inpkt, outpkt, &light, sizeof(light), COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
    return coap.coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
}

char wellknowncore[MAXRESPLEN];

// TODO: get rid of this architecture:
int getWellKnownCore(const coap_packet_t *inpkt, coap_packet_t *outpkt) {
    coap.buildWellKnownCoreString(wellknowncore,sizeof(wellknowncore));
    printf("%d\n",strlen(wellknowncore));
    return coap.coap_make_response(inpkt, outpkt, (uint8_t*)wellknowncore, strlen(wellknowncore), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

void updateResources() {
#if defined(WIRINGPI) || defined(ARDUINO)
#ifdef BUTTON
    int buttonValue = digitalRead(BUTTON);
    if (buttonValue==HIGH) {
        turnOnLight();
    }
    else if (buttonValue==LOW) {
        turnOffLight();
    }
#endif // BUTTON
#endif
}

void setup() {
#ifdef WIRINGPI
    wiringPiSetup();
#endif // WIRINGPI

#ifdef LED
#if defined(WIRINGPI)
    pinMode(LED, PWM_OUTPUT);
    pwmWrite(LED,light);
#endif // WIRINGPI
#if defined(ARDUINO)
    analogWrite(LED, light);
#endif // ARDUINO
#endif // LED
#ifdef BUTTON // BUTTON
#if defined(WIRINGPI) || defined(ARDUINO)
    pinMode(BUTTON, INPUT);
#endif // WIRINGPI || ARDUINO
#endif

    coap.addEndpoint(COAP_METHOD_GET,getWellKnownCore,&path_well_known_core,"ct=40");
    coap.addEndpoint(COAP_METHOD_GET,getLight,&pathLight,"ct=0;obs",&lightChanged);
    coap.addEndpoint(COAP_METHOD_PUT,putLight,&pathLight); // TODO
#ifdef ARDUINO
    connectToWiFi();
#endif // ARDUINO
    coap.begin(); // TODO: check if success
}

// TODO: listen for hw button
void loop() {
    // TODO: check for network connection for arduino-based
    updateResources();
    coap.answerForObservations();
    coap.answerForIncomingRequest();
}

// for posix-based OSes compability:
#ifndef ARDUINO
int main(int argc, char **argv) {
    setup();
    while(1) {
        loop();
    }
    return 0;
}
#endif // !ARDUINO
