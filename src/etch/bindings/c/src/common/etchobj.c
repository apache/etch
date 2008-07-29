/**
 * etchobj.c -- default implementations of etch object virtual methods
 */
#include "etch_global.h"
#include "etchexcp.h"
#include "etch_arraylist.h"


/**
 * Default virtual destructor for etchobject. 
 * Frees memory for the object's value, and then for the object shell.
 * Implementor should override etchobject.destroy_value() to provide cleanup for  
 * a custom object value on the heap. Implementor could override entire dtor
 * but this should not be necessary, in fact a good argument can be made for
 * making the dtor non-virtual, to ensure cleanup takes place.
 */
void destroy_etchobject(etchobject* p)
{
    void* object_value = NULL;
    if (p == NULL) return;  

    if (p->result)
    {   /* if object result exists, free it and possibly its exception */
        destroy_exception(p->result->exception);
        etch_free(p->result);
    }

    /*  here we'll simply call the object's destroy_value() if not static.
     *  however there are some well-known heap allocated object values we can
     *  for now anyway, clean up here and therefore avoid constructing and 
     *  caching override vtables for those object types. in the future we
     *  will probably prefer to be completely general and lose this switch.
     */
    if (p->is_static_data);
    else switch(p->type)     /* free mem for object value on heap */
    {
         case ETCHTYPE_BOXED_STRING:           
              if (p->value_ptr_to)
                  etch_free(p->value_ptr_to);
              break;

         ///* boxed arraylist needs a dedicated vtable constructor. this is lame. 
         // * warning: list content memory is freed here, *unless* the is_readonly
         // * parameter was specified during new_etch_arraylist() construction.
         // */
         //case ETCHTYPE_BOXED_ARRAYLIST:          
         //     destroy_boxed_arraylist(p);             
         //     break;

         default:                       
              p->vtab->destroy_value(p, p->value_ptr_to);
    }     
     
    if (!p->is_static_object)  /* free mem for this object shell */
        etch_free(p);  
}


/**
 * Default virtual destructor for a heap-allocated etchobject value. 
 * Default impl does nothing. Implementor should override this method 
 * to clean up a custom value allocated on the heap.
 */
void destroy_etchobject_value(etchobject* thisp, void* etchobject_value)
{
}


/**
 * Default copy for a heap-allocated etchobject value. 
 * Default impl does nothing. Implementor should override this method   
 * to copy a non-string custom value allocated on the heap.
 */
void copy_etchobject_value(etchobject* thisp, void* etchobject_value)
{
}


/**
 * default virtual copy constructor for etchobject. shallow copy. invokes
 * etchobject.clone_value() to perform deep copy if any. Implementor could 
 * override entire copy but this should not be necessary, in fact we could
 * decide to make the shallow copy non-virtual. Note that we make a copy of 
 * an object even if it is static, however if a heap allocated object value   
 * is static we do not clone it, instead copying the reference.
 */
etchobject* clone_etchobject(etchobject* pthis)
{
    void* object_value = NULL;
    etchobject* pnew = NULL; 
    if (pthis == NULL) return pnew;          
    pnew = etch_malloc (sizeof(etchobject), ETCHTYPEB_ETCHOBJECT); 
    if (!pnew) return pnew;
    memcpy(pnew, pthis, sizeof(etchobject));  /* copy object contents */

    /**
     * use etchobject.clone_value() to perform deep copy if any. Implementor  
     * could override entire copy but this should not be necessary, in fact  
     * we could decide to make the shallow copy non-virtual. Note that we make  
     * a copy of an object even if it is static, however if an object value  
     * (on heap) is static we do not clone it, instead copying the reference.
     */
    switch(pthis->type)  /* deep copy of object value on heap */
    {
         case ETCHTYPE_BOXED_STRING: /* copy underlying string value */            
              pnew->value_ptr_to = etchobject_clone_string_value(pnew);          
              break;

         case ETCHTYPE_BOXED_ARRAYLIST: /* copy underlying arraylist */            
              pnew->value_ptr_to = etchobject_clone_arraylist(pnew);          
              break;
    }

    return pnew;  
}


/**
 * default virtual hashcode() for etchobject.  
 */
int64 hashcode_etchobject(etchobject* this_p)
{   /* TODO: return hash code if indeed we still need this virtual method */
    return 0;  
}


/**
 *  new_etchresult()
 *  generate etchobject result object
 */
etchresult* new_etchresult(const int result, const int reason)
{
    etchresult* newresult = etch_malloc(sizeof(etchresult), ETCHTYPEB_RESULT);
    memset(newresult, 0, sizeof(etchresult));
    newresult->resultcode = result;
    newresult->reasoncode = reason;
    return newresult;
}


/**
 * 'private' default constructor for etchobject. Allocates the object, initializes   
 *  it as an empty object, populates vtable methods and caches vtable if necessary,
 *  and returns the object reference. 
 */
