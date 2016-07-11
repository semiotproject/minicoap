#include "minicoap.h"
#include "wellknowncoreresource.h"
#include "lightresource.h"
#include "configresource.h"

#include <ESP8266WiFi.h>
/* Set these to your desired credentials. */
const char *ssid = "SSID";
const char *password = "password";

const char led1Name[] = "led1";
const char wnkAnswer[] = "<\/led1>,<\/config>"; // TODO: scheme

MiniCoAP coap;
WellKnownCoreResource wnkRes(wnkAnswer,&coap);
LightResource led1(led1Name,D6,true,&coap);
ConfigResource conf(&coap);

void setup() {
    Serial.begin(115200);
    delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("~");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
    coap.begin();
}

// TODO: listen for hw button
void loop() {
    // change WiFi.mode
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
