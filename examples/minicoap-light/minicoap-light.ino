#include "minicoap.h"
#ifdef WIRINGPI
#include <wiringPi.h>
#endif // WIRINGPI
#ifdef ARDUINO
#include "WiFiex.h"
#endif // ARDUINO

MiniCoAP coap;

// TODO: get rid of static content argument?
bool lightOn = true;
static unsigned char light = '1';
bool lightChanged = true;

void turnOffLight() {
    if (light=='1') {
        light = '0';
        lightChanged = true;
#if defined(WIRINGPI) || defined(ARDUINO)
#ifdef LED
        digitalWrite(LED, LOW);
#endif // LED
#endif // WIRINGPI || ARDUINO
    }
}

void turnOnLight() {
    if (light=='0') {
        light = '1';
        lightChanged = true;
#if defined(WIRINGPI) || defined(ARDUINO)
#ifdef LED
        digitalWrite(LED, HIGH);
#endif // LED
#endif // WIRINGPI || ARDUINO
    }
}

static const coap_endpoint_path_t pathLight = {1, {"light"}};
int getLight(const coap_packet_t *inpkt, coap_packet_t *outpkt) {
    return coap.coap_make_response(inpkt, outpkt, &light, sizeof(light), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

int putLight(const coap_packet_t *inpkt, coap_packet_t *outpkt) {
    if (inpkt->payload.len == 1) {
        char pld = inpkt->payload.p[0];
        if (pld == '1') {
            lightOn = true;
        }
        else if (pld == '0'){
            lightOn = false;
        }
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
    if (digitalRead(BUTTON)==HIGH) {
        lightOn = true;
    }
    else if (digitalRead(BUTTON)==LOW) {
        lightOn = false;
    }
    if (lightOn) {
        turnOnLight();
    }
    else {
        turnOffLight();
    }
#endif // BUTTON
#endif
}

void setup() {
#ifdef WIRINGPI
    wiringPiSetup();
#endif // WIRINGPI
#if defined(WIRINGPI) || defined(ARDUINO)
#ifdef LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH); // FIXME: according to light
#endif // LED
#ifdef BUTTON // BUTTON
    pinMode(BUTTON, INPUT);
#endif
#endif // WIRINGPI || ARDUINO
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
