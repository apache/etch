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
 * etch_object.c
 */

#include "etch_cache.h"
#include "etch_exception.h"
#include "etch_nativearray.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"
#include <wchar.h>


unsigned int primitive_objsize[] =
{
    sizeof(etch_byte),  sizeof(etch_boolean), sizeof(etch_int8),  sizeof(etch_int16), 
    sizeof(etch_int32), sizeof(etch_int64),   sizeof(etch_float), sizeof(etch_double),
    sizeof(etch_string),sizeof(etch_date),  
};

/**
 * destroy_object()
 * default virtual destructor for an etch_object* based object 
 * (other than etch_object), invoked by all other object dtors.
 */
int destroy_object(void* thisx)
{
    etch_object* thisobj = (etch_object*)thisx;
    etch_object* parentobj = NULL;
    if (thisobj == NULL) return -1;
    parentobj = thisobj->parent;

    if (thisobj->result)
    {   
        etch_free(thisobj->result);
    }
 
    if (!is_etchobj_static_shell(thisobj))
        etch_free(thisobj);

    etch_object_destroy(parentobj);
    
    return 0;
}

/**
 * destroy_objectex()
 * mark object as "refcount already decremented" and invoke destroy_object
 */
int destroy_objectex(etch_object* thisobj)
{   
    return destroy_object(thisobj);
}

etch_status_t etch_object_destroy(void* pobject)
{
    if(pobject){
        ((etch_object*)pobject)->destroy((etch_object*)pobject);
    }
    return ETCH_SUCCESS;
}


etch_object* etch_object_clone_func(void* pobject)
{
    etch_object* object = (etch_object*)pobject;
    if(object != NULL) {
        return object->clone(object);
    }
    return NULL;
}


/**
 * set_etch_assignable_arg_from()
 * populate an argument to etchobj_is_assignable_from() from specified object
 */
void set_etch_assignable_arg_from(etch_objclass* arg, etch_object* obj)
{
    vtabmask* vtab = NULL;
    memset(arg, 0, sizeof(etch_objclass));
    if (NULL == obj) return;

    ((etch_objclass*)arg)->obj_type = obj->obj_type;
    ((etch_objclass*)arg)->class_id = obj->class_id;
    arg->parent   = obj->parent;

    if (vtab = (vtabmask*) ((etch_object*)obj)->vtab)
    {   
        arg->vtable_class_id = ((etch_object*)vtab)->class_id;
        arg->inherits_from   = vtab->inherits_from;
    }   

    switch(((etch_object*)obj)->obj_type)
    {
        case ETCHTYPEB_NATIVEARRAY:
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
    if (((etch_objclass*)source)->class_id == target->class_id && source->numdims == 0) 
        return TRUE;  /* identity case */

    /* if left side is Object wrapper (not Object[]), anything can be assigned 
     * to it, since the c binding does not receive anything that is unwrapped.
     * i.e. in java Object.class is not assignable from int.class; however here
     * we don't have an int.class, only wrapped integers. if this were not the
     * case this test would come after the etch primitives test, since a class
     * not derived from object can't be assigned to object.
     */
    if (is_etch_object_type(((etch_objclass*)target)->obj_type, ((etch_objclass*)target)->class_id)) return TRUE;

    if (((etch_objclass*)target)->obj_type == ETCHTYPEB_PRIMITIVE   
     || ((etch_objclass*)source)->obj_type == ETCHTYPEB_PRIMITIVE) return FALSE;  

    /* if left (target) side is an array, and right (source) side is an 
     * array of the same dimensions, and either of the same content type,    
     * or an array of Object, then it is assignable.
     * this code is not robust - need to rethink assignability for default
     * array validator with various validation object array types.
     * currently we are validating very loosely for array types. we need
     * a more general means of validating arrays, i.e. common attributes
     * among array types (native, value, list)
     */
    if (is_etch_objarray_type   (((etch_objclass*)target)->obj_type, ((etch_objclass*)target)->class_id)
     || is_etch_arraylist_type  (((etch_objclass*)target)->obj_type, ((etch_objclass*)target)->class_id) 
     || is_etch_nativearray_type(((etch_objclass*)target)->obj_type, ((etch_objclass*)target)->class_id))
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
        etch_object* thisparent = source->parent;   

        while(thisparent) /* walk source object's inheritance chain */
        {
            if (((etch_object*)thisparent)->class_id == target->class_id) return TRUE;
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
                   (((etch_objclass*)source)->class_id, source->inherits_from, ndx++)))
                break;

            if (((etch_objclass*)parentinfo)->class_id == target->class_id)
                return TRUE;
        } 
    } 

    return FALSE;  
}

