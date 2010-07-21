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
 * etch_validator.h
 */

#ifndef ETCH_VALIDATOR_H
#define ETCH_VALIDATOR_H

#include "etch_object.h"
#include "etch_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCHVTOR_MAX_NDIMS  3  /* max array dimensions */
#define ETCHVTOR_MAX_CACHED 4  /* max cached vtors per type */

struct etch_validator;
typedef int (*etchvtor_validate)  (struct etch_validator*, etch_object*);
typedef int (*etchvtor_checkvalue)(struct etch_validator*, etch_object*, byte* out);
typedef etch_object* (*etchvtor_validate_value)(struct etch_validator*, etch_object*);
typedef struct etch_validator* (*etchvtor_element_validator)(struct etch_validator*);


/** 
 *  etch_validator
 *  vet message values 
 */
typedef struct etch_validator
{
    etch_object object;

    /**
     * etch_validator virtuals
     */
    etchvtor_validate   validate;
    etchvtor_checkvalue check_value;
    etchvtor_validate_value    validate_value;
    etchvtor_element_validator element_validator;

    /** 
     * etch_type_validator instance data
     */
    unsigned short  expected_obj_type;
    unsigned short  expected_class_id;
    int             numdimensions;
    char*           description;   /* owned */
    etch_type*      struct_type;   /* not owned */
    etchparentinfo* inherits_from; /* owned or not, see flag */
    unsigned char   is_cached;
    unsigned char   is_subclassable;
    unsigned char   is_owned_inherits_from;  
    unsigned char   unused;

    /* etch_combo_validator instance data
     */
    struct etch_validator* vtor_a; /* not owned */
    struct etch_validator* vtor_b; /* not owned */

} etch_validator;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * validator constructors and public methods
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
etch_validator* new_validator();

etch_validator* new_validator_from(etchvtor_validate, etchvtor_checkvalue, etchvtor_element_validator, etchvtor_validate_value);

etch_validator* new_type_validator_1
   (const unsigned short vtor_classid,
    const unsigned short scalar_obj_type,
    const unsigned short scalar_classid, 
    const unsigned short scalar_vtabkey,
    const unsigned short array_classid, 
    const int ndims, char* descr);

etch_validator* new_type_validator_2
   (const unsigned short vtor_classid,
    const unsigned short scalar_obj_type,
    const unsigned short scalar_classid, 
    const unsigned short array_classid, 
    etchparentinfo* inheritlist,
    const int ndims, char* descr);

int etchvtor_check_dimensions(const int);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * validator cache
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/* the validator cache is a single memory array, subdivided into caches for
 * each validator type, which caches instantiated validators. validators are 
 * cached to a max number of dimensions. end of service processing should 
 * iterate the static cache and free memory for validators cached therein,
 * in order for memory allocation tests to pass. validators have an is_cached
 * flag, regarded by the etch_validator dtor, so validator user can destroy()
 * validator regardless of its cache state, the dtor will check the flag and
 * refuse to destroy if cached. the final cleanup must therefore clear the
 * is_cached marker on each cached validator prior to calling destroy(). 
 */
#define ETCHVTOR_CACHED_TYPE_COUNT 12 /* count of cached validator types */
#define ETCHVTOR_BYTES_PER_CACHE (sizeof(void*) * ETCHVTOR_MAX_CACHED) 

// the validator cache
extern etch_validator* etchvtor_cache[ETCHVTOR_CACHED_TYPE_COUNT * ETCHVTOR_MAX_CACHED];

void etchvtor_clear_cache();


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * boolean validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_BOOLEAN  0   /* boolean validator cache slot index */
extern etch_validator** etchvtor_cache_boolean; /* boolean validator cache address */
etch_validator*  etchvtor_boolean_get(const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * byte validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_BYTE     1   /* byte validator cache slot index */
extern etch_validator** etchvtor_cache_byte;    /* byte validator cache address */
etch_validator*  etchvtor_byte_get(const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * int8 validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_INT8     2   /* int8 validator cache slot index */
extern etch_validator** etchvtor_cache_int8;    /* int8 validator cache address */
etch_validator*  etchvtor_int8_get(const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * int16 validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_INT16    3   /* int16 validator cache slot index */
extern etch_validator** etchvtor_cache_int16;   /* int16 validator cache address */
etch_validator*  etchvtor_int16_get(const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * int32 validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_INT32    4   /* int32 validator cache slot index */
extern etch_validator** etchvtor_cache_int32;   /* int32 validator cache address */
etch_validator*  etchvtor_int32_get(const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * int64 validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_INT64    5    /* int64 validator cache slot index */
extern etch_validator** etchvtor_cache_int64;    /* int64 validator cache address */
etch_validator*  etchvtor_int64_get(const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * float validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_FLOAT    6   /* float validator cache slot index */
extern etch_validator** etchvtor_cache_float;   /* float validator cache address */
etch_validator*  etchvtor_float_get (const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * double validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_DOUBLE    7   /* double validator cache slot index */
extern etch_validator** etchvtor_cache_double;   /* double validator cache address */
etch_validator*  etchvtor_double_get (const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * string validator
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_STRING    8   /* string validator cache slot index */
extern etch_validator** etchvtor_cache_string;   /* string validator cache address */
etch_validator*  etchvtor_string_get (const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * object validator (etch_object* anonymous wrapper)
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_OBJECT    9   /* object validator cache slot index */
extern etch_validator** etchvtor_cache_object;   /* object validator cache address */
etch_validator*  etchvtor_object_get (const int dimensions);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * exception validator 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_EXCEPTION 10   /* excp validator cache slot index */
extern etch_validator** etchvtor_cache_exception; /* excp validator cache address */
etch_validator*  etchvtor_exception_get(); 


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * eod_validator (end-of-data maker "none")
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#define ETCHVTOR_CACHE_SLOT_EOD    11  /* eod validator cache slot index */
extern etch_validator** etchvtor_cache_eod;   /* eod validator cache address */
etch_validator*  etchvtor_eod_get();


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * struct validator (not cached since unique per type)
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
etch_validator*  new_validator_struct();
etch_validator*  etchvtor_struct_get(etch_type*, const int numdims);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * combo validator (not cached)
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
etch_validator* new_combo_validator(etch_validator* vtor_a, etch_validator* vtor_b);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * custom validator (cached in a custom cache)
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
etch_validator* new_validator_custom(const unsigned short obj_type, const unsigned short class_id, etch_type*, const int numdims);
etch_validator* etchvtor_custom_get(const unsigned short obj_type, const unsigned short class_id, etch_type*, const int numdims);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_VALIDATOR_H */
