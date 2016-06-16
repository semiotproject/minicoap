#include "minicoap.h"
#include "wellknowncoreresource.h"
#include "lightresource.h"

#ifdef ARDUINO
/* Set these to your desired credentials. */
const char *softap_ssid = "ESPap";
const char *softap_password = "thereisnospoon";
#endif // ARDUINO

const char led1Name[] = "led1";
const char led2Name[] = "led2";
const char wnkAnswer[] = "</led1>,<led2>";

MiniCoAP coap;
WellKnownCoreResource wnkRes(wnkAnswer,&coap);
#ifdef LED1
LightResource led1(led1Name,LED1,true,&coap);
#endif // LED1

#ifdef LED2
LightResource led2(led2Name,LED2,true,&coap);
#endif // LED2

void setup() {
#ifdef ARDUINO
    pinMode(BUTTON, INPUT);

    coap.begin(); // TODO: check if success
#endif // ARDUINO
}

void updateResources() {
#ifdef ARDUINO
    if (digitalRead(BUTTON)==HIGH) {
        WiFi.softAP(softap_ssid, softap_password);
    }
    else {
        // TODO: read from eeprom
        Wifi.begin(ssid,pass);
    }
#endif // ARDUINO
}

// TODO: listen for hw button
void loop() {
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
