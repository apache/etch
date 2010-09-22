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
 * etch_validator.c
 * validators
 */

#include "etch_validator.h"
#include "etch_tagged_data.h"
#include "etch_cache.h"
#include "etch_arrayval.h"
#include "etch_structval.h"
#include "etch_encoding.h"
#include "etch_nativearray.h"
#include "etch_objecttypes.h"
#include "etch_type.h"
#include "etch_mem.h"

#ifdef WIN32
#pragma warning (disable:4996)
#endif

etch_validator* etchvtor_cache[ETCHVTOR_CACHED_TYPE_COUNT * ETCHVTOR_MAX_CACHED];

etch_validator** etchvtor_cache_boolean; /* boolean validator cache address */
etch_validator** etchvtor_cache_byte;    /* byte validator cache address */
etch_validator** etchvtor_cache_int8;    /* int8 validator cache address */
etch_validator** etchvtor_cache_int16;   /* int16 validator cache address */
etch_validator** etchvtor_cache_int32;   /* int32 validator cache address */
etch_validator** etchvtor_cache_int64;    /* int64 validator cache address */
etch_validator** etchvtor_cache_float;   /* float validator cache address */
etch_validator** etchvtor_cache_double;   /* double validator cache address */
etch_validator** etchvtor_cache_string;   /* string validator cache address */
etch_validator** etchvtor_cache_object;   /* object validator cache address */
etch_validator** etchvtor_cache_exception; /* excp validator cache address */
etch_validator** etchvtor_cache_eod;   /* eod validator cache address */


/* cached validators have private constructors,
 * get() is the public method of construction and caching 
 */
etch_validator*  new_validator_boolean(const int dimensions);
etch_validator*  new_validator_byte   (const int dimensions);
etch_validator*  new_validator_int8   (const int dimensions);
etch_validator*  new_validator_int16  (const int dimensions);
etch_validator*  new_validator_int32  (const int dimensions);
etch_validator*  new_validator_int64  (const int dimensions);
etch_validator*  new_validator_float  (const int dimensions);
etch_validator*  new_validator_double (const int dimensions);
etch_validator*  new_validator_string (const int dimensions);
etch_validator*  new_validator_object (const int dimensions);
etch_validator*  new_validator_exception();
etch_validator*  new_validator_eod(); 

int etch_typevtor_validate(etch_validator*, etch_object*);
int etch_typevtor_check_value(etch_validator*, etch_object*, byte*);
etch_validator* etch_typevtor_element_validator(etch_validator*);

etch_object* etch_typevtor_validate_value(etch_validator*, etch_object*);
etch_validator* etchvtor_cache_validator(etch_validator*, etch_validator** cache);
etch_validator* new_type_validator(const unsigned short vtor_classid, 
   const unsigned short scalar_obj_type, const unsigned short scalar_classid, 
   const unsigned short array_classid, const int ndims, char* description);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * contructors, destructors
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * destroy_validator()
 * validator object destructor
 */
int destroy_validator(void* data)
{
  etch_validator* vtor = (etch_validator*)data;
    /* don't honor destroy() request if object is cached */
    if (vtor->is_cached) return -1;

    if (!is_etchobj_static_content(vtor))
        etch_free(vtor->description);

    return destroy_objectex((etch_object*)vtor);
}


/**
 * new_validator_from()
 * constructor for etch_validator
 */
etch_validator* new_validator()
{
    return new_validator_from(NULL, NULL, NULL, NULL);
}


/**
 * new_validator_from()
 * constructor 2 for etch_validator
 */
