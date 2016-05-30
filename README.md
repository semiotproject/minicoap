# minicoap
microcoap-based Arduino/POSIX-based systems C++ [CoAP](https://tools.ietf.org/html/rfc7252) library with subscriptions support

## Description

miniCoAP library is a [microcoap](https://github.com/1248/microcoap) port to C++ language.

Unlike the microcoap library, which is focusing on the smallest memory footprint for microcontrollers,
miniCoAP library focusing on full CoAP protocol support.

## Supported platforms

### Raspberry Pi 2
Using [WiringPi](https://github.com/WiringPi/WiringPi) library

### ESP8266
Using [Arduino](https://github.com/esp8266/Arduino) libraries.

### General arduino with Ethernet Shield (TODO)

### General POSIX-based OS

## Features

+ Resources creation (inheriting the `CoAPResource` class)
+ Subscriptions support (Observe CoAP option)
