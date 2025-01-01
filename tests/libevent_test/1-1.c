#include <cstdio>
#include <stdio.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <memory.h>

void read_cb(struct bufferevent *bev, void *arg) {

    char buf[4096] = {0};
    char *pBody = NULL;
    uint64_t nLen = 0;

    // A temporary variable used by the receiving process
    struct evbuffer *evbuf = evbuffer_new();
    if (!evbuf) {
        // Handle allocation failure
        return;
    }

   

    // The full length of the protocol packet
    uint32_t pstrlen = 0;
    


    // loop receiving data
    while ((nLen = bufferevent_read(bev, buf, sizeof(buf))) != 0) {
        evbuffer_add(evbuf, buf, nLen);

        uint32_t tmp = 0;
        //CHANGE:
        // You should pass &tmp instead of tmp
        /*memcpy(tmp, buf, sizeof(uint32_t));*/
        if (nLen >= sizeof(uint32_t)) {
            memcpy(&tmp, buf, sizeof(uint32_t));
        } else {
            evbuffer_free(evbuf);
            return;
        }

        

        // Receives the data packet header, roughly verifies that the packet is valid, and assigns the value
        if (pstrlen == 0 && nLen > 36 && tmp > nLen) {
            pstrlen = tmp;
        }

        // We have a complete packet！
        if (pstrlen <= evbuffer_get_length(evbuf)) {
            break;
        }
    }

    nLen = evbuffer_get_length(evbuf);
    if (nLen == 0) {
        evbuffer_free(evbuf);
        return;
    }

    pBody = malloc(nLen + 256);
    if (!pBody) {
        evbuffer_free(evbuf);
        return;
    }
    memset(pBody, 0, nLen + 256);

    evbuffer_remove(evbuf, pBody, nLen);

    evbuffer_free(evbuf);

    // After the data packet is received, it is parsed and processed
    //...
}



