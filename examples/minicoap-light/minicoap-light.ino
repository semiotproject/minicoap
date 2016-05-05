#include "minicoap.h"

static MiniCoAP coap(5683);

// TODO: get rid of static content argument?
static uint8_t light = 0;

void turnOffLight() {
    light = 1;
#ifdef ARDUINO
    digitalWrite(LED, LOW);
#endif // ARDUINO
}

void turnOnLight() {
    light = 0;
#ifdef ARDUINO
    digitalWrite(LED, HIGH);
#endif // ARDUINO
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
    coap.addEndpoint(COAP_METHOD_GET,getLight,&pathLight);
    coap.addEndpoint(COAP_METHOD_GET,putLight,&pathLight);
}

// TODO: listen for button
void loop() {
    int i = 0;
    while(1)
    {
        coap.answerForObservation(i);
        coap.answerForIncomingRequest();
        if (i<MAX_OBSERVATIONS_COUNT) {
            i++;
        }
        else {
            i=0;
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