etch_validator* new_validator_from(etchvtor_validate fv, etchvtor_checkvalue fcv, 
    etchvtor_element_validator fev, etchvtor_validate_value fvv)
{
    etch_validator* newvtor = (etch_validator*) new_object(sizeof(etch_validator), 
         ETCHTYPEB_VALIDATOR, CLASSID_VALIDATOR);

    ((etch_object*)newvtor)->destroy = destroy_validator;
    newvtor->validate = fv;
    newvtor->check_value = fcv;
    newvtor->element_validator = fev;
    newvtor->validate_value = fvv;

    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * validator cache
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * etchvtor_cache_validator()
 * convenience method to cache specified validator, marking it cached.  
 */
etch_validator* etchvtor_cache_validator(etch_validator* vtor, etch_validator** cache)
{
    cache[vtor->numdimensions] = vtor; /* assumed pre-validated by caller */
    vtor->is_cached = TRUE;
    return vtor;
}


/**
 * etchvtor_clear_cache()
 * clear the validator cache, destroying any validators found
 */
void etchvtor_clear_cache() 
{
    const int cachebytes = ETCHVTOR_BYTES_PER_CACHE * ETCHVTOR_CACHED_TYPE_COUNT;
    const int cacheslots = cachebytes / sizeof(void*);
    int i=0;

    for(; i < cacheslots; i++)
    {
        etch_validator* p = etchvtor_cache[i];
        if (p == NULL || ((etch_object*)p)->obj_type != ETCHTYPEB_VALIDATOR) continue;
        p->is_cached = FALSE;
        etch_object_destroy(p);
    }

    memset(etchvtor_cache, 0, cachebytes);
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * combo validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */


/**
 * etch_combovtor_validate()
 * combo validator default validate() virtual
 */
int etch_combovtor_validate(etch_validator* vtor, etch_object* value)
{
    int resulta = 0, resultb = 0;
    etch_validator *vtor_a = vtor->vtor_a, *vtor_b = vtor->vtor_b;

    resulta = vtor_a? vtor_a->validate(vtor_a, value): -1;
    if (0 == resulta) return 0;

    resultb = vtor_b? vtor_b->validate(vtor_b, value): -1;
    return resultb;
}


/**
 * etch_combovtor_validate_value()
 * combo validator default validate_value() virtual
 * note carefully: the returned value may or may not be the same object   
 * as the passed value. see comments later in this module.
 */
etch_object* etch_combovtor_validate_value(etch_validator* vtor, etch_object* value)
{
    etch_validator *vtor_a = vtor->vtor_a, *vtor_b = vtor->vtor_b;
    etch_object *validatedobj = NULL;

    validatedobj = vtor_a? vtor_a->validate_value(vtor_a, value): NULL;

    if (NULL == validatedobj)      
    {
        validatedobj = vtor_b? vtor_b->validate_value(vtor_b, value): NULL;
    }

    return validatedobj; 
}


/**
 * etch_combovtor_check_value()
 * combo validator default check_value() virtual
 */
int etch_combovtor_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{
    byte typecode = 0;
    int  resulta = 0, resultb = 0;
    etch_validator *vtor_a = vtor->vtor_a, *vtor_b = vtor->vtor_b;

    resulta = vtor_a? vtor_a->check_value(vtor_a, value, &typecode): -1;
    if (-1 == resulta) 
    {   resultb = vtor_b? vtor_b->check_value(vtor_b, value, &typecode): -1; 
        if (-1 == resultb) return -1;
    }

    *typecode_out = typecode;
    return 0;
}


/**
 * etch_combovtor_element_validator()
 * combo validator default element_validator() virtual
 */
etch_validator* etch_combovtor_element_validator(etch_validator* vtor) 
{
    etch_validator *vtor_a = vtor->vtor_a, *vtor_b = vtor->vtor_b;

    etch_validator *eltvtor_a = vtor_a? vtor_a->element_validator(vtor_a): NULL;
    etch_validator *eltvtor_b = vtor_b? vtor_b->element_validator(vtor_b): NULL;

    if (eltvtor_a == NULL && eltvtor_b == NULL) return NULL;
    if (eltvtor_a == NULL) return eltvtor_b;
    if (eltvtor_b == NULL) return eltvtor_a;
    return new_combo_validator(eltvtor_a, eltvtor_b);
}


/**
 * destroy_combo_validator()
 * combo validator destructor
 * destroys all validators in the chain
 */
int destroy_combo_validator(void* data)
{
  etch_validator* vtor = (etch_validator*)data;
    if (vtor->is_cached) return -1;

    if (!is_etchobj_static_content(vtor))
    {
        etch_object_destroy(vtor->vtor_a);
        etch_object_destroy(vtor->vtor_b);
        etch_free(vtor->description);
    }

    return destroy_objectex((etch_object*)vtor);
}


/**
 * new_combo_validator()
 * constructor for combo validator.
 * caveat: the destructor frees memory for all validators in the chain.
 * if the same validator reference were to appear more than once in a chain,
 * it must be either a reference from the cache, or otherwise marked such that 
 * the destructor will not attempt to free it (validator marked as static, 
 * or combo parent marked as static content). the safest way to chain non-
 * cached validators is to ensure that each chained validator reference is
 * unique, i.e. newly instantiated. 
 */
etch_validator* new_combo_validator(etch_validator* vtor_a, etch_validator* vtor_b)
{
    etch_validator* newvtor = new_validator_from
      (etch_combovtor_validate, 
       etch_combovtor_check_value,
       etch_combovtor_element_validator, 
       etch_combovtor_validate_value);

    ((etch_object*)newvtor)->class_id = CLASSID_COMBO_VALIDATOR;
    ((etch_object*)newvtor)->destroy = destroy_combo_validator;
    newvtor->vtor_a = vtor_a;
    newvtor->vtor_b = vtor_b;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * type validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * new_type_validator()
 * private constructor for a type validator
 */
etch_validator* new_type_validator(const unsigned short vtor_classid, 
   const unsigned short scalar_obj_type, const unsigned short scalar_classid, 
   const unsigned short array_classid, const int ndims, char* description)
{
    etch_validator* newvtor = new_validator();
    ((etch_object*)newvtor)->class_id = vtor_classid;
    newvtor->expected_class_id = ndims? array_classid: scalar_classid;
    newvtor->numdimensions  = ndims;
    newvtor->description    = new_char(description);
    newvtor->validate_value = etch_typevtor_validate_value;
    newvtor->validate = etch_typevtor_validate;
    newvtor->check_value = etch_typevtor_check_value;
    newvtor->element_validator = etch_typevtor_element_validator;
    return newvtor;
}


/**
 * new_type_validator_1()
 * constructor for a type validator for types using inheritance scheme 1
 * @param vtable_class_id the class of the expected class' vtable, zero if the
 * class inherits only from object. the class inheritance list if any is fetched 
 * from the cached vtable, and is not disposable.
 */
etch_validator* new_type_validator_1(const unsigned short vtor_classid, 
    const unsigned short scalar_obj_type, const unsigned short scalar_classid,  
    const unsigned short vtable_class_id, const unsigned short array_classid, 
    const int ndims, char* description) 
{
    etch_validator* newvtor = new_type_validator(vtor_classid, scalar_obj_type, 
        scalar_classid, array_classid, ndims, description);
 
    if (vtable_class_id)
    {   /* cache a reference to the expected class' inheritance list if any */
        vtabmask* vtab = etch_cache_find(get_vtable_cachehkey(vtable_class_id), 0);
        if (vtab)
        {   newvtor->inherits_from = vtab->inherits_from;
            newvtor->is_owned_inherits_from = FALSE;
        }
    }

    return newvtor;
}


/**
 * new_type_validator_2()
 * constructor for a type validator
 * @param inherit_list a complete and disposable inheritance list for the class. 
 * for classes inheriting via method 2 that have more than two objects in the
 * inheritance chain, this list must be artificially created, since no object  
 * in the chain will have a complete list.
 */
etch_validator* new_type_validator_2(const unsigned short vtor_classid, 
    const unsigned short scalar_obj_type, const unsigned short scalar_classid,  
    const unsigned short array_classid, etchparentinfo* inherit_list,
    const int ndims, char* description) 
{
    etch_validator* newvtor = new_type_validator(vtor_classid, scalar_obj_type, 
        scalar_classid, array_classid, ndims, description);
 
    newvtor->inherits_from  = inherit_list;
    newvtor->is_owned_inherits_from = TRUE;
    return newvtor;
}


/**
 * etch_typevtor_check_dimensions()
 * convenience function to validate dimension count
 */
int etchvtor_check_dimensions(const int ndims)
{
    return ndims < 0 || ndims > ETCHVTOR_MAX_NDIMS? -1: 0;
}


/**
 * etchvtor_set_classparams()
 * populate arguments to etchobj_is_assignable_from(), for use by validate()
 */
void etchvtor_set_classparams(etch_objclass* targetparams, 
  etch_objclass* sourceparams, etch_validator* targetvtor, etch_object* sourceobj)
{
    memset(targetparams, 0, sizeof(etch_objclass)); 
    ((etch_objclass*)targetparams)->obj_type = targetvtor->expected_obj_type;
    ((etch_objclass*)targetparams)->class_id = targetvtor->expected_class_id;
    targetparams->numdims  = targetvtor->numdimensions;
    targetparams->inherits_from = targetvtor->inherits_from;

    set_etch_assignable_arg_from(sourceparams, sourceobj);
}


/**
 * etch_typevtor_validate()
 * type validator default validate() virtual
 */
int etch_typevtor_validate(etch_validator* vtor, etch_object* value)
{
    int result = 0, source_numdims = 0;
    if (!value) return -1;

    /* java uses different classes for arrays of the same type but different 
     * dimensions. we do not, so we also validate dimensions of array objects
     * to augment class comparison. 
     */
    if (is_etch_object_type(((etch_object*)value)->obj_type, ((etch_object*)value)->class_id)) return 0;

    switch(((etch_object*)value)->obj_type)
    {   case ETCHTYPEB_NATIVEARRAY:
             source_numdims = ((etch_nativearray*)value)->numdims;
             break;
        case ETCHTYPEB_ARRAYVAL:
             source_numdims = ((etch_arrayvalue*)value)->dim;
             break;
        case ETCHTYPEB_ARRAYLIST:
             source_numdims = 1;
             break;
    }

    #if(0)
    /* this validation was moved to the object validator's validate() */
    if (is_etch_object_type(((etch_object*)value)->obj_type, ((etch_object*)value)->class_id) && vtor->numdims == 0)
        result = 0; 
    else
    #endif
    if (source_numdims != vtor->numdimensions) 
        result = -1;
    else 
    if (((etch_object*)value)->class_id != vtor->expected_class_id) 
    {   
        /* objects not same class: verify source class assignable to target */
        etch_objclass target, source;
        etchvtor_set_classparams(&target, &source, vtor, (etch_object*) value);

        if (etchobj_is_assignable_from(&target, &source));
        else result = -1;
    }

    return result;
}


/**
 * etch_typevtor_check_value()
 * default implementation
 */
int etch_typevtor_check_value(etch_validator* v, etch_object* x, byte* tc)
{
    return -1;
}


/**
 * etch_typevtor_element_validator()
 * default implementation 
 */
etch_validator* etch_typevtor_element_validator(etch_validator* v)
{
    return NULL;
}


/**
 * etch_typevtor_validate_value()
 * type validator default validate_value() virtual.
 * note carefully: the returned value may or may not be the same object as the
 * passed value. the reason for this clunkiness is the port from java, in which
 * the original could simply be abandoned. the caller must therefore test if the 
 * objects are the same, and if not, destroy the original. for this reason also,
 * this method should not be called with the same symbol specified for both the
 * passed and return objects, for this would leak memory.
 * note further: the further reason for this is, for example, that the caller 
 * may get back an etch_byte*, but the object being validated is an etch_int32*
 * whose value is downsized into a newly instantiated etch_byte. we have padded
 * out all the primitives to have 8 bytes after the header, so we could, if need
 * be, morph the object passed to validate_value to be the validated object type.
 * for example, if we passed an etch_int32 with value 1 to the byte validator, 
 * currently byte's validate_value would instantiate and return a new etch_byte,
 * but we could easily change the etch_int32 to an etch_byte, if this would help.
 */
etch_object* etch_typevtor_validate_value(etch_validator* vtor, etch_object* value)
{ 
    return (0 == vtor->validate(vtor, value))? value: NULL; 
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * boolean validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_boolean  /* boolean validator cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_BOOLEAN * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_boolean_get()
 * get a boolean validator for specified dimensions
 */
etch_validator* etchvtor_boolean_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 
     
    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_boolean(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_boolean[ndims])) 
        vtor = etchvtor_cache_validator
                (new_validator_boolean(ndims), etchvtor_cache_boolean); 
    return vtor;
}


/**
 * etchvtor_boolean_check_value()
 * check_value override for boolean validator
 * returns external type of specified value
 */
int etchvtor_boolean_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{        
    if  (-1 == vtor->validate(vtor, value)) return -1;  
        
    if  (vtor->numdimensions > 0) 
         *typecode_out = ETCH_XTRNL_TYPECODE_ARRAY; 
    else *typecode_out =((etch_boolean*)value)->value?  
         ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE: ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE;   
        
    return 0;
}


/**
 * etchvtor_boolean_element_validator()
 * element_validator override for boolean validator
 * gets validator for array element or scalar
 */
etch_validator* etchvtor_boolean_element_validator(etch_validator* vtor)
{
    return etchvtor_boolean_get(vtor->numdimensions - 1);
}


/**
 * new_validator_boolean()
 * constructor for boolean validator
 */
etch_validator* new_validator_boolean(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "bool[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);

    newvtor = new_type_validator_1(CLASSID_VALIDATOR_BOOL, ETCHTYPEB_PRIMITIVE,
        CLASSID_PRIMITIVE_BOOL, 0, CLASSID_ARRAY_BOOL, numdimensions, name);

    newvtor->check_value = etchvtor_boolean_check_value;
    newvtor->element_validator = etchvtor_boolean_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * byte validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_byte  /* byte validator cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_BYTE * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_byte_get()
 * get a byte validator for specified dimensions
 */
etch_validator* etchvtor_byte_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 
     
    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_byte(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_byte[ndims])) 
        vtor = etchvtor_cache_validator(
                 new_validator_byte(ndims), etchvtor_cache_byte); 
    return vtor;
}
    

/**
 * etchvtor_byte_validate()
 * validate() override verifying value is in range of byte
 */
int etchvtor_byte_validate(etch_validator* vtor, etch_object* value)
{
    if (vtor->numdimensions > 0)
        return etch_typevtor_validate(vtor, value);

    if (NULL == value) return -1;

    switch(((etch_object*)value)->class_id)
    {   case CLASSID_PRIMITIVE_BYTE: 
             return 0;
        case CLASSID_PRIMITIVE_INT32:  
             return is_inrange_byte(((etch_int32*)value)->value)? 0: -1;
        case CLASSID_PRIMITIVE_INT64:  
             return is_inrange_byte(((etch_int64*)value)->value)? 0: -1;
        case CLASSID_PRIMITIVE_INT16:  
             return is_inrange_byte(((etch_int16*)value)->value)? 0: -1;
        case CLASSID_PRIMITIVE_BOOL:
             return is_inrange_bool(((etch_boolean*)value)->value)? 0: -1; 
        case CLASSID_PRIMITIVE_INT8:  
             return 0;
    }

    return -1;
}


/**
 * etchvtor_byte_check_value()
 * check_value override for byte validator
 * returns external type of specified value
 */
int etchvtor_byte_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{        
    if  (-1 == vtor->validate(vtor, value)) return -1;  

    switch(vtor->numdimensions)
    { case 0:  *typecode_out = ETCH_XTRNL_TYPECODE_BYTE;  break;
      case 1:  *typecode_out = ETCH_XTRNL_TYPECODE_BYTES; break;
      default: *typecode_out = ETCH_XTRNL_TYPECODE_ARRAY;
    }
   
    return 0;
}


/**
 * etchvtor_byte_validate_value()
 * validate_value override for byte validator
 */
etch_object* etchvtor_byte_validate_value(etch_validator* vtor, etch_object* value)
{     
    byte byteval = 0;

    etch_object* valobj = etch_typevtor_validate_value(vtor, value);
    if (NULL == valobj) return NULL;  

    /* here, the returned object is the passed object, caller does no cleanup */
    if (vtor->numdimensions > 0 || is_etch_byte(value)) return value; 

    if (-1 == etchtagdata_byte_value (value, &byteval)) return NULL;

    /* here, the returned object is not the same as the passed object, 
     * so caller must replace the reference, and destroy the passed object */
    return (etch_object*) new_byte(byteval);
}


/**
 * etchvtor_byte_element_validator()
 * element_validator override for byte validator
 * gets validator for array element or scalar
 */
etch_validator* etchvtor_byte_element_validator(etch_validator* vtor)
{
    return etchvtor_byte_get(vtor->numdimensions - 1);
}


/**
 * new_validator_byte()
 * constructor for byte validator
 */
etch_validator* new_validator_byte(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "byte[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);

    newvtor = new_type_validator_1(CLASSID_VALIDATOR_BYTE, ETCHTYPEB_PRIMITIVE,
        CLASSID_PRIMITIVE_BYTE, 0, CLASSID_ARRAY_BYTE, numdimensions, name);

    newvtor->validate          = etchvtor_byte_validate;
    newvtor->check_value       = etchvtor_byte_check_value;
    newvtor->validate_value    = etchvtor_byte_validate_value;
    newvtor->element_validator = etchvtor_byte_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * int8 validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_int8  /* int8 validator cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_INT8 * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_int8_get()
 * get a int8 validator for specified dimensions
 */
etch_validator* etchvtor_int8_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 
     
    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_int8(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_int8[ndims])) 
        vtor = etchvtor_cache_validator(
                 new_validator_int8(ndims), etchvtor_cache_int8); 
    return vtor;
}


