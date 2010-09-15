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
 * etch_conxevent.c
 * event handler overrides for the various connection superclasses
 */

#include "etch_thread.h"
#include "etch_tcp_connection.h"
#include "etch_tcp_server.h"
#include "etch_objecttypes.h"
#include "etch_log.h"

static const char* LOG_CATEGORY = "etch_connection_event";

/*
 * etch_deftcplistener_on_event()
 * default handler for listener events
 */
int etch_deftcplistener_on_event(etch_tcp_server* l, etch_tcp_connection* c, const int e, int p1, void* p2)
{
    char cxstr[24], *smask = "server %u";  
       
    if (c)
    {   c->cx.listener = (etch_object*) l;
        return  c->cx.on_event(c, e, p1, p2);   
    }

    apr_snprintf(cxstr, sizeof(cxstr), smask, l->listener_id);
      
    switch(e)
    {
       case ETCH_CONXEVT_CREATED:
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s created\n", cxstr);
            break;

       case ETCH_CONXEVT_CREATERR:
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s not created\n", cxstr);
            break;

       case ETCH_CONXEVT_OPENING:
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s opening ...\n", cxstr);
            break;

       case ETCH_CONXEVT_DESTROYING:
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s destroying ...\n", cxstr);
            break;

       case ETCH_CONXEVT_DESTROYED:
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s destroyed\n", cxstr);
            break;

       case ETCH_CONXEVT_SHUTDOWN:
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, "%s shutdown request detected\n", cxstr);
            break;
    }

    return 0;
}


/*
 * etch_tcpconx_on_event()
 * default handler for connection events
 */
