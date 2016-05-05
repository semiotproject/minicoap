#include "minicoap.h"

MiniCoAP::MiniCoAP(unsigned int coapPort)
{
    port = coapPort;
    scratch_buf = {scratch_raw, sizeof(scratch_raw)};

    // TODO: different platforms
#ifdef IPV6
    fd = socket(AF_INET6,SOCK_DGRAM,0);
#else /* IPV6 */
    fd = socket(AF_INET,SOCK_DGRAM,0);
#endif /* IPV6 */

    fcntl(fd, F_SETFL, O_NONBLOCK);

    bzero(&servaddr,sizeof(servaddr));
#ifdef IPV6
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(port);
#else /* IPV6 */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
#endif /* IPV6 */
    int bind_result = ::bind(fd,(struct sockaddr *)&servaddr, sizeof(servaddr));

#ifdef DEBUG
    printf("MiniCOAP: bind socket result: %d\n",bind_result);
#endif
    endpoint_setup();
}

int MiniCoAP::addEndpoint(coap_method_t method, coap_endpoint_func handler, const coap_endpoint_path_t *path, bool* obs_changed, const char *core_attr)
{
    if (endpointsCount+1<=MAX_ENDPOINTS_COUNT) {
        endpoints[endpointsCount].method=method;
        endpoints[endpointsCount].handler=handler;
        endpoints[endpointsCount].path=path;
        endpoints[endpointsCount].core_attr=core_attr;
        endpoints[endpointsCount].obs_changed=obs_changed;
        endpointsCount++;
        // build_rsp(); // TODO:
        // endpoint_setup();
        return 1;
    }
    return 0;
}

void MiniCoAP::answerForIncomingRequest()
{
    int x = receiveUDP();
    if (x>0) { // filling buf
        rsplen=x; // FIXME: could overflow?
#ifdef DEBUG
        printf("Received %d bytes:",rsplen);
        coap_dump(buf, rsplen, true);
        printf("\n");
#endif
        int rc;
        coap_packet_t pkt;
        if (0 == (rc = coap_parse(&pkt, buf, (size_t)rsplen))) {
            answer(&pkt);
        }
#ifdef DEBUG
        else {
            printf("Bad packet rc=%d\n", rc);
        }
#endif // DEBUG
    }
}

void MiniCoAP::answerForObservation(unsigned int index)
{
    if (index<MAX_OBSERVATIONS_COUNT) {
        if (!observers[index].cliaddr.available) {
            int enpoint_index = -1;
            for (int i=0;i<MAX_ENDPOINTS_COUNT;i++) {
                if (coap_compare_uri_path_opt(&observers[index].inpkt,endpoints[i].path)) {
                    if (endpoints[i].obs_changed) {
                        if (*endpoints[i].obs_changed) {
                            // TODO: fill cliaddr
                            coap_client_socket_t *clisock = &observers[index].cliaddr;
                            // TODO: different platforms:
                            cliaddr.sin_family = clisock->socket.sin_family;
                            cliaddr.sin_addr.s_addr = clisock->socket.sin_addr.s_addr;
                            cliaddr.sin_port = clisock->socket.sin_port;
                            answer(&observers[index].inpkt); // TODO: remove obs if not answer
                            // TODO: if answer:
                            observers[index].obs_tick++;
                            enpoint_index = i;
                        }
                    }
                }
            }
            if (enpoint_index>-1) {
                *endpoints[enpoint_index].obs_changed=false;
            }
        }
    }
}

void MiniCoAP::answerForObservations()
{
    for(int i = 0; i<=MAX_OBSERVATIONS_COUNT; i++) {
        answerForObservation(i);
    }
}

int MiniCoAP::addObserver(const coap_packet_t *inpkt)
{
    for (int i=0;i<MAX_OBSERVATIONS_COUNT;i++) {
        if (!observers[i].cliaddr.available) {
            if ((observers[i].cliaddr.socket.sin_family==cliaddr.sin_family) &&
                    (observers[i].cliaddr.socket.sin_addr.s_addr==cliaddr.sin_addr.s_addr) &&
                    (observers[i].cliaddr.socket.sin_port==cliaddr.sin_port)) {
                return i;
            }
        }
    }
    int x = -1;
    for (int i=0;i<MAX_OBSERVATIONS_COUNT;i++) {
        if (observers[i].cliaddr.available) {
            x = i;
            break;
        }
    }
    if (x>-1) {
        observers[x].obs_tick=1;
        observers[x].inpkt=*inpkt;
        observers[x].cliaddr.socket=cliaddr;
        observers[x].cliaddr.available=false;
    }
    return x;
}

