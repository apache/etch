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
 * etch_stop.c
 */

#include <stdio.h>
#include <conio.h>
#include <apr_general.h>
#include <apr_network_io.h>
#include <apr_strings.h>
#define ETCHSTOP_SOCK_TIMEOUT (APR_USEC_PER_SEC * 5) 
int waitforkey(const int, const int);
  
 
/**
 * main   
 */
int main(int argc, char * argv[])
{
    apr_sockaddr_t *sa = 0;
    apr_socket_t *s = 0;
    apr_pool_t *mp = 0;
    int arc = 0, result = -1;
    const char* DATALENGTHONE = "$";  /* server used to assume that payload length 1 means shutdown */
    const char* OTHERDATA  = "$BOGUSDATA";
    const char* ETCH_SHUTDOWNSIGNAL = "$ETCHQUIT";  /* server now recognizes this string defined in etchdefs.h */
    const int   IS_WAITKEY = TRUE;

    const char* SERVERIP   = "127.0.0.1";
    const int   SERVERPORT = 4004;

    char* serverdata = (char*) ETCH_SHUTDOWNSIGNAL;  /* the string for this compile */
    apr_size_t datalen = strlen(serverdata);

    do
    {   if (0 != (arc = apr_initialize())) break;
        if (0 != (arc = apr_pool_create(&mp, NULL))) break;

        if (0 != (arc = apr_sockaddr_info_get(&sa, SERVERIP, APR_INET, SERVERPORT, 0, mp)))
        {   printf("STOP socket info request failed\n");
            break;
        }
        
        if (0 != (arc = apr_socket_create(&s, sa->family, SOCK_STREAM, APR_PROTO_TCP, mp)))
        {   printf("STOP socket create request failed\n");
            break;
        }

        apr_socket_opt_set(s, APR_SO_NONBLOCK, FALSE);
        apr_socket_timeout_set(s, ETCHSTOP_SOCK_TIMEOUT);  

        if (0 != (arc = apr_socket_connect(s, sa)))
        {   printf("STOP could not connect to %s:%d\n", SERVERIP, SERVERPORT);
            break;
        }

        printf("STOP connected to %s:%d\n", SERVERIP, SERVERPORT);

        if (0 !=  (arc = apr_socket_send(s, serverdata, &datalen)))
        {   printf("STOP could not transmit to server\n");
            break;
        }

        printf("STOP sent %d-byte request %s to server\n", datalen, serverdata);
        result = 0;

    } while(0);

    if (s) apr_socket_close(s);
    apr_terminate();

    return waitforkey(IS_WAITKEY, result);
}


int waitforkey(const int is_waitkey_enabled, const int result)
{
    if (is_waitkey_enabled)
    {   printf("any key ..."); 
        while(!_getch());
        printf("\n");
    }
    return result;
}