int etch_tcpconx_on_event(void* data, const int e, int p1, void* p2)
{
    etch_tcp_connection* c = (etch_tcp_connection*)data;
    int result = 0, lid = 0;
    char cxstr[32], estr[128]; 
    char *scmask = "server %u connxn %u", *cmask = "connxn %u";

    if  (is_etch_tcpserver(c->cx.listener))
         lid = ((etch_tcp_server*)(c->cx.listener))->listener_id;
    if  (lid)
         sprintf(cxstr, scmask, lid, c->cx.conxid);     
    else sprintf(cxstr, cmask, c->cx.conxid);

    switch(e)
    {
        case ETCH_CONXEVT_RECEIVING:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "%s begin receive (block) ...\n", cxstr);
             break;

        case ETCH_CONXEVT_RECEIVERR:
             apr_strerror(p1, estr, 128);
             if(IS_ETCH_SOCKET_TIMEOUT(p1))
                  ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s socket receive timed out\n", cxstr); 
             else
             if(p1 == APR_OTHER_END_CLOSED)
                  ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN,"%s connection was broken\n", cxstr);  
             else
                 ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR,  "%s apr_socket_recv() %s\n", cxstr, estr); 
             break;

        case ETCH_CONXEVT_RECEIVED:   /* "eod=%d", p1 */
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s end receive %d bytes\n", cxstr, (int)(size_t)p2);    
             break;

       case ETCH_CONXEVT_CONXCLOSED:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s local connection closed\n", cxstr);
             break;

        case ETCH_CONXEVT_PEERCLOSED:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s peer connection closed\n", cxstr);
             break;

        case ETCH_CONXEVT_RECEIVEND:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "%s exit receive loop\n", cxstr, p1);                
             break;

        case ETCH_CONXEVT_SENDING:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "%s begin send\n", cxstr);
             break;

        case ETCH_CONXEVT_SENDERR:
             apr_strerror(p1, estr, 128);
             if(IS_ETCH_SOCKET_TIMEOUT(p1))
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s socket send timed out\n", cxstr); 
             else
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s apr_socket_send() %s\n", cxstr, estr); 
             break;

        case ETCH_CONXEVT_SENDEND:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s end send %d bytes\n", cxstr, p1);                
             break;

        case ETCH_CONXEVT_RCVPUMP_START:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s start receive pump\n", cxstr, p1);                
             break;

        case ETCH_CONXEVT_RCVPUMP_RECEIVING:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s thread %d receiving ...\n", cxstr, p1);                
             break;

        case ETCH_CONXEVT_RCVPUMP_ERR:
             result = -1;   
             if(p1 == APR_OTHER_END_CLOSED)
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, "%s connection was broken\n", cxstr);  
             else
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s receive failed\n", cxstr);  
             break;

        case ETCH_CONXEVT_RCVPUMP_STOP:
             result = p1;
             if (result >= 0)
                 ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s receive pump on thread %d exited\n", cxstr, (int) (size_t) p2);
             else
                 ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s receive pump abnormal exit\n", cxstr);
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s destroying accepted connection\n", cxstr);
             break;

        case ETCH_CONXEVT_ACCEPTING:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s accepting ...\n", cxstr);
             break;

        case ETCH_CONXEVT_ACCEPTERR:
             result = -1;
             apr_strerror((apr_status_t)(size_t)p2, estr, 128);
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s apr_socket_accept() %s\n", cxstr, estr);
             break;

        case ETCH_CONXEVT_ACCEPTED:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s accepted\n", cxstr);
             break;

        case ETCH_CONXEVT_CREATED:
             if(p1)
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s created for socket %x\n", cxstr, p1);
             else
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s created\n", cxstr);
             break;

        case ETCH_CONXEVT_CREATERR:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s not created\n", cxstr);
             break;

        case ETCH_CONXEVT_OPENING:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s opening ...\n", cxstr);
             break;

        case ETCH_CONXEVT_OPENERR:
             result = -1;

             switch(p1)
             {
                case 0:
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s not opened\n", cxstr);
                    break;
                case 2:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, "%s socket connect: %s\n", cxstr, estr);
                    break;
                case 3:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, 
                        "%s socket create: %s\n", cxstr, estr);
                    break;
                case 4:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, 
                        "%s sockaddr info: %s\n", cxstr, estr);
                    break;
                case 5:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, 
                        "%s socket bind: %s\n", cxstr, estr);
                    break;
                case 6:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s socket listen: %s\n", cxstr, estr);
                    break;
             }
             break;

        case ETCH_CONXEVT_OPENED:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s opened\n", cxstr);
             break;

        case ETCH_CONXEVT_STARTING:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s starting ...\n", cxstr);
             break;

        case ETCH_CONXEVT_STARTERR:
             result = -1;
             switch(p1)
             {
                case 0:
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s not started\n", cxstr);
                    break;
                case 1:
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s etch_threadpool.run()\n", cxstr);
                    break;
             }
             break;

        case ETCH_CONXEVT_STARTED:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s started\n", cxstr);
             break;

        case ETCH_CONXEVT_ACCEPTPUMPEXIT:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s accept pump on thread %d exited\n", cxstr, p1);
             break;

        case ETCH_CONXEVT_ACCEPTPUMPEXITERR:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s accept pump abnormal exit\n", cxstr);
             break;

        case ETCH_CONXEVT_LISTENED:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s server id is %d\n", cxstr, p1);
             break;

        case ETCH_CONXEVT_UP:
        case ETCH_CONXEVT_DOWN:
             break;

        case ETCH_CONXEVT_SOCKOPTERR:
             result = 1; /* to increment counter */
             apr_strerror(p1, estr, 128);
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s set socket option %s: %s\n", cxstr, p2, estr);
             break;

        case ETCH_CONXEVT_SHUTDOWN:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, "%s shutdown request detected\n", cxstr);
             break;

        case ETCH_CONXEVT_STOPPING:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s stopping ...\n", cxstr);
             break;

        case ETCH_CONXEVT_STOPERR:
             result = -1;
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s not stopped\n", cxstr);
             break;

        case ETCH_CONXEVT_STOPPED:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s stopped\n", cxstr);
             break;

        case ETCH_CONXEVT_CLOSING:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s closing ...\n", cxstr);
             break;

        case ETCH_CONXEVT_CLOSERR:
             result = -1;

             switch(p1)
             {
                case 0:
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s not closed\n", cxstr);
                    break;
                case 1:
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s close when not open\n", cxstr);
                    break;
                case 2:
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s concurrent close denied\n", cxstr);
                    break;
                case 3:
                    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, 
                        "%s apr_socket_close() error %d\n", cxstr, (int)(size_t)p2);
                    break;
             }
             break;

        case ETCH_CONXEVT_CLOSED:    
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s closed\n", cxstr);
             break;

        case ETCH_CONXEVT_DESTROYING:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s destroying ...\n", cxstr);
             break;

        case ETCH_CONXEVT_DESTROYED:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s destroyed\n", cxstr);
             break;
    }

    return result;
}

