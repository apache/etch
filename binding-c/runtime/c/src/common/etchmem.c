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
 * etchmem.c -- heap memory allocate and free.
 * the c binding wraps the heap allocator in order to track allocations. 
 * we supply the etch_malloc macro which, when ETCH_DEBUGALLOC is defined, 
 * will accept module name and code line number, along with object type and
 * allocation byte length, in order to track allocations and frees, and thus 
 * enable identification of memory leaks. 
 */

#include "etch_global.h"
#include "etchlog.h"
char* ETCHMEMX = "MEMX";

#ifdef ETCH_DEBUGALLOC  /* defined, or not, in etchmem.h */

#pragma warning (disable:4313) /* display of ptr as %08x warning */

memrec* debug_find_memrec(void* p, char* file, const short line);


/*
 * find_module_name()
 * private method which looks up a code module name and if not present,
 * adds it to the filepath cache. a hash of the code file name is returned, 
 * or zero if error.
 */
unsigned find_module_name(char* name)  /* __FILE__ is not unicode */
{
    unsigned hash = 0;
    char* namefound = NULL;
    etch_hashitem  hashbucket; 
    etch_hashitem* thisitem = &hashbucket;
    memset(thisitem, 0, sizeof(etch_hashitem));

    /* look up name in cache. NULL is returned because the value is the key */
    cache_findx(name, &thisitem);
    if (thisitem->key) return thisitem->hash;

    /* insert name into cache. no value needed, name key is value. */
    hash = cache_insertx (name, NULL, FALSE);
    return hash;
}


/*
 * instantiate_memtable()
 * create the singleton memory allocation tracking hashtable. 
 */
void instantiate_memtable()
{
    is_memtable_instance = TRUE; /* flag to not debug_malloc() this allocation */
    memtable = new_hashtable(DEFETCHHEAPTABLESIZE);
    is_memtable_instance = FALSE;

    memtable->is_readonly_keys  = memtable->is_readonly_values = FALSE;
    memtable->is_tracked_memory = FALSE;
}


/*
 * showentry()
 * display contents of a memory allocation table entry. 
 * free the table entry if requested.
 */
void showentry(void** pkey, memrec* memtblentry, 
    const int is_freeitem, const int is_console)
{
    char* filepath = NULL;
    etch_hashitem  hashbucket; 
    etch_hashitem* thisitem = &hashbucket;
    memset(thisitem, 0, sizeof(etch_hashitem));

    /* look up key by its hash value. value of key is a source file path */
    cache_find_by_hash(memtblentry->filehash, &thisitem);
    filepath = thisitem->key;

    if (NULL == filepath)
        if (is_console)
            etchlog (ETCHMEMX, ETCHLOG_ERROR, 
                "etch cache error - no path with hashkey %u cached\n", 
                 memtblentry->filehash);
            /* printf("\nMEMX etch cache error - no path with hashkey %u cached\n", 
                memtblentry->filehash); 
             */
        else;
    else
    if (is_console)
        #if IS_USING_ALLOC_ID
        etchlog (ETCHMEMX, ETCHLOG_DEBUG, 
           "%s line %d serial %d size %d at %08x\n", filepath, 
            memtblentry->line, memtblentry->id, memtblentry->size, *pkey);
        /* printf("\n%s line %d serial %d size %d at %08x\n", filepath, 
               memtblentry->line, memtblentry->id, memtblentry->size, *pkey);
         */
        #else  /* IS_USING_ALLOC_ID */
         etchlog (ETCHMEMX, ETCHLOG_DEBUG, 
              "%s line %d obj %d size %d at %08x\n", filepath, 
               memtblentry->line, memtblentry->objtype, memtblentry->size, *pkey);
        /* printf("\n%s line %d obj %d size %d at %08x\n", filepath, 
               memtblentry->line, memtblentry->objtype, memtblentry->size, *pkey);
         */
        #endif /* IS_USING_ALLOC_ID */

    /* use is_freeitem with care, what we are saying here is show me what remains
     * allocated, and if there are allocations remaining, clean them up, i.e. they
     * are memory leaks. both the tracked etch_malloc allocation, and its memtable
     * bucket, are freed when this is the case.
     */
    if (is_freeitem)  
    {
        etch_free(*pkey);
    }
}


