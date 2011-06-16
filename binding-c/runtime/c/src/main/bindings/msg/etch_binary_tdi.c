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
 * etch_binary_tdi.c -- binary tagged data input implementation.
 */

#include "etch_binary_tdi.h"
#include "etch_cache.h"
#include "etch_structval.h"
#include "etch_arrayval.h"
#include "etch_tagged_data.h"
#include "etch_default_value_factory.h"
#include "etch_encoding.h"
#include "etch_exception.h"
#include "etch_nativearray.h"
#include "etch_log.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"

static const char* LOG_CATEGORY =  "etch_binary_tdi";

/* - - - - - - - - - - - - - - - - - - - -
 * private method signatures
 * - - - - - - - - - - - - - - - - - - - -
 */

i_binary_tdi* new_binarytdi_vtable();

etch_message* bintdi_start_message (tagged_data_input*);
int bintdi_end_message (tagged_data_input*, etch_message*);
etch_message* bintdi_read_message(tagged_data_input*, etch_flexbuffer*);

etch_structvalue* bintdi_start_struct(tagged_data_input*);
etch_structvalue* bintdi_read_struct (tagged_data_input*);
int bintdi_end_struct  (tagged_data_input*, etch_structvalue*);

etch_arrayvalue*  bintdi_start_array(tagged_data_input*);
etch_arrayvalue*  bintdi_read_array (tagged_data_input*, etch_validator*);    
int bintdi_end_array(tagged_data_input*, etch_arrayvalue*);

etch_arrayvalue*  bintdi_alloc_array(tagged_data_input*, const byte array_content_type,
     etch_type* custom_type, const int ndims, const int nelements);

int  bintdi_read_keys_values(binary_tagged_data_input*, etch_structvalue*);
int  bintdi_read_values(binary_tagged_data_input*, etch_arrayvalue*, etch_validator*);
etch_object* bintdi_read_value (binary_tagged_data_input*, etch_validator*);
etch_object* bintdi_read_valuex(binary_tagged_data_input*, etch_validator*, boolean);
etch_type*   bintdi_get_custom_structtype(etch_object*, 
     const unsigned short, const unsigned short);
etch_type*   bintdi_read_type(binary_tagged_data_input*);
etch_int32*  bintdi_read_intvalue(binary_tagged_data_input*);
int  bintdi_read_value_rawint(binary_tagged_data_input* tdi, int* out);

etch_validator* bintdi_get_validator_for(binary_tagged_data_input* tdi, etch_field* field);
etch_object* bintdi_validate_value(binary_tagged_data_input*, 
    etch_validator*, boolean is_none_ok, etch_object*);
etch_object* bintdi_validate_valuex(binary_tagged_data_input*, 
    etch_validator*, boolean is_none_ok, etch_object*);

int  bintdi_get_native_type(const signed char external_typecode, etch_array_id_params* out); 
int  bintdi_get_component_type(tagged_data_input*, const byte array_content_type,
     etch_type* custom_type, const int dims, etch_array_id_params* out);
signed char bintagdata_get_native_typecode(const unsigned short, const unsigned short);


/* - - - - - - - - - - - - - - - - - - - -
 * constructors/destructors
 * - - - - - - - - - - - - - - - - - - - -
 */


/**
 * destroy_binary_tagged_data_input()
 */
int destroy_binary_tagged_data_input(void* data)
{
    binary_tagged_data_input* tdi = (binary_tagged_data_input*)data;

    if (!is_etchobj_static_content(tdi))
        etch_object_destroy(tdi->impl);

    /* destroy private instance data */
    etch_free(tdi->static_nullobj);
    etch_free(tdi->static_eodmarker);
    clear_etchobj_static_all(tdi->static_emptystring);
    etch_object_destroy(tdi->static_emptystring);
     
    return destroy_objectex((etch_object*)tdi);
}


/**
 * clone_tagged_data_input()
 * tdi copy constructor. if the tdi object implements a separate instance data
 * object, that object is cloned as well. 
 */
void* clone_binary_tagged_data_input(void* data)
{
  binary_tagged_data_input* tdi = (binary_tagged_data_input*)data;
    binary_tagged_data_input* newtdi = (binary_tagged_data_input*) clone_object((etch_object*) tdi);
    newtdi->impl = tdi->impl?  tdi->impl->clone(tdi->impl): NULL;
    return newtdi;
}


/**
 * new_binary_tagdata_input()
 * binary_tagged_data_input constructor 
 */
binary_tagged_data_input* new_binary_tagdata_input(etch_value_factory* vf)
{
    i_binary_tdi* vtab = NULL;

    binary_tagged_data_input* tdi = (binary_tagged_data_input*) new_object
       (sizeof(binary_tagged_data_input), ETCHTYPEB_BINARYTDI, CLASSID_BINARYTDI);

    ((etch_object*)tdi)->destroy = destroy_binary_tagged_data_input;
    ((etch_object*)tdi)->clone   = clone_binary_tagged_data_input;
    tdi->vf = vf;

    vtab = etch_cache_find(get_vtable_cachehkey(CLASSID_BINARYTDI_VTAB), 0);

    if(!vtab)  
    {   vtab = new_binarytdi_vtable();
        etch_cache_insert(((etch_object*)vtab)->get_hashkey(vtab), vtab, FALSE);
    } 
 
    ((etch_object*)tdi)->vtab = (vtabmask*)vtab;  
    tdi->vtor_int = etchvtor_int32_get(0); 
    tdi->static_nullobj     = etchtagdata_new_nullobj(TRUE);
    tdi->static_eodmarker   = etchtagdata_new_eodmarker(TRUE);
    tdi->static_emptystring = etchtagdata_new_emptystring(TRUE);
    return tdi; 
}


