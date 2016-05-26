#include "minicoap.h"
#include "lightresource.h"

#ifdef ARDUINO
#include "WiFiex.h"
#endif // ARDUINO

MiniCoAP coap;
LightResource led1(LED1,true,&coap);
LightResource led2(LED2,true,&coap);
LightResource led3(LED3,false,&coap);

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