/*
 * debug_showmem()
 * show debug_malloc()'ed heap allocations outstanding. 
 * returns number of bytes currently allocated. is_freeitem specifies that we
 * should free each allocation remaining. is_console asks for stdout display
 * of statistics including source file and line number of each allocation.
 */
int debug_showmem(const int is_freeitem, const int is_console)
{
    int leaks = 0, count = 0;
    size_t allocated_bytes;
    etch_hashitem  hashbucket;
    etch_hashitem* thisentry = &hashbucket;
    if (!memtable) return 0; 
    allocated_bytes = etchheap_currbytes;

    if (is_console)
    {
       etchlog (ETCHMEMX, ETCHLOG_DEBUG,
           "etch heap hiwater count %u bytes %u\n", etchheap_hiwater, etchheap_maxbytes);
        etchlog (ETCHMEMX, ETCHLOG_DEBUG, 
           "%d bytes remain allocated on etch heap\n", allocated_bytes);
    }

    count = memtable->vtab->count(memtable->realtable, 0, 0);
    if  (0 == count) return 0;
                                      
    if  (0 == memtable->vtab->first(memtable->realtable, NULL, &thisentry))   
    {     
         showentry((void**)thisentry->key, (memrec*)thisentry->value, 
                    is_freeitem, is_console);
         leaks = 1;
    }
    else 
    if (is_console) 
        etchlog (ETCHMEMX, ETCHLOG_ERROR, "etch hashtable.first() failed\n");
                                     
    while(0 == memtable->vtab->next(memtable->realtable, NULL, &thisentry))          
    {      
          showentry((void**)thisentry->key, (memrec*)thisentry->value, 
                     is_freeitem, is_console);
          leaks++;
    }

    if (is_console)
    {
       if  (is_freeitem)
             etchlog (ETCHMEMX, ETCHLOG_ERROR,
                "%d etch heap allocations leaked %d bytes of which %d remain\n",
                 leaks, allocated_bytes, etchheap_currbytes); 
        else etchlog (ETCHMEMX, ETCHLOG_ERROR,
                "%d etch heap allocations totaling %d bytes remain\n", 
                 leaks, allocated_bytes);
    }      

    return (int)etchheap_currbytes;
}


/*
 * debug_dumpmem()
 * list contents of tracking table 
 */
void debug_dumpmem()
{
   etch_hashitem  hashbucket;
   etch_hashitem* myentry = &hashbucket;
   if (!memtable) return;
   #pragma warning (disable:4311)

    if  (0 == memtable->vtab->first(memtable->realtable, NULL, &myentry))   
         etchlog (ETCHMEMX, ETCHLOG_DEBUG,
            "%x: %x\n", *(unsigned*)myentry->key, (unsigned)myentry->value);
    else etchlog (ETCHMEMX, ETCHLOG_ERROR,"etch memtable.first() failed\n");  
        
    /*
    if  (0 == memtable->vtab->first(memtable->realtable, NULL, &myentry))   
         printf("\n%x: %x\n", *(unsigned*)myentry->key, (unsigned)myentry->value);
    else printf("\nMEMX etch memtable.first() failed\n");  
    */
     
    while(0 == memtable->vtab->next(memtable->realtable, NULL, &myentry))   
         etchlog (ETCHMEMX, ETCHLOG_DEBUG, 
            "%x: %x\n", *(unsigned*)myentry->key, (unsigned)myentry->value);      
         /* printf("%x: %x\n", *(unsigned*)myentry->key, (unsigned)myentry->value); */
}


/*
 * next_alloc_id: return memory allocation serial number 
 */
unsigned next_alloc_id() { return ++curr_alloc_id; }



/**
 *  debug_malloc()
 *  tracks mallocs by entering the malloc'ed object info, code file, and code line,
 *  into a tracking table. This is a private method -- it should not be invoked
 *  directly, but rather via the etch_malloc macro, when ETCH_DEBUGALLOC is defined.
 */
