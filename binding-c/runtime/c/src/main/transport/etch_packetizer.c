/* $Id$ 
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more 
 * contributor license agreements. See the NOTICE file distributed with  
 * this work for additional information regarding copyright ownership. 
 * The ASF licenses this file to you under the Apache License, Version  
 * 2.0 (the "License"); you may not use this file except in compliance  
 * with the License. You may obtain a copy of the License at 
 * 
 * http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and 
 * limitations under the License. 
 */ 

/*
 * etch_packetizer.c
 * packetizes a stream data source. Reads and verfifies a packet header consisting
 * of a 32-bit signature and 32-bit length. verifies the flag, using length extracted
 * from header; reads packet data and forwards it to the packet handler. as a packet 
 * source, accepts a packet and prepends a packet header prior to delivering it to a 
 * data source.
 */

#include "etch_packetizer.h"
#include "etch_objecttypes.h"
#include "etch_url.h"
#include "etch_log.h"

static const char* LOG_CATEGORY = "etch_packetizer";

etch_packetizer* new_packetizer_b (i_transportdata*, const int maxpktsize);
int      etch_pktizer_session_control  (void*, etch_event*, etch_object*);
int      etch_pktizer_session_notify   (void*, etch_event*);
etch_object* etch_pktizer_session_query    (void*, etch_query*); 
int      etch_pktizer_transport_control(void*, etch_event*, etch_object*);
int      etch_pktizer_transport_notify (void*, etch_event*);
etch_object* etch_pktizer_transport_query  (void*, etch_query*); 

const wchar_t* ETCH_PKTIZER_MAX_PKT_SIZE_TERM = L"Packetizer.maxPktSize";
const int ETCH_PKTIZER_DEFMAXPKTSIZE = 10240;
const int ETCH_PKTIZER_HEADERSIZE = 8;
const int ETCH_PKTIZER_SIG = 0xdeadbeef;


/* - - - - - - - - - - - - - - - - - - - - - - 
 * packetizer constructor/destructor
 * - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * new_packetizer()
 * * etch_packetizer public constructor
 * @param itd i_transportdata interface owned by caller
 * @param uri a URI string owned by caller.
 * @param resources a resources map owned by caller.
 */
etch_packetizer* new_packetizer (i_transportdata* itd, wchar_t* uri, etch_resources* resxmap) 
{
    etch_url* url = new_url(uri);   

    etch_packetizer* packetizer = new_packetizer_a (itd, url, resxmap);  

    etch_object_destroy(url);
    return packetizer;
}


/**
 * new_packetizer_a()
 * etch_packetizer constructor 2
 */
etch_packetizer* new_packetizer_a (i_transportdata* itd, etch_url* url, etch_resources* resxmap)
{
    int maxpktsize = 0, result = 0;

    result = etchurl_get_integer_term (url, ETCH_PKTIZER_MAX_PKT_SIZE_TERM, &maxpktsize);
    if (-1 == result || maxpktsize <= 0) maxpktsize = ETCH_PKTIZER_DEFMAXPKTSIZE;

    return new_packetizer_b (itd, maxpktsize);
}


/*
 * destroy_packetizer()
 * etch_packetizer destructor
 */
int destroy_packetizer(void* data)
{
    etch_packetizer* thisx = (etch_packetizer*)data;
    etch_status_t status = ETCH_SUCCESS;
    if (NULL == thisx) return -1;

    if (!is_etchobj_static_content(thisx))
    {  
        etch_object_destroy(thisx->sessiondata);

        etch_object_destroy(thisx->transportpkt);

        if (thisx->datalock) {
            status = etch_mutex_destroy(thisx->datalock);
            ETCH_ASSERT(status == ETCH_SUCCESS);
        }

        etch_object_destroy(thisx->savebuf); 
    }
            
    return destroy_objectex((etch_object*)thisx);
}


/**
 * etch_pktizer_transport_packet()
 * delivers packet to transport after adding packet header.
 * @param whoto recipient - caller retains this memory.
 * @param fbuf buffer positioned at the packet data, with space for header
 * @return 0 success, -1 error
 */
int etch_pktizer_transport_packet(void* data, void* whoData, void* bufferData)
{
    etch_who* whoto = (etch_who*)whoData;
    etch_flexbuffer* fbuf = (etch_flexbuffer*)bufferData;
    etch_packetizer* thisx = (etch_packetizer*)data;
    int result = -1;
    ETCH_ASSERT(is_etch_packetizer(thisx));

    do
    {   size_t saveindex = 0;
        const int datalen = (int) etch_flexbuf_avail(fbuf);

        if (datalen < ETCH_PKTIZER_HEADERSIZE) break;

        saveindex = fbuf->index;

        etch_flexbuf_put_int(fbuf, ETCH_PKTIZER_SIG);
        etch_flexbuf_put_int(fbuf, datalen - ETCH_PKTIZER_HEADERSIZE);

        fbuf->index = saveindex;       

        /* deliver packet buffer to transport */        
        result = thisx->transport->transport_data(thisx->transport->thisx, whoto, fbuf);

    } while(0);

    return result;
}