int MiniCoAP::receiveUDP()
{
#ifdef ARDUINO
    return -0 // TODO: Arduino
#else
    socklen_t len = sizeof(cliaddr);
    ssize_t x = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &len); // TODO: save rsplen here
    return x;
#endif // ARDUINO
}

int MiniCoAP::sendUDP()
{
#ifdef ARDUINO
    return -0 // TODO: Arduino
#else
    return sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
#endif // ARDUINO

}

int MiniCoAP::answer(coap_packet_t* pkt)
{
    int rc;
    rsplen = sizeof(buf); // FIXME: possibly could ruin everything
    coap_packet_t rsppkt;
#ifdef DEBUG
    coap_dumpPacket(pkt);
#endif
    coap_handle_req(pkt, &rsppkt);

    if (0 != (rc = coap_build(buf, (size_t*)&rsplen, &rsppkt))) // FIXME: not size_t
        printf("coap_build failed rc=%d\n", rc);
    else
    {
#ifdef DEBUG
        printf("Sending: ");
        coap_dump(buf, rsplen, true);
        printf("\n");
#endif
#ifdef DEBUG
        coap_dumpPacket(&rsppkt);
#endif
        sendUDP();
    }
    return 0; // TODO:
}

int MiniCoAP::coap_parse(coap_packet_t *pkt, const uint8_t *buf, size_t buflen)
{
    int rc;
    // coap_dump(buf, buflen, false);
    if (0 != (rc = coap_parseHeader(&pkt->hdr, buf, buflen))) {
        return rc;
    }
    // coap_dumpHeader(&hdr);
    if (0 != (rc = coap_parseToken(&pkt->tok, &pkt->hdr, buf, buflen))) {
        return rc;
    }
    pkt->numopts = MAXOPT;
    if (0 != (rc = coap_parseOptionsAndPayload(pkt->opts, &(pkt->numopts), &(pkt->payload), &pkt->hdr, buf, buflen))) {
        return rc;
    }
    // coap_dumpOptions(opts, numopt);
    return 0;
}

int MiniCoAP::coap_make_response(const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t *content, size_t content_len, coap_responsecode_t rspcode, coap_content_type_t content_type)
{
    uint8_t msgid_hi = inpkt->hdr.id[0];
    uint8_t msgid_lo = inpkt->hdr.id[1];
    outpkt->hdr.ver = 0x01;
    outpkt->hdr.t = COAP_TYPE_ACK;
    outpkt->hdr.tkl = 0;
    outpkt->hdr.code = rspcode;
    outpkt->hdr.id[0] = msgid_hi;
    outpkt->hdr.id[1] = msgid_lo;
    outpkt->numopts = 1;

    // need token in response
    if (inpkt->hdr.tkl) {
        outpkt->hdr.tkl = inpkt->hdr.tkl;
        outpkt->tok = inpkt->tok;
    }

    // cheking for observe option:
    bool obs = false;
    const coap_option_t *observeOption = coap_findOptions(inpkt,COAP_OPTION_OBSERVE);
    if (observeOption != NULL) {
        obs = true;
        // https://tools.ietf.org/html/rfc7641#section-2
        if (observeOption->buf.len == 0) { // register
            int observer_count = addObserver(inpkt);
            if (observer_count>-1) {
                outpkt->numopts = 2;
                // safe because 1 < MAXOPT
                outpkt->opts[0].num = COAP_OPTION_OBSERVE;
                outpkt->opts[0].buf.p = &observers[observer_count].obs_tick;
                outpkt->opts[0].buf.len = 1; // TODO: size of array
            }

        }
        else if ((observeOption->buf.len == 1) && (*observeOption->buf.p==1)) {
            // TODO: removeObserver
        }
    }

    outpkt->opts[obs].num = COAP_OPTION_CONTENT_FORMAT;
    outpkt->opts[obs].buf.p = scratch_buf.p;
    if (scratch_buf.len < 2)
        return COAP_ERR_BUFFER_TOO_SMALL;
    scratch_buf.p[0] = ((uint16_t)content_type & 0xFF00) >> 8;
    scratch_buf.p[1] = ((uint16_t)content_type & 0x00FF);
    outpkt->opts[obs].buf.len = 2;
    outpkt->payload.p = content;
    outpkt->payload.len = content_len;

    return 0;
}

