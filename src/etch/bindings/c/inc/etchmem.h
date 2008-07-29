/**
 * etchmem.h -- heap memory allocate and free.
 * The C binding wraps the heap allocator in order to track allocations. We supply 
 * the etch_malloc macro which, when LEAKTEST is defined,  will accept module name
 * and code line number, along with object type and allocation size, in order to
 * track heap allocations and frees, and to subsequently report memory leaks. 
 */

#ifndef ETCHMEM_H
#define ETCHMEM_H

#include "etch.h"
#include "etchhash.h"

#define LEAKTEST

/*
 * map etch_malloc() to the standard allocator or the debug allocator, 
 * depending on whether LEAKTEST is defined. Likewise etch_free().
 */
#ifdef  LEAKTEST

void* debug_malloc(size_t nbytes, const short objtype, char* file, const short line);
void* debug_realloc(void* p, size_t nbytes, const short objtype, char* file, const short line);
int   debug_free(void* p, char* file, const short line); /* __FILE__ is not unicode */ 
int   debug_showmem(const int is_freeitem, const int is_console);
void  debug_dumpmem();

#define etch_malloc(n,o) debug_malloc(n,o,__FILE__,(const short) __LINE__)
#define etch_realloc(p,n,o) debug_realloc(p,n,o,__FILE__,(const short) __LINE__)
#define etch_free(n) debug_free(n,__FILE__,(const short) __LINE__)
#define etch_showmem(f,c) debug_showmem(f,c) 
#define etch_dumpmem() debug_dumpmem()

#else   /* LEAKTEST */

#define etch_malloc(n,o) malloc(n)
#define etch_realloc(p,n,o) realloc(p,n)
#define etch_free(n) free(n)
#define etch_showmem(f,c)  
#define etch_dumpmem()

#endif  /* LEAKTEST */



#ifdef LEAKTEST

#define MAXMODNAMELEN (MAXPATHSIZE-1) /* max length of a file name */
#define MAXMODULES   100  /* max number of tracked code modules */
#define DEFTABLESIZE 512  /* initial size of tracking table */

/*
 * frec is a node in a list of code module file names we maintain
 * when tracking memory allocations 
 */
struct frec
{
   char   name[MAXMODNAMELEN+1];  /* __FILE__ is not unicode */
   struct frec* next;  /* list link pointer */
}; 
typedef struct frec frec;


/*
 * memrec is a record of an individual memory allocation
 */
typedef struct memrec
{
  size_t size;
  short  objtype;
  short  line; 
  char*  file;  /* __FILE__ is not unicode */
} memrec;


/*
 * fnamelist is a cache of code module names. We'll do this as a list or hash,
 * table; however for the time being we're simply allocating a fixed number of
 * code file entries at compile time.
 */
frec fnamecache[MAXMODULES]; 
int  fnamecount; 


/*
 * memtable is the hash table used for heap allocation tracking.
 */
etch_hashtable* memtable;
int  is_memtable_instance;  /* indicator to not etch_malloc() the memtable */
size_t memtable_allocated;  /* total bytes currently etch_malloc()'ed */


#endif /* LEAKTEST */

#endif /* #ifndef ETCHMEM_H*/ 