/**
 * new_binary_tdi()
 * casts result to generic tdi for use by interfaces
 */
tagged_data_input* new_binary_tdi(etch_value_factory* vf)
{
    return (tagged_data_input*) new_binary_tagdata_input(vf);
}




/**
 * new_new_binarytdi_vtable()
 * constructor for binary tdi virtual function table
 */
i_binary_tdi* new_binarytdi_vtable()
{
    etchparentinfo* inheritlist = new_etch_inheritance_list(3, 2, NULL); 
   
    i_binary_tdi* vtab 
        = new_vtable(NULL, sizeof(i_binary_tdi), CLASSID_BINARYTDI_VTAB);

    /* i_tagged_data_input */
    vtab->start_message = bintdi_start_message;
    vtab->read_message  = bintdi_read_message;
    vtab->end_message   = bintdi_end_message;
    vtab->start_struct  = bintdi_start_struct;
    vtab->read_struct   = bintdi_read_struct;
    vtab->end_struct    = bintdi_end_struct;
    vtab->start_array   = bintdi_start_array;
    vtab->read_array    = bintdi_read_array;
    vtab->end_array     = bintdi_end_array;

    /* i_tagdata */
    vtab->check_value = etchtagdata_check_value;
    vtab->get_native_type = bintdi_get_native_type;
    vtab->get_native_type_code  = bintagdata_get_native_typecode;
    vtab->get_custom_structtype = bintdi_get_custom_structtype;

    /* inheritance */
    inheritlist[1].o.obj_type = ETCHTYPEB_TAGDATAINP;
    inheritlist[1].c.class_id = CLASSID_TAGDATAINP;
    inheritlist[2].o.obj_type = ETCHTYPEB_TAGDATA;
    inheritlist[2].c.class_id = CLASSID_TAGDATA;

    vtab->inherits_from = inheritlist;

    return vtab;
}


/* - - - - - - - - - - - - - - - - - - - -
 * read message
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdi_start_message()
 * starts reading a message from the stream.
 * returns NULL if the wire data was insufficient to construct a  
 * message object, otherwise returns the new message object,
 * of the type read off the wire herein.
 */
etch_message* bintdi_start_message(tagged_data_input* tdix)
{
    binary_tagged_data_input* tdi = (binary_tagged_data_input*) tdix;
    etch_type*  typeobj   = NULL; /* not owned here */
    etch_message* newmsg  = NULL; /* disposable, returned */
    int message_itemcount = 0, result = 0;
    byte wire_version = 0;

    do 
    {   result = etch_flexbuf_get_byte (tdi->flexbuf, &wire_version);

        if (wire_version != ETCH_BINTAGDATA_CURRENT_VERSION) 
        {   
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR,"message version expected %d found %d", ETCH_BINTAGDATA_CURRENT_VERSION, wire_version);
            break;
        }

        /* bintdi_read_type returns us a non-disposable reference to a global type,     
         * which we then give to the new message, which does not own its type. 
         */
        if (NULL == (typeobj = bintdi_read_type (tdi))) break;

        if (-1 != bintdi_read_value_rawint (tdi, &message_itemcount))
            newmsg = new_message(typeobj, message_itemcount, tdi->vf);

    } while(0);

    if(newmsg) {
         ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "message receive starts\n"); 
    }
    else {
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "error starting message receive\n"); 
    }
    return newmsg;
}


/**
 * bintdi_end_message()
 * end message deserialization
 */
int bintdi_end_message(tagged_data_input* tdi, etch_message* msg)
{
    int result = 0;
    /* if the newly-deserialized message is not a reply message, 
     * there is nothing to do here, since bintdi_read_keys_values() 
     * already read the eod marker. if it is a reply message, and
     * if the message contains an exception, we will ensure that the 
     * message result object both exists and reflects that exception.
     */

    if (etch_message_get_in_reply_to (msg))
    {
        etch_field* xkey = builtins._mf_msg;
        /* look for an exception in the newly-deserialized message */
        etch_exception* excpobj = (etch_exception*) etch_message_get (msg, xkey);

        if (is_etch_exception(excpobj))
        {   /* look for a result object in the message */
            etch_field* resobj_key = builtins._mf_result;
            etch_object* resobj = etch_message_get (msg, resobj_key);
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "exception found in message\n"); 
            //replace result with exception object
            //TODO memory leak here? (resobj not destroyed, should we?)
            resobj = ((etch_object*)excpobj)->clone(excpobj);
            result = etch_message_put (msg, resobj_key, resobj);
        }
    }

    return result; 
}


/**
 * bintdi_read_message() 
 * "non-static" read message version, accepts a tdi
 */
etch_message* bintdi_read_message (tagged_data_input* tdix, etch_flexbuffer* fbuf)
{
    etch_message* newmsg = NULL;
    binary_tagged_data_input* tdi = (binary_tagged_data_input*) tdix;
    tdi->flexbuf = fbuf;

    if (NULL == (newmsg = bintdi_start_message(tdix)))
        return NULL;

    /* todo verify that this deserialization failure is eventually session_notify'ed */ 
    if (-1 == bintdi_read_keys_values (tdi, newmsg->sv)) {   
        etch_object_destroy(newmsg);
        return NULL; 
    }

    bintdi_end_message(tdix, newmsg);
    return newmsg;
} 


/**
 * bintdi_read_message_fromf() 
 * reads a message from the supplied flex buffer           
 */
