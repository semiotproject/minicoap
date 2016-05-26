#include "minicoap.h"
#include "lightresource.h"

#ifdef ARDUINO
#include "WiFiex.h"
#endif // ARDUINO

MiniCoAP coap;
LightResource led1(&coap);
LightResource led2(&coap);
LightResource led3(&coap);

void setup() {
    led1.setLightName("led1");
    led1.setPin(LED1);
    led2.setLightName("led2");
    led1.setPin(LED2);
    led3.setLightName("led3");
    led1.setPin(LED3);
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