/**
 * new_validator_int8()
 * constructor for int8 validator
 */
etch_validator* new_validator_int8(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "int8[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);
 
    newvtor = new_type_validator_1(CLASSID_VALIDATOR_INT8, ETCHTYPEB_PRIMITIVE,
        CLASSID_PRIMITIVE_INT8, 0, CLASSID_ARRAY_INT8, numdimensions, name);

    newvtor->validate          = etchvtor_byte_validate;
    newvtor->check_value       = etchvtor_byte_check_value;
    newvtor->validate_value    = etchvtor_byte_validate_value;
    newvtor->element_validator = etchvtor_byte_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * int16 validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
etch_validator** etchvtor_cache_int16  /* int16 validator cache address */
  = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_INT16 * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_int16_get()
 * get a int16 validator for specified dimensions
 */
etch_validator* etchvtor_int16_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 

    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_int16(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_int16[ndims])) 
        vtor = etchvtor_cache_validator(
                 new_validator_int16(ndims), etchvtor_cache_int16); 
    return vtor;
}


/**
 * etchvtor_int16_validate()
 * validate() override verifying value is in range of short
 */
int etchvtor_int16_validate(etch_validator* vtor, etch_object* value)
{
    int result = -1;

    if (vtor->numdimensions > 0)
        result = etch_typevtor_validate(vtor, value);
    else
    if (NULL == value) 
        result = -1;
    else switch(((etch_object*)value)->class_id)
    {   case CLASSID_PRIMITIVE_BYTE: 
        case CLASSID_PRIMITIVE_INT8: 
        case CLASSID_PRIMITIVE_INT16:  
             result = 0;
             break;
        case CLASSID_PRIMITIVE_INT32:  
             if (is_inrange_int16(((etch_int32*)value)->value)) 
                 result = 0;
             break;
        case CLASSID_PRIMITIVE_INT64:  
             if (is_inrange_int16(((etch_int64*)value)->value))
                 result = 0;
    }

    return result;
}