etch_message* bintdi_read_message_fromf (etch_value_factory* vf, etch_flexbuffer* fbuf)
{
    binary_tagged_data_input* tdi = new_binary_tagdata_input(vf);

    return bintdi_read_message((tagged_data_input*) tdi, fbuf);
} 


/**
 * bintdi_read_message_from() 
 * reads a message from the supplied data buffer           
 */
etch_message* bintdi_read_message_from (etch_value_factory* vf, byte* buf, 
    const int bufsize)
{
    etch_flexbuffer* fbuf = new_flexbuffer_from(buf, bufsize, bufsize, 0);

    return bintdi_read_message_fromf(vf, fbuf);
} 


/**
 * bintdi_read_message_fromo() 
 * reads a message from the supplied data buffer, at the specified offset           
 */
etch_message* bintdi_read_message_fromo (etch_value_factory* vf, byte* buf, 
    const int bufsize, const int msglen, const int offset)
{
    etch_flexbuffer* fbuf = new_flexbuffer_from(buf, bufsize, msglen, offset);

    return bintdi_read_message_fromf(vf, fbuf);
} 


/* - - - - - - - - - - - - - - - - - - - -
 * read struct
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdi_start_struct()
 * starts reading a struct from the stream.
 */
etch_structvalue* bintdi_start_struct(tagged_data_input* tdix)
{
    binary_tagged_data_input* tdi = (binary_tagged_data_input*) tdix; 
    etch_structvalue* newsv = NULL;
    etch_type* thistype = NULL;
    int struct_itemcount = 0;

    /* bintdi_read_type returns us a non-disposable reference to a global type, 
     * which we then give to the new structvalue, which does not own its type. 
     */
    if (NULL == (thistype = bintdi_read_type(tdi))) {
        return NULL;
    }

    if (-1 != bintdi_read_value_rawint(tdi, &struct_itemcount))
        newsv = new_structvalue(thistype, struct_itemcount);

    return newsv;
}


/**                              
 * bintdi_end_struct()
 * does nothing since read_keys_values read to end of stream 
 */
int bintdi_end_struct(tagged_data_input* tdi, etch_structvalue* sv)
{
    return 0;  
}


/**
 * bintdi_read_struct()
 * read a struct out of the buffer, create and return a new struct value 
 */
etch_structvalue* bintdi_read_struct(tagged_data_input* tdix)
{
    binary_tagged_data_input* tdi = (binary_tagged_data_input*) tdix;
    etch_structvalue* newsv = NULL;

    newsv = ((struct i_binary_tdi*)((etch_object*)tdi)->vtab)->start_struct((tagged_data_input*) tdi);
    if(newsv){
        if (-1 == bintdi_read_keys_values(tdi, newsv)) {   
            etch_object_destroy(newsv);
            newsv = NULL;
        }else  
        {
            bintdi_end_struct(tdix, newsv);
        }
    }
    return newsv;
} 


/* - - - - - - - - - - - - - - - - - - - -
 * read array
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdi_start_array()
 */
etch_arrayvalue* bintdi_start_array(tagged_data_input* tdix)
{
    etch_type*  custom_type = NULL;    /* not owned here */
    etch_arrayvalue* arrayval = NULL;  /* owned by caller */
    binary_tagged_data_input* tdi = (binary_tagged_data_input*) tdix;
    int numdimensions = 0, numelements = 0, result = 0;
    byte array_content_type = 0;

    result = etch_flexbuf_get_byte(tdi->flexbuf, &array_content_type);

    switch((signed char)array_content_type)
    { case ETCH_XTRNL_TYPECODE_CUSTOM:
      case ETCH_XTRNL_TYPECODE_STRUCT:
           /* bintdi_read_type returns a non-disposable type reference */
           custom_type = bintdi_read_type(tdi);   
           if (NULL == custom_type) return NULL;
    }
   
    bintdi_read_value_rawint(tdi, &numdimensions);
    bintdi_read_value_rawint(tdi, &numelements);

    if(numdimensions <= 0 || numelements < 0){
        return NULL;
    }

    /* create the arrayvalue. we pass no memory ownership here */
    arrayval = bintdi_alloc_array(tdix, array_content_type, custom_type, numdimensions, numelements);
    return arrayval;
}


/**
 * bintdi_end_array()
 * ends array in progress
 */
int bintdi_end_array(tagged_data_input* tdi, etch_arrayvalue* x)
{
    return 0;
}


/**
 * bintdi_read_array()
 * read an array out of the buffer, create and return a new array object 
 *
 * todo (eventually). arrays are not optimal. following the java binding model,
 * an array on the wire is read into an array_value. in c, this means an array
 * of objects, one object per array element. this can make for very large data
 * structures, 60 bytes of overhead per array element. we would like to instead
 * use the etch_nativearray format, where an array is a blob of bytes, with
 * attributes and methods describing how to index into the array. however
 * it was not known if doing so would paint the c binding into a corner at 
 * some point, so for now, we create the arrayvalue just as the java binding, 
 * and export that structure to an etch_nativearray when and if we need it.
 */
etch_arrayvalue* bintdi_read_array(tagged_data_input* tdix, etch_validator* vtor)
{
    binary_tagged_data_input* tdi = (binary_tagged_data_input*) tdix;

    etch_arrayvalue* newarray = bintdi_start_array((tagged_data_input*) tdi);
    if (NULL == newarray) return NULL;
                                     
    if (-1 == bintdi_read_values(tdi, newarray, vtor)) {   
        etch_object_destroy(newarray);
        return NULL;
    }

    bintdi_end_array((tagged_data_input*)tdi, newarray);

    return newarray;
} 


