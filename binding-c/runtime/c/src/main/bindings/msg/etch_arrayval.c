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
 * etch_arrayval.c -- etch_arrayvalue implementation.
 *
 * todo: modify this class to permit using an etch_nativearray as the arrayvalue
 * backing store. this could be very useful for large arrays of small values,
 * where not all values are accessed individually. the key to this conversion  
 * is always returning a non-disposable object from get(), and we ensure this
 * by always returnings item[i] from the arraylist. when a get(i) is requested,
 * we check the arraylist first, lazy-allocating it if necessary. if arraylist[i]
 * is null, we populate arraylist[i] from natarray[i]. we then return arraylist[i]
 * which is always non-disposable.
 */

#include "etch_arrayval.h"
#include "etch_tagged_data.h"
#include "etch_nativearray.h"
#include "etch_exception.h"
#include "etch_objecttypes.h"

etch_arrayvalue* new_arrayvalue_init(const int, const int, const int, const int);
etch_arrayvalue* populate_arrayvalue_from(etch_arrayvalue*);
unsigned short etch_itemtype_to_arrayclass(unsigned short item_obj_type);
int  array_value_add(etch_arrayvalue* thisp, ETCH_ARRAY_ELEMENT* content);

typedef struct etch_arrayvalue_content_wrapper {
    etch_object object;
    void* value;
} etch_arrayvalue_content_wrapper;


/**
 * new_arrayvalue()
 * primary constructor for etch_arrayvalue
 * @param type_code wire ID of the array content class.
 * @param custom_struct_type non-disposable type of custom struct, 
   caller retains ownership as with all types.
 */
etch_arrayvalue* new_arrayvalue (const byte type_code, etch_type* custom_struct_type, 
    const int dim, const int initsize, const int deltsize, 
    const int is_readonly, const int is_synchronized)
{
    etch_arrayvalue* newobj = new_arrayvalue_init(initsize, deltsize, is_readonly, is_synchronized);

    newobj->dim = dim;
    newobj->type_code = type_code;
    ((etch_object*)newobj)->class_id  = etch_arraytype_to_classid(type_code);

    newobj->custom_struct_type = custom_struct_type; /* not owned */

    return newobj;
}


/**
 * new_arrayvalue_from()
 * etch_arrayvalue constructor - builds arryavalue from an etch_nativearray.
 * when native array is multi-dimensioned, this constructor is invoked recursively.
 * todo: convert this to use the subarray call, which this code duplicates.
 * todo: add a parameter to permit the arrayvalue to remain unpopulated; i.e.,
 * the nativearray is present but the object array is not yet populated.
 */
