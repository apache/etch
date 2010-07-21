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
 * id_name.c
 * an id_name is a base class for etch_field and etch_type, used to bind
 * a type or field name to an associated and unique id. the id is used for
 * certain operations, such as the key in a map, comparisons, wire encoding.
 * it owns memory for its name, but not necessarily that for its impl.
 * since the character name is only used internally, not on the wire, 
 * we are gradually converting this object to use an 8-bit character 
 * name, primarily to enable type-related logging.
 */

#include "etch_id_name.h"
#include "etch_encoding.h"
#include "etch_map.h"
#include "etch_exception.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"
#include <wchar.h>

/**
 * new_id_name() 
 * constructor accepting name. id is generated from a hash of the name.
 * allocates, initializes and returns an etch_id_name*. the specified
 * name is cloned, thus the etch_id_name owns all its memory (note that
 * this may not be the case for derived classes such as etch_type).
 */
etch_id_name* new_id_name(const wchar_t* name) 
{
    etch_id_name* idn = NULL;
    wchar_t* namecopy = NULL;
    size_t   bytelen  = 0;
    int result = 0;
    if (NULL == name) return NULL;

    idn = (etch_id_name*) new_object
        (sizeof(etch_id_name), ETCHTYPEB_ID_NAME, CLASSID_ID_NAME);

    ((etch_object*)idn)->destroy = destroy_id_name;
    ((etch_object*)idn)->clone   = clone_id_name;

    bytelen  = ( wcslen(name) + 1 ) * sizeof(wchar_t);
    namecopy = etch_malloc(bytelen, ETCHTYPEB_BYTES); 
    /* wcscpy_s(namecopy, bytelen, name) causes a release build to hang at
     * shutdown when the string is length 4 or more. let's not use wcscpy_s 
     * until we can determine the cause. */
#ifdef WIN32
    #pragma warning(disable:4996) /* disable nonsecure function warning */
#endif
    wcscpy(namecopy, name);

    /* carrying both name versions is first step in the 8-bit conversion */
    /* etch_unicode_to_utf8() returns us an etch_malloc'ed buffer which we own */
    // TODO: pool
    result = etch_encoding_transcode_wchar(&idn->aname, ETCH_ENCODING_UTF8, name, NULL);
    ETCH_ASSERT(result != -1);

    idn->name = namecopy; /* use wide name to compute hash etc. for now */
    idn->id = compute_id_name_id_from_widename((wchar_t*)name); 
    idn->namebytelen = bytelen;
    ((etch_object*)idn)->get_hashkey = id_name_get_hashkey;
    ((etch_object*)idn)->get_hashkey((etch_object*)idn);

    return idn;
}


/**
 * new_id_name_from_8bitname() 
 * constructor accepting narrow character name. 
 * name is maintained internally as unicode.  
 * see comments at new_id_name()
 * note that this ctor will not be needed after 8-bit conversion.
 */
etch_id_name* new_id_name_from_8bitname(char* name)
{
    etch_id_name* idn = NULL;
    int result;

    if (name)
    {
        wchar_t* out;
        // TODO: pool
        result = etch_encoding_transcode_to_wchar(&out, name, ETCH_ENCODING_UTF8, (unsigned int)strlen(name), NULL);
        if (result == -1) {
            return 0;
        }
        idn = new_id_name(out);
        etch_free(out);
    }

    return idn;
}


/**
 * clone_id_name() 
 * copy constructor 
 */
void* clone_id_name(void* data) 
{
  const etch_id_name* thatidn = (const etch_id_name*)data;
    etch_id_name* newidn = thatidn? new_id_name (thatidn->name): NULL;

    if (newidn)
    {   
        ((etch_object*)newidn)->obj_type = ((etch_object*)thatidn)->obj_type;
        ((etch_object*)newidn)->class_id = ((etch_object*)thatidn)->class_id;
    }

    return newidn;
}


/**
 * destroy_id_name()
 * destructor for an etch_id_name object.
 * deallocates all memory allocated for the object and its contents.
 */
int destroy_id_name(void* data)
{
    etch_id_name* thisp = (etch_id_name*)data;
    
    
    if (!is_etchobj_static_content(thisp))  
    {
        etch_object_destroy(thisp->impl); 

        etch_free (thisp->name); 
        etch_free( thisp->aname);
    }

    if (!is_etchobj_static_shell(thisp))    
        etch_free(thisp);
 
    return 0;
}


/**
 * compute_id_name_id() 
 * this algorithm and its result must be identical to that of the java binding.  
 */
int compute_id_name_id (const char* name)
{
    char c, *p = (char*) name;
    int  h6, i = 0, hash = 5381;
    const int numchars = (const int) strlen(name);

    for (; i < numchars; i++, p++)
    {
    	c  = *p;
    	h6 = hash << 6;
    	hash = (h6 << 10) + h6 - hash + c;
    }

    return hash;
}


/**
 * compute_id_name_id_from_widename()
 * see comments at compute_id_name_id()
 */
int compute_id_name_id_from_widename (const wchar_t* name)
{
    char *cbuf = NULL;
    int   idname_id = 0;
    int result;

    // TODO: pool
    result = etch_encoding_transcode_wchar(&cbuf, ETCH_ENCODING_UTF8, name, NULL);
    if (-1 == result) return -1;
    idname_id = compute_id_name_id(cbuf);

    etch_free(cbuf);
    return idname_id;
}


/**
 * is_equal_id_names()
 */
int is_equal_id_names (etch_id_name* thisx, etch_id_name* thatx)
{
    const int result = thisx && thatx 
      && (((etch_object*)thisx)->class_id == ((etch_object*)thatx)->class_id) && (thisx->id == thatx->id);  
    return result; 
}


/**
 * is_good_id_name()
 * verify that the id_name is complete
 */
int is_good_id_name (etch_id_name* p)
{
    const int result = p && p->name && p->id && p->namebytelen && ((etch_object*)p)->get_hashkey(p);
    return result; 
}


/**
 * id_name_get_hashkey
 * hashkey computation for an id_name object.
 * hash key is computed using the name string as hash source.
 */
uint32 id_name_get_hashkey (void* data)
{
  etch_object* idn = (etch_object*)data;
    etch_id_name* thisx = (etch_id_name*) idn;

    /* continue to use wide name to compute hash until it is eliminated */
    idn->hashkey = etch_get_wchar_hashkey(thisx->name);

    return idn->hashkey;
}