/**
 * bintdi_alloc_array()
 * allocate an arrayvalue in which to read data for the pending array.
 * java binding allocates a native array here. we can't do that yet since we
 * don't have the attributes of each dimension until we get the entire array
 * out of the stream buffer, and we want to avoid lookahead if possible.
 * @return new array object, or NULL indicating exception condition
 */
etch_arrayvalue* bintdi_alloc_array(tagged_data_input* tdi, const byte array_content_type,
   etch_type* custom_type, const int ndims, const int nelements)
{
    etch_array_id_params arraytype;
    etch_arrayvalue* arrayval = NULL;

    if (-1 == bintdi_get_component_type(tdi, array_content_type, 
        custom_type, ndims, &arraytype))
        return NULL; 

    /* remarks regarding ownership of content for these arrayvalue objects. 
     * first, recall that their content is etch objects wrapping data read from  
     * the data buffer, and that the data so wrapped is owned by the wrapper. 
     * ownership of memory for those base data objects is assigned up the line
     * until some object assumes responsibility. content for these arrayvalues
     * is either those wrapper objects, at dimension[0], or arrayvalue objects
     * at the higher dimensions. in all cases, the arryavalue owns its content,  
     * and therefore its content will be destroyed with the arrayvalue. since  
     * the destruction is recursive, destroying the top object destroys it all.
     */
    arrayval = new_arrayvalue(array_content_type, custom_type, ndims, nelements, 0, FALSE, 0);
    if (NULL == arrayval) {
        return NULL;
    }

    /* validator will validate array based on this class */
    ((etch_object*)arrayval)->class_id = arraytype.array_class_id;

    /* content type will be used later when we need to create a nativearray */
    arrayval->content_obj_type = arraytype.content_obj_type;
    arrayval->content_class_id = arraytype.content_class_id;
    return arrayval;
}


/* - - - - - - - - - - - - - - - - - - - - -
 * read from stream and reconstruct objects
 * - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdi_read_intvalue()
 * convenience method used when an encoded integer is expected next in
 * the buffer, to read that value from the buffer and return it as an 
 * etch_int32 object, ownership of which belongs to the caller.
 */
etch_int32* bintdi_read_intvalue(binary_tagged_data_input* tdi)
{
    return (etch_int32*) bintdi_read_value(tdi, tdi->vtor_int);
}

/**
 * bintdi_read_bytes()
 * read all bytes from the buffer, returning those bytes
 * @param extra pad bytes e.g. when we are reading a string and need a null term
 */
byte* bintdi_read_bytes(binary_tagged_data_input* tdi, const int extra, int* psize)
{
    byte* buf = NULL;
    int bytecount = 0, newbufsize = 0;

    if (-1 == bintdi_read_value_rawint(tdi, &bytecount)) return NULL; 

    if(bytecount >= 0 && (size_t) bytecount <= etch_flexbuf_avail(tdi->flexbuf)) {
        newbufsize = bytecount + extra;
        buf = etch_malloc(newbufsize, ETCHTYPEB_BYTES);
        memset(buf, 0, newbufsize);
        etch_flexbuf_get_fully(tdi->flexbuf, buf, bytecount);
        if (psize) *psize = newbufsize;
    }

    return buf;
} 


/**
 * bintdi_read_type()
 * read a type ID from the buffer, map to and return the associated static type
 */
etch_type* bintdi_read_type(binary_tagged_data_input* tdi)
{
    etch_type* type = NULL;
    int  type_id = 0;

    if (0 == bintdi_read_value_rawint(tdi, &type_id))
        type = ((struct i_value_factory*)((etch_object*)tdi->vf)->vtab)->get_type_by_id(tdi->vf, type_id);               

    /* note that the type object returned by valuefactory.get_type_by_id 
     * is not disposable, it is a reference into the global types map. 
     * and we are returning that nondisposable reference here */

    return type;
}


/**
 * bintdi_read_bytearray()
 * read all bytes from the buffer, return a native array wrapping the resultant byte array
 * todo: is this OK to return nativearray in one case, and arrayvalue in another?
 * can we handle the nativearray format across the board? we can at least use nativearray
 * for byte blobs. even if we use arravalue format we should at least use nativearray
 * as the base of the arrayvalue, since if we need to be able to access elements, we can't
 * simply reflect to an array as does java, we need the subscripting of the nativearray.
 */
etch_nativearray* bintdi_read_bytearray(binary_tagged_data_input* tdi)
{
    int  size = 0;
    etch_nativearray* newarray = NULL;
    byte* buf = bintdi_read_bytes(tdi, 0, &size); /* buf is disposable, newarray will own it */
    newarray  = new_etch_nativearray_from(buf, CLASSID_ARRAY_BYTE, sizeof(byte), 1, size, 0, 0); 
    newarray->is_content_owned = TRUE;
    return newarray;
} 


/**
 * bintdi_read_string()
 * read all bytes from the buffer and translate to unicode C string
 * @return the wrapped string
 */	
etch_string* bintdi_read_string(binary_tagged_data_input* tdi)
{
    int result = 0, size = 0;
    wchar_t* widestring = NULL;
    etch_string* newstr = NULL;
    byte* buf = NULL;

    const int configured_encoding = tdi->vf? get_etch_string_encoding(tdi->vf): ETCH_ENCODING_DEFAULT;
    
    const int nulltermsize = etch_encoding_get_sizeof_terminator(configured_encoding);

    buf = bintdi_read_bytes(tdi, nulltermsize, &size); /* we own this memory */
    if(buf) {
        // TODO: pool
        result = etch_encoding_transcode_to_wchar(&widestring, buf, configured_encoding, size, NULL);
        etch_free(buf);
        /* construct string object, relinquishing ownership of string buffer */
        newstr = new_string_from(widestring, etch_encoding_for_wchar());
    }
    return newstr;
}


