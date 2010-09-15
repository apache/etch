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

/**
 * etch_nativearray.c  
 * etch_nativearray implementation
 * does allocation and subscripting for n-dimensional arrays stored as a single
 * byte vector, to overcome the well-known C compiler issues with passing around
 * multi dimensioned arrays when the low order dimension is not known at compile
 * time. the mapping here from byte vector to array tests out as k&r compliant,
 * this is verified by constructing from a static array using new_etch_nativearray_from
 * and testing that the get methods return the same result as statically indexed
 * array items. as it stands, this code handles a max of 3 dimensions. it can be 
 * generalized to n dimensions, however the caller would have to allocate, populate, 
 * and pass arrays of subscripts for this to happen.
 */

#include <stdio.h>
#include "etch_nativearray.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"

etch_nativearray* init_etch_nativearray(unsigned short, const size_t, const int, const int, const int, const int);
int etch_nativearray_put1 (etch_nativearray*, void*, int);
int etch_nativearray_put2 (etch_nativearray*, void*, int, int);
int etch_nativearray_put3 (etch_nativearray*, void*, int, int, int);
int etch_nativearray_get1 (etch_nativearray*, void*, int);
int etch_nativearray_get2 (etch_nativearray*, void*, int, int);
int etch_nativearray_get3 (etch_nativearray*, void*, int, int, int);
int etch_nativearray_set_content_type(etch_nativearray*);

/*
 * new_etch_nativearray() 
 * etch_nativearray constructor
 * note that when creating arrays, the dimensions are passed low-order first,
 * so for x[2][3][4], dim0 is 4, dim1 is 3, dim2 is 2. for y[7][9], dim0 is 9,
 * dim1 is 7, and dim2 is zero of course.
 */
etch_nativearray* new_etch_nativearray(unsigned short class_id, const size_t itemsize,  
    const int numdims, const int dim0, const int dim1, const int dim2)
{
    etch_nativearray* newarray = init_etch_nativearray(class_id, itemsize, numdims, dim0, dim1, dim2);
    if (NULL == newarray) return NULL;

    newarray->values = etch_malloc(newarray->bytecount, ETCHTYPEB_BYTES);
    memset(newarray->values, 0, newarray->bytecount);
    newarray->is_content_owned = TRUE;
  
    etch_nativearray_set_content_type(newarray);
    return newarray;
}

/*
 * new_etch_nativearray_from() 
 * etch_nativearray constructor from existing byte vector
 * @param values source array, caller retains ownership.
 */
etch_nativearray* new_etch_nativearray_from(void* values, unsigned short class_id, const size_t itemsize, 
    const int numdims, const int dim0, const int dim1, const int dim2)
{
    etch_nativearray* newarray = init_etch_nativearray(class_id, itemsize, numdims, dim0, dim1, dim2);
    if (NULL == newarray) return NULL;

    newarray->values = values;
    newarray->is_content_owned = FALSE;

    etch_nativearray_set_content_type(newarray);
    return newarray;
}

/*
 * new_etch_nativearray_of() 
 * etch_nativearray constructor for array of objects of specified type and class
 */
etch_nativearray* new_etch_nativearray_of(unsigned short content_obj_type, unsigned short content_class_id, const int numdims, const int dim0, const int dim1, const int dim2)
{
    etch_nativearray* newarray = init_etch_nativearray(CLASSID_ARRAY_OBJECT, sizeof(void*), numdims, dim0, dim1, dim2);
    if (NULL == newarray) return NULL;

    newarray->content_obj_type = content_obj_type;
    newarray->content_class_id = content_class_id;
    return newarray;
}

/*
 * init_etch_nativearray() 
 * etch_nativearray private constructor
 * assumes max number of dimensions is 3
 */