/**
 * etchvtor_int16_check_value()
 * check_value override for int16 validator
 * returns external type of specified value
 */
int etchvtor_int16_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{        
    short shortval = 0;
    int result = vtor->validate(vtor, value);

    if  (-1 == result);
    else    
    if  (vtor->numdimensions)
         *typecode_out = ETCH_XTRNL_TYPECODE_ARRAY;
    else        
    if  (-1 == (result = etchtagdata_int16_value(value, &shortval)));
    else
    if  (is_inrange_byte(shortval))
         *typecode_out = ETCH_XTRNL_TYPECODE_BYTE;
    else *typecode_out = ETCH_XTRNL_TYPECODE_SHORT;
       
    return result;
}


/**
 * etchvtor_int16_validate_value()
 * validate_value override for short int validator
 */
etch_object* etchvtor_int16_validate_value(etch_validator* vtor, etch_object* value)
{     
    short shortval = 0;

    etch_object* valobj = etch_typevtor_validate_value(vtor, value);
    if (NULL == valobj) return NULL;  

    /* here, the returned object is the passed object, caller does no cleanup */
    if (vtor->numdimensions > 0 || is_etch_int16(value))  return value; 

    if (-1 == etchtagdata_int16_value (value, &shortval)) return NULL;

    /* here, the returned object is not the same as the passed object, 
     * so caller must replace the reference, and destroy the passed object */
    return (etch_object*) new_int16(shortval);
}