etch_arrayvalue* new_arrayvalue_from(etch_nativearray* natarray, 
    const signed char type_code, etch_type* custom_struct_type, 
    const int initsize, const int deltsize, const int is_readonly)
{
    const int    SUBARRAY_NUMDIMS = natarray->numdims - 1;
    const size_t SUBARRAY_COUNT   = natarray->dimension[SUBARRAY_NUMDIMS];
    const size_t SUBARRAY_BYTELEN = natarray->dimsize  [SUBARRAY_NUMDIMS];

    int i = 0;
    etch_arrayvalue* newav = NULL;  /* nativearray currently max 3 dims*/
    if (((etch_object*)natarray)->is_null) return NULL;
    if (SUBARRAY_NUMDIMS < 0 || SUBARRAY_NUMDIMS > 2) return NULL;

    newav = new_arrayvalue_init(initsize, deltsize, is_readonly, FALSE);
    newav->is_array_owned  = !is_readonly;   /* does av own nativearray */
    newav->type_code = type_code;       /* external array content type */
    ((etch_object*)newav)->class_id  = etch_arraytype_to_classid (type_code);
    newav->natarray  = natarray;
    newav->dim       = natarray->numdims;
    ((etch_object*)newav)->class_id  = ((etch_object*)natarray)->class_id;  /* validator expects class match */

    newav->custom_struct_type = custom_struct_type;  /* not owned */

    if  (SUBARRAY_NUMDIMS == 0) /* if single dimension, populate values */
    {
        
        if(natarray->content_class_id == CLASSID_UNWRAPPED){
            arrayvalue_set_static_content(newav, FALSE);
        }
        else{
            arrayvalue_set_static_content(newav, TRUE);
        }
        newav = populate_arrayvalue_from(newav);
    }
    else 
    {   newav->content_obj_type  = ETCHTYPEB_ARRAYVAL;
        newav->content_item_size = sizeof(void*);
        arrayvalue_set_static_content(newav, FALSE);

        for(; i < (const int) SUBARRAY_COUNT; i++)      
        {
            /* the native array was multi-dimensioned. we therefore add values    
             * to this arrayvalue, which are arrayalues of one dimension less than
             * that of the parent arrayvalue. the sub-arrayvalues are created from 
             * etch_nativearrays created from offset pointers into the parent
             * native array byte vector. sub-arrays therefore do not own content.
             */
            const int itemcount = (int) natarray->dimension[SUBARRAY_NUMDIMS-1];
            const size_t vector_offset = i * SUBARRAY_BYTELEN;
            byte* subvector = (byte*) natarray->values + vector_offset;
            etch_arrayvalue* subav = NULL;

            /* create a sub-array. note we pass a possibly unused dimension   
             * value in order to avoid the extra logic to not do so */
            etch_nativearray* newarray = new_etch_nativearray_from(subvector, ((etch_object*)natarray)->class_id, 
                natarray->itemsize, SUBARRAY_NUMDIMS,  
                (int) natarray->dimension[0], (int) natarray->dimension[1], 0);

            newarray->content_class_id  = natarray->content_class_id;
            newarray->content_obj_type  = natarray->content_obj_type;
            newarray->is_content_owned  = FALSE; /* pointer into parent vector */
            newarray->counts[0] = newarray->counts[0];  /* move counts in case */
            newarray->counts[1] = newarray->counts[1]; /* caller is using them */
            newarray->counts[2] = 0;
           
            subav = new_arrayvalue_from  /* recursively create a sub-arrayvalue */
                (newarray, type_code, custom_struct_type, itemcount, 0, is_readonly);
            subav->is_array_owned = TRUE;
            arrayvalue_add(newav, (ETCH_ARRAY_ELEMENT*) subav); 
        }
    }

    return newav;
}


/**
 * new_arrayvalue_default()
 * default constructor for etch_arrayvalue
 */
etch_arrayvalue* new_arrayvalue_default ()
{
    etch_arrayvalue* newobj = new_arrayvalue_init
        (ETCH_ARRAYVALUE_DEFAULT_INITSIZE, 
         ETCH_ARRAYVALUE_DEFAULT_DELTSIZE, 
         ETCH_ARRAYVALUE_DEFAULT_READONLY,
         ETCH_ARRAYVALUE_DEFAULT_SYNCHRONIZED);

    return newobj;
}

/**
 * destroy_array_value()
 * destructor for an etch_arrayvalue object
 */
int destroy_arrayvalue (void* data)
{
    etch_arrayvalue* thisp = (etch_arrayvalue*)data;

    if (!is_etchobj_static_content(thisp))
    {
        if (thisp->natarray && thisp->is_array_owned) 
            etch_object_destroy(thisp->natarray);

        etch_object_destroy(thisp->list);
    }

    destroy_objectex((etch_object*)thisp);
    return 0;
}


void* clone_etch_arrayvalue(void* data)
{
  etch_arrayvalue* other = (etch_arrayvalue*)data;
    etch_arrayvalue* newarray = NULL;
    if(other == NULL) 
        return NULL;

    newarray = new_arrayvalue(other->type_code,other->custom_struct_type,other->dim,2,2,TRUE,FALSE);
    
    etch_object_destroy(newarray->list);


    newarray->list = new_etch_arraylist_from(other->list);
    newarray->list->is_readonly = 1;

    return newarray;
}


/**
 * new_arrayvalue_init() 
 * common initialization on etch_arrayvalue construction.
 */
