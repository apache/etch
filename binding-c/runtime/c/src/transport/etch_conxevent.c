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

#include "apr_network_io.h"
#include "etchthread.h"
#include "etch_tcpconxn.h"
#include "etch_tcpserver.h"
#include "etchlog.h"
#include "etch_global.h"


/*
 * etch_deftcplistener_on_event()
 * default handler for listener events
 */
int etch_deftcplistener_on_event(etch_tcp_server* l, etch_tcp_connection* c, const int e, int p1, void* p2)
{
    static char cxstr[24], *smask = "server %u";  
       
    if (c)
    {   c->cx.listener = (etch_object*) l;
        return  c->cx.on_event(c, e, p1, p2);   
    }

    sprintf(cxstr, smask, l->listener_id);

    switch(e)
    {
       case ETCH_CONXEVT_CREATED:
            etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s created\n", cxstr);
            break;

       case ETCH_CONXEVT_CREATERR:
            etchlog(ETCHCONX, ETCHLOG_ERROR, "%s not created\n", cxstr);
            break;

       case ETCH_CONXEVT_OPENING:
            etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s opening ...\n", cxstr);
            break;

       case ETCH_CONXEVT_DESTROYING:
            etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s destroying ...\n", cxstr);
            break;

       case ETCH_CONXEVT_DESTROYED:
            etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s destroyed\n", cxstr);
            break;

       case ETCH_CONXEVT_SHUTDOWN:
            etchlog(ETCHCONX, ETCHLOG_WARNING, "%s shutdown request detected\n", cxstr);
            break;
    }

    return 0;
}


/*
 * etch_tcpconx_on_event()
 * default handler for connection events
 */