/**
 * etch_pktizer_session_data()
 * i_sessiondata::session_data override.
 * delivers data to the session from the transport
 * @param whofrom from who sent the packet data
 * caller retains this memory, can be null.
 * @param fbuf the packet from the packet source positioned at the data, caller retains.
 * @return 0 success, -1 error. 
 */
int etch_pktizer_session_data (void* data, void* whoData, void* bufferData)
{
    etch_packetizer* pzr = (etch_packetizer*)data;
    etch_who* whofrom = (etch_who*)whoData;
    etch_flexbuffer* fbuf = (etch_flexbuffer*)bufferData;
    /* two scenarios: the first is that we have no buffered data and the 
     * entire packet is contained within the buffer. in that case we can skip  
	 * the details and ship the packet directly to the handler.
     */
    size_t curlen, curndx, bytes_avail, fbuf_avail, packet_bodylen, bytes_put;
    int  bytes_needed;
    etch_flexbuffer* savebuf = NULL;
    ETCH_ASSERT(is_etch_packetizer(pzr));
    savebuf = pzr->savebuf;

    while(1) /* while(fbuf.avail() > 0 ... */
    {   if (0 >= (fbuf_avail = etch_flexbuf_avail(fbuf))) break; 

        bytes_avail = savebuf->datalen + fbuf_avail;

        if (pzr->is_wantheader)
        {   
            if (bytes_avail >= (unsigned) ETCH_PKTIZER_HEADERSIZE)
            {   /* there are enough bytes in the pipeline for a header */
                if (savebuf->datalen == 0)
                {   /* save buffer is empty, entire header in fbuf */ 
                    packet_bodylen = etch_pktizer_process_header(pzr, fbuf, FALSE);  
                }
                else /* save non empty, header split across save and buf, so ... */
                {    /* move enough bytes from buf to save to complete a header */
                    bytes_needed = ETCH_PKTIZER_HEADERSIZE - (int) savebuf->datalen;

                    bytes_put = etch_flexbuf_put_from (savebuf, fbuf, bytes_needed);
                    if (0 == bytes_put) return -1;
                    etch_flexbuf_set_index(savebuf, 0);
                    packet_bodylen = etch_pktizer_process_header(pzr, savebuf, TRUE);
                }	
			
                if (packet_bodylen == -1) return -1;  /* bad header */
				if (packet_bodylen == 0)  continue;	  /* header with empty body */					 
				pzr->bodylength = packet_bodylen;	
				pzr->is_wantheader = FALSE;
            }
            else /* want header, but too few bytes in the pipeline */
			{    /* ... so save fbuf to the save buffer */
                etch_flexbuf_set_index (savebuf, savebuf->datalen);
                etch_flexbuf_put_from(savebuf, fbuf, -1);
		    }
        }
        else /* didn't need a header, so ... */
        if (bytes_avail >= pzr->bodylength)
        {
			/* we need the body, and there are enough bytes in the pipeline.
			 * three scenarios: the body is entirely in savebuf, the body is 
			 * split, or the body is entirely in fbuf. assert that the body   
             * is not entirely in save, or we'd have processed it last time.  
             */
            ETCH_ASSERT(savebuf->datalen < pzr->bodylength);

            if (savebuf->datalen == 0)
            {   /* saved buffer is empty, entire body is in fbuf */
                curlen = fbuf->datalen;
                curndx = fbuf->index;
                etch_flexbuf_set_length(fbuf, curndx + pzr->bodylength);

                /* send the packet in the input buffer up the chain to the next 
                 * higher layer of the transport stack. fyi packetizer.session.thisx 
                 * is that layer object, ordinarily the messagizer.
                 */
                pzr->session->session_packet (pzr->session->thisx, whofrom, fbuf);

                /* fyi the input buffer can contain a partial packet, multiple 
                 * packets, or whatever, so we may not be done with it yet. */
                etch_flexbuf_set_length(fbuf, curlen);   
                etch_flexbuf_set_index (fbuf, curndx + pzr->bodylength);
                pzr->is_wantheader = TRUE;
            }
            else 
            {   /* savebuf.datalen not zero, so body is split across the save buffer 
                 * and the input buffer. move enough bytes from input buffer fbuf 
                 * to complete the packet body. 
                 */
                bytes_needed = (int) (pzr->bodylength - savebuf->datalen);
                bytes_put = etch_flexbuf_put_from (savebuf, fbuf, bytes_needed);
                if (bytes_put <= 0) return -1;
                etch_flexbuf_set_index(savebuf, 0);

                /* send the newly-isolated packet up the chain to the next higher
                 * layer of the transport stack. fyi packetizer.session.thisx 
                 * is that layer object, ordinarily the messagizer.
                 */
                pzr->session->session_packet (pzr->session->thisx, whofrom, savebuf);
                
                etch_flexbuf_reset(savebuf);
                pzr->is_wantheader = TRUE;                
            }
        }
        else /* need body but too few bytes in pipeline to complete it */
        {    /* ... so save the input buffer to the save buffer */
            bytes_put = etch_flexbuf_put_from (savebuf, fbuf, ETCH_FLEXBUF_PUT_ALL);
        }
    }

    /* buffer is now empty and we're done */
    return fbuf_avail == 0? 0: -1;
}