etchobject* new_etchobject(const int object_type)   
{
    etchobject* obj = NULL;
    i_etchobject* vtab = NULL;
    const int vtab_type = ETCHTYPE_VTABLE_ETCHOBJECT;
    const int cachecount = cache_count();

    vtab = cached_etchobject_vtable? cached_etchobject_vtable:
           cache_find(vtab_type); 

    if(!vtab)  /* cache the vtable for this object type */
    {          /* we do not etch_malloc object vtables */
        vtab = malloc(ETCHOBJ_VTABSIZE);
        memset(vtab, 0, ETCHOBJ_VTABSIZE);

        vtab->destroy  = destroy_etchobject;   /* default destructor */
        vtab->clone    = clone_etchobject;     /* default copy */
        vtab->hashcode = hashcode_etchobject;  /* default hashcode() */
        vtab->destroy_value = destroy_etchobject_value; /* default value free */
        vtab->clone_value = copy_etchobject_value;      /* default value copy */
        
        cached_etchobject_vtable = vtab;      /* main vtable is static cached */       
        cache_add(vtab_type, vtab);
    } 

   obj = etch_malloc(sizeof(etchobject), ETCHTYPEB_ETCHOBJECT); if (!obj) return obj;
   memset(obj, 0, sizeof(etchobject));   
   obj->vtab = vtab; 
   obj->type = object_type;

   obj->obj_type = short_type(ETCHTYPEA_IS_OBJECT, ETCHTYPEB_ETCHOBJECT);
   obj->class_id = 0;
   return obj;  
}


/**
 * etchobject_clone_string_value() 
 * etchobject value copy for ETCH_STRING.   
 * returns the new string, the existing string if static, or NULL.
 */
wchar_t* etchobject_clone_string_value(etchobject* thisp)
{
    wchar_t* newstr = NULL;
    if (thisp == NULL) return NULL;

    if (thisp->is_static_data) /* if static string return it */
        return thisp->value_ptr_to;
                               /* null if nothing to copy */
    if (thisp->value_ptr_to == NULL || thisp->size < 1) 
        return NULL;  

     newstr = etch_malloc((size_t)thisp->size, ETCHTYPEB_BYTES);
 
     memcpy(newstr, thisp->value_ptr_to, (size_t)thisp->size); 
     return newstr;     
}


/**
 * etchobject_clone_arraylist()
 * TODO move this code to etch_arraylist  
 */
void* etchobject_clone_arraylist(etchobject* thisp)
{
    int i = 0, result = 0;
    void* content = NULL;
    etch_arraylist* newlist = NULL, *oldlist = NULL;
    if (thisp == NULL) return NULL;

    if (thisp->is_static_data) /* if static string return it */
        return thisp->value_ptr_to;
                               /* null if nothing to copy */
    if (thisp->value_ptr_to == NULL || thisp->size < 1) 
        return NULL;  

    oldlist = (etch_arraylist*) thisp->value_ptr_to;
    newlist = new_arraylist(oldlist->count, oldlist->delta);
    newlist->is_readonly  = oldlist->is_readonly;
    newlist->content_type = oldlist->content_type;

    while((result == 0) && (NULL != (content = arraylist_get(oldlist, i++)))) 
           result = arraylist_add(newlist, content);
  
    return newlist;     
}


/**
 * short_type
 * return a 16-bit type code of 2 8-bit parts
 */
short short_type(unsigned i, unsigned j) 
{ 
    return (short) (  ( ((byte)i) << 16 ) | ((byte)j)  );
} 



/**
 * verify_object
 * verify that the object passed is of the specified type and class.
 * zero is a match for either so pass zero to not validate either.
 * return the 4 byte address following type and class if requested.
 */
int verify_object(objmask* obj, const unsigned short type, const unsigned short id, void** out)
{
    if (obj == NULL) return -1;
    if (type != 0 && obj->obj_type != type) return -1;
    if (id   != 0 && obj->class_id != id)   return -1;  
    if (out) *out = obj->addr;
    return 0; 
}


/**
 * destroy_instancedata
 * destroy some object's instance data implementation
 */
int destroy_instancedata(void* impl, const int objtype)
{
    impl_destroy_handler destroy = NULL;

    int result = verify_object((objmask*)impl, objtype, 0, (void**) &destroy);
    if (result == -1) return -1;   /* object passed was not expected object */
    if (destroy == NULL) return 0; /* instance data contained no destroy handler */

    result = destroy(impl); /* invoke destructor for specific instance data */    
    return result;
}


/**
 * get_base_vtable
 * walks a vtable chain returning the final vtab in the chain
 */
void* get_base_vtable(objmask* obj) 
{
    objmask* basevtab = obj->addr;
    while (basevtab->addr) basevtab = basevtab->addr;
    return basevtab;
}


/**
 * new_vtable
 * instantiate a new vtable of specified type, defaulting to parent methods if requested
 */
void* new_vtable(const void* parentvtab, const size_t size, const short type, const short classid)
{
    objmask* newvtab = malloc(size);  /* vtables not tracked */

    if  (parentvtab)
         memcpy(newvtab, parentvtab, size);
    else memset(newvtab, 0, size);

    newvtab->obj_type = ETCHTYPEB_VTABLE;  /* TODO: short type */
    newvtab->class_id = classid;

    return newvtab;
}




