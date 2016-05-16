#include "minicoap.h"
#ifdef RPI2
#include <wiringPi.h>
#endif // RPI2

static MiniCoAP coap(5683);

// TODO: get rid of static content argument?
static unsigned char light = '1';
bool lightChanged = true;
bool needToSwitchTheLight = false;

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

// FIXME: too much for interrupt:
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

void buttonInterrupt() {
    needToSwitchTheLight = true;
}

void setup() {
#ifdef RPI2
    wiringPiSetup();
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH); // FIXME: according to light
#endif // RPI2
#ifdef BUTTON // RPI2 BUTTON
    pinMode(BUTTON, INPUT);
    wiringPiISR(BUTTON, INT_EDGE_BOTH, &buttonInterrupt); // TODO: handle error
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
    int i = 0;
    while(1)
    {
        if (needToSwitchTheLight) {
            if (light=='1') {
                turnOffLight();
            }
            else if (light=='0') {
                turnOnLight();
            }
            needToSwitchTheLight = false;
        }
        else if (needToSwitchTheLight==-1) {
            turnOffLight();
        }
        coap.answerForObservation(i);
        coap.answerForIncomingRequest();
        if (i<MAX_OBSERVATIONS_COUNT-2) {
            i++;
        }
        else {
            i=0;
            // FIXME: get rid of
            // light++;
            // lightChanged=true;
        }
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