/**
 * etch_pktizer_set_session()
 * set packetizer session interface presumably to that of the next higher layer.
 * @param session an i_sessionpacket reference. caller owns this object.
 */
void etch_pktizer_set_session (void* data, void* sessionData)
{
    etch_packetizer* thisx = (etch_packetizer*)data;
    i_session* newsession = (i_session*)sessionData;
    ETCH_ASSERT(is_etch_packetizer(thisx));
    ETCH_ASSERT(is_etch_sessionpacket(newsession));
    thisx->session = (i_sessionpacket*)newsession;
}


/**
 * new_packetizer_b()
 * etch_packetizer private constructor
 * @param transport the transport interface of the next lower layer of the stack,
 * that being the connection, e.g. etch_tcp_connection. not owned.
 * @param maxpktsize maximum number of bytes in a packet (default currently 10240)
 */
etch_packetizer* new_packetizer_b (i_transportdata* itd, const int maxpktsize)
{
    etch_packetizer* packetizer = NULL;
    i_transport* itransport = NULL;
    i_session* isession = NULL;
    etch_mutex* mutex = NULL;
    int result = -1;
    ETCH_ASSERT(is_etch_transportdata(itd));

    do
    {   
        #if(ETCHPZR_HAS_MUTEX)
        if (NULL == (mutex = new_mutex(etch_apr_mempool, ETCHMUTEX_NESTED))) break;
        #endif

        /* - - - - - - - - - - - - - - -
         * etch_packetizer
         * - - - - - - - - - - - - - - -
         */
        packetizer = (etch_packetizer*) new_object
            (sizeof(etch_packetizer), ETCHTYPEB_PACKETIZER, CLASSID_PACKETIZER);

        ((etch_object*)packetizer)->destroy  = destroy_packetizer;  
        ((etch_object*)packetizer)->clone    = clone_null;
        packetizer->datalock = mutex;

        packetizer->headersize    = ETCH_PKTIZER_HEADERSIZE;
        packetizer->is_wantheader = TRUE;
        packetizer->maxpacketsize = maxpktsize > 0? maxpktsize: ETCH_PKTIZER_DEFMAXPKTSIZE;

        packetizer->savebuf = new_flexbuffer(ETCH_DEFSIZE); /* 2K default */

        /* set our transport to that of next lower layer (connection) */
        packetizer->transport = itd;  /* not owned */
            

        /* - - - - - - - - - - - - - - -
         * i_transportpacket
         * - - - - - - - - - - - - - - -
         */
        itransport = new_transport_interface_ex (packetizer,  
            (etch_transport_control) etch_pktizer_transport_control, 
            (etch_transport_notify)  etch_pktizer_transport_notify, 
            (etch_transport_query)   etch_pktizer_transport_query,
             etch_pktizer_get_session, 
             etch_pktizer_set_session);

        /* instantiate i_transportpacket interface which packetizer implements */
        packetizer->transportpkt = new_transportpkt_interface (packetizer, 
            etch_pktizer_transport_packet, 
            itransport);  /* transportpkt now owns itransport */

        /* copy i_transportpacket interface methods up to packetizer */
        packetizer->transport_packet  = etch_pktizer_transport_packet;
        packetizer->transport_control = itransport->transport_control;
        packetizer->transport_notify  = itransport->transport_notify;
        packetizer->transport_query   = itransport->transport_query;
        packetizer->get_session       = itransport->get_session;
        packetizer->set_session       = itransport->set_session;


        /* - - - - - - - - - - - - - - -
         * i_sessiondata
         * - - - - - - - - - - - - - - -
         */
        isession = new_session_interface (packetizer,  
            (etch_session_control) etch_pktizer_session_control, 
            (etch_session_notify)  etch_pktizer_session_notify, 
            (etch_session_query)   etch_pktizer_session_query);

        /* instantiate i_sessiondata interface which packetizer implements */
        packetizer->sessiondata = new_sessiondata_interface(packetizer, 
            etch_pktizer_session_data, 
            isession);  /* sessiondata now owns isession */ 

        /* copy session interface to parent */
        packetizer->session_data    = etch_pktizer_session_data;
        packetizer->session_control = isession->session_control;
        packetizer->session_notify  = isession->session_notify;
        packetizer->session_query   = isession->session_query;

        /* finally set session of next lower layer (tcp connection) to our session */
        /* fyi we must pass the implementor of transport as thisx, i.e. tcpconnection */
        packetizer->transport->set_session (packetizer->transport->thisx, packetizer->sessiondata);
        
        result = 0;

    } while(0);


    if (-1 == result) { 
        etch_object_destroy(packetizer);
        packetizer = NULL;
    } 

    return packetizer;
}