/**
 * etchvtor_int16_element_validator()
 * element_validator override for int16 validator
 * gets validator for array element or scalar
 */
etch_validator* etchvtor_int16_element_validator(etch_validator* vtor)
{
    return etchvtor_int16_get(vtor->numdimensions - 1);
}


/**
 * new_validator_int16()
 * constructor for int16 validator
 */
etch_validator* new_validator_int16(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "int16[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);

    newvtor = new_type_validator_1(CLASSID_VALIDATOR_INT16, ETCHTYPEB_PRIMITIVE,
        CLASSID_PRIMITIVE_INT16, 0, CLASSID_ARRAY_INT16, numdimensions, name);

    newvtor->validate          = etchvtor_int16_validate;
    newvtor->check_value       = etchvtor_int16_check_value;
    newvtor->validate_value    = etchvtor_int16_validate_value;
    newvtor->element_validator = etchvtor_int16_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * int32 validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_int32  /* int32 validator cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_INT32 * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_int32_get()
 * get a int32 validator for specified dimensions
 */
etch_validator* etchvtor_int32_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 
     
    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_int32(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_int32[ndims])) 
        vtor = etchvtor_cache_validator(
                 new_validator_int32(ndims), etchvtor_cache_int32); 
    return vtor;
}


/**
 * etchvtor_int32_validate()
 * validate() override verifying value is in range of int
 */
int etchvtor_int32_validate(etch_validator* vtor, etch_object* value)
{
    int result = -1;
    if (vtor->numdimensions > 0)
        result = etch_typevtor_validate(vtor, value);
    else
    if (NULL == value) 
        result = -1;
    else switch(((etch_object*)value)->class_id)
    {   case CLASSID_PRIMITIVE_BYTE: 
        case CLASSID_PRIMITIVE_INT8: 
        case CLASSID_PRIMITIVE_INT16: 
        case CLASSID_PRIMITIVE_INT32:  
             result = 0;
             break;
        case CLASSID_PRIMITIVE_INT64:  
             if (is_inrange_int32(((etch_int64*)value)->value))
                 result = 0;
    }

    return result;
}


/**
 * etchvtor_int32_validate_value()
 * validate_value override for int validator
 */
etch_object* etchvtor_int32_validate_value(etch_validator* vtor, etch_object* value)
{     
    int intval = 0;

    etch_object* valobj = etch_typevtor_validate_value(vtor, value);
    if (NULL == valobj) return NULL;  

    /* here, the returned object is the passed object, caller does no cleanup */
    if (vtor->numdimensions > 0 || is_etch_int32(value))  return value; 

    if (-1 == etchtagdata_int32_value (value, &intval)) return NULL;

    /* here, the returned object is not the same as the passed object, 
     * so caller must replace the reference, and destroy the passed object */
    return (etch_object*) new_int32(intval);
}


/**
 * etchvtor_int32_element_validator()
 * element_validator override for int32 validator
 * gets validator for array element or scalar
 */
etch_validator* etchvtor_int32_element_validator(etch_validator* vtor)
{
    return etchvtor_int32_get(vtor->numdimensions - 1);
}


/**
 * etchvtor_int32_check_value()
 * check_value override for int32 validator
 * returns external type of specified value
 */
int etchvtor_int32_check_value(etch_validator* vtor, etch_object* value, byte* typecode_out)
{        
    int intval = 0;

    int result = vtor->validate(vtor, value);
    if (-1 == result);
    else    
    if (vtor->numdimensions)
        *typecode_out = ETCH_XTRNL_TYPECODE_ARRAY;
    else        
    if (-1 == (result = etchtagdata_int32_value(value, &intval)));
    else
    if  (is_inrange_byte(intval))
         *typecode_out = ETCH_XTRNL_TYPECODE_BYTE;
    else
    if  (is_inrange_int16(intval))
         *typecode_out = ETCH_XTRNL_TYPECODE_SHORT;
    else *typecode_out = ETCH_XTRNL_TYPECODE_INT;
       
    return result;
}


/**
 * new_validator_int32()
 * constructor for int32 validator
 */
etch_validator* new_validator_int32(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "int32[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);

    newvtor = new_type_validator_1(CLASSID_VALIDATOR_INT32, ETCHTYPEB_PRIMITIVE,
       CLASSID_PRIMITIVE_INT32, 0, CLASSID_ARRAY_INT32, numdimensions, name);

    newvtor->validate          = etchvtor_int32_validate;
    newvtor->check_value       = etchvtor_int32_check_value;
    newvtor->validate_value    = etchvtor_int32_validate_value;
    newvtor->element_validator = etchvtor_int32_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * int64 validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_int64  /* int64 validator cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_INT64 * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_int64_get()
 * get a int64 validator for specified dimensions
 */
etch_validator* etchvtor_int64_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 
     
    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_int64(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_int64[ndims])) 
        vtor = etchvtor_cache_validator(
                new_validator_int64(ndims), etchvtor_cache_int64); 
    return vtor;
}


/**
 * etchvtor_int64_validate()
 * validate() override verifying value is in range of long long
 */