/* - - - - - - - - - - - - - - - - - - - -
 * read structured content
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdi_read_keys_values()
 * deserializes a message from a buffer.
 * read all key/value pairs from buffer, populating the specified struct.
 * @param tdi caller retains.
 * @param sv caller retains.
 * @return 0 success, -1 deserialization failed, caller should throw exception.
 */
int bintdi_read_keys_values (binary_tagged_data_input* tdi, etch_structvalue* sv)
{
	etch_type* svtype = sv->struct_type;
    etch_validator* thisvtor = NULL;  /* non-disposable ref to type vtor */
    etch_object* thisobj     = NULL;  /* disposable return from read_value */
    etch_object* thisvalue   = NULL;  /* disposable return from read_value */
    etch_field*  key_field   = NULL;  /* non-disposable ref to static field */
    etch_field*  key_clone   = NULL;  /* disposable copy of structvalue key */
    etch_int32*  this_idobj  = NULL;  /* non-disposable copy of thisobj */
    int result = 0;

    while(result == 0)
    {    
        thisobj = bintdi_read_valuex(tdi, tdi->vtor_int, TRUE); /* disposable */ 
        
        //could not read next key field
        if(!thisobj) {
            break;
        }

        if (etchtagdata_is_eod (thisobj)) break;    /* data end marker found */

        key_clone = NULL; 
        thisvalue = NULL; 
        result = -1;

        this_idobj = (etch_int32*) thisobj; 
        key_field  = etchtype_get_field_by_id (svtype, this_idobj->value);  
 
        if (NULL == key_field) 
        {
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR,"field lookup failed, trying to skip that value\n"); 
            //skip the value of unknown keys...
            thisvalue = bintdi_read_value(tdi, etchvtor_object_get(0));  /* returns a disposable ref */ 
            //if this fails, we cannot know where we are, so stop
            if(thisvalue == NULL) {
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR,"cant skip value, error in deserialization\n"); 
                result = -1;
                break;
            } else  {//else we can continue
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR,"skipped the value for the unknown key\n"); 
                result = 0;
                continue;
            }
        }

        thisvtor  = (etch_validator*) etchtype_get_validator_by_name(svtype, key_field->name);  /* returns us a non-disposable ref */ 
       
        thisvalue = bintdi_read_value(tdi, thisvtor);  /* returns a disposable ref */ 
        if (NULL == thisvalue){
            break; /* validation or other deserialization error */
        }

        key_clone = (etch_field*)etch_object_clone_func(key_field);

        /* structvalue_put() expects disposable key and value objects, i.e. it will
         * call destructors on the objects when the struct is destroyed. however if
         * the put() fails, we still own the objects, which are accounted for below.
         * note also that this contract differs from the etch_message interface to  
         * a struct, which eats its put parameters regardless of outcome.
         */
        result = structvalue_put(sv, key_clone, (etch_object*) thisvalue);

        //printf("read_keys_values: got field %s, value: %p\n",key_clone->aname,thisvalue);

        /* if put was OK we have relinquished ownership of valuobj */
        /* key_field was relinquished regardless. */

        etch_object_destroy(thisobj); 
		thisobj = NULL;
    }

    if (-1 == result) 
    {   /* some error, usually failed validation */ 
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR,"message deserialization failed\n"); 
	    etch_object_destroy(key_clone);
	    key_clone = NULL;

        etch_object_destroy(thisvalue);
	    thisvalue = NULL;
    }

    etch_object_destroy(thisobj);
    thisobj = NULL;

    return result; 
}


/**
 * bintdi_read_values()
 * read values from buffer, populating the specified array with the values so read
 */
int bintdi_read_values (binary_tagged_data_input* tdi, 
    etch_arrayvalue* av, etch_validator* vtor)
{
    int counter = 0;
    etch_validator* ev = vtor? vtor->element_validator(vtor): NULL;
    etch_object* thisobj = 0;
    int result = 0;
    if (!av || !vtor) return -1;

    while(result == 0)
    {    

        thisobj = bintdi_read_valuex(tdi, ev, TRUE);

        if (NULL == thisobj) return -1;
        if (etchtagdata_is_eod(thisobj)) break;  

        /* relinquish ownership of thisobj to the arrayvalue */
        result = arrayvalue_add(av, thisobj);
        if (0 == result) thisobj = NULL;
        counter++;
    }

    etch_object_destroy(ev);
    etch_object_destroy(thisobj); 
    return result; 
}


/* - - - - - - - - - - - - - - - - - - - -
 * read tokens
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdi_read_value()
 */
etch_object* bintdi_read_value(binary_tagged_data_input* tdi, etch_validator* vtor)
{
    return bintdi_read_valuex(tdi, vtor, FALSE);
}


/**
 * bintdi_read_valuex()
 * reads a tag byte from the buffer, and based on the tag, reads zero or more
 * bytes from the buffer, appropriate to the data type indicated by the tag.
 * @return a *disposable* object which appropriately wraps the bytes or object
 * read from input buffer. however if the data tag indicated a null object, a
 * nullobj is returned, or if the tag indicated end of data, an eod object is
 * returned. if validation fails on the object, NULL is returned, otherwise an
 * object is always returned on which the caller is expected to call destroy().
 */