/* - - - - - - - - - - - - - - -
 * i_transportpacket
 * - - - - - - - - - - - - - - -
 */


/**
 * etch_pktizer_get_session()
 * @return a reference to the packetizer i_sessiondata interface.
 * this object is owned by whatever object set the packetizer session.
 */
i_session* etch_pktizer_get_session (void* data)
{
    etch_packetizer* thisx = (etch_packetizer*)data;
    ETCH_ASSERT(is_etch_packetizer(thisx));
    return (i_session*)thisx->session;
}




/**
 * etch_pktizer_transport_control()
 * i_transportpacket::transport_control override.
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int etch_pktizer_transport_control (void* data, etch_event* control, etch_object* value)
{
  etch_packetizer* thisx = (etch_packetizer*)data;
    ETCH_ASSERT(is_etch_packetizer(thisx));
    /*      pzr     itd                          pzr      itd    tcpconx */
    return thisx->transport->transport_control (thisx->transport->thisx, control, value); 
}


/**
 * etch_pktizer_transport_notify()
 * i_transportpacket::transport_notify override.
 * @param evt, caller relinquishes.
 */
int etch_pktizer_transport_notify (void* data, etch_event* evt)
{
  etch_packetizer* thisx = (etch_packetizer*)data;
    ETCH_ASSERT(is_etch_packetizer(thisx));
    return thisx->transport->transport_notify (thisx->transport->thisx, evt);
}


/**
 * etch_pktizer_transport_query()
 * i_transportpacket::transport_query override.
 * @param query, caller relinquishes.
 */
etch_object* etch_pktizer_transport_query (void* data, etch_query* query) 
{
    etch_packetizer* thisx = (etch_packetizer*)data;
    ETCH_ASSERT(is_etch_packetizer(thisx));
    return thisx->transport->transport_query (thisx->transport->thisx, query);
}


/* - - - - - - - - - - - - - - -
 * i_sessiondata
 * - - - - - - - - - - - - - - -
 */
  
/*
 * etch_pktizer_process_header()
 * extract header information from packet
 * returns data length from header, or -1 if bad header
 */
int etch_pktizer_process_header (etch_packetizer* pzr, etch_flexbuffer* fbuf, const int is_reset)
{
    int curlen = 0, signature = 0;
    int result = etch_flexbuf_get_int(fbuf, &signature);
    if (-1 == result || ETCH_PKTIZER_SIG != signature) return -1;

    result = etch_flexbuf_get_int(fbuf, &curlen);

    if (is_reset)
        etch_flexbuf_reset(fbuf);

    if( curlen > (int) pzr->maxpacketsize) {
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, "Packetsize in header is too long, dropping header.");
    }

    return result == -1 || curlen < 0 || curlen > (int) pzr->maxpacketsize? 
        -1: curlen;
}


/**
 * etch_pktizer_session_control()
 * i_sessiondata::session_control override.
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int etch_pktizer_session_control (void* data, etch_event* control, etch_object* value)
{
  etch_packetizer* thisx = (etch_packetizer*)data;
    ETCH_ASSERT(is_etch_packetizer(thisx));
    return thisx->session->session_control (thisx->session->thisx, control, value);
}


/**
 * etch_pktizer_session_notify()
 * i_sessiondata::session_notify override.
 * @param event, caller relinquishes.
 */
int etch_pktizer_session_notify  (void* data, etch_event* evt)
{
  etch_packetizer* thisx = (etch_packetizer*)data;
    ETCH_ASSERT(is_etch_packetizer(thisx));
    return thisx->session->session_notify (thisx->session->thisx, evt);
}


/**
 * etch_pktizer_session_query()
 * i_sessiondata::session_query override.
 * @param query, caller relinquishes.
 */
etch_object* etch_pktizer_session_query (void* data, etch_query* query) 
{
    etch_packetizer* thisx = (etch_packetizer*)data;
    ETCH_ASSERT(is_etch_packetizer(thisx));
    return thisx->session->session_query (thisx->session->thisx, query);
}