int etchvtor_int64_validate(etch_validator* vtor, etch_object* value)
{
    int result = -1;

    if (vtor->numdimensions > 0)
        result = etch_typevtor_validate(vtor, value);
    else
    if (NULL == value);
    else switch(((etch_object*)value)->class_id)
    {   case CLASSID_PRIMITIVE_BYTE: 
        case CLASSID_PRIMITIVE_INT8: 
        case CLASSID_PRIMITIVE_INT16: 
        case CLASSID_PRIMITIVE_INT32:
        case CLASSID_PRIMITIVE_INT64:  
             result = 0;
    }
    return result;
}


/**
 * etchvtor_int64_validate_value()
 * validate_value override for long validator
 */
etch_object* etchvtor_int64_validate_value(etch_validator* vtor, etch_object* value)
{     
    int64 longval = 0;

    etch_object* valobj = etch_typevtor_validate_value(vtor, value);
    if (NULL == valobj) return NULL;  

    /* here, the returned object is the passed object, caller does no cleanup */
    if (vtor->numdimensions > 0 || is_etch_int64(value)) return value; 

    if (-1 == etchtagdata_int64_value (value, &longval)) return NULL;

    /* here, the returned object is not the same as the passed object, 
     * so caller must replace the reference, and destroy the passed object */
    return (etch_object*) new_int64(longval);
}


/**
 * etchvtor_int64_element_validator()
 * element_validator override for int64 validator
 * gets validator for array element or scalar
 */
etch_validator* etchvtor_int64_element_validator(etch_validator* vtor)
{
    return etchvtor_int64_get(vtor->numdimensions - 1);
}


/**
 * etchvtor_int64_check_value()
 * check_value override for int64 validator
 * returns external type of specified value
 */
int etchvtor_int64_check_value(etch_validator* vtor, etch_object* value, byte* typecode_out)
{        
    int64 longval = 0;

    int result = vtor->validate(vtor, value);
    if (-1 == result);
    else    
    if (vtor->numdimensions)
        *typecode_out = ETCH_XTRNL_TYPECODE_ARRAY;
    else        
    if (-1 == (result = etchtagdata_int64_value(value, &longval)));
    else
    if  (is_inrange_byte(longval))
         *typecode_out = ETCH_XTRNL_TYPECODE_BYTE;
    else
    if  (is_inrange_int16(longval))
         *typecode_out = ETCH_XTRNL_TYPECODE_SHORT;
    else
    if  (is_inrange_int32(longval))
         *typecode_out = ETCH_XTRNL_TYPECODE_INT;
    else *typecode_out = ETCH_XTRNL_TYPECODE_LONG;
       
    return result;
}


/**
 * new_validator_int64()
 * constructor for int64 validator
 */
etch_validator* new_validator_int64(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "int64[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);

    newvtor = new_type_validator_1(CLASSID_VALIDATOR_INT64, ETCHTYPEB_PRIMITIVE,
       CLASSID_PRIMITIVE_INT64, 0, CLASSID_ARRAY_INT64, numdimensions, name);

    newvtor->validate          = etchvtor_int64_validate;
    newvtor->check_value       = etchvtor_int64_check_value;
    newvtor->validate_value    = etchvtor_int64_validate_value;
    newvtor->element_validator = etchvtor_int64_element_validator;

    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * float validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_float  /* float validator cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_FLOAT * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_float_get()
 * get a float validator for specified dimensions
 */
etch_validator* etchvtor_float_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 
     
    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_float(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_float[ndims])) 
        vtor = etchvtor_cache_validator(
                new_validator_float(ndims), etchvtor_cache_float); 
    return vtor;
}


/**
 * etchvtor_float_check_value()
 * check_value override for float validator
 * returns external type of specified value
 */
int etchvtor_float_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{        
   if (-1 == vtor->validate(vtor, value)) return -1;     
    *typecode_out = (vtor->numdimensions)?
              ETCH_XTRNL_TYPECODE_ARRAY: ETCH_XTRNL_TYPECODE_FLOAT;     
    return 0;
}


/**
 * etchvtor_float_element_validator()
 * element_validator override for float validator
 * gets validator for array element or scalar
 */
etch_validator* etchvtor_float_element_validator(etch_validator* vtor)
{
    return etchvtor_float_get(vtor->numdimensions - 1);
}


/**
 * new_validator_float()
 * constructor for float validator
 */
etch_validator* new_validator_float(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "float[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);

    newvtor = new_type_validator_1(CLASSID_VALIDATOR_FLOAT, ETCHTYPEB_PRIMITIVE,
       CLASSID_PRIMITIVE_FLOAT, 0, CLASSID_ARRAY_FLOAT, numdimensions, name);

    newvtor->check_value       = etchvtor_float_check_value;
    newvtor->element_validator = etchvtor_float_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * double validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_double  /* double validator cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_DOUBLE * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_double_get()
 * get a double validator for specified dimensions
 */
etch_validator* etchvtor_double_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 
     
    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_double(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_double[ndims])) 
        vtor = etchvtor_cache_validator(
                new_validator_double(ndims), etchvtor_cache_double); 
    return vtor;
}


/**
 * etchvtor_double_check_value()
 * check_value override for double validator
 * returns external type of specified value
 */
int etchvtor_double_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{        
   if (-1 == vtor->validate(vtor, value)) return -1; 
    
   *typecode_out = (vtor->numdimensions)?
              ETCH_XTRNL_TYPECODE_ARRAY: ETCH_XTRNL_TYPECODE_DOUBLE;     
    return 0;
}


/**
 * etchvtor_double_element_validator()
 * element_validator override for double validator
 * gets validator for array element or scalar
 */
etch_validator* etchvtor_double_element_validator(etch_validator* vtor)
{
    return etchvtor_double_get(vtor->numdimensions - 1);
}


/**
 * new_validator_double()
 * constructor for double validator
 */