etch_object* bintdi_read_valuex (binary_tagged_data_input* tdi, etch_validator* v, boolean is_none_ok)
{
    /* if this method returns NULL it must first destroy any object created 
     * herein. normally this is accomplished in bintdi_validate_value.
     */
    union_alltypes u;
    signed char objtype = 0;
    
    if (-1 == etch_flexbuf_get_byte (tdi->flexbuf, (byte*)&objtype))
        return NULL;

    switch((signed char)objtype)
    {
        case ETCH_XTRNL_TYPECODE_NULL: 
             /* returns the instance null object. it is considered disposable because
              * caller can and will call destroy() on it. however the destructor will
              * have no effect, the null object is destroyed in the tdi destructor.*/
             return (etch_object*) bintdi_validate_valuex (tdi, v, FALSE, 
                (etch_object*) tdi->static_nullobj);

	    case ETCH_XTRNL_TYPECODE_NONE:  
             /* returns the instance eod object. it is considered disposable because
              * caller can and will call destroy() on it. however the destructor will
              * have no effect, the eod object is destroyed in the tdi destructor. */
             return bintdi_validate_value(tdi, v, is_none_ok, 
                (etch_object*) tdi->static_eodmarker);                                                 	                                            
    	 
	    case ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE:                                          
             return bintdi_validate_value (tdi, v, FALSE, (void*) new_boolean(FALSE));                                                
    	
	    case ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE:                                           
             return bintdi_validate_value (tdi, v, FALSE, (void*) new_boolean(TRUE));                                               
    	
	    case ETCH_XTRNL_TYPECODE_BYTE: 
	      if (-1 == etch_flexbuf_get_byte(tdi->flexbuf, (byte*)(&u.vbyte))) break;
             return bintdi_validate_valuex (tdi, v, FALSE, (void*) new_byte(u.vbyte));                                               
    	
	    case ETCH_XTRNL_TYPECODE_SHORT: 
             if (-1 == etch_flexbuf_get_short(tdi->flexbuf, &u.vint16)) break;                                                                                                
             return bintdi_validate_valuex (tdi, v, FALSE, (void*) new_int16(u.vint16));                                                                                                 
    	
	    case ETCH_XTRNL_TYPECODE_INT:  
             if (-1 == etch_flexbuf_get_int(tdi->flexbuf, &u.vint32)) break;
             return bintdi_validate_valuex (tdi, v, FALSE, (void*) new_int32(u.vint32));
 
	    case ETCH_XTRNL_TYPECODE_LONG: 
             if (-1 == etch_flexbuf_get_long(tdi->flexbuf, &u.vint64)) break;                                                                             
             return bintdi_validate_value (tdi, v, FALSE, (void*) new_int64(u.vint64));                                               
    	
	    case ETCH_XTRNL_TYPECODE_FLOAT:
             if (-1 == etch_flexbuf_get_float(tdi->flexbuf, &u.vfloat)) break;                                                                                                                     
             return bintdi_validate_value (tdi, v, FALSE, (void*) new_float(u.vfloat));                                               
    	
	    case ETCH_XTRNL_TYPECODE_DOUBLE:                                       
             if (-1 == etch_flexbuf_get_double(tdi->flexbuf, &u.vdouble)) break;                                                                                                                     
             return bintdi_validate_value(tdi, v, FALSE, (void*) new_double(u.vdouble));                                               
    	
	    case ETCH_XTRNL_TYPECODE_BYTES: 
             /* must return arrayvalue for symmetry with tdo */
             /* todo modify arrayvalue to not populate objects when so requested */
             u.vnatarray = bintdi_read_bytearray(tdi);  
             return bintdi_validate_value (tdi, v, FALSE, (void*) u.vnatarray);                                   
             /*
             u.varrayval = new_arrayvalue_from(u.vnatarray, ETCH_XTRNL_TYPECODE_BYTES, 
                 NULL, (int) u.vnatarray->bytecount, 0, FALSE);
             return bintdi_validate_value (tdi, v, FALSE, (void*) u.varrayval);                                   
    	    */

             //return (etch_object*)u.vnatarray;
	    case ETCH_XTRNL_TYPECODE_EMPTY_STRING:                 
             return bintdi_validate_value (tdi, v, FALSE, (void*) tdi->static_emptystring);                              
    	
	    case ETCH_XTRNL_TYPECODE_STRING:   
             u.vstring = bintdi_read_string(tdi);                                      
             return bintdi_validate_value (tdi, v, FALSE, (void*) u.vstring);   
    	
	    case ETCH_XTRNL_TYPECODE_STRUCT:   
             u.vsv = bintdi_read_struct((void*) tdi);                                    
             return bintdi_validate_value (tdi, v, FALSE, (void*) u.vsv);                                               
    	
	    case ETCH_XTRNL_TYPECODE_ARRAY: 
             u.varrayval = bintdi_read_array((tagged_data_input*) tdi, v);
             return bintdi_validate_value (tdi, v, FALSE, (void*) u.varrayval);   
    	
	    case ETCH_XTRNL_TYPECODE_CUSTOM:  
        {    
             etch_object* reconstituted_object = NULL;
             /* acquire struct */
             etch_structvalue* keys_values = bintdi_read_struct((tagged_data_input*) tdi);

             //deserialization failed
             if(! keys_values) {
                return NULL;
             }
             /* relinquish struct */
             reconstituted_object = ((struct i_value_factory*)((etch_object*)tdi->vf)->vtab)->import_custom_value(tdi->vf, keys_values); 

             return bintdi_validate_value (tdi, v, FALSE, (void*) reconstituted_object);   
        }                                            
    	
	    default:                                          
	      if (is_inrange_tiny_for_signed_chars(objtype))
                 return bintdi_validate_valuex (tdi, v, FALSE, (void*) new_byte(objtype));
    }
    
    return NULL;
}


