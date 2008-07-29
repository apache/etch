/**
   hashfunc.c -- implementation of a hash function.
   We use the hash function from the Jenkins hashtable.
*/

#include "etch.h"
#include "..\..\3rdparty\jenkins\jenkhash\jenklook.h"


/**
 * etchhash -- global method to hash an arbitrary byte string to 32 bits.
 * Note that keylen is the key byte length, not string length, as these of
 * course differ for unicode.
 * priorhash is result of the previous operation, or any arbitrary value --
 * see Jenkin's comments below for an example.
 * Returns hash of the supplied key, as used by the jenkins hashtable, 
 * or zero if parameters were in error.
 *
 * Jenkins' comments: If you need less than 32 bits, use a bitmask.  
 * For example, if you need only 10 bits, do h = (h & hashmask(10)),
 * in which case, the hash table should have hashsize(10) elements.
 * If you are hashing n strings (unsigned char**)k, do it like this:
 * for (i=0, h=0; i < n; ++i) h = lookup(k[i], len[i], h);   
*/
unsigned __cdecl etchhash(const void* pkey, const int keylen, const unsigned priorhash)  
{                                 /* jenkhash.lib */
    if (!pkey || keylen < 1 || keylen > MAX_ETCHHASHKEY) return 0;    
    return lookup((ub1*)pkey, keylen, priorhash); 
}