etch_validator* new_validator_double(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "double[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);

    newvtor = new_type_validator_1(CLASSID_VALIDATOR_DOUBLE, ETCHTYPEB_PRIMITIVE,
       CLASSID_PRIMITIVE_DOUBLE, 0, CLASSID_ARRAY_DOUBLE, numdimensions, name);

    newvtor->check_value       = etchvtor_double_check_value;
    newvtor->element_validator = etchvtor_double_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * string validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_string  /* string validator cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_STRING * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_string_get()
 * get a string validator for specified dimensions
 */
etch_validator* etchvtor_string_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 
     
    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_string(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_string[ndims])) 
        vtor = etchvtor_cache_validator(
                new_validator_string(ndims), etchvtor_cache_string); 
    return vtor;
}


/**
 * etchvtor_string_check_value()
 * check_value override for string validator
 * returns external type of specified value
 */
int etchvtor_string_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{     
    byte typecode = 0;   
    if (-1 == vtor->validate(vtor, value)) return -1; 

    if  (vtor->numdimensions)
         typecode = ETCH_XTRNL_TYPECODE_ARRAY;
    else
    if  (((etch_string*)value)->char_count)
         typecode = ETCH_XTRNL_TYPECODE_STRING;
    else typecode = ETCH_XTRNL_TYPECODE_EMPTY_STRING;
   
    if (typecode) *typecode_out = typecode;
    return 0;
}


/**
 * etchvtor_string_element_validator()
 * element_validator override for string validator
 * gets validator for array element or scalar
 */
etch_validator* etchvtor_string_element_validator(etch_validator* vtor)
{
    return etchvtor_string_get(vtor->numdimensions - 1);
}


/**
 * new_validator_string()
 * constructor for string validator
 */
