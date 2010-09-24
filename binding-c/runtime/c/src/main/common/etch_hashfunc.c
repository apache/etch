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
 * hashfunc.c -- implementation of a hash function.
 * we use the hash function from the jenkins hashtable.
 */

#include "etch.h"
#include "jenklook.h"

#define ETCH_HASH_MAX_KEYLENGTH 1024 /* arbitrary max byte length of a hashtable key */

/**
 * etchhash -- global method to hash an arbitrary byte string to 32 bits.
 * note that keylen is the key byte length, not string length, as these of
 * course differ for unicode.
 * priorhash is result of the previous operation, or any arbitrary value --
 * see jenkin's comments below for an example.
 * returns hash of the supplied key, as used by the jenkins hashtable, 
 * or zero if parameters were in error.
 *
 * jenkins' comments: if you need less than 32 bits, use a bitmask.  
 * for example, if you need only 10 bits, do h = (h & hashmask(10)),
 * in which case, the hash table should have hashsize(10) elements.
 * if you are hashing n strings (unsigned char**)k, do it like this:
 * for (i=0, h=0; i < n; ++i) h = lookup(k[i], len[i], h);   
*/
uint32 etchhash(const void* pkey, const int keylen, const unsigned priorhash)  
{                                 /* jenkhash.lib */
    if (!pkey || keylen < 1 || keylen > ETCH_HASH_MAX_KEYLENGTH) return 0;    
    return lookup((ub1*)pkey, keylen, priorhash); 
}