/**
 * bintdi_read_value_rawint()
 * read an integer value from the buffer, returning the 32-bit primitive
 * in the out parameter.
 * @param out a pointer to an int to receive the value read from the buffer.
 * @return 0 success, -1 if an integer could not be read from the buffer
 */
int bintdi_read_value_rawint(binary_tagged_data_input* tdi, int* out)
{
    int thisint = 0, result = 0; 
    signed char objtype = 0;
    union_alltypes u; 

    if (0 != etch_flexbuf_get_byte(tdi->flexbuf, (byte*)&objtype))
        result = -1;
    else
    if (is_inrange_tiny_for_signed_chars(objtype))
        thisint = objtype;
    else switch(objtype)
    {
        case ETCH_XTRNL_TYPECODE_INT:  
             if (0 == (result = etch_flexbuf_get_int(tdi->flexbuf, &u.vint32)))
                 thisint = u.vint32;
             break;
        case ETCH_XTRNL_TYPECODE_SHORT: 
            if (0 == (result = etch_flexbuf_get_short(tdi->flexbuf, &u.vint16)))
                thisint = u.vint16;
            break;
        case ETCH_XTRNL_TYPECODE_BYTE: 
	  if (0 == (result = etch_flexbuf_get_byte(tdi->flexbuf, (byte*)&u.vbyte)))
                thisint = u.vbyte;
            break;
        default:
            result = -1;
    }

    *out = thisint;
    return result;
}


/* - - - - - - - - - - - - - - - - - - - -
 * utility methods  
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * bintdi_get_component_type()
 *  
 * @return etch c obj_type and class_id of an array of specified external type,
 * and its content, or -1 indicating exception condition.
 */
int bintdi_get_component_type(tagged_data_input* tdi, const byte array_content_type,
    etch_type* custom_type, const int dims, etch_array_id_params* out)
{
    if (NULL == out) return -1;
    memset(out, 0, sizeof(etch_array_id_params));
    
    return bintdi_get_native_type(array_content_type, out);
}


/*
 * bintdi_get_native_type()
 * returns the internal object types and class ids corresponding to the 
 * external byte typecode indicating content type of an array on the wire.
 * class_id may or may not be significant, depending on particular obj_type.
 */
int bintdi_get_native_type(const signed char external_typecode, etch_array_id_params* out) 
{
    int result = 0;
    memset(out, 0, sizeof(etch_array_id_params));
    out->content_obj_type = ETCHTYPEB_PRIMITIVE;
    out->array_obj_type   = ETCHTYPEB_NATIVEARRAY;
    

    switch(external_typecode) 
    {
        case ETCH_XTRNL_TYPECODE_INT:
             out->content_class_id = CLASSID_PRIMITIVE_INT32;
             out->array_class_id   = CLASSID_ARRAY_INT32;
             break;

        case ETCH_XTRNL_TYPECODE_STRING:
        case ETCH_XTRNL_TYPECODE_EMPTY_STRING:
             out->content_class_id = CLASSID_STRING;
             out->array_class_id   = CLASSID_ARRAY_STRING;
             break;

        case ETCH_XTRNL_TYPECODE_BYTE:
             out->content_class_id = CLASSID_PRIMITIVE_BYTE;
             out->array_class_id   = CLASSID_ARRAY_BYTE;
             break;

        case ETCH_XTRNL_TYPECODE_LONG:
             out->content_class_id = CLASSID_PRIMITIVE_INT64;
             out->array_class_id   = CLASSID_ARRAY_INT64;
             break;

        case ETCH_XTRNL_TYPECODE_SHORT:
             out->content_class_id = CLASSID_PRIMITIVE_INT16;
             out->array_class_id   = CLASSID_ARRAY_INT16;
             break;

        case ETCH_XTRNL_TYPECODE_DOUBLE:
             out->content_class_id = CLASSID_PRIMITIVE_DOUBLE;
             out->array_class_id   = CLASSID_ARRAY_DOUBLE;
             break;

        case ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE:
        case ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE:
             out->content_class_id = CLASSID_PRIMITIVE_BOOL;
             out->array_class_id   = CLASSID_ARRAY_BOOL;
             break;

        case ETCH_XTRNL_TYPECODE_FLOAT:
             out->content_class_id = CLASSID_PRIMITIVE_FLOAT;
             out->array_class_id   = CLASSID_ARRAY_FLOAT;
             break;

        case ETCH_XTRNL_TYPECODE_ANY:
        case ETCH_XTRNL_TYPECODE_ARRAY:
        case ETCH_XTRNL_TYPECODE_STRUCT:
        case ETCH_XTRNL_TYPECODE_CUSTOM:
        case ETCH_XTRNL_TYPECODE_NULL:
             out->content_obj_type = ETCHTYPEB_ETCHOBJECT;
             out->content_class_id = CLASSID_OBJECT;
             out->array_class_id   = CLASSID_ARRAY_OBJECT;
             break;

        case ETCH_XTRNL_TYPECODE_BYTES:
             out->content_obj_type = ETCHTYPEB_ARRAYVAL;
             out->content_class_id = CLASSID_ARRAY_BYTE;
             out->array_class_id   = CLASSID_ARRAY_BYTE;
             break;

        default:
             if (is_inrange_tiny_for_signed_chars(external_typecode))
             {
                 out->content_class_id = CLASSID_PRIMITIVE_INT8;
                 out->array_class_id   = CLASSID_ARRAY_INT8;
             }
             else
             {   out->content_obj_type = ETCHTYPEB_NONE;
                 out->content_class_id = CLASSID_NONE;
                 result = -1;
             }
    }

    return result; 
}


