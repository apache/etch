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
 * etchobj.c  
 */

#include "etch_global.h"
#include "etchexcp.h"
  

unsigned int primitive_objsize[] =
{
    sizeof(etch_byte),  sizeof(etch_boolean), sizeof(etch_int8),  sizeof(etch_int16), 
    sizeof(etch_int32), sizeof(etch_int64),   sizeof(etch_float), sizeof(etch_double),
    sizeof(etch_string),sizeof(etch_date),  
};


/**
 * destroy_object()
 * default virtual destructor for an objmask* based object 
 * (other than etch_object), invoked by all other object dtors. 
 */
int destroy_object(objmask* thisobj)
{
    objmask* parentobj = NULL;
    if (thisobj == NULL) return -1;  
    parentobj = thisobj->parent;   

    /* if a calling destructor did not already decrement the refcount ... */
    if (!is_etchobj_refcount_decremented(thisobj))
        if (thisobj->refcount > 0)        /* if object is refcounted */                                     
            if (--thisobj->refcount > 0) /* destroy only if last ref */
                return -1;    

    if (thisobj->result)
    {    
        destroy_exception(thisobj->result->exception);
        etch_free(thisobj->result);
    }
 
    if (!is_etchobj_static_shell(thisobj))
        etch_free(thisobj); 

    if (parentobj && parentobj->destroy)
        parentobj->destroy(parentobj);
    
    return 0;
}


/**
 * destroy_objectex()
 * mark object as "refcount already decremented" and invoke destroy_object
 */
int destroy_objectex(objmask* thisobj)
{   /* we usurp hashkey for this purpose since object is being destroyed anyway */
    if (thisobj) thisobj->hashkey = ETCH_NOREFCOUNT_MARKER;
    return destroy_object(thisobj);
}


/**
 * destroy_etch_object()
 * default virtual destructor for an anonymous object wrapper.
 * such a wrapper is final (it will not have a parent)
 */
int destroy_etch_object(etch_object* thisobj)
{
    objmask* childobj  = NULL;
    int is_childobj_etchobj = 0, is_childobj_owned = 0, is_static_content = 0;
    if (thisobj == NULL) return -1; 

    if (thisobj->refcount > 0)        /* if object is refcounted */                                     
        if (--thisobj->refcount > 0) /* destroy only if last ref */
            return -1;    

    if (thisobj->result)
    {   /* a result object pointer is not part of static content since never copied */
        destroy_exception(thisobj->result->exception);
        etch_free(thisobj->result);
    }

    childobj = thisobj->value;
    is_childobj_etchobj = thisobj->is_value_object;
    is_childobj_owned   = thisobj->is_value_owned;
    is_static_content   = is_etchobj_static_content(thisobj);

    if (!is_etchobj_static_shell(thisobj))
        etch_free(thisobj); 

    if (childobj && is_childobj_owned && !is_static_content)
    {   if  (is_childobj_etchobj)
             childobj->destroy(childobj);
        else etch_free(childobj);
    }
    
    return 0;
}


/**
 * destroy_etch_object_value()
 * destroy the content of an object wrapper.
 * this is not called by the object destructor because it destroys the wrapper first.
 */
int destroy_etch_object_value(etch_object* x)
{
    int  result = 0;

    if  (is_etchobj_static_content(x))
         result = -1;
    else
    if  (x->is_value_owned)
         if  (x->is_value_object)
             ((objmask*)x->value)->destroy(x->value);
         else etch_free(x->value);
    else result = -1;

    return result;
}


/**
 * set_etch_assignable_arg_from()
 * populate an argument to etchobj_is_assignable_from() from specified object
 */
void set_etch_assignable_arg_from(etch_objclass* arg, objmask* obj)
{
    vtabmask* vtab = NULL;
    memset(arg, 0, sizeof(etch_objclass));
    if (NULL == obj) return;

    arg->obj_type = obj->obj_type;
    arg->class_id = obj->class_id;
    arg->parent   = obj->parent;

    if (vtab = (vtabmask*) obj->vtab)
    {   arg->vtable_class_id = vtab->class_id;
        arg->inherits_from   = vtab->inherits_from;
    }   

    switch(obj->obj_type)
    { case ETCHTYPEB_NATIVEARRAY:
           arg->numdims          = ((etch_nativearray*)obj)->numdims;
           arg->content_obj_type = ((etch_nativearray*)obj)->content_obj_type;
           arg->content_class_id = ((etch_nativearray*)obj)->content_class_id;
           break;
      case ETCHTYPEB_ARRAYVAL:
           arg->numdims          = ((etch_collection_mask*)obj)->n;
           arg->content_obj_type = ((etch_collection_mask*)obj)->content_obj_type;
           arg->content_class_id = ((etch_collection_mask*)obj)->content_class_id;
           break;
      case ETCHTYPEB_ARRAYLIST:
           arg->numdims          = 1;
           arg->content_obj_type = ((etch_collection_mask*)obj)->content_obj_type;
           arg->content_class_id = ((etch_collection_mask*)obj)->content_class_id;
    }
}