/**
 * etchobj_is_assignable_from_ex()
 * see comments for etchobj_is_assignable_from()
 */
int etchobj_is_assignable_fromobj(etch_object* targetobj, etch_object* sourceobj)
{
    etch_objclass targetarg, sourcearg;  
    set_etch_assignable_arg_from(&targetarg, targetobj);
    set_etch_assignable_arg_from(&sourcearg, sourceobj);
    return etchobj_is_assignable_from(&targetarg, &sourcearg);
}



/**
 * destroy_string()
 */
int destroy_string(void* data) 
{
    etch_string* thisp = data;
    if (!is_etchobj_static_content(thisp))
        etch_free(thisp->v.value);  /* OK if null */
    destroy_objectex((etch_object*)thisp);
    return 0;
}

/**
 * default virtual copy constructor for etch object.   
 */
void* clone_object(void* thisx)
{
    etch_object* pthis = (etch_object*)thisx;
    void* pnew = NULL; 
    unsigned objsize = pthis->length;
    if (objsize < sizeof(etch_object)) objsize = sizeof(etch_object);
    
    pnew = etch_malloc (objsize, ((etch_object*)pthis)->obj_type); 
    memcpy(pnew, pthis, objsize);  

    return pnew;  
}

/**
 * default virtual copy constructor for objects requiring deep copy  
 */
void* clone_null(void* pthis)
{
    return NULL;
}

/**
 * clone_string()
 */
