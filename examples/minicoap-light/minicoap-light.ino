#include "minicoap.h"
#include "lightresource.h"

#ifdef ARDUINO
#include "WiFiex.h"
#endif // ARDUINO

const char led1Name[] = "led1";
const char led2Name[] = "led2";
const char led3Name[] = "led3";

MiniCoAP coap;
LightResource led1(led1Name,LED1,true,&coap);
LightResource led2(led2Name,LED2,true,&coap);

void setup() {
#ifdef ARDUINO
    connectToWiFi();
#endif // ARDUINO
    coap.begin(); // TODO: check if success // FIXME: remove it
}

// TODO: listen for hw button
void loop() {
    // TODO: check for network connection for arduino-based
    // updateResources();
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
