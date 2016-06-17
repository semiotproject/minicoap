#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define DEBUG

/*
 * Select between supported platforms:
 * Raspberry Pi 2
 * ESP8266
 * General arduino with Ethernet Shield (TODO)
 * General POSIX-based OS
 */

//----Raspberry Pi 2:--------------
// #define WIRINGPI
// #define LED // enable LEDs
// #define LED1 1 // GPIO 1 PIN 12 -- PWM0
// #define LED2 23 // GPIO 23 PIN 33 -- PWM1
// #define LED3 3 // GPIO 3 PIN 15 -- General
// #define BUTTON 0 // GPIO 0 PIN 11

//----ESP8266:------------
#define LED 1 // GPIO 1
#define BUTTON 2 // GPIO 2

//----General arduino with Ethernet Shield:----
// TODO:

//----General POSIX-based OS:--------
// Just comment all the options above

// undef OBS_SUPPORT to remove subsribtions support
#define OBS_SUPPORT
#define MAX_ENDPOINTS_COUNT 10
#define MAX_OBSERVATIONS_COUNT 50 // Map: observer + resource
// FIXME: get rid of MAX_SEGMENTS
#define MAX_SEGMENTS 2  // 2 = /foo/bar, 3 = /foo/bar/baz
#define PORT 5683
// TODO: count MAXRESPLEN:
#define MAXRESPLEN 1500 // FIXME: buflen 4096

#endif // DEFINITIONS_H