int MiniCoAP::coap_parseHeader(coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    if (buflen < 4)
        return COAP_ERR_HEADER_TOO_SHORT;
    hdr->ver = (buf[0] & 0xC0) >> 6;
    if (hdr->ver != 1)
        return COAP_ERR_VERSION_NOT_1;
    hdr->t = (buf[0] & 0x30) >> 4;
    hdr->tkl = buf[0] & 0x0F;
    hdr->code = buf[1];
    hdr->id[0] = buf[2];
    hdr->id[1] = buf[3];
    return 0;
}

int MiniCoAP::coap_parseToken(coap_buffer_t *tokbuf, const coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    if (hdr->tkl == 0)
    {
        tokbuf->p = NULL;
        tokbuf->len = 0;
        return 0;
    }
    else
    if (hdr->tkl <= 8)
    {
        if (4U + hdr->tkl > buflen)
            return COAP_ERR_TOKEN_TOO_SHORT;   // tok bigger than packet
        tokbuf->p = buf+4;  // past header
        tokbuf->len = hdr->tkl;
        return 0;
    }
    else
    {
        // invalid size
        return COAP_ERR_TOKEN_TOO_SHORT;
    }
}

int MiniCoAP::coap_parseOptionsAndPayload(coap_option_t *options, uint8_t *numOptions, coap_buffer_t *payload, const coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    size_t optionIndex = 0;
    uint16_t delta = 0;
    const uint8_t *p = buf + 4 + hdr->tkl;
    const uint8_t *end = buf + buflen;
    int rc;
    if (p > end)
        return COAP_ERR_OPTION_OVERRUNS_PACKET;   // out of bounds

    //coap_dump(p, end - p);

    // 0xFF is payload marker
    while((optionIndex < *numOptions) && (p < end) && (*p != 0xFF))
    {
        if (0 != (rc = coap_parseOption(&options[optionIndex], &delta, &p, end-p)))
            return rc;
        optionIndex++;
    }
    *numOptions = optionIndex;

    if (p+1 < end && *p == 0xFF)  // payload marker
    {
        payload->p = p+1;
        payload->len = end-(p+1);
    }
    else
    {
        payload->p = NULL;
        payload->len = 0;
    }

    return 0;
}

int MiniCoAP::coap_parseOption(coap_option_t *option, uint16_t *running_delta, const uint8_t **buf, size_t buflen)
{
    const uint8_t *p = *buf;
    uint8_t headlen = 1;
    uint16_t len, delta;

    if (buflen < headlen) // too small
        return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;

    delta = (p[0] & 0xF0) >> 4;
    len = p[0] & 0x0F;

    // These are untested and may be buggy
    if (delta == 13)
    {
        headlen++;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        delta = p[1] + 13;
        p++;
    }
    else
    if (delta == 14)
    {
        headlen += 2;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        delta = ((p[1] << 8) | p[2]) + 269;
        p+=2;
    }
    else
    if (delta == 15)
        return COAP_ERR_OPTION_DELTA_INVALID;

    if (len == 13)
    {
        headlen++;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        len = p[1] + 13;
        p++;
    }
    else
    if (len == 14)
    {
        headlen += 2;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        len = ((p[1] << 8) | p[2]) + 269;
        p+=2;
    }
    else
    if (len == 15)
        return COAP_ERR_OPTION_LEN_INVALID;

    if ((p + 1 + len) > (*buf + buflen))
        return COAP_ERR_OPTION_TOO_BIG;

    //printf("option num=%d\n", delta + *running_delta);
    option->num = delta + *running_delta;
    option->buf.p = p+1;
    option->buf.len = len;
    //coap_dump(p+1, len, false);

    // advance buf
    *buf = p + 1 + len;
    *running_delta += delta;

    return 0;
}