/**
 * etchobj_is_assignable_from()
 * determines if the class of thisobj is the same as, or is a superclass of,
 * the class specified by that_class_id. tests whether the type represented 
 * by that_obj_type and that_class_id can be converted to the type of thisobj 
 * via an identity conversion or via a widening reference conversion. 
 *
 * @param target the class of left side of the assignment (to). 
 * during validation, this is the class of object the validator expects
 * to validate. if target object is an array, this is the array content class.

 * @param source the class of right side of the assignment (from). 
 * during validation, this is the class of the object being validated.
 * if target object is an array, this is the array content class.
 */
int etchobj_is_assignable_from(etch_objclass* target, etch_objclass* source)  
{
    if (source->class_id == target->class_id && source->numdims == 0) 
        return TRUE;  /* identity case */

    /* if left side is Object wrapper (not Object[]), anything can be assigned 
     * to it, since the c binding does not receive anything that is unwrapped.
     * i.e. in java Object.class is not assignable from int.class; however here
     * we don't have an int.class, only wrapped integers. if this were not the
     * case this test would come after the etch primitives test, since a class
     * not derived from object can't be assigned to object.
     */
    if (is_etch_object_type(target->obj_type, target->class_id)) return TRUE;

    if (target->obj_type == ETCHTYPEB_PRIMITIVE   
     || source->obj_type == ETCHTYPEB_PRIMITIVE) return FALSE;  

    /* if left (target) side is an array, and right (source) side is an 
     * array of the same dimensions, and either of the same content type,    
     * or an array of Object, then it is assignable.
     * this code is not robust - need to rethink assignability for default
     * array validator with various validation object array types.
     * currently we are validating very loosely for array types. we need
     * a more general means of validating arrays, i.e. common attributes
     * among array types (native, value, list)
     */
    if (is_etch_objarray_type   (target->obj_type, target->class_id)
     || is_etch_arraylist_type  (target->obj_type, target->class_id) 
     || is_etch_nativearray_type(target->obj_type, target->class_id))
    {   
        if (target->numdims == source->numdims)
        {
           /* this line added to pass anything using default array validator */
           if (target->content_obj_type == 0 && target->content_class_id == 0)
               return TRUE;

           if (target->content_class_id == CLASSID_UNWRAPPED)
               return target->content_obj_type == source->content_obj_type;
                 
           if (target->content_class_id == CLASSID_OBJECT 
            || target->content_class_id == source->content_class_id) 
               return TRUE; 
        }
      
        return FALSE;   
    }
    else /* if source inherits from target, source can be assigned to target */ 
    if (source->parent)
    {   /* inheritance model 2 - inherited objects are instantiated and chained */
        objmask* thisparent = source->parent;   

        while(thisparent) /* walk source object's inheritance chain */
        {
            if (thisparent->class_id == target->class_id) return TRUE;
            thisparent = thisparent->parent;
        }
    }
    else
    {   int ndx = 0;
        etchparentinfo* parentinfo;
        /* inheritance model 1 - inherited data flattened into single object */

        while(1) /* iterate source object's inheritance list */
        { 
            if (NULL == (parentinfo = get_next_etch_parentex 
                   (source->class_id, source->inherits_from, ndx++)))
                break;

            if (parentinfo->class_id == target->class_id)
                return TRUE;
        } 
    } 

    return FALSE;  
}


/**
 * etchobj_is_assignable_from_ex()
 * see comments for etchobj_is_assignable_from()
 */
int etchobj_is_assignable_fromobj(objmask* targetobj, objmask* sourceobj)
{
    etch_objclass targetarg, sourcearg;  
    set_etch_assignable_arg_from(&targetarg, targetobj);
    set_etch_assignable_arg_from(&sourcearg, sourceobj);
    return etchobj_is_assignable_from(&targetarg, &sourcearg);
}


/*
 * etchobj_assign_to() 
 * assign object b to object a, if legal to do so.
 * handles assignments of wrapped objects, array objects, and scalar objects. 
 * @return 0 or -1
 */
