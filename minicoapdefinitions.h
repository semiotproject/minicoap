#ifndef MINICOAPDEFINITIONS_H
#define MINICOAPDEFINITIONS_H

//-----------------------------------------------------------
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
//#define LED 2 // GPIO 2 PIN 13
//#define BUTTON 0 // GPIO 0 PIN 11

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

//-----------------------------------------------------------

#define MAXOPT 16

//http://tools.ietf.org/html/rfc7252#section-3
typedef struct
{
    uint8_t ver;                /* CoAP version number */
    uint8_t t;                  /* CoAP Message Type */
    uint8_t tkl;                /* Token length: indicates length of the Token field */
    uint8_t code;               /* CoAP status code. Can be request (0.xx), success reponse (2.xx),
                                 * client error response (4.xx), or rever error response (5.xx)
                                 * For possible values, see http://tools.ietf.org/html/rfc7252#section-12.1 */
    uint8_t id[2];
} coap_header_t;

typedef struct
{
    const uint8_t *p;
    size_t len;
} coap_buffer_t;

typedef struct
{
    uint8_t *p;
    size_t len;
} coap_rw_buffer_t;

typedef struct
{
    uint8_t num;                /* Option number. See http://tools.ietf.org/html/rfc7252#section-5.10 */
    coap_buffer_t buf;          /* Option value */
} coap_option_t;

typedef struct
{
    coap_header_t hdr;          /* Header of the packet */
    coap_buffer_t tok;          /* Token value, size as specified by hdr.tkl */
    uint8_t numopts;            /* Number of options */
    coap_option_t opts[MAXOPT]; /* Options of the packet. For possible entries see
                                 * http://tools.ietf.org/html/rfc7252#section-5.10 */
    coap_buffer_t payload;      /* Payload carried by the packet */
} coap_packet_t;

/////////////////////////////////////////

//http://tools.ietf.org/html/rfc7252#section-12.2
typedef enum
{
    COAP_OPTION_IF_MATCH = 1,
    COAP_OPTION_URI_HOST = 3,
    COAP_OPTION_ETAG = 4,
    COAP_OPTION_IF_NONE_MATCH = 5,
    COAP_OPTION_OBSERVE = 6,
    COAP_OPTION_URI_PORT = 7,
    COAP_OPTION_LOCATION_PATH = 8,
    COAP_OPTION_URI_PATH = 11,
    COAP_OPTION_CONTENT_FORMAT = 12,
    COAP_OPTION_MAX_AGE = 14,
    COAP_OPTION_URI_QUERY = 15,
    COAP_OPTION_ACCEPT = 17,
    COAP_OPTION_LOCATION_QUERY = 20,
    COAP_OPTION_PROXY_URI = 35,
    COAP_OPTION_PROXY_SCHEME = 39
} coap_option_num_t;

//http://tools.ietf.org/html/rfc7252#section-12.1.1
typedef enum
{
    COAP_METHOD_GET = 1,
    COAP_METHOD_POST = 2,
    COAP_METHOD_PUT = 3,
    COAP_METHOD_DELETE = 4
} coap_method_t;

//http://tools.ietf.org/html/rfc7252#section-12.1.1
typedef enum
{
    COAP_TYPE_CON = 0,
    COAP_TYPE_NONCON = 1,
    COAP_TYPE_ACK = 2,
    COAP_TYPE_RESET = 3
} coap_msgtype_t;

//http://tools.ietf.org/html/rfc7252#section-5.2
//http://tools.ietf.org/html/rfc7252#section-12.1.2
#define MAKE_RSPCODE(clas, det) ((clas << 5) | (det))
typedef enum
{
    COAP_RSPCODE_CONTENT = MAKE_RSPCODE(2, 5),
    COAP_RSPCODE_NOT_FOUND = MAKE_RSPCODE(4, 4),
    COAP_RSPCODE_BAD_REQUEST = MAKE_RSPCODE(4, 0),
    COAP_RSPCODE_CHANGED = MAKE_RSPCODE(2, 4)
} coap_responsecode_t;

//http://tools.ietf.org/html/rfc7252#section-12.3
typedef enum
{
    COAP_CONTENTTYPE_NONE = -1, // bodge to allow us not to send option block
    COAP_CONTENTTYPE_TEXT_PLAIN = 0,
    COAP_CONTENTTYPE_APPLICATION_LINKFORMAT = 40,
} coap_content_type_t;

///////////////////////

typedef enum
{
    COAP_ERR_NONE = 0,
    COAP_ERR_HEADER_TOO_SHORT = 1,
    COAP_ERR_VERSION_NOT_1 = 2,
    COAP_ERR_TOKEN_TOO_SHORT = 3,
    COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER = 4,
    COAP_ERR_OPTION_TOO_SHORT = 5,
    COAP_ERR_OPTION_OVERRUNS_PACKET = 6,
    COAP_ERR_OPTION_TOO_BIG = 7,
    COAP_ERR_OPTION_LEN_INVALID = 8,
    COAP_ERR_BUFFER_TOO_SMALL = 9,
    COAP_ERR_UNSUPPORTED = 10,
    COAP_ERR_OPTION_DELTA_INVALID = 11,
} coap_error_t;

///////////////////////

typedef int (*coap_endpoint_func)(const coap_packet_t *inpkt, coap_packet_t *outpkt); // FIXME: is there a case where hi and low are not equals to inpkt's data?

typedef struct
{
    int count;
    const char *elems[MAX_SEGMENTS];
} coap_endpoint_path_t;

typedef struct
{
    coap_method_t method;               /* (i.e. POST, PUT or GET) */
    coap_endpoint_func handler;         /* callback function which handles this
                                         * type of endpoint (and calls
                                         * coap_make_response() at some point) */
    const coap_endpoint_path_t *path;   /* path towards a resource (i.e. foo/bar/) */
    const char *core_attr;              /* the 'ct' attribute, as defined in RFC7252, section 7.2.1.:
                                         * "The Content-Format code "ct" attribute
                                         * provides a hint about the
                                         * Content-Formats this resource returns."
                                         * (Section 12.3. lists possible ct values.) */
    bool * obs_changed; // NULL if not observable
} coap_endpoint_t;
////////////////////////////////////////////

const coap_endpoint_path_t path_well_known_core = {2, {".well-known", "core"}};

#ifdef OBS_SUPPORT
    typedef struct
    {
        // TODO: different for platforms
        #ifdef ARDUINO
            IPAddress host;
            unsigned int port;
        #else // ARDUINO
            struct sockaddr_in socket;
        #endif // ARDUINO
        bool available = true;
    } coap_client_socket_t;

    typedef struct
    {
        coap_client_socket_t cliaddr;
        coap_packet_t inpkt;
        unsigned char obs_tick; // TODO: change size to 3 bytes (array of char)
        int endpoint_path_index = -1;
        uint8_t scratch_raw[8]; // for token
    } coap_observer_t;
#endif // OBS_SUPPORT

#endif // MINICOAPDEFINITIONS_H
