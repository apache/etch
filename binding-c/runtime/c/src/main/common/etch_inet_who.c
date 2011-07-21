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
 * etch_inet_who.c
 * inet who object
 */

#include "etch_inet_who.h"
#include "etch_log.h"
#include "etch_objecttypes.h"

static const char* LOG_CATEGORY = "etch_inet_who";


etch_inet_who* new_inet_who_init(apr_sockaddr_t*);

/**
 * new_inet_who()  
 * constructor for etch_inet_who
 * the value factory is a reference, the type is a copy to be passed through
 */
etch_inet_who* new_inet_who (apr_sockaddr_t* sockaddr)
{
    etch_inet_who* newinetwho = NULL;

	if (!sockaddr)
		return NULL;

    newinetwho = new_inet_who_init(sockaddr);
    if (!newinetwho)
		return NULL;

    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "creating inet who %x\n", newinetwho);
    return newinetwho;
}

/**
 * clone_inet_who()
 * create clone for etch_inet_who
 */ 
void* clone_inet_who(void* data)
{
	etch_inet_who* inetwho = (etch_inet_who*)data;
	etch_who* who = (etch_who*)(inetwho->object.parent);
	etch_inet_who* newinetwho = new_inet_who_init((apr_sockaddr_t*)(who->value));

	ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "cloning inet who %x to %x\n", inetwho, newinetwho);
	return newinetwho;
}

/**
 * destroy_inet_who()
 * destructor for etch_inet_who
 */ 
int destroy_inet_who (void* data) 
{
    etch_inet_who* inetwho = (etch_inet_who*)data;

    if (!is_etchobj_static_content(inetwho))
    {
		etch_who* who = (etch_who*)inetwho->object.parent;

        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "destroying inet who %x\n", inetwho);
		etch_free(who->value);
		who->value = NULL;
    }

    return destroy_objectex((etch_object*)inetwho);
}

/**
 * etch_inet_who defgethashkey
 * default hashkey computation for an etch_inet_who object
 */
uint32 defgethashkey_inet_who(void* data)
{
    etch_inet_who* inetwho = (etch_inet_who*)data;
	apr_sockaddr_t* hashitem = inet_who_sockaddr(inetwho); /* uses the socket address as hash source */

	ETCH_ASSERT(hashitem != NULL);

	if(inetwho->object.hashkey == 0)
        inetwho->object.hashkey = etchhash((char*)hashitem, sizeof(*hashitem), 0);

	return inetwho->object.hashkey;
}

/**
 * new_message_init() 
 * common initialization on etch_message construction.
 */
etch_inet_who* new_inet_who_init(apr_sockaddr_t* sockaddr)
{
    etch_inet_who* newinetwho = NULL;
	etch_object* parent = NULL;
	apr_sockaddr_t* newsockaddr = NULL;

	if (!sockaddr)
		goto _error;

	newsockaddr = (apr_sockaddr_t*)etch_malloc(sizeof(apr_sockaddr_t), 0);
	if (!newsockaddr)
		goto _error;
	*newsockaddr = *sockaddr;
	newsockaddr->pool = 0;

	parent = (etch_object*)new_who(newsockaddr);
	if (!parent)
		goto _error;

    newinetwho = (etch_inet_who*) new_object(sizeof(etch_inet_who), 
        ETCHTYPEB_INET_WHO, CLASSID_INET_WHO);
	if (!newinetwho)
		goto _error;

	newinetwho->object.parent = parent;
	newinetwho->object.clone = clone_inet_who;
	newinetwho->object.destroy = destroy_inet_who;
	newinetwho->object.get_hashkey = defgethashkey_inet_who;

    return newinetwho;

_error:
	if (parent)
		etch_object_destroy(parent);
	if (newsockaddr)
		etch_free(newsockaddr);

	return NULL;
}


/**
 * inet_who_sockaddr()
 * return sockaddr.
 * @return a reference to the who sockaddr, caller does not own it.
 */
apr_sockaddr_t* inet_who_sockaddr (etch_inet_who* inetwho)
{
	etch_who* who = (etch_who*)inetwho->object.parent;

	return (apr_sockaddr_t*)who->value;
}