etch_nativearray* init_etch_nativearray(unsigned short class_id, const size_t itemsize,  
    const int numdims, const int dim0, const int dim1, const int dim2)
{
    etch_nativearray* newarray = NULL;
    int i,j;
    if (numdims < 1 || itemsize < 1) return NULL;
    if (numdims > 1 && dim1 < 1)     return NULL;
    if (numdims > 2 && dim2 < 1)     return NULL;

    newarray = (etch_nativearray*)new_object(sizeof(etch_nativearray), ETCHTYPEB_NATIVEARRAY, class_id);

    newarray->numdims  = numdims;
    newarray->itemsize = itemsize;

    /* dimension array contains the array dimensions, 
     * e.g., for x[2][3][4], [0] is 4, [1] is 3, [2] is 2 
     */
    newarray->dimension[0] = dim0;
    newarray->dimension[1] = numdims > 1? dim1: 0;
    newarray->dimension[2] = numdims > 2? dim2: 0;

    /* dimsize array contains size in bytes of the next lower dimension, 
     * stored so that the values need not be calculated on each array access. 
     * [0] is item size, so for int x[5][7][9], [0] is sizeof(int), 
     * [1] is 9*sizeof(int), [2] is 7*9*sizeof(int)
     */
    newarray->dimsize[0] = itemsize;
    newarray->bytecount  = itemsize * dim0;

    for(i = 1, j = 0; i < numdims; i++, j++)
    {
        newarray->dimsize[i] = newarray->dimension[j] * newarray->dimsize[j];
        newarray->bytecount *= newarray->dimension[i];
    }

    ((etch_object*)newarray)->clone   = etch_nativearray_clone;
    ((etch_object*)newarray)->destroy = destroy_etch_nativearray;

    newarray->put1 = etch_nativearray_put1;
    newarray->put2 = etch_nativearray_put2;
    newarray->put3 = etch_nativearray_put3;
    newarray->get1 = etch_nativearray_get1;
    newarray->get2 = etch_nativearray_get2;
    newarray->get3 = etch_nativearray_get3;

    return newarray;      
}



void* etch_nativearray_clone(void* obj)
{
    etch_nativearray* source = (etch_nativearray*)obj;
    etch_nativearray* newarray = NULL;

    if(source == NULL) {
        return NULL;
    }

    newarray = new_etch_nativearray_from(source->values, source->content_class_id, source->itemsize, source->numdims, (int)source->dimension[0], (int)source->dimension[1], (int)source->dimension[2]);


    // always own copies of unwrapped content
    if(source->content_class_id == CLASSID_UNWRAPPED){
        newarray->values = etch_malloc(source->bytecount, ETCHTYPEB_BYTES);
        memcpy(newarray->values,source->values,source->bytecount);
        newarray->is_content_owned = TRUE;
    }else {
        //never own anything else
        newarray->is_content_owned = FALSE;
        newarray->values = source->values;
    }
    newarray->content_class_id = source->content_class_id;
    newarray->content_obj_type = source->content_obj_type;
    ((etch_object*)newarray)->class_id = ((etch_object*)source)->class_id;
    return newarray;

}

/**
 * destroy_etch_nativearray_content()
 * destroy nativearray's content vector.
 * return -1 if content not owned by nativearray and therefore not destroyed;
 * otherwise return zero, regardless of whether content was null.
 */
int destroy_etch_nativearray_content(etch_nativearray* a)
{
    int  result = 0;
    if  (!is_etchobj_static_content(a) && a->is_content_owned) {
        if(a->content_class_id != CLASSID_UNWRAPPED){
            size_t i = 0;
            size_t length = a->bytecount / sizeof(etch_object*);
            ETCH_ASSERT(a->bytecount % sizeof(etch_object*) == 0);
            for(; i < length; i++){
                void** temp = a->values;
                if( temp[i] ){
                    etch_object_destroy(temp[i]);
                }
            }
        }
        etch_free(a->values);  /* OK if values null */  
    }
 
    else result = -1;
    return result;
}


/**
 * destroy_etch_nativearray()
 * etch_nativearray destructor
 */
int destroy_etch_nativearray(void* obj) 
{
    etch_nativearray* a = (etch_nativearray*)obj;
    if (a->is_content_owned && !is_etchobj_static_content(a)) 
        destroy_etch_nativearray_content(a);
    destroy_objectex((etch_object*)a);
    return 0;
}


/*
 * new_subarray() 
 * construct and return subaarray[i] of this array.
 * subarray points into parent content and so does not own its content
 */
etch_nativearray* new_subarray(etch_nativearray* a, const int i) 
{
    etch_nativearray* subarray = NULL;
    const int sub_numdims = a->numdims - 1;

    if (a->numdims > 1 && i < (int) a->dimension[sub_numdims]) 
    {   
        const size_t sub_bytelen = a->dimsize[sub_numdims];
        const size_t vector_offset = i * sub_bytelen;
        byte* subvector = (byte*) a->values + vector_offset;

        subarray = new_etch_nativearray_from(subvector, 
            ((etch_object*)a)->class_id, (int) a->itemsize, sub_numdims,  
            (int) a->dimension[0], (int) a->dimension[1], 0);

        subarray->bytecount = sub_bytelen;
        subarray->content_obj_type = a->content_obj_type;
        subarray->content_class_id = a->content_class_id;
        subarray->is_content_owned = a->is_content_owned;

        subarray->counts[0] = a->counts[0]; /* user-maintained counts */  
        subarray->counts[1] = a->counts[1];  
        subarray->counts[2] = 0;
    }

    return subarray;
}