void* debug_malloc(size_t nbytes, const short objtype, char* file, const short line)
{                                       
    char *pkey = 0, breakpoint = 0;  /* __FILE__ is not unicode */
    int   hashresult  = 0;
    unsigned pathhash = 0;
    memrec* rec = 0;
    void* p = NULL; 
    if (nbytes <= 0) return NULL; 
    p = malloc(nbytes);  /* complete user's malloc request at least */

    pathhash = find_module_name(file);  /* cache source file name */
    if (pathhash == 0) return p; 

    if (!memtable)  /* lazy load the tracking table */  
        instantiate_memtable();
    
    rec = malloc(sizeof(memrec));  /* don't track tracking object allocations */
    rec->size     = nbytes;
    rec->objtype  = objtype;
    rec->line     = line;
    rec->filehash = pathhash;

    /* we can watch for a specific allocation sequence number and break into the
     * debugger here when it occurs. after an execution we would call debug_showmem()
     * to display leaked allocations and their sequence numbers. all the etch unit
     * tests do so after each test, see etch_showmem(). on a subsequent execution,
     * we would compile in a set of the global etch_watch_id to that sequence number, 
     * ( preferably in main(), for example, etch_watch_id = 127; ) and rerun.
     * we can set a breakpoint below, or compile in IS_USING_DEBUGBREAK, to break
     * into the debugger at the desired memory allocation. we can then examine the
     * call stack to determine the source of that particular and presumably leaked
     * memory allocation.
     */
    #if IS_USING_ALLOC_ID
    rec->id = next_alloc_id();
    #if IS_TRACKING_ETCHHEAP
    if (etch_watch_id == rec->id)          /* * * * *  watch breakpoint  * * * * */
    #if IS_USING_DEBUGBREAK
        __debugbreak();
    #else
        breakpoint++;  /* set breakpoint here when watching an allocation number */
    #endif  /* IS_USING_DEBUGBREAK */
    #endif  /* #if IS_TRACKING_ETCHHEAP */
    #endif  /* #if IS_USING_ALLOC_ID */

    /* the key for our tracking record key/value pair, is the pointer to heap   
     * memory returned by malloc; i.e. we build a map keyed on memory addresses. 
     * the map therefore stores a void**, a pointer to the pointer to the original
     * malloc. debug_malloc() also allocates 4 bytes for the hashkey, which is 
     * eventually freed by debug_free(). 
     */
    pkey = malloc(sizeof(void*));    /* allocate memory for the hashkey */
    memcpy(pkey, &p, sizeof(void*)); /* copy user's mem* into that key */ 

    #if IS_TRACKING_ETCHHEAP
    if (etch_watch_addr == (size_t) pkey)          
    #if IS_USING_DEBUGBREAK
        __debugbreak();
    #else
        breakpoint++;  /* set breakpoint here when watching for a memory address */
    #endif  /* IS_USING_DEBUGBREAK */
    #endif  /* #if IS_TRACKING_ETCHHEAP */
    
    /* insert this allocation record into the tracking map */
    hashresult = memtable->vtab->insert(memtable->realtable, 
         pkey, sizeof(void*), rec, sizeof(memrec), 0, 0); 
    
    if  (hashresult == -1) 
         etchlog(ETCHMEMX, ETCHLOG_ERROR, 
             "error inserting entry to heap tracking store\n");  
    #if(0)
    else etchlog(ETCHMEMX, ETCHLOG_XDEBUG,"memtbl inserted key '%08x' obj %d size %d\n", 
           (unsigned)p, objtype, nbytes);
    /*
    else printf("\nMEMX memtbl inserted key '%08x' obj %d size %d\n", 
           (unsigned)p, objtype, nbytes);
     */
    #endif
      
    etchheap_currbytes += nbytes;
    if (etchheap_currbytes > etchheap_maxbytes) etchheap_maxbytes = etchheap_currbytes;
    if (++etchheap_count > etchheap_hiwater) etchheap_hiwater = etchheap_count;
    return p;
}


/**
 *  debug_realloc()
 *  tracks mallocs by entering the malloc'ed object info, code file, and code line,
 *  into a tracking table. This is a private method -- it should not be invoked
 *  directly, but rather via the etch_malloc macro, when ETCH_DEBUGALLOC is defined.
 */