etch_arrayvalue* new_arrayvalue_init (const int initsize, const int deltsize, 
    const int is_readonly, const int is_synchronized)
{
    etch_arrayvalue* newobj = (etch_arrayvalue*) new_object(sizeof(etch_arrayvalue), 
         ETCHTYPEB_ARRAYVAL, CLASSID_ARRAYVALUE);

    ((etch_object*)newobj)->destroy = destroy_arrayvalue;
    ((etch_object*)newobj)->clone   = clone_etch_arrayvalue;

    /* the underlying arraylist is marked as content type object, meaning we can
     * interpret content as etchobject and call methods on the object accordingly,
     * most notably destroy(). when is_readonly is true, the underlying list will
     * not free memory for its content when the list is destroyed.
     */
    newobj->list = new_arrayvalue_arraylist(initsize, deltsize, is_readonly, is_synchronized);  

    return newobj;
}



/** 
 * populate_arrayvalue_from()
 * populate the specified arrayvalue from its attached single-dimensioned native array
 */
etch_arrayvalue* populate_arrayvalue_from (etch_arrayvalue* av)  
{
    etch_nativearray* nat = av->natarray;
    const int numentries  = (int) nat->dimension[0];
    etch_object* wrapped_value = NULL;
    int i = 0, result = 0;
    if (nat->numdims != 1) return NULL;

    /* TODO: check itemsize if this is always the size of the real element size */
    av->content_obj_type  = nat->content_obj_type;
    av->content_item_size = (short) nat->itemsize;

    for(; i < numentries; i++) {
        result = etch_nativearray_get_wrapped_component(nat, i, &wrapped_value);
        arrayvalue_add(av, (ETCH_ARRAY_ELEMENT*) wrapped_value); 
    }

    return av;
}


/**
 * arrayvalue3_to_nativearray()
 * convert an arrayvalue of dimension 3 to a native array. assumes that an
 * appropriately sized and configured etch_nativearray object is resident in
 * the arrayvalue object.
 */
int arrayvalue3_to_nativearray(etch_arrayvalue* av2)
{
    const int item2count = arrayvalue_count(av2);
    int item1count = 0, item0count = 0;
    etch_nativearray* natv = av2->natarray;
    etch_arrayvalue_content_wrapper* wrapper = NULL;
    etch_arrayvalue *av1 = NULL, *av0 = NULL;
    int i, j, k, items=0; 

    for(i=0; i < item2count; i++)
    {    
        av1 = arrayvalue_get(av2, i);
        if (!is_etch_arrayvalue(av1)) return -1; 
        item1count = arrayvalue_count(av1);

        for(j=0; j < (const int) item1count; j++)
        {   
            av0 = arrayvalue_get(av1, j);
            if (!is_etch_arrayvalue(av0)) return -1; 
            item0count = arrayvalue_count(av0);

            for(k=0; k < (const int) item0count; k++)
            {  /* insert native values into nativearray byte vector. we mask the
                * wrapped primitive with an etch_object. we can do so because the
                * etch primitive object is guaranteed to offset its value the
                * same as that of an etch_object, right after the object header. 
                */ 
                if (NULL == (wrapper = arrayvalue_get(av0, k))) return -1;
                if (0 == natv->put3(natv, &wrapper->value, i, j, k))
                    items++;
                else return -1;
            }   
        }    
    }    

    return items;
}


/**
 * arrayvalue2_to_nativearray()
 * convert an arrayvalue of dimension 2 to a native array. assumes that an
 * appropriately sized and configured etch_nativearray object is resident in
 * the arrayvalue object.
 */
int arrayvalue2_to_nativearray(etch_arrayvalue* av1)
{
    const int item1count = arrayvalue_count(av1);
    int item0count = 0;
    etch_nativearray* natv = av1->natarray;
    etch_arrayvalue_content_wrapper* wrapper = NULL;
    etch_arrayvalue *av0 = NULL;
    int i, j, items=0; 

    for(i = 0; i < item1count; i++)
    {    
        av0 = arrayvalue_get(av1, i);
        if (!is_etch_arrayvalue(av0)) return -1; 
        item0count = arrayvalue_count(av0);

        for(j = 0; j < (const int) item0count; j++)
        {   /* insert native values into nativearray byte vector. we mask the
             * wrapped primitive with an etch_object. we can do so because the
             * etch primitive object is guaranteed to offset its value the
             * same as that of an etch_object, right after the object header. 
             */ 
            if (NULL == (wrapper = arrayvalue_get(av0, j))) return -1;
            if (0 == natv->put2(natv, &wrapper->value, i, j))
                items++;
            else return -1;        
        }   
    }    

    return items;
}


