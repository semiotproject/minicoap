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
        // TODO: ArduinoJson
    // TODO: check if request is from softAP
#ifdef ARDUINO
    IPAddress host = server->getCurrentSocket().host;
    // FIXME: custom gatewayIP in combined mode
    if (host[0]==192 && host[1]==168 && host[2]==4)  {
        memset(config,0,MAXRESPLEN);
        if (inpkt->payload.len<MAXRESPLEN);
        memcpy(config,inpkt->payload.p,inpkt->payload.len);
        rootJson = jsonBuffer.parseObject(config);
        // TODO: check if success
        WiFi.begin(rootJson["wifi-name"],rootJson["wifi-password"]);
        return getServer()->coap_make_response(inpkt, outpkt, (uint8_t *)config, strlen(config), COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
#endif ARDUINO
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_TEXT_PLAIN);

}