void* debug_realloc(void* p, size_t nbytes, const short objtype, 
    char* file, const short line)
{
    void   *new_ptr = NULL;
    memrec *oldrec_ptr = NULL;

    if (p == NULL) /* acts as malloc */
        return debug_malloc(nbytes, objtype, file, line);

    if (nbytes == 0) /* acts as free */
    {
        debug_free(p, file, line);
        return NULL;
    }

    oldrec_ptr = debug_find_memrec(p, file, line);
    ETCH_ASSERT(oldrec_ptr != NULL);
    if (oldrec_ptr == NULL) return NULL;

    new_ptr = debug_malloc(nbytes, objtype, file, line);
    if (new_ptr == NULL) return NULL; /* per realloc spec */

    memcpy(new_ptr, p, min(nbytes, oldrec_ptr->size));

    debug_free(p, file, line);
   
    return new_ptr;
}


/**
 *  debug_free()
 *  tracks free()s by looking up the associated malloc() record and removing it
 *  from the tracking table. this is a private method -- it should not be invoked
 *  directly, but rather via the etch_free macro, when ETCH_DEBUGALLOC is defined.
 */
int debug_free(void* p, char* file, const short line) /* __FILE__ not unicode */
{  
    int  hashresult = 0, buckresult = 0, breakpoint = 0;
    etch_hashitem  hashbucket;
    etch_hashitem* thisentry = &hashbucket;
    void*   this_key = 0;
    memrec* mrec = 0;
    if (!p) return -1;

    do 
    {   if (!memtable) break;

        #if IS_TRACKING_ETCHHEAP
        if (etch_watch_addr == (size_t) p)          
        #if IS_USING_DEBUGBREAK
            __debugbreak();
        #else
            breakpoint++;  /* set breakpoint here when watching for a memory address */
        #endif  /* IS_USING_DEBUGBREAK */
        #endif  /* #if IS_TRACKING_ETCHHEAP */
    
        /* first free the memory tracking table entry. recall that the key to the   
         * tracking record is the value of the heap address returned by malloc. 
         */
        hashresult = memtable->vtab->find   /* retrieve the map entry */
           (memtable->realtable, &p, sizeof(void*), NULL, &thisentry); 

        if  (hashresult < 0)    
        {    etchlog(ETCHMEMX, ETCHLOG_ERROR,            
                "etch heap tracking store missing key '%08x'\n", (unsigned)p);
             break;
        }

        this_key = thisentry->key;
        mrec = (memrec*) thisentry->value;

        #if IS_TRACKING_ETCHHEAP
        if (etch_watch_id == mrec->id)
            breakpoint++;  /* set breakpoint here when watching an alloc number */
        #endif /* #if IS_TRACKING_ETCHHEAP */       
         
        #if(0)
        printf("\nDFRE memtbl freeing key '%08x' obj %d size %d line %d file %s\n",   
              (unsigned)p, mrec->objtype, mrec->size, mrec->line, mrec->file); 
        #endif
         
        buckresult = memtable->vtab->remove    /* free the map bucket */
           (memtable->realtable, &p, sizeof(void*), NULL, &thisentry); 

        etchheap_count--;
        etchheap_currbytes -= mrec->size;
        free(mrec);   /* free memory for the tracking record */
        free(this_key);      /* free memory for the hashkey */

    } while(0);

    free(p);  /* finally free the actual object */
    return hashresult;
}


/** 
 * debug_find_memrec()
 * looks up the memory record in the map for specified address p. 
 * returns NULL if not found.
 */
memrec* debug_find_memrec(void* p, char* file, const short line)  
{  
    etch_hashitem  hashbucket, *thisentry = &hashbucket;
    memrec* this_memrec = NULL;

    if (!p || !memtable);
    else  
    if  (0 == memtable->vtab->find(memtable->realtable, &p, 
              sizeof(void*), NULL, &thisentry)) 
 
         this_memrec = (memrec*) thisentry->value;

    else etchlog(ETCHMEMX, ETCHLOG_ERROR, 
         "etch heap tracking store missing key '%08x'\n", (unsigned)p);  

    return this_memrec;
}


#endif /* #ifdef ETCH_DEBUGALLOC */