/**
 * arrayvalue1_to_nativearray()
 * convert an arrayvalue of dimension 1 to a native array. assumes that an
 * appropriately sized and configured etch_nativearray object is resident in
 * the arrayvalue object.
 */
int arrayvalue1_to_nativearray(etch_arrayvalue* av)
{
    const int itemcount = arrayvalue_count(av);
    etch_nativearray* natv = av->natarray;
    etch_arrayvalue_content_wrapper* wrapper = NULL;
    int i=0, items = 0;  
   
    for(; i < itemcount; i++)
    {   /* insert native values into nativearray byte vector. we mask the
         * wrapped primitive with an etch_object. we can do so because the
         * etch primitive object is guaranteed to offset its value the
         * same as that of an etch_object, right after the object header. 
         */ 
        if (NULL == (wrapper = arrayvalue_get(av, i))) return -1;
        if (0 == natv->put1(natv, &wrapper->value, i))
            items++;
        else return -1;    
    }

    return items;
}


/**
 * arrayvalue_to_nativearray()
 * convert an arrayvalue to a native array
 * if the arrayvalue is created from a native array, the source native array is
 * still resident, however this method assumes we want to create the native array
 * regardless of whether one is resident. is also assumes that if we have created
 * the arrayvalue from scratch, we have done so properly, i.e., a multi-dimensioned
 * arrayvalue is and arrayvalue of arrayvalues. It additionally assumes that the
 * arrayvalue is populated with at least one value; if this were not the case we  
 * would need to switch on the byte type_code to determine the item size.
 */
int arrayvalue_to_nativearray (etch_arrayvalue* av)
{
    int dim0count=0, dim1count=0, dim2count=0, itemsize=0;
    etch_arrayvalue *av2 = NULL, *av1 = NULL, *av0 = NULL;
    unsigned short array_class_id = 0;
    etch_nativearray* newna = NULL;
    const int dimensions = av->dim;

    switch(dimensions)  /* determine native array size */
    {   case 1:
            av0 = av;
            break;

        case 2:
            dim1count = arrayvalue_count(av);
            av1 = (etch_arrayvalue*) arrayvalue_get(av, 0);
            if (is_etch_arrayvalue(av1)) 
                av0 = av1;
            break;

        case 3:
            dim2count = arrayvalue_count(av);
            av2 = (etch_arrayvalue*) arrayvalue_get(av, 0);
            if (!is_etch_arrayvalue(av2)) break;

            dim1count = arrayvalue_count(av2);
            av1 = (etch_arrayvalue*) arrayvalue_get(av2, 0);
            if (is_etch_arrayvalue(av1)) 
                av0 = av1;
    }

    if (av0) /* if arrayvalue was populated ... */
    {   itemsize  = av0->content_item_size;
        dim0count = arrayvalue_count(av0);
        if(av0->content_class_id == CLASSID_UNWRAPPED){
           array_class_id = CLASSID_UNWRAPPED;
        }else{
           array_class_id = etch_itemtype_to_arrayclass(av0->content_obj_type);
        }
    }

    if (dim0count == 0 || itemsize == 0) 
        return NULL;  /* bad arrayvalue */

    newna = new_etch_nativearray  /* allocate appropriately sized native array */
       (array_class_id, itemsize, dimensions, dim0count, dim1count, dim2count);
    if (NULL == newna) return NULL;

    /* if previous native array was attached to the arrayvalue, free it */
    etch_object_destroy(av->natarray);

    av->natarray = newna;  /* attach new native array to its arrayvalue */

    switch(dimensions)    /* populate new native array from array value */
    {   case 1: return arrayvalue1_to_nativearray(av);
        case 2: return arrayvalue2_to_nativearray(av);  
        case 3: return arrayvalue3_to_nativearray(av);  
    }

    return NULL;
}


/**
 * arrayvalue_add()
 * returns 0 or -1
 */
int arrayvalue_add (etch_arrayvalue* av, ETCH_ARRAY_ELEMENT* content)
{
    const int result = etch_arraylist_add(av->list, content);
    return result;
}


/**
 * array_value_add()
 * returns 0 or -1
 */
int array_value_add (etch_arrayvalue* av, ETCH_ARRAY_ELEMENT* content)
{
    const int result = etch_arraylist_add(av->list, content);
    return result;
}


/**
 * arrayvalue_get()
 * return item at specified index
 * returns array item, always an address, or NULL.
 */
void* arrayvalue_get (etch_arrayvalue* thisp, const int i) 
{
    return etch_arraylist_get(thisp->list, i);
}


/**
 * arrayvalue_count()
 * return item count
 */
int arrayvalue_count (etch_arrayvalue* thisp)
{
    return thisp? thisp->list? thisp->list->count: 0: 0;
}


/**
 * new_arrayvalue_arraylist()
 * allocates and returns an arraylist configured appropriately for use as arrayvalue backing store
 */
etch_arraylist* new_arrayvalue_arraylist (const int initsize, const int deltsize, 
    const int is_readonly, const int is_synchronized)
{
    etch_arraylist* list = is_synchronized? 
        new_etch_arraylist_synchronized(initsize, deltsize):
        new_etch_arraylist(initsize, deltsize);

    list->is_readonly = is_readonly != 0;  
    list->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    return list;
}


static const signed char etch_primitive_typecodes[11] 
 = {ETCH_XTRNL_TYPECODE_CUSTOM,       /* CLASSID_ARRAY_OBJECT */
    ETCH_XTRNL_TYPECODE_BYTE,         /* CLASSID_ARRAY_BYTE   */
    ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, /* CLASSID_ARRAY_BOOL   */
    ETCH_XTRNL_TYPECODE_BYTE,         /* CLASSID_ARRAY_BYTE   */
    ETCH_XTRNL_TYPECODE_SHORT,        /* CLASSID_ARRAY_INT16  */
    ETCH_XTRNL_TYPECODE_INT,          /* CLASSID_ARRAY_INT32  */
    ETCH_XTRNL_TYPECODE_LONG,         /* CLASSID_ARRAY_INT64  */
    ETCH_XTRNL_TYPECODE_FLOAT,        /* CLASSID_ARRAY_FLOAT  */
    ETCH_XTRNL_TYPECODE_DOUBLE,       /* CLASSID_ARRAY_DOUBLE */
    ETCH_XTRNL_TYPECODE_STRING,       /* CLASSID_ARRAY_STRING */
    0,        
   };

static const unsigned short etch_arrayval_classids[11] 
 = {CLASSID_ARRAY_OBJECT,       /* ETCH_XTRNL_TYPECODE_CUSTOM */
    CLASSID_ARRAY_BYTE,         /* ETCH_XTRNL_TYPECODE_BYTE   */
    CLASSID_ARRAY_BOOL,         /* ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE */
    CLASSID_ARRAY_BYTE,         /* ETCH_XTRNL_TYPECODE_BYTE   */
    CLASSID_ARRAY_INT16,        /* ETCH_XTRNL_TYPECODE_SHORT  */
    CLASSID_ARRAY_INT32,        /* ETCH_XTRNL_TYPECODE_INT    */
    CLASSID_ARRAY_INT64,        /* ETCH_XTRNL_TYPECODE_LONG   */
    CLASSID_ARRAY_FLOAT,        /* ETCH_XTRNL_TYPECODE_FLOAT  */
    CLASSID_ARRAY_DOUBLE,       /* ETCH_XTRNL_TYPECODE_DOUBLE */
    CLASSID_ARRAY_STRING,       /* ETCH_XTRNL_TYPECODE_STRING */
    0,
   };


/**
 * etch_arraytype_to_classid()
 * returns array class ID corresponding to serialization byte code.
 */
unsigned short etch_arraytype_to_classid (const signed char typecode)
{
    int i = 0;
    unsigned short idout = 0;
    signed char* p = (signed char*) etch_primitive_typecodes;

    for(; *p; i++, p++)
        if (*p == typecode)
        {   idout = etch_arrayval_classids[i];
            break;
        }

    return idout;
}


/**
 * etch_classid_to_arraytype()
 * returns serialization bytecode corresponding to array class ID
 */
signed char etch_classid_to_arraytype (const unsigned short class_id)
{
    int i = 0;
    signed char typecodeout = 0;
    unsigned short* p = (unsigned short*) etch_arrayval_classids;

    for(; *p; i++, p++)
        if (*p == class_id)
        {   typecodeout = etch_primitive_typecodes[i];
            break;
        }

    return typecodeout;
}


