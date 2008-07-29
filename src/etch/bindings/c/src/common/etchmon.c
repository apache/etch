#include "etchmon.h"

/**
 * Create an monitor to be used for thread synchronization. Initialize the monitor to the given value.
 *
 * @param desc the description of this monitor.
 * @param val_ptr pointer to the initial value to be associated with this monitor.
 * @param val_size size of the initial data value.
 * @param pool_ptr the memory pool to be used for memory allocation.
 *
 * @return a monitor initialized or NULL if there is an error.
 *
 */
etchmon* etchmon_create(char *desc, void *val_ptr, size_t val_size, apr_pool_t *pool_ptr)
{
    etchmon *mon_ptr = NULL;
    apr_status_t r;

    mon_ptr = (etchmon*) apr_pcalloc(pool_ptr, sizeof(etchmon));
    if (mon_ptr == NULL)
    {
        return NULL;
    }

    mon_ptr->pool_ptr = pool_ptr;

    r = apr_thread_mutex_create(&mon_ptr->mutex_ptr, APR_THREAD_MUTEX_NESTED, pool_ptr);
    if (r != APR_SUCCESS)
    {
        /* log the error */
        
        /* return null */
        return NULL;
    }
    
    r = apr_thread_cond_create(&mon_ptr->cond_ptr, pool_ptr);
    if (r != APR_SUCCESS)
    {
        /* log the error */
        
        r = apr_thread_mutex_destroy(mon_ptr->mutex_ptr);
        if (r != APR_SUCCESS)   //something seriously wrong
        {
            /* log the error */
        }

        mon_ptr->mutex_ptr = NULL;
        return NULL;
    }
    
    if (desc != NULL)
        strncpy(mon_ptr->desc, desc, sizeof(mon_ptr->desc)-1);
        
    mon_ptr->val.value_ptr_to = apr_palloc(mon_ptr->pool_ptr, val_size);
    if (mon_ptr->val.value_ptr_to == NULL)
    {
        /* memory error, clean up and return */
        r = apr_thread_cond_destroy(mon_ptr->cond_ptr);
        r = apr_thread_mutex_destroy(mon_ptr->mutex_ptr);

        return NULL;
    }
    
    memcpy(mon_ptr->val.value_ptr_to, val_ptr, val_size);
    mon_ptr->val_size = val_size;

    return mon_ptr;
}

/**
 * Set the monitor to the given value.
 *
 * @param mon_ptr   the monitor to be set.
 * @param val the value for the monitor.
 *
 * @return true if successful.
 *
 */
#if 0
int etchmon_set(etchmon *mon_ptr, unsigned char val)
{
    int rc = ETCHMON_STATUS_FAILURE;

    apr_status_t r = apr_thread_mutex_lock(mon_ptr->mutex_ptr);
    if (r != APR_SUCCESS)
    {
        return rc;
    }

    mon_ptr->val.value_byte = val;

    r = apr_thread_cond_signal(mon_ptr->cond_ptr);
    if (r != APR_SUCCESS)
    {
        /* log the error */
    }
    else
        rc = ETCHMON_STATUS_SUCCESS;
    
    r = apr_thread_mutex_unlock(mon_ptr->mutex_ptr);
    if (r != APR_SUCCESS)
    {
        /* log error */
    }
    
    return rc;
}
#endif
/**
 * Set the monitor to the given value.
 *
 * @param mon_ptr   the monitor to be set.
 * @param val the value for the monitor.
 *
 * @return true if successful.
 *
 */
int etchmon_set(etchmon *mon_ptr, void *val_ptr, size_t size)
{
    int rc = ETCHMON_STATUS_FAILURE;

    apr_status_t r = apr_thread_mutex_lock(mon_ptr->mutex_ptr);
    if (r != APR_SUCCESS)
    {
        return rc;
    }
    
    mon_ptr->val.value_ptr_to = apr_palloc(mon_ptr->pool_ptr, size);
    if (mon_ptr->val.value_ptr_to != NULL)
    {
        memcpy(mon_ptr->val.value_ptr_to, val_ptr, size);
        mon_ptr->val_size = size;
    }
    else
    {
        /* memory allocation error, log it */
        r = apr_thread_mutex_unlock(mon_ptr->mutex_ptr);
        if (r != APR_SUCCESS)
        {
            /* log error */
        }

        return rc;
    }

    r = apr_thread_cond_signal(mon_ptr->cond_ptr);
    if (r != APR_SUCCESS)
    {
        /* log the error */
    }
    else
        rc = ETCHMON_STATUS_SUCCESS;
    
    r = apr_thread_mutex_unlock(mon_ptr->mutex_ptr);
    if (r != APR_SUCCESS)
    {
        /* log error */
    }
    
    return rc;
}

