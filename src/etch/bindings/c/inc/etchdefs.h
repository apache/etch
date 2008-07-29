/* 
 * etchdefs.h -- type and constant definitions etc
 */

#ifndef ETCHDEFS_H
#define ETCHDEFS_H

typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned char byte;
typedef unsigned char boolean;
#undef  NULL
#define NULL  0
#undef  FALSE
#define FALSE 0
#undef  TRUE
#define TRUE  1
#undef  BOOLEAN
#define BOOLEAN int
#undef  BYTE


#ifdef  IS_WINDOWS_ETCH
#define MAXPATHSIZE 260  /* value of MAX_PATH in windef.h */
#else
#define MAXPATHSIZE 260  /* TODO max pathsize for linux etc */
#endif

#ifndef _TCHAR
#define _TCHAR wchar_t
#endif

#define MAX_ETCHHASHKEY 128 /* arbitrary max byte length of a hashtable key */

#define INITIAL_CACHE_SIZE_ITEMS 32  /* global cache starts out capacity 32 */

#define ETCH_ASSERT(condition) assert(condition)

#define TODO_OBJTYPE 0   /* todo - define all etch_malloc types and remove this */

#define ETCH_DEBUG 
#ifdef  ETCH_DEBUG
int g_debugtest;
#endif 


#endif /* #ifndef ETCHDEFS_H */ 