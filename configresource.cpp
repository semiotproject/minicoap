#include "configresource.h"
#include <string.h>
#ifdef ARDUINO
#include <ArduinoJson.h>
#endif

// <json/json.h>

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
    char pld = inpkt->payload.p[0];
    // TODO: connect to different wifi
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t *)config, strlen(config), COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);

    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);

}