/*
 * etch_nativearray_get_element() 
 * return element[i] of specified array, which, if array is multi-dimensioned,
 * will be another native array, otherwise an etch object. for example, if
 * the supplied array is byte[2][4], a native array of byte[4] is returned.
 * if the supplied array is byte[4], an etch_byte object is returned.
 * returned subarrays do not own content, their content references the content
 * of their parent.
 */
etch_object* etch_nativearray_get_element(etch_nativearray* a, const int i) 
{
    int result = 0;
    etch_object* wrapped_element = NULL;

    const int sub_numdims = a->numdims - 1;
    if (sub_numdims < 0) return NULL;

    if (sub_numdims > 0) 
        return (etch_object*) new_subarray(a, i);

    result = etch_nativearray_get_wrapped_component(a, i, &wrapped_element);

    return wrapped_element;
}


/*
 * etch_nativearray_set_content_type() 
 * set content_obj_type and content_class_id based on array class_id.
 * returns zero if content type was set, otherwise -1
 */
int etch_nativearray_set_content_type(etch_nativearray* a)
{
    unsigned short obj_type = 0;
    int result = 0;

    switch(((etch_object*)a)->class_id) 
    {
        case CLASSID_ARRAY_BYTE:    obj_type = ETCHTYPEB_BYTE;   break;
        case CLASSID_ARRAY_BOOL:    obj_type = ETCHTYPEB_BOOL;   break;
        case CLASSID_ARRAY_INT8:    obj_type = ETCHTYPEB_INT8;   break;
        case CLASSID_ARRAY_INT16:   obj_type = ETCHTYPEB_INT16;  break;
        case CLASSID_ARRAY_INT32:   obj_type = ETCHTYPEB_INT32;  break;
        case CLASSID_ARRAY_INT64:   obj_type = ETCHTYPEB_INT64;  break;
        case CLASSID_ARRAY_FLOAT:   obj_type = ETCHTYPEB_IEEE32; break;
        case CLASSID_ARRAY_DOUBLE:  obj_type = ETCHTYPEB_IEEE64; break;
    }

    if (obj_type) {
        a->content_obj_type = obj_type;
        a->content_class_id = CLASSID_UNWRAPPED;
    }
    else switch(((etch_object*)a)->class_id) 
    {
        case CLASSID_ARRAY_OBJECT: 
            a->content_obj_type = ETCHTYPEB_ETCHOBJECT; 
            a->content_class_id = CLASSID_OBJECT;
            break;

        case CLASSID_ARRAY_STRING:  
            a->content_obj_type = ETCHTYPEB_STRING; 
            a->content_class_id = CLASSID_STRING;
            break;

        default:
            result = -1; /* indicates content type not set */
    }
    
    return result;
}


/**
 * put1()
 * insert item to singly dimensioned array
 * array access can be generalized by passing an array of subscripts; however
 * since we are currently handling max 3 dimensions, the calling sequence is 
 * much simpler when we have separate accessors for each dimensionality.
 */
int etch_nativearray_put1(etch_nativearray* a, void* value, int i) 
{
    size_t offset = etch_nativearray_off1(a, i);
    if (value == NULL || offset < 0) return -1;
    memcpy((byte*)a->values + offset, value, a->itemsize); 
    return 0;
}


/**
 * put2()
 * insert item to 2-dimensional array
 */
int etch_nativearray_put2(etch_nativearray* a, void* value, int i, int j) 
{
    size_t offset = etch_nativearray_off2(a, i, j);
    if (value == NULL || offset < 0) return -1;
    memcpy((byte*)a->values + offset, value, a->itemsize);  
    return 0;
}


/**
 * put3()
 * insert item to 3-dimensional array
 */
int etch_nativearray_put3(etch_nativearray* a, void* value, int i, int j, int k) 
{
    size_t offset = etch_nativearray_off3(a, i, j, k);
    if (value == NULL || offset < 0) return -1;
    memcpy((byte*)a->values + offset, value, a->itemsize); 
    return 0;
}


/**
 * get1()
 * access and return item from singly dimensioned array
 */
int etch_nativearray_get1(etch_nativearray* a, void* value, int i) 
{
    size_t offset = etch_nativearray_off1(a, i);
    if (value == NULL || offset < 0) return -1;
    memcpy(value, (byte*)a->values + offset, a->itemsize); 
    return 0;
}


/**
 * get2()
 * access and return item from 2-dimensional array
 */
int etch_nativearray_get2(etch_nativearray* a, void* value, int i, int j) 
{
    size_t offset = etch_nativearray_off2(a, i, j);
    if (value == NULL || offset < 0) return -1;
    memcpy(value, (byte*)a->values + offset, a->itemsize); 
    return 0;
}


/**
 * get3()
 * access and return item from 3-dimensional array
 */
