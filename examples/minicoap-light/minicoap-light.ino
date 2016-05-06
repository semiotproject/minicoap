#include "minicoap.h"
#ifdef RASPBERRY
#include <wiringPi.h>
#endif // RASPBERRY

static MiniCoAP coap(5683);

// TODO: get rid of static content argument?
static uint8_t light = 1;
bool lightChanged = true;

void turnOffLight() {
    light = 1;
#ifdef ARDUINO
    digitalWrite(LED, LOW);
#endif // ARDUINO
#ifdef RASPBERRY
    digitalWrite(LED, LOW);
#endif // RASPBERRY
}

void turnOnLight() {
    light = 0;
#ifdef ARDUINO
    digitalWrite(LED, HIGH);
#endif // ARDUINO
#ifdef RASPBERRY
    digitalWrite(LED, HIGH);
#endif // RASPBERRY
}

static const coap_endpoint_path_t pathLight = {1, {"light"}};
int getLight(const coap_packet_t *inpkt, coap_packet_t *outpkt) {
    return coap.coap_make_response(inpkt, outpkt, &light, sizeof(light), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

int putLight(const coap_packet_t *inpkt, coap_packet_t *outpkt) {
    if (inpkt->payload.len == 1) {
        if (inpkt->payload.p[0] == '1') {
            turnOnLight();
        }
        else if (inpkt->payload.p[0] == '0'){
            turnOffLight();
        }
        return coap.coap_make_response(inpkt, outpkt, &light, sizeof(light), COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
    return coap.coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
}

void setup() {
#ifdef RASPBERRY
    wiringPiSetup();
    pinMode(LED, OUTPUT);
    digitalWrite(LED, light);
#endif // RASPBERRY
#ifdef ARDUINO
    pinMode(LED, OUTPUT);
    digitalWrite(LED, light);
#endif // ARDUINO
    coap.addEndpoint(COAP_METHOD_GET,getLight,&pathLight,&lightChanged,"ct=0;obs");
    coap.addEndpoint(COAP_METHOD_PUT,putLight,&pathLight); // TODO
}

// TODO: listen for hw button
void loop() {
    int i = 0;
    while(1)
    {
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