int MiniCoAP::coap_compare_uri_path_opt(const coap_packet_t *inpkt, const coap_endpoint_path_t *path)
{
    if (path) {
        uint8_t count;
        const coap_option_t *opt;
        if (NULL != (opt = coap_findOptions(inpkt, COAP_OPTION_URI_PATH, &count)))
        {
            if (count != path->count)
                return 0;
            for (int i=0;i<count;i++)
            {
                if (opt[i].buf.len != strlen(path->elems[i]))
                    return 0;
                if (0 != memcmp(path->elems[i], opt[i].buf.p, opt[i].buf.len))
                    return 0;
            }
            // match!
            return 1;
        }
    }
    return 0;
}

int MiniCoAP::coap_handle_req(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    coap_endpoint_t *ep = &endpoints[0];

    while(NULL != ep->handler)
    {
        if (ep->method != inpkt->hdr.code)
            goto next;
        if (coap_compare_uri_path_opt(inpkt,ep->path)) {
            // match!
            return ep->handler(inpkt, outpkt);
        }
next:
        // e++;
        // ep = &endpoints[e]; // FIXME: crazy stuff
        ep++;
    }

    coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_NONE);

    return 0;
}

int MiniCoAP::coap_build(uint8_t *buf, size_t *buflen, const coap_packet_t *pkt)
{
    size_t opts_len = 0;
    size_t i;
    uint8_t *p;
    uint16_t running_delta = 0;

    // build header
    if (*buflen < (4U + pkt->hdr.tkl))
        return COAP_ERR_BUFFER_TOO_SMALL;

    buf[0] = (pkt->hdr.ver & 0x03) << 6;
    buf[0] |= (pkt->hdr.t & 0x03) << 4;
    buf[0] |= (pkt->hdr.tkl & 0x0F);
    buf[1] = pkt->hdr.code;
    buf[2] = pkt->hdr.id[0];
    buf[3] = pkt->hdr.id[1];

    // inject token
    p = buf + 4;
    if ((pkt->hdr.tkl > 0) && (pkt->hdr.tkl != pkt->tok.len))
        return COAP_ERR_UNSUPPORTED;

    if (pkt->hdr.tkl > 0)
        memcpy(p, pkt->tok.p, pkt->hdr.tkl);

    // // http://tools.ietf.org/html/rfc7252#section-3.1
    // inject options
    p += pkt->hdr.tkl;

    for (i=0;i<pkt->numopts;i++)
    {
        uint32_t optDelta;
        uint8_t len, delta = 0;

        if (((size_t)(p-buf)) > *buflen)
             return COAP_ERR_BUFFER_TOO_SMALL;
        optDelta = pkt->opts[i].num - running_delta;
        coap_option_nibble(optDelta, &delta);
        coap_option_nibble((uint32_t)pkt->opts[i].buf.len, &len);

        *p++ = (0xFF & (delta << 4 | len));
        if (delta == 13)
        {
            *p++ = (optDelta - 13);
        }
        else
        if (delta == 14)
        {
            *p++ = ((optDelta-269) >> 8);
            *p++ = (0xFF & (optDelta-269));
        }
        if (len == 13)
        {
            *p++ = (pkt->opts[i].buf.len - 13);
        }
        else
        if (len == 14)
        {
            *p++ = (pkt->opts[i].buf.len >> 8);
            *p++ = (0xFF & (pkt->opts[i].buf.len-269));
        }

        memcpy(p, pkt->opts[i].buf.p, pkt->opts[i].buf.len);
        p += pkt->opts[i].buf.len;
        running_delta = pkt->opts[i].num;
    }

    opts_len = (p - buf) - 4;   // number of bytes used by options

    if (pkt->payload.len > 0)
    {
        if (*buflen < 4 + 1 + pkt->payload.len + opts_len)
            return COAP_ERR_BUFFER_TOO_SMALL;
        buf[4 + opts_len] = 0xFF;  // payload marker
        // FIXME: crash here if not static data source:
        memcpy(buf+5 + opts_len, pkt->payload.p, pkt->payload.len);
        *buflen = opts_len + 5 + pkt->payload.len;
    }
    else
        *buflen = opts_len + 4;
    return 0;
}