int etch_nativearray_get3(etch_nativearray* a, void* value, int i, int j, int k) 
{
    size_t offset = etch_nativearray_off3(a, i, j, k);
    if (value == NULL || offset < 0) return -1;
    memcpy(value, (byte*)a->values + offset, a->itemsize); 
    return 0;
}


/*
 * etch_nativearray_off1() 
 * calculate byte offset into byte vector for a 1-dimension array
 */
size_t etch_nativearray_off1(etch_nativearray* a, int i)
{
    size_t offset = 0;    
    if (i >= (int) a->dimension[0] || i < 0) return -1; 

    offset = i * a->dimsize[0];
    return offset;
}


/*
 * etch_nativearray_off2() 
 * calculate byte offset into byte vector for a 2-dimension array
 */
size_t etch_nativearray_off2(etch_nativearray* a, int i, int j)
{
    size_t offset = 0;    
    if ((i >= (int) a->dimension[1] || i < 0)  
     || (j >= (int) a->dimension[0] || j < 0)) return -1; 

    offset = (i * a->dimsize[1]) + (j * a->dimsize[0]);
    return offset;
}


/*
 * etch_nativearray_off3() 
 * calculate byte offset into byte vector for a 3-dimension array
 */
size_t etch_nativearray_off3(etch_nativearray* a, int i, int j, int k)
{
    size_t offset = 0;    
    if ((i >= (int) a->dimension[2] || i < 0)  
     || (j >= (int) a->dimension[1] || j < 0)   
     || (k >= (int) a->dimension[0] || k < 0)) return -1; 

    offset = (i * a->dimsize[2]) + (j * a->dimsize[1]) + (k * a->dimsize[0]);
    return offset;
}




/** 
 * etch_nativearray_get_wrapped_component()
 * get wrapped component[i] from the single-dimensioned native array.
 * caller owns returned object.
 * todo: test that if array content was wrapped, the is_value_owned
 * setting is sufficient for tranparent memory management of returned object.
 */
int etch_nativearray_get_wrapped_component(etch_nativearray* a, const int i, etch_object** out)
{
    const int numentries = (int) a->dimension[0];
    const int is_wrapped_content = a->content_class_id != 0;
    int result = 0;
    union
    { char  vbyte; short  vint16; int   vint32; int64 vint64; 
      float vfloat;double vdouble;void* vaddr;  int64 all;
    } v;

    if (a->numdims != 1 || i >= numentries) return -1;
    v.all = 0;

    if (is_wrapped_content)
    {   /* content of the native array is wrapped, e.g, etch_object*,   
         * so we return the item as is */   
         result = a->get1(a, &v.vaddr, i);
         *out = v.vaddr;        
    }
    else 
    {
       
        
        switch(a->content_obj_type)
        {   /* content of the native array is unwrapped, e.g, int[], so we wrap   
             * the array item with an etch object prior to returning it */
            case ETCHTYPEB_BYTE: 
                 result = a->get1(a, &v.vbyte, i);
                 *out = (etch_object*) new_byte(v.vbyte);
                 break;
      
            case ETCHTYPEB_BOOL: 
                 result = a->get1(a, &v.vbyte, i);
                 *out = (etch_object*) new_boolean(v.vbyte);
                 break;   

            case ETCHTYPEB_INT8: 
                 result = a->get1(a, &v.vbyte, i);
                 *out = (etch_object*) new_int8(v.vbyte);
                 break; 
         
            case ETCHTYPEB_INT16:  
                 result = a->get1(a, &v.vint16, i);
                 *out = (etch_object*) new_int16(v.vint16);
                 break; 

            case ETCHTYPEB_INT32: 
                 result = a->get1(a, &v.vint32, i);
                 *out = (etch_object*) new_int32(v.vint32);
                 break; 

            case ETCHTYPEB_INT64: 
                 result = a->get1(a, &v.vint64, i);
                 *out = (etch_object*) new_int64(v.vint64);
                 break;  

            case ETCHTYPEB_IEEE32:  
                 result = a->get1(a, &v.vfloat, i);
                 *out = (etch_object*) new_float(v.vfloat);
                 break;      

            case ETCHTYPEB_IEEE64:
                 result = a->get1(a, &v.vdouble, i);
                 *out = (etch_object*) new_double(v.vdouble);
                 break; 

            case ETCHTYPEB_STRING: 
                 result = a->get1(a, &v.vaddr, i);
                 *out = (etch_object*) new_stringw(v.vaddr);
		 printf("warning\n");
                 break; 

            default: /* item is an anonymous pointer to a value owned by caller */
            {    
                result = a->get1(a, &v.vaddr, i);   
                *out = (etch_object*) v.vaddr;
            } 
        }
        /*
        if(out && *out && ! a->is_content_owned)
           set_etchobj_static_content((*out));
        */
    }  /* switch(a->content_obj_type) */ 

    return result;
}