objmask* etchobj_assign_to(objmask* a, objmask* b)
{ 
    objmask* resultobj = a;

    if (is_etch_object(a))
    {        
        if  (is_etch_object(b))  
        {    /* both sides are wrappers so just copy right value to left */
             destroy_etch_object_value((etch_object*)a);   
            ((etch_object*)a)->value = ((etch_object*)b)->value;   
            ((etch_object*)a)->is_value_object = ((etch_object*)b)->is_value_object;
            ((etch_object*)a)->is_value_owned  = ((etch_object*)b)->is_value_owned;
        }
        else /* left side is a wrapper, right side is not a wrapper */
        {    /* so embed right side value inside left side wrapper */
             destroy_etch_object_value((etch_object*)a); 
            ((etch_object*)a)->value = b; /* wrap b inside a */
            ((etch_object*)a)->is_value_object = TRUE;
            ((etch_object*)a)->is_value_owned  = FALSE;
        }
    }
    else
    if (is_etch_object(b))
    {   /* left side is not a wrapper, right side is a wrapper */
        /* we do not currently support copying right side wrapped content
         * to the left side, although technically we could easily do so.
         */ 
        resultobj = NULL;
    }
    else
    if (!etchobj_is_assignable_fromobj(a, b)) 
    {   /* source object is not legally assignable to target */
        resultobj = NULL;
    }
    else  /* neither side is a wrapper, right side is assignable to left */
    if (is_etch_nativearray(a)) 
    {     /* both sides are arrays per is_assignable_from(), 
           * attempt to copy right side array to left */ 
        resultobj = (objmask*) etch_nativearray_assign_to   
           ((etch_nativearray*) a, (etch_nativearray*) b);  
    }  
    else /* both sides are scalar */
    if (b->parent != NULL)
    {    /* inheritance type 2: parent object(s) is/are instantiated 
          * and chained to the inheritor. with this type of inheritance, 
          * it is possible that the memory reference we return is not the 
          * same as the target reference we pass. this is because we can't 
          * copy source to target to do the assignment, since the child 
          * object we would want to assign to it may in fact be larger 
          * than the target object.
          */
         objmask* thisparent = b;
         resultobj = NULL;

         while(thisparent) /* walk the parent chain */
         {   if (thisparent->class_id == a->class_id)
             {   resultobj = thisparent;
                 break;
             }
             else thisparent = thisparent->parent;
         }          
    }
    else /* inheritance type 1: parent data are members of child object. */  
    {    /* since the right side is derived from left, and since we enforce  
          * a rule that derived objects must append instance data to the end 
          * of the parent object, we can simply copy memory from the source
          * to the target, limited to the known shorter length of the target;
          * and then restore the individual fields which should be retained.
          * this scheme of course depends on object constructors populating 
          * obj->length with sizeof(instantiated struct), in all cases. */
         
          /* save off items we need to restore post-copy */
          objmask save, *saved = &save; 
          memcpy(saved, a, sizeof(objmask));

          /* mark target shell immutable so destructor will not free it */
          set_etchobj_static_shell(a);

          /* invoke target destructor to free any content owned by target */
          a->destroy(a);

          /* copy source to target */
          memcpy(a, b, a->length);

          /* mark target as a copy, its content not owned by it */
          a->is_copy = TRUE;
          clear_etchobj_static_shell(a);
          set_etchobj_static_content(a);     

          /* finally restore the retained target fields */
          a->obj_type = saved->obj_type;
          a->class_id = saved->class_id;
          a->refcount = saved->refcount;  
          a->result   = saved->result; 
          a->destroy  = saved->destroy;
          a->clone    = saved->clone; 
          a->get_hashkey = saved->get_hashkey;
          if (is_etchobj_static_shell(saved))
              set_etchobj_static_shell(a);
    }

    return resultobj;
}


/**
 * etch_addref()
 * increment object's reference count. if previously zero, begins reference
 * counting of an object. obj.refcount > 0 indicates that the object is 
 * reference counted. conversely, if obj.refcount is zero, an object is not 
 * reference counted. if an object is refcounted, then invocation of its 
 * destructor implies decrementing the refcount, destroying the object only
 * if the refcount is now zero.
 */
unsigned etch_addref(objmask* thisobj)
{
    return ++thisobj->refcount;
}


/**
 * etch_release()
 * decrement object's reference count and destroy the object if now zero.
 * returns new reference count. zero indicates the object was destroyed.
 * use of this method is optional, since invocation of a refcounted object's
 * destructor does an implicit decrement of the refcount. however it is
 * included since coding destroy() might be misleading to the reader.
 */
unsigned etch_release(objmask* thisobj) 
{
    return destroy_object(thisobj) == 0? 0: thisobj->refcount;
}


/**
 * etch_release_wrapper()
 * see comments above at etch_release()
 */
unsigned etch_release_wrapper(etch_object* thisobj) 
{
    return destroy_etch_object(thisobj) == 0? 0: thisobj->refcount;
}


/**
 * destroy_string()
 */
