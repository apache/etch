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
 * etch_transportpkt.c
 * i_transportpacket interface
 */

#include "etch_transport_packet.h"
#include "etch_message.h"
#include "etch_flexbuffer.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"
/*
static const char* LOG_CATEGORY = "etch_transportpkg";
*/
#define ETCHPACKET_DEFHEADERSIZE 8

int etchtransportpkt_def_transportpacket (void* tp, void* sender, void* buffer)
{
      return -1;
}

/*
 * etchtransportpkt_def_headersize()
 * default implementation of get_header_size() 
 * @return size of packet header in bytes
 */
int etchtransportpkt_def_headersize (void* data)
{
    i_transportpacket* tp = (i_transportpacket*)data;
    return tp->header_size;
}


/*
 * destroy_transportpacket()
 * i_transportpacket destructor
 */
int destroy_transportpacket(void* data)
{
    i_transportpacket* itp = (i_transportpacket*)data;
    ETCH_ASSERT(is_etch_transportpkt(itp));

    if (!is_etchobj_static_content(itp))
    {   etch_free(itp->itransport);
    }

    return destroy_objectex((etch_object*) itp);
}


/**
 * new_transportpkt_interface()
 * i_transportpacket constructor
 * @param tp transport_packet virtual function overrides, 
 * @param itransport transport interface virtual function overrides, 
 * caller relinquishes ownership of this memory
 */
i_transportpacket* new_transportpkt_interface(void* thisx, etch_transport_packet tp, i_transport* itransport)  
{
    i_transportpacket* newi = (i_transportpacket*) new_object
        (sizeof(i_transportpacket), ETCHTYPEB_TRANSPORTPKT, CLASSID_TRANSPORTPKT);

    ((etch_object*)newi)->clone   = clone_null;
    ((etch_object*)newi)->destroy = destroy_transportpacket;
    newi->thisx   = thisx;
    newi->header_size = ETCHPACKET_DEFHEADERSIZE;

    newi->transport_packet = tp? tp: etchtransportpkt_def_transportpacket;

    newi->itransport = itransport? itransport: new_default_transport_interface(thisx);

    newi->transport_control = newi->itransport->transport_control; 
    newi->transport_notify  = newi->itransport->transport_notify;
    newi->transport_query   = newi->itransport->transport_query;
    newi->get_session       = newi->itransport->get_session;
    newi->set_session       = newi->itransport->set_session;

    newi->get_headersize    = etchtransportpkt_def_headersize;  

    return newi;
}
