#include "configresource.h"
#include <string.h>

ConfigResource::ConfigResource(MiniCoAP *coapServer):CoAPResource(coapServer)
{

}

int ConfigResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    if (strlen(config)) {
        return getServer()->coap_make_response(inpkt, outpkt, (uint8_t *)config, strlen(config), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_JSON);
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
    IPAddress host = server->getCurrentSocket().host;
    // FIXME: custom gatewayIP in combined mode
    IPAddress softIP = WiFi.softAPIP();
    if (host[0]==softIP[0] && host[1]==softIP[1] && host[2]==softIP[2])  {
        memset(config,0,MAXRESPLEN);
        if (inpkt->payload.len<MAXRESPLEN);
        memcpy(config,inpkt->payload.p,inpkt->payload.len);
        rootJson = jsonBuffer.parseObject(config);
        // TODO: save to EEPROM:
        // (rootJson["wifi-name"],rootJson["wifi-password"]);
        return getServer()->coap_make_response(inpkt, outpkt, (uint8_t *)config, strlen(config), COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
#endif ARDUINO
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_TEXT_PLAIN);

}