/*
 * bintagdata_get_native_typecode()
 * returns the external type code corresponding to internal type.
 * etchtagdata_get_native_typecode() override
 */
signed char bintagdata_get_native_typecode
   (const unsigned short obj_type, const unsigned short class_id)
{
    byte xtype = 0;

    static const byte primitives[10] 
     = {ETCH_XTRNL_TYPECODE_CUSTOM,       /* CLASSID_NONE             = 0x0 */
        ETCH_XTRNL_TYPECODE_BYTE,         /* CLASSID_PRIMITIVE_BYTE   = 0x1 */
        ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, /* CLASSID_PRIMITIVE_BOOL   = 0x2 */
        ETCH_XTRNL_TYPECODE_BYTE,         /* CLASSID_PRIMITIVE_INT8   = 0x3 */
        ETCH_XTRNL_TYPECODE_SHORT,        /* CLASSID_PRIMITIVE_INT16  = 0x4 */
        ETCH_XTRNL_TYPECODE_INT,          /* CLASSID_PRIMITIVE_INT32  = 0x5 */
        ETCH_XTRNL_TYPECODE_LONG,         /* CLASSID_PRIMITIVE_INT64  = 0x6 */
        ETCH_XTRNL_TYPECODE_FLOAT,        /* CLASSID_PRIMITIVE_FLOAT  = 0x7 */
        ETCH_XTRNL_TYPECODE_DOUBLE,       /* CLASSID_PRIMITIVE_DOUBLE = 0x8 */
        ETCH_XTRNL_TYPECODE_STRING        /* CLASSID_STRING           = 0x9 */
       };
     
    switch(obj_type)
    { 
        case ETCHTYPEB_PRIMITIVE:
             if  (class_id <= CLASSID_STRING)
                  xtype = primitives[class_id];
             else xtype = ETCH_XTRNL_TYPECODE_CUSTOM;   
             break;

        case ETCHTYPEB_ETCHOBJECT:
             xtype = ETCH_XTRNL_TYPECODE_ANY;
             break;

        default:
             xtype = ETCH_XTRNL_TYPECODE_CUSTOM;             
    }

    return xtype;
}


/*
 * bintdi_get_custom_structtype()
 * override of etchtagdata_get_custom_structtype. 
 * defers to value factory to return a non-disposable struct type 
 * for the specified class.
 */
etch_type* bintdi_get_custom_structtype(etch_object* thisx,
    const unsigned short obj_type, const unsigned short class_id)
{
    etch_type *static_type = NULL; 
    binary_tagged_data_input *tdi = (binary_tagged_data_input*) thisx;
    etch_value_factory  *vf = tdi->vf;
    if(vf) static_type = ((struct i_value_factory*)((etch_object*)vf)->vtab)->get_custom_struct_type(vf, class_id);
    return static_type;    
}


/**
 * bintdi_validate_value()
 * not an override.
 * &return an object *of the type being validated*, or null. this may be the same
 * object as the passed value, or may be different. for example if we are working
 * with an array of int, and a zero value was serialized, it will have been
 * deserialized into an etch_byte, and the int validator validate_value will  
 * create and return an etch_int32 in its stead.
 * null return indicates a validation error. null object return indicates value
 * read was logically null. eod object return indicates end of data. if validation
 * fails on a object, that object's destructor is invoked here.
 */
etch_object* bintdi_validate_value (binary_tagged_data_input* tdi, 
    etch_validator* vtor, boolean is_none_ok, etch_object* value)
{
    etch_object* resultobj = NULL;

    if(! value)
        return NULL;

    if  (NULL == vtor) {
        resultobj = NULL;
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "no validator for type %x class %x\n", ((etch_object*)value)->obj_type, ((etch_object*)value)->class_id);  
        etch_object_destroy(value);
        value = NULL;
        return NULL;
    }
    else if  (NULL == value) {
         return NULL;
    }
    else if  (etchtagdata_is_eod(value)  && is_none_ok) {
         resultobj = value;
    }
    else if  (etchtagdata_is_null(value) && is_none_ok) {
         resultobj = value;
    }
    else if  (NULL == (resultobj = vtor->validate_value (vtor, value)))
    {    
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "validation failed for type %x class %x\n", ((etch_object*)value)->obj_type, ((etch_object*)value)->class_id);
        etch_object_destroy(value);
        value = NULL;

         /* todo it would be nice to get an exception back across the wire here
          * rather than kibosh the session, but not sure what the path would be
          * to get it there, since we don't have a message yet at this point.
          */
    }
 

    /* resultobj may be the same object as value, or may be different.
     * if value was not an object of the type being validated, i.e. the validator
     * is the int validator but the value object is an etch_byte representing zero,
     * resultobj will be an etch_int32. if validation failed resultobj is null.
     */
    return resultobj;
}


/**
 * bintdi_validate_valuex()
 * invokes bintdi_validate_value on a value object, and if the validated object
 * to be returned is not the same object as the passed value object, that value
 * object's destructor is invoked. within the tdi, this will not necessarily
 * destroy the value object, as the tdi can pass protected static objects, 
 * such as an object representing null, for validation. 
 * @return a validated object of the same class as that of the supplied validator,
 * which may or may not be the same object as the passed value object.
 */
etch_object* bintdi_validate_valuex(binary_tagged_data_input* tdi, 
    etch_validator* vtor, boolean is_none_ok, etch_object* valueobj)
{
    etch_object* resultobj = bintdi_validate_value(tdi, vtor, is_none_ok, valueobj);

    if  (resultobj && valueobj && (resultobj != valueobj))
         etch_object_destroy(valueobj);
      
    return resultobj;
}
