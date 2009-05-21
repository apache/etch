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
 * etch_syncobj.c
 * constructors for synchronized objects
 * we separate construction of objects which instantiate mutexes
 * in order to avoid circular header references in collections 
 */

#include "etchthread.h"
#include "etchmutex.h"
#include "etch_syncobj.h"
#include "etch_global.h"
#include "etchlog.h"


/**
 * new_synchronized_hashtable()
 */
etch_hashtable* new_synchronized_hashtable(const int initialsize)
{
    etch_hashtable* hashtable = NULL;
    if (NULL == etch_apr_mempool) etch_apr_init();
    if (NULL == etch_apr_mempool) return NULL;

    hashtable = new_hashtable(initialsize);
    hashtable->synclock = new_mutex(etch_apr_mempool, ETCHMUTEX_NESTED); 
    hashtable->synchook = hashtable_defsynchook;
    return hashtable;
}


/**
 * new_synchronized_arraylist()
 */
etch_arraylist* new_synchronized_arraylist(const int initialsize, const int deltasize)
{
    etch_arraylist* arraylist = NULL;
    if (NULL == etch_apr_mempool) etch_apr_init();
    if (NULL == etch_apr_mempool) return NULL;

    arraylist = new_arraylist(initialsize, deltasize);
    arraylist->synclock = new_mutex(etch_apr_mempool, ETCHMUTEX_NESTED); 
    arraylist->synchook = etchmutex_hookproc;
    return arraylist;
}


/**
 * new_synchronized_arraylist_from()
 */
etch_arraylist* new_synchronized_arraylist_from(etch_arraylist* thatlist)
{
    etch_arraylist* thislist
     = new_synchronized_arraylist(thatlist->size, thatlist->delta);

    arraylist_copyattrs(thislist, thatlist);

    memcpy(thislist->base, thatlist->base, thatlist->count * sizeof(void*));
    thislist->count = thatlist->count;

    return thislist;
}



