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
 * etch_nativearray.h -- etch_nativarray implementation.
 */
#ifndef ETCH_NATIVEARRAY_H
#define ETCH_NATIVEARRAY_H

/**
 * etch_nativearray  
 * object wrapper and methods for an n-dimensioned array of any type
 * represented in memory as a single byte vector.
 */

#include "etch_object.h"

#ifdef __cplusplus
extern "C" {
#endif

// etch_nativearray defines
#define ETCH_MAX_NATIVE_DIM 3 /* arbitrary limit on native array dimensions */

/**
 * etch_nativearray
 */
typedef struct etch_nativearray
{
    etch_object object;
    
    unsigned char   is_content_owned;
    
    void*  values;  /* flattened array content */
    unsigned short content_obj_type;  /* ETCHTYPEB_INT32 means content is int */
    unsigned short content_class_id;  /* CLASSID_NONE means content not wrapped */
    int  numdims;   /* number of dimensions, e.g., 2 for x[3][4]  */

    /* this object may be masked by etch_collection_mask to determine content
     * type and class, so do not add any fields above this comment */

    size_t itemsize;  /* size in bytes of an item, e.g. sizeof(int) */
    size_t bytecount; /* length in bytes of array content (values)  */
    size_t dimension[ETCH_MAX_NATIVE_DIM]; /* for int x[2][3] [0] is 3, [1] is 2  */
    size_t dimsize  [ETCH_MAX_NATIVE_DIM]; /* for int x[2][3] [0] is 4, [1] is 12 */
    size_t counts   [ETCH_MAX_NATIVE_DIM]; /* optional actual population counts */

    // dim size example
    // int[2][3] array 2 * 3
    //
    // int[0][0] 4
    // int[0][1] 8
    // int[0][2] 12
    // int[1][0] 16
    // int[1][1] 20
    // int[1][2] 24
    // | dim size 0 = 4                          | dim size 1 = 12 = 3 * sizeof(int)
    // | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 |

    int (*put1) (struct etch_nativearray*, void* v, int i); 
    int (*put2) (struct etch_nativearray*, void* v, int i, int j); 
    int (*put3) (struct etch_nativearray*, void* v, int i, int j, int k);
    int (*get1) (struct etch_nativearray*, void* v, int i); 
    int (*get2) (struct etch_nativearray*, void* v, int i, int j); 
    int (*get3) (struct etch_nativearray*, void* v, int i, int j, int k); 
} etch_nativearray;

etch_nativearray* new_etch_nativearray (unsigned short class_id, const size_t itemsize, const int numdims, const int dim0, const int dim1, const int dim2);
etch_nativearray* new_etch_nativearray_from (void* values, unsigned short class_id, const size_t itemsize, const int numdims, const int dim0, const int dim1, const int dim2);
etch_nativearray* new_etch_nativearray_of(unsigned short content_obj_type, unsigned short content_class_id, const int numdims, const int dim0, const int dim1, const int dim2);
etch_nativearray* new_subarray(etch_nativearray* a, const int i); etch_nativearray* etch_nativearray_assign_to(etch_nativearray*, etch_nativearray*);
etch_object* etch_nativearray_get_element(etch_nativearray*, const int i);
int etch_nativearray_get_component_type(etch_object* classobj, etch_component_type_params*);
int etch_nativearray_get_wrapped_component(etch_nativearray*, const int i, etch_object** out);
int destroy_etch_nativearray_content(etch_nativearray*);
int destroy_etch_nativearray(void*);
size_t etch_nativearray_off1 (etch_nativearray* a, int i);
size_t etch_nativearray_off2 (etch_nativearray* a, int i, int j);
size_t etch_nativearray_off3 (etch_nativearray* a, int i, int j, int k);
void* etch_nativearray_clone(void* other);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_MUTEX_H */
