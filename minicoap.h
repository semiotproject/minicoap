#ifndef MINICOAP_H
#define MINICOAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#ifdef Arduino

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <strings.h>
#endif

#include "minicoapdefinitions.h"

class MiniCoAP
{
public:
    MiniCoAP(unsigned int coapPort);
    int coap_make_response(const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t *content, size_t content_len, coap_responsecode_t rspcode, coap_content_type_t content_type);
    int addEndpoint(coap_method_t method, coap_endpoint_func handler, const coap_endpoint_path_t *path, bool *obs_changed = NULL, const char *core_attr = NULL);
    void answerForIncomingRequest();
#ifdef MAX_OBSERVATIONS_COUNT
    void answerForObservation(unsigned int index);
    void answerForObservations();
#endif // #ifdef MAX_OBSERVATIONS_COUNT

private:
#ifdef MAX_OBSERVATIONS_COUNT
    int addObserver(const coap_packet_t *inpkt);
    // TODO: removeObserver
#endif // MAX_OBSERVATIONS_COUNT
    int receiveUDP();
    int sendUDP();
    int answer(coap_packet_t *pkt);
    unsigned int endpointsCount = 0;
    coap_endpoint_t endpoints[MAX_ENDPOINTS_COUNT];

    unsigned int port = PORT;
    int fd;
#ifdef IPV6 // FIXME
    struct sockaddr_in6 servaddr, cliaddr;
#else /* IPV6 */
    struct sockaddr_in servaddr, cliaddr;
#endif /* IPV6 */
    uint8_t buf[4096];
    uint8_t scratch_raw[4096];
    coap_rw_buffer_t scratch_buf;
    int coap_parse(coap_packet_t *pkt, const uint8_t *buf, size_t buflen);
    int coap_parseHeader(coap_header_t *hdr, const uint8_t *buf, size_t buflen);
    int coap_parseToken(coap_buffer_t *tokbuf, const coap_header_t *hdr, const uint8_t *buf, size_t buflen);
    // http://tools.ietf.org/html/rfc7252#section-3.1
    int coap_parseOptionsAndPayload(coap_option_t *options, uint8_t *numOptions, coap_buffer_t *payload, const coap_header_t *hdr, const uint8_t *buf, size_t buflen);
    // advances p
    int coap_parseOption(coap_option_t *option, uint16_t *running_delta, const uint8_t **buf, size_t buflen);
    int coap_compare_uri_path_opt(const coap_packet_t* inpkt, const coap_endpoint_path_t* path);
    int coap_handle_req(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    int coap_build(uint8_t *buf, size_t *buflen, const coap_packet_t *pkt);
    void coap_option_nibble(uint32_t value, uint8_t *nibble);
    // options are always stored consecutively, so can return a block with same option num
    const coap_option_t *coap_findOptions(const coap_packet_t *pkt, uint8_t num, uint8_t *count = NULL);
    void endpoint_setup(void);
    void build_rsp(); // build .well-known/core answer
    uint16_t rsplen = MAXRESPLEN; // .well-known/core answer length // FIXME: it's just a buf len
    char rsp[MAXRESPLEN] = ""; // .well-known/core answer
    int handle_get_well_known_core(const coap_packet_t *inpkt, coap_packet_t *outpkt);
#ifdef DEBUG
    void coap_dumpHeader(coap_header_t *hdr);
    void coap_dump(const uint8_t *buf, size_t buflen, bool bare);
    void coap_dumpOptions(coap_option_t *opts, size_t numopt);
    void coap_dumpPacket(coap_packet_t *pkt);
#endif // DEBUG

#ifdef MAX_OBSERVATIONS_COUNT
    coap_observer_t observers[MAX_OBSERVATIONS_COUNT];
#endif // MAX_OBSERVATIONS_COUNT
};

#endif // MINICOAP_H
