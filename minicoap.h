#ifndef MINICOAP_H
#define MINICOAP_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "wellknowncoreresource.h"
#include "systemresource.h"
#include "configresource.h"

#define COAPBUFLEN 500
#define URIBUFLEN 300
#define resourcesListSize 4

class MiniCoAP
{
public:
    MiniCoAP();
    int begin(bool sleepy = false);
    int setButton(int pin);
    int addResource(CoAPResource *resource);
    int handleClient();
private:
    bool isSleepy = false;
    int buttonState = -3;
    int sending = 0;

    bool useButton = false;
    int buttonPin;
    int numberOfSTAAttempts = 23; // about 10 sec
    int numberOfRdAttempts = 100; // FIXME
    bool toAP();
    bool toSTA();

    WiFiUDP udp;
    unsigned char udpRxBuf[COAPBUFLEN];
    size_t udpRxBufLen;
    char uriBuf[URIBUFLEN];
    int uriBufLen;
    int receivePacket();
    void sendPacket();

    WellKnownCoreResource *wnkRes;
    void addResToWnk(CoAPResource *resource);
    SystemResource *rootRes; // not in resourcesList
    ConfigResource *confRes;

    CoAPResource* resourcesList[resourcesListSize];
    int resourcesCount = 2; // wnkRes + confRes; rootRes is separated

    int handleAwake();
    int handleSleepy();

    CoapPDU *currentPacket;
    CoapPDU *responsePacket;
    CoapPDU *sleepyPacket;
    char *rdCacheDiscoverUri = "/.well-known/core?rt=core.rd-cache";
    int registerSleepy();
    int sendSleepy();
    int waitForCoapAnswer();

    bool basicAuthIsOk();
    // TODO: observers
};

#endif // MINICOAP_H