/**
 * Wait for the monitor to be set.
 *
 * @param mon_ptr the monitor to be waited.
 * @param max_wait the time(milli seconds) the caller is willing to wait. If it is 0, the caller will  
 *  wait forever.
 *
 * @return ETCHMON_STATUS_SUCCESS if monitor value is set within the wait period,
 *         ETCHMON_STATUS_TIMEOUT if time expires before the monitor is set,
 *          ETCHMON_STATUS_FAILURE if there is any error while waiting.
 *
 */
int etchmon_wait_until_set(etchmon *mon_ptr, long max_wait)
{
    int rc = ETCHMON_STATUS_FAILURE;

    apr_status_t r = apr_thread_mutex_lock(mon_ptr->mutex_ptr);
    if (r != APR_SUCCESS)
    {   
        /* log and return */
        return ETCHMON_STATUS_FAILURE;
    }

    if (max_wait == 0)  //wait forever
        r = apr_thread_cond_wait(mon_ptr->cond_ptr, mon_ptr->mutex_ptr);
    else
        r = apr_thread_cond_timedwait(mon_ptr->cond_ptr, mon_ptr->mutex_ptr, max_wait*1000);
    
    if (r == APR_SUCCESS)
    {
        r = apr_thread_mutex_unlock(mon_ptr->mutex_ptr);
        if (r != APR_SUCCESS)
        {
            rc = ETCHMON_STATUS_FAILURE;
        }
        else
            rc = ETCHMON_STATUS_SUCCESS;
    }
    else
    {
        if (r == APR_TIMEUP)
            rc = ETCHMON_STATUS_TIMEOUT;
    }

    return rc;
}

/**
 * Wait for the monitor to be set.
 *
 * @param mon_ptr the monitor to be waited.
 * @param val_ptr the pointer to the value to be compared against.
 * @param size the size of the value to be compared against.
 * @param max_wait the time(milli seconds) the caller is willing to wait. If it is 0, the caller will  
 *  wait forever.
 *
 * @return ETCHMON_STATUS_SUCCESS if monitor value is set within the wait period,
 *         ETCHMON_STATUS_TIMEOUT if time expires before the monitor is set,
 *          ETCHMON_STATUS_FAILURE if there is any error while waiting.
 *
 */
int etchmon_wait_until_equal(etchmon *mon_ptr, void *val_ptr, size_t size, long max_wait)
{
    apr_time_t end;
    int rc = ETCHMON_STATUS_TIMEOUT;    /* return code */
    int s;
    long wait;

    /* if caller wants to wait forever */
    if (max_wait == 0)
    {
        rc = ETCHMON_STATUS_FAILURE;
        while (etchmon_wait_until_set(mon_ptr, 0) == ETCHMON_STATUS_SUCCESS)
        {
            if (mon_ptr->val_size == size && memcmp(mon_ptr->val.value_ptr_to, val_ptr, size) == 0)
            {
                /* we got the value */
                rc = ETCHMON_STATUS_SUCCESS;
                break;
            }
        }
    }
    else /* wait with timeout */
    {
        end = apr_time_as_msec(apr_time_now()) + max_wait;
        while (apr_time_as_msec(apr_time_now()) < end)
        {
            wait = (long) (end - apr_time_as_msec(apr_time_now()));
            s = etchmon_wait_until_set(mon_ptr, wait);
            if (s == ETCHMON_STATUS_SUCCESS) /* the value is set, compare it against our target */
            {
                if (mon_ptr->val_size == size && memcmp(mon_ptr->val.value_ptr_to, val_ptr, size) == 0)
                {
                    /* we got the value */
                    rc = ETCHMON_STATUS_SUCCESS;
                    break;
                }
            }/* SUCCESS */
            else if (s == ETCHMON_STATUS_TIMEOUT)
            {
                /* continue to wait */
            }
            else
            {
                /* error */
                rc = ETCHMON_STATUS_FAILURE;
                break;
            }
        }/* while */
    }

    return rc;
}

/**
 * Destroy the monitor and free up all the resources.
 *
 * @param mon_ptr the monitor to be destoryed.
 *
 */
void etchmon_destroy(etchmon *mon_ptr)
{
    apr_status_t r = apr_thread_mutex_destroy(mon_ptr->mutex_ptr);
    if (r != APR_SUCCESS)
    {
    }
    else
        mon_ptr->mutex_ptr = NULL;

    r = apr_thread_cond_destroy(mon_ptr->cond_ptr);
    if (r != APR_SUCCESS)
    {
    }
    else
        mon_ptr->cond_ptr = NULL;
}