etch_validator* new_validator_string(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "string[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);

    newvtor = new_type_validator_1(CLASSID_VALIDATOR_STRING, ETCHTYPEB_PRIMITIVE,
       CLASSID_STRING, 0, CLASSID_ARRAY_STRING, numdimensions, name);

    newvtor->check_value       = etchvtor_string_check_value;
    newvtor->element_validator = etchvtor_string_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * object validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_object  /* object validator cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_OBJECT * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_object_get()
 * get a object validator for specified dimensions
 */
etch_validator* etchvtor_object_get(const int ndims)
{
    etch_validator* vtor = NULL;
    if (-1 == etchvtor_check_dimensions(ndims)) return NULL; 
     
    if (ndims >= ETCHVTOR_MAX_CACHED)
        vtor = new_validator_object(ndims);
    else
    if (NULL == (vtor = etchvtor_cache_object[ndims])) 
        vtor = etchvtor_cache_validator(
                new_validator_object(ndims), etchvtor_cache_object); 
    return vtor;
}


/**
 * etchvtor_object_validate()
 * any etch c object can be assigned to a scalar etch c object. 
 * this validation is necessary in the c code whereas it is not in the java version.
 */
int etchvtor_object_validate(etch_validator* vtor, etch_object* value)
{
    return value? 0: -1;
}


/**
 * etchvtor_object_check_value()
 * check_value override for object validator
 * returns external type of specified value
 */
int etchvtor_object_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{     
    if (-1 == vtor->validate(vtor, value)) return -1; 

    if  (vtor->numdimensions)
         *typecode_out = ETCH_XTRNL_TYPECODE_ARRAY;
    else *typecode_out = ETCH_XTRNL_TYPECODE_ANY;

    return 0;
}


/**
 * etchvtor_object_element_validator()
 * element_validator override for object validator
 * gets validator for array element or scalar
 */
etch_validator* etchvtor_object_element_validator(etch_validator* vtor)
{
    return vtor->numdimensions? etchvtor_object_get(vtor->numdimensions - 1): vtor;
}


/**
 * new_validator_object()
 * constructor for object validator
 */
etch_validator* new_validator_object(const int numdimensions) 
{
    etch_validator* newvtor = NULL;
    static char* mask = "object[%d]"; char name[20]; 
    sprintf(name,mask,numdimensions);

    newvtor = new_type_validator_1(CLASSID_VALIDATOR_OBJECT, ETCHTYPEB_ETCHOBJECT,
        CLASSID_OBJECT, 0, CLASSID_ARRAY_OBJECT, numdimensions, name);

    newvtor->validate          = etchvtor_object_validate;
    newvtor->check_value       = etchvtor_object_check_value;
    newvtor->element_validator = etchvtor_object_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * exception validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_exception  /* exception vtor cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_EXCEPTION * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_exception_get()
 * get an exception validator  
 */
etch_validator* etchvtor_exception_get()
{
    etch_validator* vtor = NULL;

    if (NULL == (vtor = etchvtor_cache_exception[0])) 
        vtor = etchvtor_cache_validator(
                 new_validator_exception(), etchvtor_cache_exception); 
    return vtor;
}


/**
 * etchvtor_exception_check_value()
 * check_value override for exception validator
 * returns external type of specified value
 */
int etchvtor_exception_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{     
    if (-1 == vtor->validate(vtor, (etch_object*) value)) return -1; 
    *typecode_out = ETCH_XTRNL_TYPECODE_CUSTOM;
    return 0;
}


/**
 * etchvtor_exception_element_validator()
 */
etch_validator* etchvtor_exception_element_validator(etch_validator* vtor)
{
    return etchvtor_exception_get(0);
}


/**
 * new_validator_exception()
 * constructor for exception validator
 */
etch_validator* new_validator_exception(const int numdimensions) 
{
    etch_validator* newvtor = new_type_validator_1(CLASSID_VALIDATOR_EXCEPTION,
        ETCHTYPEB_EXCEPTION, CLASSID_EXCEPTION, 0, CLASSID_ARRAY_OBJECT, 0, "excp");
    newvtor->check_value       = etchvtor_exception_check_value;
    newvtor->element_validator = etchvtor_exception_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * eod object validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

etch_validator** etchvtor_cache_eod  /* eod vtor cache address */
    = &etchvtor_cache[ETCHVTOR_CACHE_SLOT_EOD * ETCHVTOR_MAX_CACHED];

/**
 * etchvtor_eod_get()
 * get an eod object validator  
 */
etch_validator* etchvtor_eod_get()
{
    etch_validator* vtor = NULL;

    if (NULL == (vtor = etchvtor_cache_eod[0])) 
        vtor = etchvtor_cache_validator(
                 new_validator_eod(), etchvtor_cache_eod); 
    return vtor;
}


/**
 * etchvtor_eod_validate()
 * validate() override  
 */
int etchvtor_eod_validate(etch_validator* vtor, etch_object* value)
{
    return etchtagdata_is_eod(value)? 0: -1;
}


/**
 * new_validator_eod()
 * constructor for eod validator
 */
etch_validator* new_validator_eod(const int numdimensions) 
{
    etch_validator* newvtor = new_type_validator_1(CLASSID_VALIDATOR_EOD,
        ETCHTYPEB_NONE, CLASSID_NONE, 0, CLASSID_NONE, 0, "eod");
    newvtor->validate = etchvtor_eod_validate;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * struct validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * etchvtor_struct_get()
 * get a struct validator  
 */
etch_validator* etchvtor_struct_get(etch_type* type, const int numdims) 
{
    etch_validator* vtor = new_validator_struct(type, numdims);
    /* caller must free this validator since is_cached is false */  
    return vtor;
}


/**
 * etchvtor_struct_validate()
 * validate() override verifying value is struct of expected type
 */
int etchvtor_struct_validate(etch_validator* vtor, etch_object* value)
{
    int result = etch_typevtor_validate(vtor, value);
    if (result == -1);
    else
    if  (vtor->numdimensions) 
         if  (is_etch_nativearray(value));
         else result = -1;
    else
    if  (is_etch_struct(value))
         if  (is_equal_types(vtor->struct_type, 
                 ((etch_structvalue*)value)->struct_type));
         else result = -1;
    else result = -1;

    return result;
}


/**
 * etchvtor_struct_check_value()
 * check_value override for struct validator
 * returns external type of specified value
 */
int etchvtor_struct_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{     
    if (-1 == vtor->validate(vtor, (etch_object*) value)) return -1; 
    *typecode_out = vtor->numdimensions? 
         ETCH_XTRNL_TYPECODE_ARRAY: ETCH_XTRNL_TYPECODE_CUSTOM;
    return 0;
}


/**
 * etchvtor_struct_element_validator()
 */
etch_validator* etchvtor_struct_element_validator(etch_validator* vtor)
{    
    return etchvtor_struct_get(vtor->struct_type, vtor->numdimensions - 1);
}


char* construct_namebuf(etch_type* type, char* prefix, int numdimensions) 
{
    char *abuf = NULL, *namebuf = NULL;    
    // this mallocs mem in abuf
    etch_encoding_transcode_wchar(&abuf, ETCH_ENCODING_UTF8, type->name, NULL);
    namebuf = (char*) etch_malloc(7 + strlen(abuf) + 8 /* max length of dimension */ + 2 + 1, NULL);
    sprintf(namebuf, "%s%s[%d]", prefix, abuf, numdimensions);
    etch_free(abuf);  /* todo change id_name to ascii */     
    return namebuf;
}


/**
 * new_validator_struct()
 * constructor for struct validator
 */
etch_validator* new_validator_struct (etch_type* type, const int numdimensions)
{
    
    etch_validator* newvtor = NULL;
    char* namebuf = NULL;
    if (!type || !is_good_type(type)) return NULL;

    //this mallocs namebuf
    namebuf = construct_namebuf(type, "struct_", numdimensions);
    newvtor = new_type_validator_1(CLASSID_VALIDATOR_STRUCT, ETCHTYPEB_STRUCTVAL,
        CLASSID_STRUCTVALUE, 0, CLASSID_ARRAY_OBJECT, numdimensions, namebuf);
    etch_free(namebuf);
    newvtor->struct_type = type;
    newvtor->validate          = etchvtor_struct_validate;
    newvtor->check_value       = etchvtor_struct_check_value;
    newvtor->element_validator = etchvtor_struct_element_validator;
    return newvtor;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * custom validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * etchvtor_custom_get()
 * get a custom validator  
 */
etch_validator* etchvtor_custom_get (const unsigned short obj_type, 
    const unsigned short class_id, etch_type* type, const int numdims) 
{
    etch_validator* vtor = new_validator_custom (obj_type, class_id, type, numdims);
    /* caller must free this validator since is_cached is false */  
    return vtor;
}


/**
 * etchvtor_custom_validate()
 * validate() override verifying value is of expected type
 */
int etchvtor_custom_validate (etch_validator* vtor, etch_object* value)
{
    int result = etch_typevtor_validate(vtor, value);

    return result;
}


/**
 * etchvtor_struct_check_value()
 * check_value override for struct validator
 * returns external type of specified value.
 */
int etchvtor_custom_check_value(etch_validator* vtor, 
    etch_object* value, byte* typecode_out)
{     
    if (-1 == vtor->validate(vtor, (etch_object*) value)) return -1; 
    *typecode_out = vtor->numdimensions? 
         ETCH_XTRNL_TYPECODE_ARRAY: ETCH_XTRNL_TYPECODE_CUSTOM;
    return 0;
}


/**
 * etchvtor_custom_element_validator()
 */
etch_validator* etchvtor_custom_element_validator (etch_validator* vtor)
{
    return etchvtor_custom_get(vtor->expected_obj_type, vtor->expected_class_id,  vtor->struct_type, vtor->numdimensions - 1);
}


/**
 * new_validator_custom()
 * constructor for custom validator
 */
etch_validator* new_validator_custom (const unsigned short obj_type,
    const unsigned short class_id, etch_type* type, const int numdims)
{
    char* namebuf = NULL;
    etch_validator* newvtor = NULL;
    if (!type || !is_good_type(type)) return NULL;
    namebuf = construct_namebuf(type, "custom_", numdims);
    newvtor = new_type_validator_1(CLASSID_VALIDATOR_CUSTOM, obj_type, class_id, 0, CLASSID_ARRAY_OBJECT, numdims, namebuf);
    etch_free(namebuf);
    newvtor->struct_type = type;

    newvtor->validate          = etchvtor_custom_validate;
    newvtor->check_value       = etchvtor_custom_check_value;
    newvtor->element_validator = etchvtor_custom_element_validator;
    return newvtor;
}






