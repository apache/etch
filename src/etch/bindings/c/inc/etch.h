/*
 * etch.h -- includes common to the entire etch C binding
 * Note that all signatures are assumed to have __cdecl calling convention.
 */

#ifndef ETCH_H
#define ETCH_H

#undef  IS_WINDOWS_ETCH  
#ifdef  WIN32
#define IS_WINDOWS_ETCH 32
#endif  /* WIN32 */

#include "etchwarn.h"
#include <tchar.h>       /* everything is unicode */
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "etchdefs.h"    /* constants, #defines, typedefs, etc. */
#include "objtypes.h"    /* internal etch object type constants */

unsigned etchhash(const void* key, const int keylen, const unsigned priorhash);  

typedef int (*etch_comparator) (void* myobj, void* otherobj);


#endif /* #ifndef ETCH_H */