int etch_tcpconx_on_event(etch_tcp_connection* c, const int e, int p1, void* p2)
{
    int result = 0, lid = 0;
    static char cxstr[32], estr[128]; 
    static char *scmask = "server %u connxn %u", *cmask = "connxn %u";

    if  (is_etch_tcpserver(c->cx.listener))
         lid = ((etch_tcp_server*)(c->cx.listener))->listener_id;
    if  (lid)
         sprintf(cxstr, scmask, lid, c->cx.conxid);     
    else sprintf(cxstr, cmask, c->cx.conxid);

    switch(e)
    {
        case ETCH_CONXEVT_RECEIVING:
             etchlog(ETCHCONX, ETCHLOG_XDEBUG, "%s begin receive (block) ...\n", cxstr);
             break;

        case ETCH_CONXEVT_RECEIVERR:
             apr_strerror(p1, estr, 128);
             if  (IS_ETCH_SOCKET_TIMEOUT(p1))
                  etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s socket receive timed out\n", cxstr); 
             else
             if  (p1 == APR_OTHER_END_CLOSED)
                  etchlog(ETCHCONX, ETCHLOG_WARNING,"%s connection was broken\n", cxstr);  
             else etchlog(ETCHCONX, ETCHLOG_ERROR,  "%s apr_socket_recv() %s\n", cxstr, estr); 
             break;

        case ETCH_CONXEVT_RECEIVED:   /* "eod=%d", p1 */
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s end receive %d bytes\n", cxstr, (int)(size_t)p2);    
             break;

       case ETCH_CONXEVT_CONXCLOSED:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s local connection closed\n", cxstr);
             break;

        case ETCH_CONXEVT_PEERCLOSED:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s peer connection closed\n", cxstr);
             break;

        case ETCH_CONXEVT_RECEIVEND:
             etchlog(ETCHCONX, ETCHLOG_XDEBUG, "%s exit receive loop\n", cxstr, p1);                
             break;

        case ETCH_CONXEVT_SENDING:
             etchlog(ETCHCONX, ETCHLOG_XDEBUG, "%s begin send\n", cxstr);
             break;

        case ETCH_CONXEVT_SENDERR:
             apr_strerror(p1, estr, 128);
             if  (IS_ETCH_SOCKET_TIMEOUT(p1))
                  etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s socket send timed out\n", cxstr); 
             else etchlog(ETCHCONX, ETCHLOG_ERROR, "%s apr_socket_send() %s\n", cxstr, estr); 
             break;

        case ETCH_CONXEVT_SENDEND:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s end send %d bytes\n", cxstr, p1);                
             break;

        case ETCH_CONXEVT_RCVPUMP_START:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s start receive pump\n", cxstr, p1);                
             break;

        case ETCH_CONXEVT_RCVPUMP_RECEIVING:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s thread %d receiving ...\n", cxstr, p1);                
             break;

        case ETCH_CONXEVT_RCVPUMP_ERR:
             result = -1;   
             if  (p1 == APR_OTHER_END_CLOSED)
                  etchlog(ETCHCONX, ETCHLOG_WARNING, "%s connection was broken\n", cxstr);  
             else etchlog(ETCHCONX, ETCHLOG_ERROR, "%s receive failed\n", cxstr);  
             break;

        case ETCH_CONXEVT_RCVPUMP_STOP:
             result = p1;
             if (result >= 0)
                 etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s receive pump on thread %d exited\n", 
                    cxstr, (int) (size_t) p2);
             else
                 etchlog(ETCHCONX, ETCHLOG_ERROR, "%s receive pump abnormal exit\n", cxstr);
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s destroying accepted connection\n", cxstr);                    
             break;

        case ETCH_CONXEVT_ACCEPTING:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s accepting ...\n", cxstr);
             break;

        case ETCH_CONXEVT_ACCEPTERR:
             result = -1;
             apr_strerror((apr_status_t)(size_t)p2, estr, 128);
             etchlog(ETCHCONX, ETCHLOG_ERROR, "%s apr_socket_accept() %s\n", cxstr, estr);
             break;

        case ETCH_CONXEVT_ACCEPTED:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s accepted\n", cxstr);
             break;

        case ETCH_CONXEVT_CREATED:
             if (p1)
                 etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s created for socket %x\n", cxstr, p1);
             else
                 etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s created\n", cxstr);
             break;

        case ETCH_CONXEVT_CREATERR:
             etchlog(ETCHCONX, ETCHLOG_ERROR, "%s not created\n", cxstr);
             break;

        case ETCH_CONXEVT_OPENING:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s opening ...\n", cxstr);
             break;

        case ETCH_CONXEVT_OPENERR:
             result = -1;

             switch(p1)
             {  case 0:
                    etchlog(ETCHCONX, ETCHLOG_ERROR, "%s not opened\n", cxstr);
                    break;
                case 2:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    etchlog(ETCHCONX, ETCHLOG_WARNING,
                         "%s socket connect: %s\n", cxstr, estr);
                    break;
                case 3:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    etchlog(ETCHCONX, ETCHLOG_ERROR, 
                        "%s socket create: %s\n", cxstr, estr);
                    break;
                case 4:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    etchlog(ETCHCONX, ETCHLOG_ERROR, 
                        "%s sockaddr info: %s\n", cxstr, estr);
                    break;
                case 5:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    etchlog(ETCHCONX, ETCHLOG_ERROR, 
                        "%s socket bind: %s\n", cxstr, estr);
                    break;
                case 6:
                    apr_strerror((apr_status_t)(size_t)p2, estr, 128);
                    etchlog(ETCHCONX, ETCHLOG_ERROR, 
                        "%s socket listen: %s\n", cxstr, estr);
                    break;
             }
             break;

        case ETCH_CONXEVT_OPENED:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s opened\n", cxstr);
             break;

        case ETCH_CONXEVT_STARTING:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s starting ...\n", cxstr);
             break;

        case ETCH_CONXEVT_STARTERR:
             result = -1;
             switch(p1)
             {  case 0:
                    etchlog(ETCHCONX, ETCHLOG_ERROR, "%s not started\n", cxstr);
                    break;
                case 1:                     
                    etchlog(ETCHCONX, ETCHLOG_ERROR,
                         "%s etch_threadpool.run()\n", cxstr);
                    break;
             }
             break;

        case ETCH_CONXEVT_STARTED:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s started\n", cxstr);
             break;

        case ETCH_CONXEVT_ACCEPTPUMPEXIT:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s accept pump on thread %d exited\n", cxstr, p1);
             break;

        case ETCH_CONXEVT_ACCEPTPUMPEXITERR:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s accept pump abnormal exit\n", cxstr);
             break;

        case ETCH_CONXEVT_LISTENED:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s server id is %d\n", cxstr, p1);
             break;

        case ETCH_CONXEVT_UP:
        case ETCH_CONXEVT_DOWN:
             break;

        case ETCH_CONXEVT_SOCKOPTERR:
             result = 1; /* to increment counter */
             apr_strerror(p1, estr, 128);
             etchlog(ETCHCONX, ETCHLOG_ERROR, 
                "%s set socket option %s: %s\n", cxstr, p2, estr);
             break;

        case ETCH_CONXEVT_SHUTDOWN:
             etchlog(ETCHCONX, ETCHLOG_WARNING, "%s shutdown request detected\n", cxstr);
             break;

        case ETCH_CONXEVT_STOPPING:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s stopping ...\n", cxstr);
             break;

        case ETCH_CONXEVT_STOPERR:
             result = -1;
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s not stopped\n", cxstr);
             break;

        case ETCH_CONXEVT_STOPPED:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s stopped\n", cxstr);
             break;

        case ETCH_CONXEVT_CLOSING:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s closing ...\n", cxstr);
             break;

        case ETCH_CONXEVT_CLOSERR:
             result = -1;

             switch(p1)
             {  case 0:
                    etchlog(ETCHCONX, ETCHLOG_ERROR, "%s not closed\n", cxstr);
                    break;
                case 1:
                    etchlog(ETCHCONX, ETCHLOG_ERROR, "%s close when not open\n", cxstr);
                    break;
                case 2:
                    etchlog(ETCHCONX, ETCHLOG_ERROR, "%s concurrent close denied\n", cxstr);
                    break;
                case 3:
                    etchlog(ETCHCONX, ETCHLOG_ERROR, 
                        "%s apr_socket_close() error %d\n", cxstr, (int)(size_t)p2);
                    break;
             }
             break;

        case ETCH_CONXEVT_CLOSED:    
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s closed\n", cxstr);
             break;

        case ETCH_CONXEVT_DESTROYING:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s destroying ...\n", cxstr);
             break;

        case ETCH_CONXEVT_DESTROYED:
             etchlog(ETCHCONX, ETCHLOG_DEBUG, "%s destroyed\n", cxstr);
             break;
    }

    return result;
}

