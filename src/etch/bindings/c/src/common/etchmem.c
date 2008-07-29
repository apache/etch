/**
 * etchmem.c -- heap memory allocate and free.
 * The C binding wraps the heap allocator in order to track allocations. We supply 
 * the etch_malloc macro which, when LEAKTEST is defined, will accept module name
 * and code line number, along with object type and allocation size, in order to
 * track allocations and frees, and thus enable identification of memory leaks. 
 */
#include "etchmem.h"
#include "etchhash.h"
#include <stdio.h>

#ifdef LEAKTEST  /* defined, or not, in etchmem.h */

#pragma warning (disable:4313) /* display of ptr as %08x warning */

static memrec* debug_find_memrec(void* p, char* file, const short line);

/*
 * find_module_name is a private method which looks up a code module name and if 
 * not present, adds it to the fnamelist table. A reference to the code file name   
 * is returned, or NULL if error.
 */
char* find_module_name(char* name)  /* __FILE__ is not unicode */
{
    int i;
    size_t inlen;
    frec* thisrec = 0;
    if (name == NULL) return NULL;
    inlen = strlen(name); if (inlen > MAXMODNAMELEN) return NULL;  
    thisrec = fnamecache; 
    
    for(i=0; i < fnamecount; i++, thisrec++)
        if (0 == strcmp(thisrec->name, name))  
            return thisrec->name;

    if (fnamecount == MAXMODULES) return NULL;

    thisrec = &fnamecache[fnamecount];
    memset(thisrec, 0, sizeof(frec));
    strncpy_s(thisrec->name, MAXMODNAMELEN, name, MAXMODNAMELEN);
    fnamecount++; 
    /* printf("fncache: cached module %s\n", thisrec->name); */
    return thisrec->name;
}


/*
 * instantiate_memtable: create the singleton memory allocation tracking hashtable. 
 */
void instantiate_memtable()
{
    is_memtable_instance = TRUE; /* flag to not debug_malloc() this allocation */
    memtable = new_hashtable(DEFTABLESIZE);
    is_memtable_instance = FALSE;
}


/*
 * display contents of a memory allocation table entry. 
 * free the table entry if requested.
 */
void showentry(void** pkey, memrec* memtblentry, const int is_freeitem, const int is_console)
{
    if (is_console)
        printf("\n%s line %d obj %d size %d at %08x\n", memtblentry->file, 
               memtblentry->line, memtblentry->objtype, memtblentry->size, *pkey);

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
 * debug_showmem(): show debug_malloc()'ed heap allocations outstanding. 
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
    allocated_bytes = memtable_allocated;

    if (is_console)
        printf("\n\n%d total heap bytes currently allocated\n", allocated_bytes);

    count = memtable->vtab->count(memtable->realtable, 0, 0);
    if  (0 == count) return 0;
                                      
    if  (0 == memtable->vtab->first(memtable->realtable, NULL, &thisentry))   
    {     
         showentry((void**)thisentry->key, (memrec*)thisentry->value, is_freeitem, is_console);
         leaks = 1;
    }
    else 
    if (is_console) 
        printf("hashtable.first() failed\n");       
                                     
    while(0 == memtable->vtab->next(memtable->realtable, NULL, &thisentry))          
    {      
          showentry((void**)thisentry->key, (memrec*)thisentry->value, is_freeitem, is_console);
          leaks++;
    }

    if (is_console)
        if  (is_freeitem)
             printf("\n%d allocations leaked %d bytes of which %d remain\n",
                    leaks, allocated_bytes, memtable_allocated); 
        else printf("\n%d allocations totaling %d bytes remain\n", 
                    leaks, allocated_bytes);

    return (int)memtable_allocated;
}


/*
 * dumpmem: list contents of tracking table 
 */
void debug_dumpmem()
{
   etch_hashitem  hashbucket;
   etch_hashitem* myentry = &hashbucket;
   if (!memtable) return;
   #pragma warning (disable:4311)
        
    if  (0 == memtable->vtab->first(memtable->realtable, NULL, &myentry))   
         printf("\n%x: %x\n", *(unsigned*)myentry->key, (unsigned)myentry->value);
    else printf("memtable.first() failed\n");  
     
    while(0 == memtable->vtab->next(memtable->realtable, NULL, &myentry))          
         printf("%x: %x\n", *(unsigned*)myentry->key, (unsigned)myentry->value);
}


/**
 *  debug_malloc
 *  tracks mallocs by entering the malloc'ed object info, code file, and code line,
 *  into a tracking table. This is a private method -- it should not be invoked
 *  directly, but rather via the etch_malloc macro, when LEAKTEST is defined.
 */