void* clone_string(void* obj) 
{
  etch_string* thisp = (etch_string*)obj;
    etch_string* newobj = (etch_string*) clone_object((etch_object*)thisp);

    newobj->v.value = etch_malloc(thisp->byte_count, ETCHTYPEB_STRING);
    memcpy(newobj->v.value, thisp->v.value, thisp->byte_count);
    newobj->byte_count = thisp->byte_count;
    newobj->char_count = thisp->char_count;

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
uint32 defgethashkey(void* data)
{
    etch_object* obj = (etch_object*)data;
    void* hashitem = obj; /* uses the object address as hash source */
    if (NULL == hashitem) return 0;
    if(obj->hashkey == 0){
        obj->hashkey = etchhash((char*)&hashitem, sizeof(void*), 0);
    }
    return obj->hashkey;
}

/**
 *  new_object()
 *  basic object constructor 
 *  sets type, class, size, default destructor, and copy constructor. 
 */
etch_object* new_object(const int objsize, const unsigned short obj_type, const unsigned short class_id)
{
    etch_object* newobj = 0;

    newobj = etch_malloc(objsize, obj_type);

    memset(newobj, 0, objsize);
    ((etch_object*)newobj)->obj_type = obj_type;
    ((etch_object*)newobj)->class_id = class_id;
    newobj->length   = objsize;
    newobj->destroy  = destroy_object; 
    newobj->clone    = clone_object;
    newobj->get_hashkey = defgethashkey;
    //newobj->get_hashkey(newobj);  
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
 * verify_object()
 * verify that the object passed is of the specified type and class.
 * zero is a match for either so pass zero to not validate either.
 */
int verify_object(etch_object* obj, const unsigned short type, const unsigned short id, void** out)
{
    if (obj == NULL) return -1;
    if (type != 0 && ((etch_object*)obj)->obj_type != type) return -1;
    if (id   != 0 && ((etch_object*)obj)->class_id != id)   return -1;  
    return 0; 
}

/**
 * get_base_vtable()
 * walks a vtable chain returning the final vtab in the chain
 */
void* get_base_vtable(etch_object* obj) 
{
    vtabmask* basevtab = ((etch_object*)obj)->vtab;
    while (basevtab && ((etch_object*)basevtab)->vtab) 
        basevtab = ((etch_object*)basevtab)->vtab;
    return basevtab;
}

/**
 * destroy_vtable()
 */
int destroy_vtable(void* data) 
{
  etch_object* vtab = (etch_object*)data;
    etchparentinfo* inheritlist = ((vtabmask*)vtab)->inherits_from;

    if (inheritlist && !is_etchobj_static_content(vtab)) 
    {
        free(inheritlist); /* vtables not tracked */
        ((vtabmask*)vtab)->inherits_from = NULL;
    }

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
uint32 get_vtabobj_hashkey(void* data)
{
  etch_object* vtabobj = (etch_object*)data;
    return vtabobj->hashkey = get_vtable_cachehkey(((etch_object*)vtabobj)->class_id);
}

/**
 * new_vtable()
 * instantiate a new virtual function table of the specified type, 
 * defaulting all methods to those of specified parent if requested
 */
void* new_vtable(const void* parentvtab, const size_t size, const short classid)
{
    etch_object* newvtab = etch_malloc(size, 0);  /* vtable memory is not tracked */

    if(parentvtab)
        memcpy(newvtab, parentvtab, size);
    else
        memset(newvtab, 0, size);
   
    ((etch_object*)newvtab)->obj_type = ETCHTYPEB_VTABLE;   
    ((etch_object*)newvtab)->class_id = classid;
    newvtab->length   = (unsigned) size;
    newvtab->destroy  = destroy_vtable;
    newvtab->clone    = clone_object;

    newvtab->get_hashkey = get_vtabobj_hashkey;
    //newvtab->get_hashkey(newvtab);

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
etchparentinfo* get_vtab_inheritance_list(etch_object* obj, const short size, const short count, const short vtabclass)
{
   etchparentinfo *oldlist = NULL, *newlist = NULL;

   /* if such a list is already cached, return it now */
   vtabmask* vtab = ((etch_object*)obj)->vtab? obj->vtab: etch_cache_find(get_vtable_cachehkey(vtabclass), 0); 
   oldlist = vtab? vtab->inherits_from: NULL; 
   
   if (oldlist && oldlist->o.list_size >= size)
   {
       oldlist[0].c.list_count = count;
       ((etch_object*)obj)->vtab = vtab;
       return oldlist;
   }

   newlist = new_etch_inheritance_list(size, count, oldlist);
   if(newlist == NULL) return NULL;
      
   /* note that we are creating a placeholder vtable here. we could not add
    * virtuals to such a vtable, since this vtable consists of the vtable 
    * header only, per sizeof(vtabmask), following.
    */
   if(vtab == NULL) {
       vtab = new_vtable(NULL, sizeof(vtabmask), vtabclass);
       vtab->inherits_from = newlist;
       etch_cache_insert(((etch_object*)vtab)->get_hashkey(vtab), vtab, FALSE);
       ((etch_object*)obj)->vtab = vtab;
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
  
   newlist = etch_malloc(newbytes, 0);  /* vtables not tracked */
   memset(newlist, 0, newbytes);

   if (oldlist) /* we may be expanding an existing list, copy into new list */
   {   const int oldbytes = oldlist->o.list_size * sizeof(etchparentinfo);
       memcpy(newlist, oldlist, oldbytes);  
//       free(oldlist); /* vtables not tracked */
//       oldlist = NULL;
   }
   else newlist[0].c.list_count = count;

   newlist[0].o.list_size = size;  /* list attributes are in first entry */     
   return newlist;
}


/**
 * is_derives_from_object()
 * determine if specified object derives from object. 
 * all etch_object-masked objects are etch c objects by definition; however it is
 * here that we would artificially specify that certain wrapped objects are not
 * derived from object in the logical etch sense, if such a need arises.
 */ 
int is_derives_from_object(etch_object* obj)
{
    return obj? is_derives_from_object_class(((etch_object*)obj)->class_id): FALSE;
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
etchparentinfo* get_next_etch_parent(etch_object* obj, int current_index)
{
    etchparentinfo* inherit_list = obj && ((etch_object*)obj)->vtab? obj->vtab->inherits_from: NULL;
    return get_next_etch_parentex(((etch_object*)obj)->class_id, inherit_list, current_index);
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
  static etchparentinfo object_parent = { {ETCHTYPEB_ETCHOBJECT}, {CLASSID_OBJECT} };
    etchparentinfo* nextparent  = NULL;

    if  (NULL == inherit_list && current_index > 0);
    else
    if ((NULL == inherit_list) || (current_index == inherit_list[0].c.list_count))    
         if  (is_derives_from_object_class(class_id))
              nextparent = &object_parent;
         else; 
    else
    if  (current_index < inherit_list[0].c.list_count) 
         nextparent = &inherit_list[++current_index]; /* list is one-based */

    return nextparent;
}


/**
 * new_primitive()
 * allocate, initialize and return a primitive object
 */
etch_object* new_primitive(const unsigned obj_len, const unsigned short class_id) 
{
    etch_object* newobj  = new_object(obj_len, ETCHTYPEB_PRIMITIVE, class_id);
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
#ifdef WIN32
    #pragma warning(disable:4996) /* wcscpy unsafe warning */
#endif
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
    //newobj->get_hashkey((etch_object*)newobj);
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
    //newobj->get_hashkey((etch_object*)newobj);
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
    //newobj->get_hashkey((etch_object*)newobj);
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
    //newobj->get_hashkey((etch_object*)newobj);
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
    //newobj->get_hashkey((etch_object*)newobj);
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
    //newobj->get_hashkey((etch_object*)newobj);
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
    //newobj->get_hashkey((etch_object*)newobj);
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
    //newobj->get_hashkey((etch_object*)newobj);
    return newobj;
}


int32 etch_number_as_int32(const void* object) 
{
    if(is_etch_byte(object)){
        return (int32)((etch_byte*)object)->value;
    }else if(is_etch_int16(object)){
        return (int32)((etch_int16*)object)->value;
    }else if(is_etch_int32(object)){
        return (int32)((etch_int32*)object)->value;
    }
    return 0;
}

/**
 * etch_string_get_hashkey
 * hashkey computation override for an etch_string.
 * hash key is computed using the raw string as hash source.
 */
uint32 etch_string_get_hashkey(void* data)
{
    etch_object* etchobj = (etch_object*)data;

    etch_string* sobj = (etch_string*) etchobj;
    ((etch_object*)sobj)->hashkey = ETCH_ENCODING_ASCII != sobj->encoding?
          etch_get_wchar_hashkey(sobj->v.valw):
          etch_get_char_hashkey(sobj->v.valc);
    return ((etch_object*)sobj)->hashkey;
}

static unsigned int utf8_string_length(const void* s) {
  unsigned int res = 0;
  const char* help = (const char*) s;
  while (*help) {
    if ((*help & 0xC0) != 0x80) {
      ++res;
    }
    ++help;
  }
  return res;
}

static unsigned int ucs_string_length(const void* s, int width) {
  unsigned int res = 0;

  switch (width) {
  case 1: {
    const char* help = (const char*)s;
    while (1) {
      if (*help == 0) {
	break;
      }
      ++help;
      ++res;
    }
    break;
  }
  case 2: {
    const int16* help = (const int16*)s;
    while (1) {
      if (*help == 0) {
	break;
      }
      ++help;
      ++res;
    }
    break;
  }
  case 4: {
    const int32* help = (const int32*)s;
    while (1) {
      if (*help == 0) {
	break;
      }
      ++help;
      ++res;
    }
    break;
  }
  default:
    ETCH_ASSERT(!"only widths of 2 and 4 supported");
    break;
  }

  return res;
}

/**
 * etch_string_init()
 * private constructor for opaque string
 */
etch_string* etch_string_init(const void* s, const unsigned char encoding)
{
    etch_string* newobj = (etch_string*)new_primitive(sizeof(struct etch_string), CLASSID_STRING);

    newobj->encoding = encoding;

    if (s) {
        switch(encoding) {
        case ETCH_ENCODING_ASCII:
	        newobj->char_count  = ucs_string_length(s, 1);
	        newobj->byte_count  = (newobj->char_count + 1) * 1;
	    break;
        case ETCH_ENCODING_UCS2:
	        newobj->char_count  = ucs_string_length(s, 2);
	        newobj->byte_count  = (newobj->char_count + 1) * 2;
	    break;
        case ETCH_ENCODING_UCS4:
	        newobj->char_count  = ucs_string_length(s, 4);
	        newobj->byte_count  = (newobj->char_count + 1) * 4;
	    break;
        case ETCH_ENCODING_UTF8:
	        newobj->char_count  = utf8_string_length(s);
	        newobj->byte_count  = (unsigned int)(strlen(s) + 1);
	    break;
        default:
	        newobj->char_count  = 0;
	        newobj->byte_count  = 0;
	        printf("encoding: %d\n", encoding);
	        ETCH_ASSERT(!"encoding not supported");
	    break;
        }
    } else {
      newobj->char_count = 0;
      newobj->byte_count = 0;
      ((etch_object*)newobj)->is_null = TRUE;
    }

    ((etch_object*)newobj)->destroy = destroy_string;
    ((etch_object*)newobj)->clone   = clone_string;

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
    {
      newobj->v.valc = etch_malloc(newobj->byte_count, ETCHTYPEB_STRING);
      memcpy(newobj->v.valc, s, (size_t)newobj->byte_count);
    }

    ((etch_object*)newobj)->get_hashkey = etch_string_get_hashkey;
    //newobj->get_hashkey((etch_object*)newobj);

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
  switch (sizeof(wchar_t)) {
  case 2:
    return new_string(s, ETCH_ENCODING_UCS2);
  case 4:
    return new_string(s, ETCH_ENCODING_UCS4);
  default:
    ETCH_ASSERT(!"illegal sizeof(wchar_t)");
    return 0;
  }
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
    ((etch_object*)newobj)->get_hashkey = etch_string_get_hashkey;
    return newobj;
}


/**
 * new_etch_event()
 */ 
etch_event* new_etch_event(const unsigned short class_id, const int value) 
{
    etch_event* newobj = (etch_event*) new_int32(value);
    if (class_id) ((etch_object*)newobj)->class_id = class_id; 
    return newobj;
}


/**
 * new_etch_query()
 */ 
etch_query* new_etch_query(const unsigned short class_id, const int value) 
{
    etch_query* newobj = (etch_query*) new_int32(value);
    if (class_id) ((etch_object*)newobj)->class_id = class_id; 
    return newobj;
}


/**
 * new_etch_control()
 */ 
etch_control* new_etch_control(const unsigned short class_id, const int value) 
{
    etch_control* newobj = (etch_control*) new_int32(value);
    if (class_id) ((etch_object*)newobj)->class_id = class_id; 
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
 */ 
etch_who* new_who(void* whoobj)
{
    etch_who* newobj = (etch_who*) new_object(sizeof(etch_who),ETCHTYPEB_ETCHOBJECT,CLASSID_WHO);
    newobj->value = whoobj;
    return newobj;
}

/**
 * new_nullobj()
 * instantiate and return a logically null object
 */
etch_object* new_nullobj()
{
   etch_object* obj = (etch_object*) new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT,CLASSID_NONE);
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
uint32 etch_number_get_hashkey(void* data)
{
    etch_object* etchobj = (etch_object*)data;
    unsigned bytelength, hashkey;

    switch(((etch_object*)etchobj)->class_id)
    {   case CLASSID_PRIMITIVE_INT32: 
            bytelength = 4;
            hashkey = etchhash(&((etch_int32*)etchobj)->value, bytelength, 0);
            break;
        case CLASSID_PRIMITIVE_FLOAT:  
            bytelength = 4;
            hashkey = etchhash(&((etch_float*)etchobj)->value, bytelength, 0);
            break;
        case CLASSID_PRIMITIVE_INT64: 
            bytelength = 8;
            hashkey = etchhash(&((etch_int64*)etchobj)->value, bytelength, 0);
            break;
        case CLASSID_PRIMITIVE_DOUBLE: 
            bytelength = 8;
            hashkey = etchhash(&((etch_double*)etchobj)->value, bytelength, 0);
            break;
        case CLASSID_DATE: 
            bytelength = 8;
            hashkey = etchhash(&((etch_date*)etchobj)->value, bytelength, 0);
            break;
        case CLASSID_PRIMITIVE_INT16: 
            bytelength = 2; 
            hashkey = etchhash(&((etch_int16*)etchobj)->value, bytelength, 0);
            break;
        default: 
            bytelength = 1;     
            hashkey = etchhash(&((etch_int8*)etchobj)->value, bytelength, 0);
            break;
    } 
    
    return etchobj->hashkey = hashkey;
}
