/**
 * etch_global.h -- logically global methods, "static" methods, etc.
 * methods which can logically be executed from any context.
 */

#ifndef ETCHGLOBAL_H
#define ETCHGLOBAL_H

#include "etchhash.h"
#include "etchmem.h"
#include "etchrun.h"


/**
 * etch_init()
 * global etch runtime startup initialization
 * instantiates the global cache
 */
int etch_init();


/**
 * etch_runtime_cleanup()
 * global etch runtime cleanup
 * clears and frees the memory tracking table, and finally the global cache
 */
int etch_runtime_cleanup(int n, int m);


/**
 * global memory tracking table checkpoint
 * clears all entries from the memory tracking table but leaves table intact.
 * we would use this for example between unit tests which had memory leaks,
 * but we did not want to carry forward the leaks to the next tests.
 */
int memtable_clear();


/**
 * cached_etchobject_vtable
 * static cache vtable for etchobject in order to avoid global cache lookup
 */
void* cached_etchobject_vtable;

#endif /* #ifndef ETCHGLOBAL_H */ 