void* debug_malloc(size_t nbytes, const short objtype, char* file, const short line)
{                                       
    char* namendx = 0, *phashkey = 0;   /* __FILE__ not unicode */
    int hashresult = 0;
    memrec* rec = 0;
    void* p = NULL; 
    if (nbytes <= 0) return NULL; 
    p = malloc(nbytes);  /* complete user's malloc request at least */

    namendx = find_module_name(file);    /* cache source file name */
    if (namendx < 0) return p;

    if (!memtable)  /* lazy load the tracking table */  
        instantiate_memtable();
    
    rec = malloc(sizeof(memrec));  /* don't track mallocs of the tracking objects */
    rec->size = nbytes;
    rec->objtype = objtype;
    rec->line = line;
    rec->file = namendx;

    /* The key for our tracking record key/value pair, is the pointer to heap   
     * memory returned by malloc; i.e. we build a table keyed on memory addresses. 
     * The hashtable therefore stores a void**, a pointer to the pointer to the
     * original malloc. We also allocate 4 bytes for the hashkey, however no one
     * sees this allocation but debug_malloc and debug_free. We cannot fool the
     * hashtable and avoid malloc'ing space for the key by storing the pointer
     * directly, because the hashtable dereferences keylen bytes at the key 
     * pointer to do the hash operation.
     */
    phashkey = malloc(sizeof(void*));    /* allocate memory for the hashkey */
    memcpy(phashkey, &p, sizeof(void*)); /* copy user's mem* into that key */ 
    
    /* Insert our malloc tracking item memrec into the table */
    hashresult = memtable->vtab->insert
      (memtable->realtable, phashkey, sizeof(void*), rec, sizeof(memrec), 0, 0); 

    
    if  (hashresult == 0)  /* debug statemements -- TODO remove */ 
    {  
         /* printf("OK memtbl inserted key '%08x' obj %d size %d\n", 
               (unsigned)p, objtype, nbytes);
         */
    }
    else printf("** memtbl error inserting hash entry at %s line %d\n", __FILE__,__LINE__);
      
    memtable_allocated += nbytes;
    return p;
}

/**
 *  debug_realloc
 *  tracks mallocs by entering the malloc'ed object info, code file, and code line,
 *  into a tracking table. This is a private method -- it should not be invoked
 *  directly, but rather via the etch_malloc macro, when LEAKTEST is defined.
 */
void* debug_realloc(void* p, size_t nbytes, const short objtype, char* file, const short line)
{
    void    *new_ptr = NULL;
    memrec  *oldrec_ptr = NULL;

    if (p == NULL) /*acts as malloc*/
        return debug_malloc(nbytes, objtype, file, line);
    if (nbytes == 0) /*acts as free*/
    {
        debug_free(p, file, line);
        return NULL;
    }

    oldrec_ptr = debug_find_memrec(p, file, line);
    if (oldrec_ptr == NULL)
    {
        printf("the passed in pointer isn't valid for realloc.");
        return NULL;
    }

    new_ptr = debug_malloc(nbytes, objtype, file, line);
    if (new_ptr == NULL) /* based on spec, we should not free the old pointer */
        return NULL;

    /* now we got the memory, need to copy the old contents over */
    memcpy(new_ptr, p, min(nbytes, oldrec_ptr->size));

    if (debug_free(p, file, line) != 0) /* failed to free the old memory */
    {
        /* not sure what to do here other than logging a message */
        printf("Failed to free old memory when realloc.");
    }

    return new_ptr;
}

/**
 *  debug_free
 *  tracks free()s by looking up the associated malloc() record and removing it
 *  from the tracking table. This is a private method -- it should not be invoked
 *  directly, but rather via the etch_free macro, when LEAKTEST is defined.
 */
int debug_free(void* p, char* file, const short line) /* __FILE__ not unicode */
{  
    int  hashresult = 0, buckresult = 0;
    etch_hashitem  hashbucket;
    etch_hashitem* thisentry = &hashbucket;
    void*   this_key = 0;
    memrec* this_memrec = 0;
    if (!p) return -1;

    do 
    {   if (!memtable) break;

        /* first free the memory tracking table entry. recall that the key to the   
         * tracking record is the value of the heap pointer returned by malloc 
         */
        hashresult = memtable->vtab->find   /* retrieve the hashtable entry */
           (memtable->realtable, &p, sizeof(void*), NULL, &thisentry); 

        if  (hashresult < 0)    
        {    printf("** memtbl missing key '%08x' at %s line %d\n", (unsigned)p, __FILE__,__LINE__);
             break;
        }

        this_key = thisentry->key;
        this_memrec = (memrec*) thisentry->value;

        /*
        printf("OK memtbl found key '%08x' obj %d size %d\n",  
              (unsigned)p, this_memrec->objtype, this_memrec->size);
        */

        buckresult = memtable->vtab->remove    /* free the hashtable bucket */
           (memtable->realtable, &p, sizeof(void*), NULL, &thisentry); 

        memtable_allocated -= this_memrec->size;
        free(this_memrec);   /* free memory for the tracking record */
        free(this_key);      /* free memory for the hashkey */

    } while(0);

    free(p);  /* finally free the actual object */
    return hashresult;
}

/** 
 * Looks up the memory record in the hashtable for given pointer p. 
 * Returns NULL if not found.
 *
 */
memrec* debug_find_memrec(void* p, char* file, const short line) /* __FILE__ not unicode */
{  
    int  hashresult = 0, buckresult = 0;
    etch_hashitem  hashbucket;
    etch_hashitem* thisentry = &hashbucket;
    void*   this_key = 0;
    memrec* this_memrec = 0;
    if (!p) return NULL;

    do 
    {   if (!memtable) break;

        /* first free the memory tracking table entry. recall that the key to the   
         * tracking record is the value of the heap pointer returned by malloc 
         */
        hashresult = memtable->vtab->find   /* retrieve the hashtable entry */
           (memtable->realtable, &p, sizeof(void*), NULL, &thisentry); 

        if  (hashresult < 0)    
        {    printf("** memtbl missing key '%08x' at %s line %d\n", (unsigned)p, __FILE__,__LINE__);
             break;
        }

        this_key = thisentry->key;
        this_memrec = (memrec*) thisentry->value;
    } while(0);

    return this_memrec;
}

#endif /* #ifdef LEAKTEST */