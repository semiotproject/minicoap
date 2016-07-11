#include "configresource.h"
#include <string.h>
#ifdef ARDUINO
#include <ESP8266WiFi.h>
#endif // ARDUINO

ConfigResource::ConfigResource(MiniCoAP *coapServer):CoAPResource(coapServer)
{
    resourcePath = {1,{"config"}};
    memset(configjs,0,MAXRESPLEN);
    strcpy(configjs,"\{\"@context\":\"\/config\/context\",\"wifi-name\":\"SSID\",\"wifi-password\":\"password\"\}");
}

int ConfigResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    if (strlen(configjs)) {
        return getServer()->coap_make_response(inpkt, outpkt, (uint8_t 
        *)configjs, strlen(configjs), COAP_RSPCODE_CONTENT, 
COAP_CONTENTTYPE_APPLICATION_JSON);
    }
    else {
        return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_APPLICATION_JSON);
    }
}

int ConfigResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    //    {
    //    "@context": "/config/context",
    //    "wifi-name": "ISST",
    //    "wifi-password": "blahbalh"
    //    }
#ifdef ARDUINO
    IPAddress host = getServer()->getCurrentSocket().host;
    // WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
    if (WiFi.getMode() == WIFI_STA)  {
        memset(configjs,0,MAXRESPLEN);
        if (inpkt->payload.len<MAXRESPLEN);
        memcpy(configjs,inpkt->payload.p,inpkt->payload.len);
        JsonObject& rootJson = jsonBuffer.parseObject(configjs);
        // TODO: save to EEPROM:
        if (rootJson.success()) {
            // (rootJson["wifi-name"],rootJson["wifi-password"]);
            return getServer()->coap_make_response(inpkt, outpkt, (uint8_t 
            *)configjs, strlen(configjs), COAP_RSPCODE_CHANGED, 
COAP_CONTENTTYPE_TEXT_PLAIN);
          }
    }
#endif // ARDUINO
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_TEXT_PLAIN);

}
