/**
 * etch_global.c -- logically global methods, "static" methods, etc.
 * methods which can logically be executed from any context.
 */

#include "etch_global.h"


/**
 * etch_init()
 * global etch runtime startup initialization
 */
int etch_init()
{
   etch_hashtable* p = cache_create();
   return p? 0: -1;
}


/**
 * etch_runtime_cleanup()
 * global etch runtime cleanup
 * clears and frees the memory tracking table, and finally the runtime cache
 */
int etch_runtime_cleanup(int n, int m)
{
    is_memtable_instance = TRUE;
    destroy_hashtable(memtable, TRUE, TRUE);
    memtable = NULL;
    is_memtable_instance = FALSE;
    cache_destroy();
    return 0;
}


/**
 * global memory tracking table checkpoint
 * clears all entries from the memory tracking table but leaves table intact.
 * we would use this for example between unit tests which uncovered memory leaks,
 * but we did not want to carry forward the leaks to the next tests.
 */
int memtable_clear()
{
    is_memtable_instance = TRUE;
    memtable->vtab->clear(memtable->realtable, TRUE, TRUE, 0, 0); 
    is_memtable_instance = FALSE;
    memtable_allocated = 0;
    return 0;
}