int destroy_string(etch_string* thisp) 
{
    if (thisp->refcount > 0 && --thisp->refcount > 0) return -1;
    if (!is_etchobj_static_content(thisp))
        etch_free(thisp->v.value);  /* OK if null */
    destroy_objectex((objmask*)thisp);
    return 0;
}


/**
 * default virtual copy constructor for etch object.   
 */
objmask* clone_object(objmask* pthis)
{
    void* object_value = NULL;
    void* pnew = NULL; 
    unsigned objsize = pthis->length;
    if (objsize < sizeof(objmask)) objsize = sizeof(objmask);
    
    pnew = etch_malloc (objsize, pthis->obj_type); 
    memcpy(pnew, pthis, objsize);  

    return (objmask*) pnew;  
}


/**
 * default virtual copy constructor for objects requiring deep copy  
 */
objmask* clone_null(objmask* pthis)
{
    return NULL;
}


/**
 * clone_string()
 */
etch_string* clone_string(etch_string* thisp) 
{
    etch_string* newobj = (etch_string*) clone_object((objmask*)thisp);
    newobj->v.value = thisp->encoding == ETCH_ENCODING_UTF16?
        (void*) new_wchar(thisp->v.valw): (void*) new_char (thisp->v.valc);
    return newobj;
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
 * defgethashkey
 * default hashkey computation for an etch object
 */
unsigned defgethashkey(objmask *obj)
{
    void* hashitem = obj; /* uses the object address as hash source */
    if (NULL == hashitem) return 0;
    return obj->hashkey = etchhash((char*)&hashitem, sizeof(void*), 0);
}


/**
 *  new_object()
 *  basic object constructor 
 *  sets type, class, size, default destructor, and copy constructor. 
 */
objmask* new_object(const int objsize, const unsigned short obj_type, const unsigned short class_id)
{
    objmask* newobj = etch_malloc(objsize, obj_type);
    memset(newobj, 0, objsize);
    newobj->obj_type = obj_type;
    newobj->class_id = class_id;
    newobj->length   = objsize;
    newobj->destroy  = destroy_object; 
    newobj->clone    = clone_object;
    newobj->get_hashkey = defgethashkey;
    newobj->get_hashkey(newobj);  
    return newobj;
}


/**
 * clone_etch_object()
 * clone_etch_object copy constructor
 */ 
objmask* clone_etch_object(objmask* thatobj)
{
    objmask* newobj = clone_object(thatobj);
    ((etch_object*) newobj)->is_value_owned = FALSE;
    newobj->is_copy = TRUE;
    return newobj;
}


/**
 *  new_etch_object() 
 *  wrapper object constructor
 */
etch_object* new_etch_object(const unsigned short class_id, objmask* childobj)   
{
    etch_object* newobj = (etch_object*) new_object
        (sizeof(etch_object), ETCHTYPEB_ETCHOBJECT, class_id); 

    newobj->is_value_object = FALSE; /* when TRUE dtor calls destroy() */
    newobj->is_value_owned  = TRUE;  /* when TRUE dtor frees content */
    newobj->value   = childobj;
    newobj->destroy = destroy_etch_object;
    newobj->clone   = clone_etch_object; 
   
    return newobj;  
}


/**
 * short_type()
 * return a 16-bit type code of 2 8-bit parts
 */
short short_type(unsigned i, unsigned j) 
{ 
    return (short) (  ( ((byte)i) << 16 ) | ((byte)j)  );
} 


/**
 * clear_lastresult()
 * clear the global lastresult object
 */
void clear_lastresult() 
{ 
    (lastresobj = &_lastresobj)->result = &lastresult;
    if (lastresult.exception) destroy_exception(lastresult.exception);
    memset(&lastresult, 0, sizeof(struct etchresult));
} 


/**
 * verify_object()
 * verify that the object passed is of the specified type and class.
 * zero is a match for either so pass zero to not validate either.
 */
int verify_object(objmask* obj, const unsigned short type, const unsigned short id, void** out)
{
    if (obj == NULL) return -1;
    if (type != 0 && obj->obj_type != type) return -1;
    if (id   != 0 && obj->class_id != id)   return -1;  
    return 0; 
}


/**
 * get_base_vtable()
 * walks a vtable chain returning the final vtab in the chain
 */
void* get_base_vtable(objmask* obj) 
{
    vtabmask* basevtab = obj->vtab;
    while (basevtab && basevtab->vtab) basevtab = basevtab->vtab;
    return basevtab;
}


/**
 * destroy_vtable()
 */
int destroy_vtable(objmask* vtab) 
{
    etchparentinfo* inheritlist = ((vtabmask*)vtab)->inherits_from;

    if (inheritlist && !is_etchobj_static_content(vtab))
        free(inheritlist); /* vtables not tracked */

    if (!is_etchobj_static_shell(vtab))
        free(vtab); /* vtables not tracked */

    return 0;
}


/**
 * get_class_cachekey()
 * get the unique value used for keying the indicated class in a class cache.
 */
unsigned get_class_cachekey(unsigned short obj_type, unsigned short class_id)
{
    unsigned key = (obj_type << 16) | class_id;
    return etchhash(&key, 4, 0);
}


/**
 * get_vtable_cachehkey()
 * calculates ad returns the *cache* key for specified vtable object
 * the hashkey on the vtable object is not an object key, it is a class key,
 * since vtables are cached by class.
 */
unsigned get_vtable_cachehkey(unsigned short class_id)
{
    return get_class_cachekey(ETCHTYPEB_VTABLE, class_id);
}


 /**
 * get_vtabobj_hashkey()
 * sets and gets the *cache* key for specified vtable object
 * the hashkey on the vtable object is not an object key, it is a class key,
 * since vtables are cached by class.
 */
unsigned get_vtabobj_hashkey(objmask* vtabobj)
{
    return vtabobj->hashkey = get_vtable_cachehkey(vtabobj->class_id);
}


/**
 * new_vtable()
 * instantiate a new virtual function table of the specified type, 
 * defaulting all methods to those of specified parent if requested
 */
void* new_vtable(const void* parentvtab, const size_t size, const short classid)
{
    objmask* newvtab = malloc(size);  /* vtable memory is not tracked */

    if  (parentvtab)
         memcpy(newvtab, parentvtab, size);
    else memset(newvtab, 0, size);
   
    newvtab->obj_type = ETCHTYPEB_VTABLE;   
    newvtab->class_id = classid;
    newvtab->length   = (unsigned) size;
    newvtab->destroy  = destroy_vtable;
    newvtab->clone    = clone_object;

    newvtab->get_hashkey = get_vtabobj_hashkey;
    newvtab->get_hashkey(newvtab);

    return newvtab;
}


/**
 * get_vtab_inheritance_list()
 * add an inheritance list to the specified object, or fetch existing list.
 * an inheritance list exists in the vtable since we don't need to duplicate it
 * for every instance. if there is no vtable in the specified object, a place-
 * holder vtable is instantiated in the object. recall that vtables are freed
 * when the global cache is cleared. the first entry in an inheritance list 
 * contains the list attributes, therefore the list is one-based. 
 * if an appropriately-sized list is already cached, the cached list is 
 * returned. if a shorter list exists it is resized, copied, and returned. 
 * @param size total number of entries to be allocated
 * @param count number of populated entries 
 */ 
etchparentinfo* get_vtab_inheritance_list(objmask* obj, 
   const short size, const short count, const short vtabclass)
{
   etchparentinfo *oldlist = NULL, *newlist = NULL;

   /* if such a list is already cached, return it now */
   vtabmask* vtab = obj->vtab? obj->vtab: cache_find(get_vtable_cachehkey(vtabclass), 0); 
   oldlist = vtab? vtab->inherits_from: NULL; 
   
   if (oldlist && oldlist->list_size >= size)
   {
       oldlist[0].list_count = count;
       obj->vtab = vtab;
       return oldlist;
   }

   newlist = new_etch_inheritance_list(size, count, oldlist);
   if (newlist == NULL) return NULL;
      
   /* note that we are creating a placeholder vtable here. we could not add
    * virtuals to such a vtable, since this vtable consists of the vtable 
    * header only, per sizeof(vtabmask), following.
    */
   if (vtab == NULL) 
   {   vtab = new_vtable(NULL, sizeof(vtabmask), vtabclass);
       vtab->inherits_from = newlist;
       cache_insert(vtab->hashkey, vtab, FALSE);
       obj->vtab = vtab;
   }
       
   vtab->inherits_from = newlist;
   return newlist;
}


/**
 * new_etch_inheritance_list()
 * allocate and return an inheritance list of the specified size
 * @param size total number of entries to be allocated
 * @param count number of populated entries 
 */ 
etchparentinfo* new_etch_inheritance_list(const short size, const short count, 
   etchparentinfo* oldlist)
{
   etchparentinfo *newlist = NULL;
   const int newbytes = size * sizeof(etchparentinfo), MAXPARENTS = 15;
   if (count < 0 || count > MAXPARENTS || size < 0 || count > size) 
       return NULL;
  
   newlist = malloc(newbytes);  /* vtables not tracked */
   memset(newlist, 0, newbytes);

   if (oldlist) /* we may be expanding an existing list, copy into new list */
   {   const int oldbytes = oldlist->list_size * sizeof(etchparentinfo);
       memcpy(newlist, oldlist, oldbytes);  
       free(oldlist); /* vtables not tracked */
       oldlist = NULL;
   }
   else newlist[0].list_count = count;

   newlist[0].list_size = size;  /* list attributes are in first entry */     
   return newlist;
}


/**
 * is_derives_from_object()
 * determine if specified object derives from object. 
 * all objmask-masked objects are etch c objects by definition; however it is
 * here that we would artificially specify that certain wrapped objects are not
 * derived from object in the logical etch sense, if such a need arises.
 */ 
int is_derives_from_object(objmask* obj)
{
    return obj? is_derives_from_object_class(obj->class_id): FALSE;
}


/**
 * is_derives_from_object_class()
 * see comments at is_derives_from_object()
 */ 
int is_derives_from_object_class(const unsigned short class_id)
{
    return TRUE;
}


/**
 * get_next_etch_parent()
 * see comments at get_next_etch_parentex() below
 */ 
etchparentinfo* get_next_etch_parent(objmask* obj, int current_index)
{
    etchparentinfo* inherit_list = obj && obj->vtab? obj->vtab->inherits_from: NULL;
    return get_next_etch_parentex(obj->class_id, inherit_list, current_index);
}


/**
 * get_next_etch_parentex()
 * returns a non-disposable reference to etchparentinfo struct containing the
 * class of next parent in this object's inheritance hierarchy, relative to the
 * specified index. if specified object does in fact inherit from other than
 * object, its inheritance list must have been previously instantiated via
 * get_vtab_inheritance_list(), above, and populated accordingly, presumably
 * int the object's constructor. the inheritance list implicitly ends with 
 * object, if the specified object's class derives from object; however object 
 * does not explicitly appear in the list and in fact must not be so populated. 
 * @param obj the etch object for which a parent is requested.
 * @param current_index the index of the currently requested parent. on the
 * first call specify zero, on subsequent calls increment current_index.
 * @return etchparentinfo as described above, or NULL if no more parents.
 * the returned reference is valid while its containing inheritance list remains
 * instantiated, which ordinarily is while its associated vtable exists, which
 * ordinarily is until service teardown.
 */ 
etchparentinfo* get_next_etch_parentex
(const unsigned short class_id, etchparentinfo* inherit_list, int current_index)
{
    static etchparentinfo object_parent = { ETCHTYPEB_ETCHOBJECT, CLASSID_OBJECT };
    etchparentinfo* nextparent  = NULL;

    if  (NULL == inherit_list && current_index > 0);
    else
    if ((NULL == inherit_list) || (current_index == inherit_list[0].list_count))    
         if  (is_derives_from_object_class(class_id))
              nextparent = &object_parent;
         else; 
    else
    if  (current_index < inherit_list[0].list_count) 
         nextparent = &inherit_list[++current_index]; /* list is one-based */

    return nextparent;
}


/**
 * new_primitive()
 * allocate, initialize and return a primitive object
 */
objmask* new_primitive(const unsigned obj_len, const unsigned short class_id) 
{
    objmask* newobj  = new_object(obj_len, ETCHTYPEB_PRIMITIVE, class_id);
    newobj->destroy  = destroy_object;
    newobj->clone    = clone_object;
    newobj->get_hashkey = etch_number_get_hashkey;
    return newobj;
}


/**
 * new_wchar()
 * wide character string clone
 */
wchar_t* new_wchar(const wchar_t* s)
{   
    #pragma warning(disable:4996) /* wcscpy unsafe warning */
    unsigned bytelen;
    wchar_t* clone; 
    if (NULL == s) return NULL;
    bytelen = (unsigned)(wcslen(s) + 1) * sizeof(wchar_t);
    clone   = etch_malloc(bytelen, ETCHTYPEB_STRING);
    wcscpy(clone, s);
    return clone;
}


/**
 * new_char()
 * narrow character string clone
 */
char* new_char(const char* s)
{   
    char* clone; 
    if (NULL == s) return NULL;
    clone = etch_malloc(strlen(s) + 1, ETCHTYPEB_STRING);
    return strcpy(clone, s);
}


/**
 * new_byte()
 */
etch_byte* new_byte(const signed char v)
{
    etch_byte* newobj = (etch_byte*) new_primitive
        (sizeof(struct etch_byte), CLASSID_PRIMITIVE_BYTE);
    newobj->value = v;
    newobj->get_hashkey((objmask*)newobj);
    return newobj;
}


/**
 * new_boolean()
 */ 
etch_boolean* new_boolean(boolean v) 
{
    etch_boolean* newobj = (etch_boolean*) new_primitive
        (sizeof(struct etch_boolean), CLASSID_PRIMITIVE_BOOL);
    newobj->value = v? TRUE: FALSE;
    newobj->get_hashkey((objmask*)newobj);
    return newobj;
}


/**
 * new_int8()
 */ 
etch_int8* new_int8(signed char v) 
{
    etch_int8* newobj = (etch_int8*) new_primitive
        (sizeof(struct etch_int8), CLASSID_PRIMITIVE_INT8);
    newobj->value = v;
    newobj->get_hashkey((objmask*)newobj);
    return newobj;
}


/**
 * new_int16()
 */  
etch_int16* new_int16(short v) 
{
    etch_int16* newobj = (etch_int16*) new_primitive
        (sizeof(struct etch_int16), CLASSID_PRIMITIVE_INT16);
    newobj->value = v;
    newobj->get_hashkey((objmask*)newobj);
    return newobj;
}


/**
 * new_int32()
 */
etch_int32* new_int32(int v) 
{
    etch_int32* newobj = (etch_int32*) new_primitive
            (sizeof(struct etch_int32), CLASSID_PRIMITIVE_INT32);
    newobj->value = v;
    newobj->get_hashkey((objmask*)newobj);
    return newobj;
}

 
/**
 * new_int64()
 */
etch_int64* new_int64(int64 v) 
{
    etch_int64* newobj = (etch_int64*) new_primitive
            (sizeof(struct etch_int64), CLASSID_PRIMITIVE_INT64);
    newobj->value = v;
    newobj->get_hashkey((objmask*)newobj);
    return newobj;
}


/**
 * new_float()
 */ 
etch_float* new_float(float v) 
{
    etch_float* newobj = (etch_float*) new_primitive
            (sizeof(struct etch_float), CLASSID_PRIMITIVE_FLOAT);
    newobj->value = v;
    newobj->get_hashkey((objmask*)newobj);
    return newobj;
}


/**
 * new_double()
 */  
etch_double* new_double(double v) 
{
    etch_double* newobj = (etch_double*) new_primitive
            (sizeof(struct etch_double), CLASSID_PRIMITIVE_DOUBLE);
    newobj->value = v;
    newobj->get_hashkey((objmask*)newobj);
    return newobj;
}


/**
 * etch_string_get_hashkey
 * hashkey computation override for an etch_string.
 * hash key is computed using the raw string as hash source.
 */
unsigned etch_string_get_hashkey(objmask* etchobj)
{
    etch_string* sobj = (etch_string*) etchobj;
    sobj->hashkey = ETCH_ENCODING_UTF16 == sobj->encoding?
          etch_get_wchar_hashkey(sobj->v.valw):
          etch_get_char_hashkey(sobj->v.valc);     
    return sobj->hashkey;
}


 /**
 * etch_string_init()
 * private constructor for opaque string
 */ 
etch_string* etch_string_init(const void* s, const unsigned char encoding)
{
    etch_string* newobj = (etch_string*) 
        new_primitive(sizeof(struct etch_string), CLASSID_STRING);

    switch(encoding)
    { case ETCH_ENCODING_UTF16:
      case ETCH_ENCODING_ASCII:
      case ETCH_ENCODING_UTF8:
           newobj->encoding = encoding;
           break;
      default: newobj->encoding = ETCH_ENCODING_DEFAULT;
    }

    if (s)
    {   switch(encoding)
        { case ETCH_ENCODING_UTF16:
               newobj->char_count  = (unsigned) wcslen((wchar_t*)s);
               newobj->byte_count  = (newobj->char_count + 1) * sizeof(wchar_t);
               break;
          default:              
               newobj->char_count  = (unsigned) strlen((char*)s);
               newobj->byte_count  = (newobj->char_count + 1) * sizeof(char);
        }
    }
    else newobj->is_null = TRUE;

    newobj->destroy = destroy_string;
    newobj->clone   = clone_string;

    return newobj;
}


/**
 * new_string()
 * clones supplied string
 * @param s a raw string to be assigned to the new string object.
 * caller retains ownership of s.
 */ 
etch_string* new_string(const void* s, const unsigned char encoding) 
{
    etch_string* newobj = etch_string_init(s, encoding); 
       
    if (s)
    {   switch(encoding)
        { case ETCH_ENCODING_UTF16:             
               newobj->v.valw = etch_malloc(newobj->byte_count, ETCHTYPEB_STRING);
               wcscpy(newobj->v.valw, (wchar_t*)s); 
               break;
          default:                          
              newobj->v.valc = etch_malloc(newobj->byte_count, ETCHTYPEB_STRING);
              strcpy(newobj->v.valc, (char*)s); 
        }
    }

    newobj->get_hashkey = etch_string_get_hashkey;
    newobj->get_hashkey((objmask*)newobj);

    return newobj;
}


/**
 * new_stringw()
 * convenience constructor for string type ETCH_ENCODING_UTF16;
 * @param s a raw string to be assigned to the new string object.
 * caller retains ownership of s.
 */ 
etch_string* new_stringw(const void* s) 
{
    return new_string(s, ETCH_ENCODING_UTF16);
}
 

/**
 * new_stringa()
 * convenience constructor for string type ETCH_ENCODING_UTF8);
 * @param s a raw string to be assigned to the new string object.
 * caller retains ownership of s.
 */ 
etch_string* new_stringa(const void* s) 
{
    return new_string(s, ETCH_ENCODING_UTF8);
}


/**
 * new_string_from()
 * does not clone supplied string
 * @param s a disposable raw string to be assigned to the new string object.
 * caller relinquishes ownership of s.
 */ 
etch_string* new_string_from(const void* s, const unsigned char encoding) 
{
    etch_string* newobj = etch_string_init(s, encoding); 
    newobj->v.value = (void*) s;
    return newobj;
}


/**
 * new_etch_event()
 */ 
etch_event* new_etch_event(const unsigned short class_id, const int value) 
{
    etch_event* newobj = (etch_event*) new_int32(value);
    if (class_id) newobj->class_id = class_id; 
    return newobj;
}


/**
 * new_etch_query()
 */ 
etch_query* new_etch_query(const unsigned short class_id, const int value) 
{
    etch_query* newobj = (etch_query*) new_int32(value);
    if (class_id) newobj->class_id = class_id; 
    return newobj;
}


/**
 * new_etch_control()
 */ 
etch_control* new_etch_control(const unsigned short class_id, const int value) 
{
    etch_control* newobj = (etch_control*) new_int32(value);
    if (class_id) newobj->class_id = class_id; 
    return newobj;
}


/**
 * new_date()
 */ 
etch_date* new_date() 
{
    etch_date* newobj = (etch_date*) 
        new_primitive(sizeof(struct etch_date), CLASSID_DATE);

    time (&newobj->value);  
    newobj->ticks = clock();
 
    return newobj;
}


/**
 * new_who()
 * a who is an etch_object wrapper around some etch object type, its purpose
 * to be a disposable object which opaquely specifies the object which is the 
 * sender or receiver component of a method. 
 * @param whoobj the object which is the actual source or destination.
 * if this object is a nondisposable refrerence, of course it must be assured
 * that the object is not destroyed prior to destruction of the etch_who
 * which references it. 
 * @param is_owned if TRUE the etch_who destructor will destroy the whoobj.
 */ 
etch_who* new_who(void* whoobj, const int is_owned)
{
    etch_who* newobj = (etch_who*) new_etch_object(CLASSID_WHO, whoobj);
    newobj->is_value_object = TRUE;
    newobj->is_value_owned  = is_owned != FALSE;
    return newobj;
}


/**
 * new_nullobj()
 * instantiate and return a logically null object
 */
objmask* new_nullobj()
{
   objmask* obj = (objmask*) new_etch_object(CLASSID_NONE, NULL);
   obj->is_null = TRUE;
   return obj;
}


/**
 * etch_get_char_hashkey
 * hashkey computation using a narrow string as source
 */
unsigned etch_get_char_hashkey(const char* s)
{
    unsigned keybytelen = 0, hashkey = 0;
    if (NULL != s) 
        keybytelen = (unsigned) strlen(s);
    if (keybytelen) 
        hashkey = etchhash(s, keybytelen, 0);
    return hashkey;
}


/**
 * etch_get_wchar_hashkey
 * hashkey computation using a unicode string as source
 */
unsigned etch_get_wchar_hashkey(const wchar_t* s)
{
    unsigned keybytelen = 0, hashkey = 0;
    if (NULL != s) 
        keybytelen = (unsigned) (wcslen(s) * sizeof(wchar_t));
    if (keybytelen) 
        hashkey = etchhash(s, keybytelen, 0);
    return hashkey;
}

 
/**
 * etch_number_get_hashkey
 * hashkey computation override for an etch wrapped primitive number.
 * hash key is computed using the numeric value as hash source.
 */
unsigned etch_number_get_hashkey(objmask* etchobj)
{
    unsigned bytelength, hashkey;

    switch(etchobj->class_id)
    {   case CLASSID_PRIMITIVE_INT32: case CLASSID_PRIMITIVE_FLOAT:  bytelength = 4; break;
        case CLASSID_PRIMITIVE_INT64: case CLASSID_PRIMITIVE_DOUBLE: case CLASSID_DATE: bytelength = 8; break;
        case CLASSID_PRIMITIVE_INT16: bytelength = 2; break;
        default: bytelength = 1;     
    } 
    
    hashkey = etchhash(&((etch_int64*)etchobj)->value, bytelength, 0);
    return etchobj->hashkey = hashkey;
}
