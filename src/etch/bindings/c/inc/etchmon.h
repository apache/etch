/**
 * etchmon.h: monitor definition for thread synchronization.
 */

#ifndef ETCHMON_H
#define ETCHMON_H

#include <apr.h>
#include <apr_pools.h>
#include <apr_thread_mutex.h>
#include <apr_thread_cond.h>
#include <apr_errno.h>
#include <apr_thread_proc.h>
#include <apr_time.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ETCHMON_DESC_LEN        128

#define ETCHMON_STATUS_SUCCESS  0
#define ETCHMON_STATUS_FAILURE  1
#define ETCHMON_STATUS_TIMEOUT  2

typedef struct etchmon
{
    apr_thread_mutex_t  *mutex_ptr;        /* mutex for locking */
    apr_thread_cond_t   *cond_ptr;          /* condition variable for signaling */
    apr_pool_t          *pool_ptr;          /* memory pool for this monitro */

    char                desc[ETCHMON_DESC_LEN]; /* description of this monitor */
    union val_t  /* map storage for monitor value or pointer to value */
    {
        unsigned char value_byte;
        unsigned char value_bool;
        short         value_int16;
        int           value_int32;
        /*int64         value_int64;*/    
        float         value_ieee32;
        double        value_ieee64;
        void*         value_ptr_to; /* allocated object e.g. string */           
    };
    union val_t     val;
    size_t          val_size;       /* size of the value if the value is a pointer to the real value. otherwise 0 */
} etchmon;

/*etchmon* etchmon_create(char *descr, int64 val, apr_pool_t *pool_ptr);*/
/*etchmon* etchmon_create(char *descr, unsigned char val, apr_pool_t *pool_ptr);
etchmon* etchmon_create(char *descr, short val, apr_pool_t *pool_ptr);
etchmon* etchmon_create(char *descr, int val, apr_pool_t *pool_ptr);

etchmon* etchmon_create(char *descr, float val, apr_pool_t *pool_ptr);
etchmon* etchmon_create(char *descr, double val, apr_pool_t *pool_ptr);
*/
etchmon* etchmon_create(char *descr, void *val_ptr, size_t size, apr_pool_t *pool_ptr);

/*bool etchmon_set(etchmon*, int64 val);*/
/*
int etchmon_set(etchmon*, unsigned char val);
int etchmon_set(etchmon*, short val);
int etchmon_set(etchmon*, int val);
int etchmon_set(etchmon*, float val);
int etchmon_set(etchmon*, double val);
*/
int etchmon_set(etchmon*, void *val_ptr, size_t size);

/* wait till the value is set for this monitor. */
int etchmon_wait_until_set(etchmon *mon_ptr, long max_wait); 

int etchmon_wait_until_equal(etchmon *mon_ptr, void *val_ptr, size_t size, long max_wait);

void etchmon_destroy(etchmon *mon_ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif