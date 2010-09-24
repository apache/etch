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
 * etch_binary_tdo.c -- binary tagged data output implementation.
 */

#include "etch_runtime.h"
#include "etch_binary_tdo.h"
#include "etch_default_value_factory.h"
#include "etch_cache.h"
#include "etch_arrayval.h"
#include "etch_encoding.h"
#include "etch_nativearray.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"

static const char* LOG_CATEGORY = "etch_binary_tdo";


byte bintagdata_get_native_typecode(const unsigned short, const unsigned short);

/* - - - - - - - - - - - - - - - - - - - -
 * private signatures
 * - - - - - - - - - - - - - - - - - - - -
 */

i_binary_tdo* new_binarytdo_vtable();
int bintdo_start_message(tagged_data_output*, etch_message*);
int bintdo_write_message(tagged_data_output*, etch_message*, etch_flexbuffer*);
int bintdo_end_message  (tagged_data_output*, etch_message*);

int bintdo_start_struct (tagged_data_output*, etch_structvalue*);
int bintdo_write_struct (tagged_data_output*, etch_structvalue*);
int bintdo_end_struct   (tagged_data_output*, etch_structvalue*);

int bintdo_start_array  (tagged_data_output*, etch_arrayvalue*);
int bintdo_write_array  (tagged_data_output*, etch_arrayvalue*, etch_validator*);             
int bintdo_end_array    (tagged_data_output*, etch_arrayvalue*);

int bintdo_write_type  (binary_tagged_data_output*, etch_type*);
int bintdo_write_values(binary_tagged_data_output*, etch_arrayvalue*, etch_validator*);
int bintdo_write_keys_values(binary_tagged_data_output*, etch_structvalue*);
int bintdo_write_bytes_from  (binary_tagged_data_output*, etch_nativearray*);
int bintdo_write_value_rawint(binary_tagged_data_output*, const int);
int bintdo_write_intvalue (binary_tagged_data_output*, const int);
int bintdo_write_nonevalue(binary_tagged_data_output*);


/* - - - - - - - - - - - - - - - - - - - -
 * constructors/destructors
 * - - - - - - - - - - - - - - - - - - - -
 */


/**
 * destroy_binary_tagged_data_output()
 */
int destroy_binary_tagged_data_output(void* data)
{
    binary_tagged_data_output* tdo = (binary_tagged_data_output*)data;
    if (!is_etchobj_static_content(tdo))
    {
        etch_object_destroy(tdo->impl);

        if (tdo->flexbuf && tdo->is_flexbuf_owned)
            etch_object_destroy(tdo->flexbuf);
    }

    /* destroy private instance data */
    etch_free(tdo->static_nullobj);
    etch_free(tdo->static_eodmarker);
    clear_etchobj_static_all(tdo->static_emptystring);
    etch_object_destroy(tdo->static_emptystring);
     
    return destroy_objectex((etch_object*)tdo);
}

/**
 * clone_tagged_data_output()
 * tdo copy constructor. if the tdo object implements a separate instance data
 * object, that object is cloned as well. 
 */
void* clone_binary_tagged_data_output(void* data)
{
    binary_tagged_data_output* tdo = (binary_tagged_data_output*)data;
    binary_tagged_data_output* newtdo = (binary_tagged_data_output*) clone_object((etch_object*) tdo);
    newtdo->impl = tdo->impl?  tdo->impl->clone(tdo->impl): NULL;
    return newtdo;
}

/**
 * new_binary_tdo()
 * binary_tagged_data_output constructor
 * @param vf a value factory. can be null. caller retains ownership.
 * @param fbuf the buffer to write to. can be null. caller retains ownership.
 */
binary_tagged_data_output* new_binary_tagdata_output(etch_value_factory* vf, etch_flexbuffer* fbuf) 
{
    i_binary_tdo* vtab = NULL;

    binary_tagged_data_output* tdo  = (binary_tagged_data_output*) new_object
        (sizeof(binary_tagged_data_output), ETCHTYPEB_TAGDATAOUT, CLASSID_TAGDATAOUT);

    ((etch_object*)tdo)->destroy = destroy_binary_tagged_data_output;
    ((etch_object*)tdo)->clone   = clone_binary_tagged_data_output;
    tdo->flexbuf = fbuf;  /* if caller passes buffer, caller owns it */ 
    if (fbuf) tdo->is_flexbuf_owned = FALSE;
    tdo->vf = vf;

    vtab = etch_cache_find(get_vtable_cachehkey(CLASSID_BINARYTDO_VTAB), 0);

    if(!vtab)  
    {   vtab = new_binarytdo_vtable();
        etch_cache_insert(((etch_object*)vtab)->get_hashkey(vtab), vtab, FALSE);
    } 

    ((etch_object*)tdo)->vtab = (vtabmask*)vtab; 
    tdo->vtor_eod = etchvtor_eod_get();  
    tdo->vtor_int = etchvtor_int32_get(0); 
    tdo->static_nullobj     = etchtagdata_new_nullobj(TRUE);
    tdo->static_eodmarker   = etchtagdata_new_eodmarker(TRUE);
    tdo->static_emptystring = etchtagdata_new_emptystring(TRUE);
    return tdo; 
}


/**
 * new_binary_tdo()
 * casts result to generic tdo for use by interfaces
 */
tagged_data_output* new_binary_tdo(etch_value_factory* vf)
{
    return (tagged_data_output*) new_binary_tagdata_output(vf, NULL);
}






/**
 * new_new_binarytdo_vtable()
 * constructor for binary tdo virtual function table
 */
i_binary_tdo* new_binarytdo_vtable()
{
    etchparentinfo* inheritlist = new_etch_inheritance_list(3, 2, NULL); 
   
    i_binary_tdo* vtab 
        = new_vtable(NULL, sizeof(i_binary_tdo), CLASSID_BINARYTDO_VTAB);

    /* i_tagged_data_input */
    vtab->start_message = bintdo_start_message;
    vtab->write_message = bintdo_write_message;
    vtab->end_message   = bintdo_end_message;
    vtab->start_struct  = bintdo_start_struct;
    vtab->write_struct  = bintdo_write_struct;
    vtab->end_struct    = bintdo_end_struct;
    vtab->start_array   = bintdo_start_array;
    vtab->write_array   = bintdo_write_array;
    vtab->end_array     = bintdo_end_array;

    /* i_tagdata */
    #if(0)
    vtab->check_value = etchtagdata_check_value;
    vtab->get_native_type = bintdo_get_native_type;
    vtab->get_native_type_code  = bintdo_get_native_typecode;
    vtab->get_custom_structtype = bintdo_get_custom_structtype;
    #endif

    /* inheritance */
    inheritlist[1].o.obj_type = ETCHTYPEB_TAGDATAOUT;
    inheritlist[1].c.class_id = CLASSID_TAGDATAOUT;
    inheritlist[2].o.obj_type = ETCHTYPEB_TAGDATA;
    inheritlist[2].c.class_id = CLASSID_TAGDATA;

    vtab->inherits_from = inheritlist;

    return vtab;
}


/* - - - - - - - - - - - - - - - - - - - -
 * write message
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdo_start_message()
 * message is unique among serialized objects in that since message is at the
 * top level, no type byte is written to mark the start of a message. a version
 * number is written to identify the btd implementation version used to write
 * the message.
 */
int bintdo_start_message(tagged_data_output* tdox, etch_message* msg)   
{
    binary_tagged_data_output* tdo = (binary_tagged_data_output*) tdox;
    ETCH_ASSERT(tdo && msg);
    etch_flexbuf_put_byte(tdo->flexbuf, ETCH_BINTAGDATA_CURRENT_VERSION);
    return bintdo_start_struct(tdox, msg->sv);
}


 /**
 * bintdo_write_message()
 * message is unique among serialized objects in that since message is at the
 * top level, no type byte is written to mark the start of a message. a version
 * number is written to identify the btd implementation version used to write
 * the message.
 */
int bintdo_write_message(tagged_data_output* tdox, etch_message* msg, etch_flexbuffer* fbuf)
{
    binary_tagged_data_output* tdo = (binary_tagged_data_output*) tdox;
    ETCH_ASSERT(tdo && msg && fbuf);
    tdo->flexbuf = fbuf;

    if (-1 == bintdo_start_message(tdox, msg)) return -1; 

    if (-1 == bintdo_write_keys_values(tdo, msg->sv)) return -1;

    return bintdo_end_message(tdox, msg);
}


/**
 * tdo_end_message()
 * marks the end of the message in process.
 */
int bintdo_end_message(tagged_data_output* tdox, etch_message* msg)   
{
    return bintdo_end_struct(tdox, msg->sv);
}


/* - - - - - - - - - - - - - - - - - - - -
 * write struct
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdo_start_struct()
 * write the beginning of struct data. 
 */
int bintdo_start_struct(tagged_data_output* tdox, etch_structvalue* sv)   
{
    binary_tagged_data_output* tdo = (binary_tagged_data_output*) tdox;
    ETCH_ASSERT(tdo && sv);
    /* caller has already written a bytecode to the buffer indicating struct follows */

    if (-1 == bintdo_write_type(tdo, sv->struct_type)) return -1;

    return bintdo_write_value_rawint(tdo, structvalue_count(sv));  
}


/**
 * bintdo_write_struct()
 */
int bintdo_write_struct(tagged_data_output* tdox, etch_structvalue* sv)  
{
    ETCH_ASSERT(tdox && sv);

    if (-1 == bintdo_start_struct(tdox, sv)) return -1;

    if (-1 == bintdo_write_keys_values((binary_tagged_data_output*)tdox, sv)) return -1;

    return bintdo_end_struct(tdox, sv);
}


/**
 * bintdo_end_struct()
 * mark end of specified struct
 */
int bintdo_end_struct(tagged_data_output* tdox, etch_structvalue* sv)   
{
    return bintdo_write_nonevalue((binary_tagged_data_output*) tdox);
}


/* - - - - - - - - - - - - - - - - - - - -
 * write array
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdo_start_array()
 * starts writing of an array 
 */
int bintdo_start_array (tagged_data_output* tdox, etch_arrayvalue* av)   
{
    int errs = 0;
    binary_tagged_data_output* tdo = (binary_tagged_data_output*) tdox;
    ETCH_ASSERT(tdo && av);

    etch_flexbuf_put_byte (tdo->flexbuf, av->type_code);

    if (ETCH_XTRNL_TYPECODE_CUSTOM == av->type_code)
        errs += bintdo_write_type (tdo, av->custom_struct_type);

    errs += bintdo_write_value_rawint (tdo, av->dim); 
    errs += bintdo_write_value_rawint (tdo, arrayvalue_count(av));   
    return errs? -1: 0;
}


/**
 * bintdo_write_array()
 */
int bintdo_write_array (tagged_data_output* tdox, etch_arrayvalue* av, etch_validator* vtor)
{
    ETCH_ASSERT(tdox && vtor);

    if (!is_etch_arrayvalue(av)) return -1;

    if (-1 == bintdo_start_array (tdox, av)) return -1;

    if (-1 == bintdo_write_values ((binary_tagged_data_output*) tdox, av, vtor)) 
        return -1;

    return bintdo_end_array(tdox, av);
}


/**
 * bintdo_end_array()
 * writes end of the array being read.
 */
int bintdo_end_array (tagged_data_output* tdo, etch_arrayvalue* av)  
{
    return bintdo_write_nonevalue ((binary_tagged_data_output*) tdo);
}


/**
 * bintdo_to_arrayvalue()
 * convert supplied native array to an etch_arrayvalue.
 * @param na the native array. caller retains.
 * @return an etch_arrayvalue. caller owns it.
 */
etch_arrayvalue* bintdo_to_arrayvalue (binary_tagged_data_output* tdo, etch_nativearray* na)
{
    etch_type* NULLTYPE = NULL;

    signed char content_typecode = arrayvalue_get_external_typecode(na->content_obj_type, na->content_class_id);

    /* todo we should calculate array size from native array 
     * metadata rather than creating it using a default size */
    etch_arrayvalue* av = NULL;
	if(((etch_object*)na)->class_id != CLASSID_ARRAY_OBJECT && content_typecode == ETCH_XTRNL_TYPECODE_CUSTOM) {
		etch_type* theType = NULL;
		//theType = class_to_type_map_get(tdo->vf)->class_to_type, na->content_class_id);
		theType = ((struct i_value_factory*)((etch_object*)tdo->vf)->vtab)->get_custom_struct_type(tdo->vf, ETCHMAKECLASS(na->content_obj_type, na->content_class_id));
		av = new_arrayvalue_from (na, content_typecode, theType, ETCH_DEFSIZE, ETCH_DEFSIZE, TRUE);
		av->content_class_id = na->content_class_id;
		av->custom_struct_type = theType;
    }
    else
    if(((etch_object*)na)->class_id == CLASSID_ARRAY_OBJECT && content_typecode == ETCH_XTRNL_TYPECODE_CUSTOM) {
        av = new_arrayvalue_from (na, ETCH_XTRNL_TYPECODE_ANY, NULLTYPE, ETCH_DEFSIZE, ETCH_DEFSIZE, TRUE);
        av->content_class_id = na->content_class_id;
    }
    else {
        av = new_arrayvalue_from (na, content_typecode, NULLTYPE, ETCH_DEFSIZE, ETCH_DEFSIZE, TRUE);
        av->content_class_id = na->content_class_id;
    }

    if(NULL == av) {
         ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "nativearray conversion failed"); 
    }
  



    return av;
}


///**
// * normalize_etch_array()
// * validate parameter as an array type with dimension <= that specified,
// * and convert to arrayvalue if necessary.
// * @param a an arrayvalue or nativearray. caller retains.
// * @param maxdim maximum number of dimensions, zero means don't validate dimensions.
// * @return the passed array expressed as an to arrayvalue, or NULL if error. 
// */
//etch_arrayvalue* normalize_etch_array(void* a, const int maxdim)
//{
//    etch_arrayvalue* av = NULL;  
//   
//    if (is_etch_nativearray(a))
//    {   etch_nativearray* na = (etch_nativearray*) a;
//        if (0 == maxdim || na->numdims <= maxdim)
//            av = bintdo_to_arrayvalue(na);
//    }
//    else
//    if (is_etch_arrayvalue(a))
//    {   etch_arrayvalue* xav = (etch_arrayvalue*) a;
//        if (0 == maxdim || xav->dim <= maxdim)
//            av = xav;
//    }
//
//    return av;
//}



/* - - - - - - - - - - - - - - - - - - - -
 * disassemble objects and write bytes 
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdo_write_nonevalue()
 * convenience method to write eod marker 
 */
int bintdo_write_nonevalue(binary_tagged_data_output* tdo)
{
    return bintdo_write_value(tdo, tdo->vtor_eod, tdo->static_eodmarker);
}


/**
 * bintdo_write_intvalue()
 * convenience method used when an encoded integer is expected next in  
 * the buffer, to write such a value to the buffer. 
 *
 * this method is no longer used, we now use bintdo_write_value_rawint()
 */
int bintdo_write_intvalue(binary_tagged_data_output* tdo, const int value)
{
    etch_int32* intobj = new_int32(value);
    int result = bintdo_write_value(tdo, tdo->vtor_int, (etch_object*) intobj);
    etch_object_destroy(intobj);
    return result;
}


/**
 * bintdo_write_value_rawint()
 * write specified 32-bit integer value to the buffer
 */
int bintdo_write_value_rawint(binary_tagged_data_output* tdo, const int value)
{
    size_t nout = 0;

    if  (is_inrange_tiny(value))
         nout = etch_flexbuf_put_byte(tdo->flexbuf, (signed char) value);
    else
    if  (is_inrange_byte(value))
         if  (sizeof(byte) == etch_flexbuf_put_byte(tdo->flexbuf, ETCH_XTRNL_TYPECODE_BYTE)) 
              nout = etch_flexbuf_put_byte(tdo->flexbuf, (signed char) value); 
         else;
    else
    if  (is_inrange_int16(value))
         if  (sizeof(byte) == etch_flexbuf_put_byte(tdo->flexbuf, ETCH_XTRNL_TYPECODE_SHORT)) 
              nout = etch_flexbuf_put_short(tdo->flexbuf, (short) value); 
         else;
    else 
    if  (sizeof(byte) == etch_flexbuf_put_byte(tdo->flexbuf, ETCH_XTRNL_TYPECODE_INT))
         nout = etch_flexbuf_put_int(tdo->flexbuf, value); 
            
    return nout? 0: -1;   
}


/**
 * bintdo_write_type()
 * convenience method used when an etch_type is to be written to the buffer, 
 * to write such a value to the buffer. caller owns the supplied type. 
 * only the type's id is written.
 */
int bintdo_write_type(binary_tagged_data_output* tdo, etch_type* type)
{
    return type? bintdo_write_value_rawint(tdo, type->id): -1;
}


/**
 * bintdo_get_bytes()
 * gets serialized bytes of the specified message. caller owns returned byte vector.
 * not sure what this is used for.
 * "static" method, no tdo is passed. 
 * @return count of bytes 
 */
int bintdo_get_bytes(etch_message* msg, etch_value_factory* vf, byte** out)
{
    size_t bytecount = 0;
    etch_flexbuffer* fbuf = new_flexbuffer(0); /* tdo will own this */
    binary_tagged_data_output* tdo = new_binary_tagdata_output(vf, fbuf);
    bintdo_write_message((tagged_data_output*)tdo, msg, fbuf);

    *out = etch_flexbuf_get_all(fbuf, &bytecount); /* new allocation */

    etch_object_destroy(tdo);
    return (int) bytecount;
}


/**
 * bintdo_write_bytes()
 * writes a byte vector to the buffer. 
 */
int bintdo_write_bytes(binary_tagged_data_output* tdo, char* bytes, const int bytecount)
{
    size_t nout = 0;
    /* TODO handle return value */
    bintdo_write_value_rawint(tdo, (int) bytecount); 

    nout = etch_flexbuf_put(tdo->flexbuf, (unsigned char*)bytes, 0, bytecount);

    return nout == bytecount? 0: -1;   
}

 
/**
 * bintdo_write_bytes_from()
 * writes a byte vector from a native array to the buffer. 
 * @param bytearray an etch_nativearray of single dimension and of content type byte. 
 */
int bintdo_write_bytes_from(binary_tagged_data_output* tdo, etch_nativearray* bytearray)
{
     /* we're assuming we always get a nativearray object and not a char*,
      * however i'm not sure yet exactly where the tdo input is created, 
      * so i'm not positive this is the way is should be. update: perhaps this
      * method should be passed an arrayvalue, keep an eye on this.
      */
    size_t bytecount = 0, nout = 0;
    int result = 0;

    //if ((is_etch_nativearray(bytearray)) 
    //&& (((etch_object*)bytearray)->class_id = CLASSID_ARRAY_BYTE)
    //&& (bytearray->numdims == 1));
    //else return -1;

    bytecount = bytearray->bytecount; /* or bytearray->dimsize[0], same thing */

    result = bintdo_write_value_rawint(tdo, (int) bytecount); 

    nout = etch_flexbuf_put(tdo->flexbuf, bytearray->values, 0, bytecount);

    return nout == bytecount? 0: -1;   
}


/**
 * bintdo_write_string()
 * writes a string value to the buffer. 
 */
