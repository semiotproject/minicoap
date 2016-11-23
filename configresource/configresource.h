#ifndef CONFIGRESOURCE_H
#define CONFIGRESOURCE_H

#include <ESP8266WiFi.h>
#include <Hash.h>
#include <FS.h>

#include "semiotresource.h"

// TODO: wifi config resource
// TODO: sleepInterval
class ConfigResource : public SemIoTResource
{
public:
    ConfigResource();
    void setSleepy(bool sleepy = true);
    void begin(bool sleepy = false);
    void readFromFS();
    void writeToFS();
    int getMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    int putMethod(uint8_t *payloadValue, int payloadLen,
                              CoapPDU::ContentFormat contentFormat);
    uint8_t *getPayloadPointer();
    int getPayloadLength();
    //char *getUri();
    bool getMethodIsPrivate();
    bool putMethodIsPrivate();
    CoapPDU::Code getCode();
    const char *STA_SSID();
    const char *STA_PSK();
    const char *AP_SSID();
    const char *AP_PSK();
    bool setSTA(String ssid, String pass);
    bool setConfigAuth(String username, String pass);
    String getBasicAuthString();
    bool setDeviceLabel(String newLabel);
    bool setLocationLabel(String newLabel);
    long getSleepIntervalMs();
    String getObservationHost();
    void setObservationHost(String newHost);
    uint16_t getObservationPort(); // FIXME: long
    void setObservationPort(uint16_t newPort);
    String getObservationUri();
    void setObservationUri(String newUri);

    // TODO:

//    bool ConfigResource::authIsOk(const coap_packet_t * inpkt)
//    {
//        if (WiFi.getMode() == WIFI_AP) {
//            return true;
//        }
//        else {
//            return getServer()->checkAuth(inpkt);
//        }
//        return false;
//    }
private:
    bool isSleepy = false;
    CoapPDU::Code currentCode;
    //char *uri = "/config";
    char *typeObject = "Configuration";
    String sta_ssid = "SemIoT_RU";
    String sta_psk = "semiotproject";
    String ap_ssid;
    String ap_psk = "semiotproject";
    // String label = "SemIoT Device Prototype";
    String configUserName = "superuser";
    String configPassword = "superpassword";

    String wifiNameUri = "wifi-name";
    String wifiPasswordUri = "wifi-password";
    String configUsernameUri = "username";
    String configPasswordUri = "password";

    long sleepIntervalMsObject = 180000;
    String sleepIntervalMsPredicate = "sleepIntervalMs";
    // observationServer
    String observationServerPredicate = "observationServer";
    String observationHostPredicate = "observationHost";
    String observationHostObject = "rdcache.semiot.ru";
    String observationPortPredicate = "observationPort";
    uint16_t observationPortObject = 5683;
    String observationUriPredicate = "observationUri";
    String observationUriObject = "/data";
    String basicAuthString;

    String answer;
    void generateAnswer();
};

#endif // CONFIGRESOURCE_H