/**
 * etch_itemtype_to_arrayclass()
 * returns array object type corresponding to item object type 
 */
unsigned short etch_itemtype_to_arrayclass(unsigned short item_obj_type)
{ 
    if  (item_obj_type > 0 && item_obj_type <= ETCHTYPEB_STRING) 
         return etch_arrayval_classids[item_obj_type]; 
    else return CLASSID_ARRAY_OBJECT;
}


/**
 * arrayvalue_get_external_typecode() 
 * returns an array type bytecode for the specified array content type.
 */
signed char arrayvalue_get_external_typecode (unsigned short obj_type, unsigned short class_id)
{
    signed char xtype = 0;

    #if(0) /* uncompiled arrays are here for documentation purposes */

    unsigned short primitive_class_ids[] =
    {   CLASSID_ANY,             /* 0x0 */              
        CLASSID_PRIMITIVE_BYTE,  /* 0x1 */
        CLASSID_PRIMITIVE_BOOL,  /* 0x2 */
        CLASSID_PRIMITIVE_INT8,  /* 0x3 */
        CLASSID_PRIMITIVE_INT16, /* 0x4 */
        CLASSID_PRIMITIVE_INT32, /* 0x5 */
        CLASSID_PRIMITIVE_INT64, /* 0x6 */
        CLASSID_PRIMITIVE_FLOAT, /* 0x7 */
        CLASSID_PRIMITIVE_DOUBLE,/* 0x8 */
        CLASSID_STRING,          /* 0x9 */
    };

    unsigned short primitive_obj_types[] =
    {   ETCHTYPEB_UNDEFINED, /* 0x0 */   
        ETCHTYPEB_BYTE,      /* 0x1 */
        ETCHTYPEB_BOOL,      /* 0x2 */  
        ETCHTYPEB_INT8,      /* 0x3 */    
        ETCHTYPEB_INT16,     /* 0x4 */     
        ETCHTYPEB_INT32,     /* 0x5 */  
        ETCHTYPEB_INT64,     /* 0x6 */   
        ETCHTYPEB_IEEE32,    /* 0x7 */  
        ETCHTYPEB_IEEE64,    /* 0x8 */      
        ETCHTYPEB_STRING,    /* 0x9 */     
    };

    #endif

    if (obj_type == ETCHTYPEB_PRIMITIVE)
    {   if (class_id < 1 || class_id > 9)
            class_id = 0;

        xtype = etch_primitive_typecodes [class_id];
    }    
    else /* primitive obj_type? */
    if (obj_type > 0 && obj_type <= 9)
        xtype = etch_primitive_typecodes [obj_type];

    else switch(obj_type)
    {   case ETCHTYPEB_STRUCTVAL:   xtype = ETCH_XTRNL_TYPECODE_CUSTOM; break; 
        case ETCHTYPEB_ARRAYVAL:    xtype = ETCH_XTRNL_TYPECODE_ARRAY;  break;
        case ETCHTYPEB_NATIVEARRAY: xtype = ETCH_XTRNL_TYPECODE_ARRAY;  break;
        default: xtype = ETCH_XTRNL_TYPECODE_CUSTOM;   
    }
    
    return xtype;
}


/* 
 * arrayvalue_set_static_content()
 * configure arraylist of object wrappers such that objects are not freed by the arraylist
 * destructor. presumably this would be used during recursive access to arrayvalue, where
 * lowest level objects are seen and destroyed prior to higher level objects such as the
 * array wrappers. 
 */
void arrayvalue_set_static_content (etch_arrayvalue* av, const int is_set)
{
    etch_arraylist* list = av? av->list: NULL;
    if (list) list->is_readonly = is_set? TRUE: FALSE;
}


/* 
 * arrayvalue_set_iterator()
 * set an iterator on the arrayvalue, which becomes an iterator on its list
 */
int arrayvalue_set_iterator (etch_arrayvalue* av, etch_iterator* iterator)
{
    int rv = 0;
    if(av == NULL || iterator == NULL) {
        return -1;
    }
    if(av->list == NULL) {
        return -1;
    }

    rv = set_iterator (iterator, av->list, &av->list->iterable);
    return rv;
}