int bintdo_write_string(binary_tagged_data_output* tdo, etch_string* s)
{
    int result = 0, wire_encoding = 0, this_encoding = 0, is_new_memory = 0;
    int bytes_to_write = 0, bytes_written = 0;
    unsigned char* bytevector = NULL;
    unsigned int terminator = 0;
    if (NULL == s) return -1;

    wire_encoding = get_etch_string_encoding(tdo->vf);
    this_encoding = s->encoding;

    if (wire_encoding == this_encoding) {
      bytevector = s->v.value;
      bytes_to_write = s->byte_count;
    } else {
        // TODO: pool
      result = etch_encoding_transcode((char**)&bytevector, wire_encoding, s->v.value, this_encoding, s->byte_count, &bytes_to_write, NULL);
        ETCH_ASSERT(result != -1);
        is_new_memory = TRUE;
    }
    terminator = etch_encoding_get_sizeof_terminator(wire_encoding);
    if ((unsigned int)bytes_to_write > terminator) {
      bytes_to_write -= terminator;
    }

    if (NULL == bytevector) return -1;

    result = bintdo_write_value_rawint(tdo, bytes_to_write);

    bytes_written = (int) etch_flexbuf_put(tdo->flexbuf, bytevector, 0, bytes_to_write);
    result = bytes_written == bytes_to_write? 0: -1;

    if (is_new_memory)
        etch_free(bytevector);

    return result;
}


/**
 * bintdo_write_values()
 * write all values from the specified array
 */
int bintdo_write_values(binary_tagged_data_output* tdo, etch_arrayvalue* av, 
    etch_validator* vtor)
{
    int  errs = 0;
    etch_validator* ev = vtor? vtor->element_validator(vtor): NULL;
    etch_iterator iterator;
    set_iterator(&iterator, av->list, &av->list->iterable);

    while(iterator.has_next(&iterator))
    {
        errs += (0 != bintdo_write_value(tdo, ev, iterator.current_value));
        iterator.next(&iterator);
    } 

    etch_object_destroy(ev);

    return errs? -1: 0;
}


/**
 * bintdo_write_keys_values()
 * write key/value pairs from the struct to the buffer
 */
int bintdo_write_keys_values (binary_tagged_data_output* tdo, etch_structvalue* sv)
{
    etch_type* struct_type = sv->struct_type;
    etch_validator* thisvtor = NULL;
    etch_field* thiskey = NULL;
    etch_object* thisval = NULL;
    int result = 0;

    etch_iterator iterator;
    set_iterator(&iterator, sv->items, &sv->items->iterable);

    while(iterator.has_next(&iterator))
    {
        thiskey  = (etch_field*) iterator.current_key;
        thisval  = (etch_object*)    iterator.current_value;
        ETCH_ASSERT(thiskey);

        thisvtor = (etch_validator*) 
            etchtype_get_validator_by_name (struct_type, thiskey->name);

        if (NULL == thisvtor)
        {
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "type '%s' missing validator '%s'\n", struct_type->aname, thiskey->aname);
            result = -1;
            break;
        }
                         
        result = bintdo_write_value_rawint (tdo, thiskey->id);
        result = bintdo_write_value (tdo, thisvtor, thisval);
        if (-1 == result) break;

        iterator.next(&iterator);
    }
   
    return result;
}


/**
 * bintdo_write_value()
 * write specified value to the buffer
 * @param vtor validator for specified value, or null if none
 * @param value the value to be encoded and written, as a *non-disposable* object,
 * i.e. caller owns memory for the value object.
 */