void MiniCoAP::coap_option_nibble(uint32_t value, uint8_t *nibble)
{
    if (value<13)
    {
        *nibble = (0xFF & value);
    }
    else
    if (value<=0xFF+13)
    {
        *nibble = 13;
    } else if (value<=0xFFFF+269)
    {
        *nibble = 14;
    }
}

const coap_option_t *MiniCoAP::coap_findOptions(const coap_packet_t *pkt, uint8_t num, uint8_t *count)
{
    // FIXME, options is always sorted, probably could find faster than this
    size_t i;
    const coap_option_t *first = NULL;
    if (count) {
        *count = 0;
    }

    for (i=0;i<pkt->numopts;i++)
    {
        if (pkt->opts[i].num == num)
        {
            if (NULL == first)
                first = &pkt->opts[i];
            if (count) {
                (*count)++;
            }
        }
        else
        {
            if (NULL != first)
                break;
        }
    }
    return first;
}

void MiniCoAP::endpoint_setup()
{
    endpoints[0].method=COAP_METHOD_GET;
    endpoints[0].handler=NULL;// (coap_endpoint_func)&MiniCoAP::handle_get_well_known_core;
    endpoints[0].path=&path_well_known_core;
    endpoints[0].core_attr="ct=40";
    build_rsp();
}

void MiniCoAP::build_rsp()
{
    uint16_t len = rsplen; // FIXME
    const coap_endpoint_t *ep = endpoints;
    int i;

    len--; // Null-terminated string

    while(NULL != ep->handler)
    {
        if (NULL == ep->core_attr) {
            ep++;
            continue;
        }

        if (0 < strlen(rsp)) {
            strncat(rsp, ",", len);
            len--;
        }

        strncat(rsp, "<", len);
        len--;

        for (i = 0; i < ep->path->count; i++) {
            strncat(rsp, "/", len);
            len--;

            strncat(rsp, ep->path->elems[i], len);
            len -= strlen(ep->path->elems[i]);
        }

        strncat(rsp, ">;", len);
        len -= 2;

        strncat(rsp, ep->core_attr, len);
        len -= strlen(ep->core_attr);

        ep++;
    }
}

int MiniCoAP::handle_get_well_known_core(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    return coap_make_response(inpkt, outpkt, (uint8_t*)rsp, strlen(rsp), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}

#ifdef DEBUG

void MiniCoAP::coap_dumpHeader(coap_header_t *hdr)
{
    printf("Header:\n");
    printf("  ver  0x%02X\n", hdr->ver);
    printf("  t    0x%02X\n", hdr->t);
    printf("  tkl  0x%02X\n", hdr->tkl);
    printf("  code 0x%02X\n", hdr->code);
    printf("  id   0x%02X%02X\n", hdr->id[0], hdr->id[1]);
}

void MiniCoAP::coap_dump(const uint8_t *buf, size_t buflen, bool bare)
{
    if (bare)
    {
        while(buflen--)
            printf("%02X%s", *buf++, (buflen > 0) ? " " : "");
    }
    else
    {
        printf("Dump: ");
        while(buflen--)
            printf("%02X%s", *buf++, (buflen > 0) ? " " : "");
        printf("\n");
    }
}

void MiniCoAP::coap_dumpOptions(coap_option_t *opts, size_t numopt)
{
    size_t i;
    printf(" Options:\n");
    for (i=0;i<numopt;i++)
    {
        printf("  0x%02X [ ", opts[i].num);
        coap_dump(opts[i].buf.p, opts[i].buf.len, true);
        printf(" ]\n");
    }
}

void MiniCoAP::coap_dumpPacket(coap_packet_t *pkt)
{
    coap_dumpHeader(&pkt->hdr);
    coap_dumpOptions(pkt->opts, pkt->numopts);
    printf("Payload: ");
    coap_dump(pkt->payload.p, pkt->payload.len, true);
    printf("\n");
}

#endif // DEBUG
