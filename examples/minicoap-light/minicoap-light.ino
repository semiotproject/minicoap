#include "minicoap.h"
#ifdef RPI2
#include <wiringPi.h>
#endif // RPI2

static MiniCoAP coap(5683);

// TODO: get rid of static content argument?
static unsigned char light = '1';
bool lightChanged = true;

void turnOffLight() {
    if (light=='1') {
        light = '0';
        lightChanged = true;
#ifdef ARDUINO
        digitalWrite(LED, LOW);
#endif // ARDUINO
#ifdef RPI2
        digitalWrite(LED, LOW);
#endif // RPI2
    }
}

void turnOnLight() {
    if (light=='0') {
        light = '1';
        lightChanged = true;
#ifdef ARDUINO
        digitalWrite(LED, HIGH);
#endif // ARDUINO
#ifdef RPI2
        digitalWrite(LED, HIGH);
#endif // RPI2
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
            turnOnLight();
        }
        else if (pld == '0'){
            turnOffLight();
        }
        return coap.coap_make_response(inpkt, outpkt, &light, sizeof(light), COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
    return coap.coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
}

void updateResources() {
#ifdef RPI2
    if (digitalRead(BUTTON)==HIGH) {
        turnOnLight();
    }
    else if (digitalRead(BUTTON)==LOW) {
        turnOffLight();
    }
#endif
}

void setup() {
#ifdef RPI2
    wiringPiSetup();
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH); // FIXME: according to light
#endif // RPI2
#ifdef BUTTON // RPI2 BUTTON
    pinMode(BUTTON, INPUT);
#endif
#ifdef ARDUINO
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH); // FIXME: according to light
#endif // ARDUINO
    coap.addEndpoint(COAP_METHOD_GET,getLight,&pathLight,&lightChanged,"ct=0;obs");
    coap.addEndpoint(COAP_METHOD_PUT,putLight,&pathLight); // TODO
}

// TODO: listen for hw button
void loop() {
    while(1)
    {
        updateResources();
        coap.answerForObservations();
        coap.answerForIncomingRequest();
    }
}

// for posix-based OSes compability:
#ifndef ARDUINO
int main(int argc, char **argv)
{
    setup();
    loop();
    return 0;
}
#endif // !ARDUINO