int bintdo_write_value (binary_tagged_data_output* tdo, etch_validator* vtor, etch_object* value)
{
    etch_config_t*  config    = NULL;
    int32           propvalue = 0;
    int             result    = 0;
    size_t          nout      = 0;
    union_alltypes  u; 
    signed char     external_typecode;

    etch_runtime_get_config(&config);
    ETCH_ASSERT(config);

    etch_config_get_property_int(config, "etch.validate.write", &propvalue);
    if (propvalue == 1)
    {   /* we should disable validate on write in production */
        if (NULL == value); /* don't recall why null value is not validated */
        else
        if (!vtor || -1 == vtor->validate (vtor, (etch_object*) value))
        {   
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "validation failed for type %x class %x\n", ((etch_object*)value)->obj_type, ((etch_object*)value)->class_id);  
            return -1;
        }
    }

    /* determine tag (fyi signed only because using the java byte constants) */
    external_typecode = etchtagdata_check_value((etch_object*) value);

    /* write tag */
    if (sizeof(byte) != etch_flexbuf_put_byte(tdo->flexbuf, external_typecode))
        return -1;

    switch(external_typecode)
    {	    
        case ETCH_XTRNL_TYPECODE_NULL:             
	    case ETCH_XTRNL_TYPECODE_NONE: 
        case ETCH_XTRNL_TYPECODE_EMPTY_STRING: 
	    case ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE:                                          
	    case ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE:  
             return 0; /* nothing to do, tag says it all */

	    case ETCH_XTRNL_TYPECODE_BYTE:  
	      if (0 == etchtagdata_byte_value((etch_object*) value, (byte*)&u.vbyte))
                 nout = etch_flexbuf_put_byte(tdo->flexbuf, u.vbyte); 
             break;   

	    case ETCH_XTRNL_TYPECODE_INT:  
             if (0 == etchtagdata_int32_value((etch_object*) value, &u.vint32))
                 nout = etch_flexbuf_put_int(tdo->flexbuf, u.vint32); 
             break;  

	    case ETCH_XTRNL_TYPECODE_LONG:  
             if (0 == etchtagdata_int64_value((etch_object*) value, &u.vint64))
                 nout = etch_flexbuf_put_long(tdo->flexbuf, u.vint64); 
             break; 

	    case ETCH_XTRNL_TYPECODE_SHORT:  
             if (0 == etchtagdata_int16_value((etch_object*) value, &u.vint16))
                 nout = etch_flexbuf_put_short(tdo->flexbuf, u.vint16); 
             break;  

	    case ETCH_XTRNL_TYPECODE_DOUBLE:  
             if (0 == etchtagdata_double_value((etch_object*) value, &u.vdouble))
                 nout = etch_flexbuf_put_double(tdo->flexbuf, u.vdouble); 
             break;   

	    case ETCH_XTRNL_TYPECODE_FLOAT:  
             if (0 == etchtagdata_float_value((etch_object*) value, &u.vfloat))
                 nout = etch_flexbuf_put_float(tdo->flexbuf, u.vfloat); 
             break;  

	    case ETCH_XTRNL_TYPECODE_BYTES:  
             /* we get an arrayvalue here. to do differently would be problematic 
              * without rewriting higher levels to not work with arrayvalue. 
              * perhaps we should simply pass arrayvalue to bintdo_write_bytes_from().
              * TODO either accept a native array here, or change
              * bintdo_write_bytes_from() to accept an arrayvalue, or both.
              */
			if(is_etch_nativearray(value)){
				u.vnatarray = (etch_nativearray*)value;
				result = bintdo_write_bytes_from(tdo,u.vnatarray);
				return result;
			}
			else{
				u.vnatarray = ((etch_arrayvalue*)value)->natarray;
				result = bintdo_write_bytes_from(tdo, u.vnatarray);
				return result;
			}

	    case ETCH_XTRNL_TYPECODE_ARRAY: 
             /* if arriving here from client app we may get an etch_nativearray,
              * which we must convert to an arrayvalue now.  
              * TODO write a version of bintdo_write_array which accepts a    
              * nativearray, so we can avoid this to_arrayvalue() conversion.
              */ 
            if (is_etch_nativearray(value)) {
                u.varrayval = bintdo_to_arrayvalue(tdo, (etch_nativearray*) value);
                result = bintdo_write_array((tagged_data_output*)tdo, u.varrayval, vtor);
                etch_object_destroy(u.varrayval);
            }
            else {
                u.varrayval = (etch_arrayvalue*) value;
                result = bintdo_write_array((tagged_data_output*)tdo, u.varrayval, vtor);
            }
            return result;

 	    case ETCH_XTRNL_TYPECODE_STRING:  
        {    etch_string* s = (etch_string*) value; 
             result = bintdo_write_string(tdo, s);
             return result;
        }

        case ETCH_XTRNL_TYPECODE_CUSTOM:
        {
             etch_structvalue* sv = ((struct i_value_factory*)((etch_object*)tdo->vf)->vtab)->export_custom_value(tdo->vf, value);
             if (NULL == sv) return -1;
             
             result = bintdo_write_struct((tagged_data_output*) tdo, sv);
             
             etch_object_destroy(sv);
             return result;
        }

        default:
             return is_inrange_tiny_for_signed_chars(external_typecode)? 0: -1;
    }

    return nout? 0: -1;   
}

