#include "otaresource.h"

OTAResource::OTAResource()
{
    uri = "/ota";
}

CoapPDU::Code OTAResource::getCode()
{

}

CoapPDU::ContentFormat OTAResource::getContentFormat()
{

}

int OTAResource::getMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{

}

bool OTAResource::getMethodIsPrivate()
{

}

int OTAResource::postMethod(uint8_t *payloadValue, int payloadLen, CoapPDU::ContentFormat contentFormat)
{
    // if upload:
    // upload to SPIFFS
    // get sha1sum
    // if command to flash:
    // flash from SPIFFS
    // ESP.restart();

    // notes:
    // uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    // Update.begin(maxSketchSpace);
    // Update.printError(Serial);
    // Update.write(upload.buf, upload.currentSize) != upload.currentSize
    // Update.end(true)
}

bool OTAResource::postMethodIsPrivate()
{
    true;
}
