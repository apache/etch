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
 * etch_serializer.c
 */

#include "etch_serializer.h"
#include "etch_default_value_factory.h"
#include "etch_arrayval.h"
#include "etch_encoding.h"
#include "etch_nativearray.h"
#include "etch_exception.h"
#include "etch_objecttypes.h"
#include "etch_map.h"
#include "etch_log.h"

// etch_log vars
const char* ETCH_SERIALIZER_CATEGORY = "etch_serializer";

etch_object* etchserializer_defexportval (etch_serializer* thisx, etch_object* objval);
etch_object* etchserializer_defimportval (etch_serializer* thisx, etch_object* structval);


/* - - - - - - - - - - - - - - - - - -
 * constructors/destructors
 * - - - - - - - - - - - - - - - - - -  
 */

/** 
 * destroy_etch_serializer()
 * etch_serializer destructor
 */
int destroy_etch_serializer(void* data) 
{
    etch_serializer* thisx = (etch_serializer*)data;

    if (!is_etchobj_static_content(thisx)){
        etch_object_destroy(thisx->impl);
        thisx->impl = NULL;
    }

    return destroy_objectex((etch_object*) thisx);
} 


/**
 * new_etch_serializer() 
 * etch_serializer constructor
 */
etch_serializer* new_etch_serializer(const int objsize) 
{
    etch_serializer* newobj = (etch_serializer*) new_object
        (sizeof(etch_serializer), ETCHTYPEB_SERIALIZER, CLASSID_NONE);

    ((etch_object*)newobj)->destroy = destroy_etch_serializer;
    newobj->import_value = etchserializer_defimportval;
    newobj->export_value = etchserializer_defexportval;

    return newobj;
}




/**
 * etch_serializer_init() 
 * generic static intitializer for import export helper serializers.
 * caller retains ownership of *all* parameters to this method.
 * @param obj_type type of the object being serialized
 * @param keyname name of an existing etch_field which will key the import
 * and export object in its serialization map. 
 * @param obj_class numeric etch object type and class id of the object 
   being serialized.  
 * @param c2tmap a vf's map of numeric class to object type associations.
 * @param vtor the validator object used to vet objects of this type.
 * @param new_szr a pointer to the constructor for serializers of the 
 * specified type.
 */
int etch_serializer_init(etch_type* obj_type, const wchar_t* keyname, 
    const unsigned obj_class, etch_hashtable* c2tmap, etch_validator* vtor, 
    etch_serializer_ctor new_szr)
{
    etch_serializer* newimpexhelper = NULL;
    /* serializer instantiation is awkward, since we are installing serializers 
     * to singleton types, but the class to type map is not singleton. 
     * investigate, and if reasonable, move the c2t maintenance to a higher  
     * level, and invoke this code only once per type instantiation.
     * unfortunately a type does not indicate its correpsonding class,
     * i.e. we know a type is "foo", but the type does not know it maps to
     * ETCHTYPE_FOO, CLASSID_FOO. so here, the serializers, in their ctors,
     * determine that mapping, and pass that class here. ideally we can eliminate
     * both the c2t and class parameters from the serializer initialization.
     */

    /* get reference to field key having specified name, not owned here */
    etch_field* szr_key = etchtype_get_field_by_name (obj_type, keyname);
    if (NULL == szr_key) return -1; 
 
    /* map this class identifier to associated type object */
    class_to_type_map_put(c2tmap, obj_class, obj_type);

    /* if the type's serializer is already instantiated, we don't want   
     * to do so again. this is the awkardness commented above. */
    if (etchtype_get_component_type(obj_type)) return 0; 

    /* set component type for arrays of specified class */
    etchtype_set_component_type(obj_type, obj_class);

    /* instantiate the new import/export helper */ 
    newimpexhelper = new_szr(obj_type, szr_key); 
   
    /* attach, and assign ownership of, the new helper object to the type */
    etchtype_set_impexphelper(obj_type, newimpexhelper);
    
    return NULL == vtor? 0: /* attach the supplied validator to the type */
           etchtype_put_validator(obj_type, clone_field(szr_key), (etch_object*) vtor);
}


/* - - - - - - - - - - - - - - - - - -
 * exception serializer
 * - - - - - - - - - - - - - - - - - -  
 */

/**
 * etchserializer_exception_export_value() 
 * export value for any exception
 * @param objval an etch_exception object. caller owns it,
 * and presumably will destroy it upon return from this method.
 * @return the exported disposable structvalue object. caller must cast it.
 */
etch_object* etchserializer_exception_export_value (etch_serializer* thisx, etch_object* objval)
{
    int result = 0;
    const int THISINITSIZE = 2;
    etch_structvalue* structval = NULL;
    etch_exception* excpobj = NULL;
    

    if (!is_etch_exception(objval)) 
    {
        ETCH_LOG(ETCH_SERIALIZER_CATEGORY, ETCH_LOG_ERROR, "value is not exception\n");
        return NULL;
    }
    excpobj = (etch_exception*) objval;
    
    structval = new_structvalue((etch_type*) thisx->type, THISINITSIZE);
    result = structvalue_put(structval, clone_field(thisx->field),((etch_object*)etch_exception_get_message(excpobj))->clone(((etch_object*)etch_exception_get_message(excpobj))));
    return (etch_object*) structval;
}


/**
 * etchserializer_exception_import_value() 
 * import value for any exception.
 * @param objval an etch_structvalue of appropriate type for the exception.
 * caller retains ownership of this object as with all imports.
 * @return an opaque etch object containing the imported exception. 
 *
 * caller owns and must destroy the returned object.  
 * use: etch_exception* x = get_exception(returnedobj);
 * to point into and inspect the exception.
 */
etch_object* etchserializer_exception_import_value
   (etch_serializer* thisx, etch_object* objval, excptype_t xtype) 
{
    wchar_t* unistring = NULL;
    etch_string* impstring = NULL;
    etch_exception* exception = NULL;
    int result;

    etch_structvalue* structval = (etch_structvalue*) objval;
    if (!is_etch_struct(structval)) return NULL;
    if (!structvalue_is_type(structval, thisx->type)) return NULL;

    /* fetch the exception string out of the struct. not owned here. */
    impstring = (etch_string*) structvalue_get (structval, thisx->field);
    if (!is_etch_string(impstring)) return NULL;

    // TODO: pool
    result = etch_encoding_transcode_to_wchar(&unistring, impstring->v.value, impstring->encoding, impstring->byte_count, NULL);
    ETCH_ASSERT(result != -1);

    /* create an exception object from the deserialized string. this object 
     * manages the exception memory according to flags supplied it. we set 
     * such a flag here, ETCHEXCP_FREETEXT, indicating that the exception 
     * owns the text buffer we give it. thus we no longer own unistring */
    exception = new_etch_exception (xtype);

    etch_exception_set_message(exception, new_stringw(unistring));
    etch_free(unistring);
    return (etch_object*) exception;
}


/**
 * etchserializer_excp_import_value() 
 * virtual import value for runtime exception.
 * @param objval an etch_structvalue of type etch runtime exception.
 * caller retains ownership of this object as with all imports.
 * @return an opaque etch object containing the imported exception. 
 *
 * caller owns and must destroy the returned object.  
 * use: etch_exception* x = get_exception(returnedobj);
 * to point into and inspect the exception.
 */
etch_object* etchserializer_excp_import_value(etch_serializer* thisx, etch_object* objval) 
{
    return etchserializer_exception_import_value(thisx, objval, EXCPTYPE_BUILTIN);
}



/**
 * new_exception_serializer() 
 * etch_serializer_excp constructor
 * @param type  - not owned
 * @param field - not owned
 */
etch_serializer* new_exception_serializer(etch_type* type, etch_field* field) 
{
    etch_serializer* newobj = new_etch_serializer(ETCH_DEFSIZE);

    ((etch_object*)newobj)->class_id = CLASSID_SERIALIZER_EXCP;
    newobj->type  = type;  /* not owned */
    newobj->field = field; /* not owned */

    newobj->export_value = etchserializer_exception_export_value;
    newobj->import_value = etchserializer_excp_import_value;

    return newobj;
}


/**
 * etch_serializer_exception_init() 
 * static intitializer for runtime exception serializer.
 * creates the impexp serializer and installs it to the supplied type.
 * @param thistype type of the serializer (runtime exception)
 */
int etch_serializer_exception_init(etch_type* thistype, etch_hashtable* c2tmap)
{
    const wchar_t* keyname = str_msg; /* vf static constant */
    const unsigned thisclass 
        = ETCHMAKECLASS(ETCHTYPEB_EXCEPTION, CLASSID_EXCEPTION);
    
    return etch_serializer_init(thistype, keyname, thisclass, c2tmap, 
        etchvtor_string_get(0), new_exception_serializer);
}


/* - - - - - - - - - - - - - - - - - -
 * runtime exception serializer
 * - - - - - - - - - - - - - - - - - -  
 */

/**
 * etchserializer_rtxcp_export_value() 
 * virtual export value for runtime exception
 * @param objval a, etchexception blob, caller owns it and presumably will
 * destroy it upon return from this method.
 * @return the exported disposable structvalue object. caller must cast it.
 */
etch_object* etchserializer_rtxcp_export_value(etch_serializer* thisx, etch_object* objval) 
{
    return etchserializer_exception_export_value(thisx, objval);
}


/**
 * etchserializer_rtxcp_import_value() 
 * virtual import value for runtime exception.
 * @param objval an etch_structvalue of type etch runtime exception.
 * caller retains ownership of this object as with all imports.
 * @return an opaque etch object containing the imported exception. 
 *
 * caller owns and must destroy the returned object.  
 * use: etch_exception* x = get_exception(returnedobj);
 * to point into and inspect the exception.
 */
etch_object* etchserializer_rtxcp_import_value(etch_serializer* thisx, etch_object* objval) 
{
    return etchserializer_exception_import_value(thisx, objval, EXCPTYPE_BUILTIN);
}


/**
 * new_runtime_exception_serializer() 
 * etch_serializer_rtxcp constructor
 * @param type  - not owned
 * @param field - not owned
 */
etch_serializer* new_runtime_exception_serializer(etch_type* type, etch_field* field) 
{
    etch_serializer* newobj = new_etch_serializer(ETCH_DEFSIZE);

    ((etch_object*)newobj)->class_id = CLASSID_SERIALIZER_RTXCP;
    newobj->type  = type;  /* not owned */
    newobj->field = field; /* not owned */

    newobj->export_value = etchserializer_rtxcp_export_value;
    newobj->import_value = etchserializer_rtxcp_import_value;

    return newobj;
}


/**
 * etch_serializer_rtxcp_init() 
 * static intitializer for runtime exception serializer.
 * creates the impexp serializer and installs it to the supplied type.
 * @param thistype type of the serializer (runtime exception)
 */
int etch_serializer_rtxcp_init(etch_type* thistype, etch_hashtable* c2tmap)
{
    const wchar_t* keyname = str_msg; /* vf static constant */
    const unsigned thisclass 
        = ETCHMAKECLASS(ETCHTYPEB_EXCEPTION, CLASSID_RUNTIME_EXCEPTION);
    
    return etch_serializer_init(thistype, keyname, thisclass, c2tmap, 
        etchvtor_string_get(0), new_runtime_exception_serializer);
}


/* - - - - - - - - - - - - - - - - - -
 * auth exception serializer
 * - - - - - - - - - - - - - - - - - -  
 */

/**
 * etchserializer_authxcp_export_value() 
 * virtual export value for auth exception
 * @param objval a, etchexception blob, caller owns it and presumably will
 * destroy it upon return from this method.
 * @return the exported disposable structvalue object. caller must cast it.
 */
etch_object* etchserializer_authxcp_export_value(etch_serializer* thisx, etch_object* objval) 
{
    return etchserializer_exception_export_value(thisx, objval);
}


/**
 * etchserializer_authxcp_import_value() 
 * virtual import value for runtime exception.
 * @param objval an etch_structvalue of type etch runtime exception.
 * caller retains ownership of this object as with all imports.
 * @return a nonspecific etch object containing the imported exception. 
 *
 * caller owns and must destroy the returned object.  
 * use: etch_exception* x = get_exception(returnedobj);
 * to point into and inspect the exception.
 */
etch_object* etchserializer_authxcp_import_value(etch_serializer* thisx, etch_object* objval) 
{
    return etchserializer_exception_import_value(thisx, objval, EXCPTYPE_BUILTIN);
}


/**
 * new_auth_exception_serializer() 
 * etch_serializer_auth exception constructor
 * @param type  - not owned
 * @param field - not owned
 */
etch_serializer* new_auth_exception_serializer(etch_type* type, etch_field* field) 
{
    etch_serializer* newobj = new_etch_serializer(ETCH_DEFSIZE);

    ((etch_object*)newobj)->class_id = CLASSID_SERIALIZER_AUTHXCP;
    newobj->type  = type;  /* not owned */
    newobj->field = field; /* not owned */

    newobj->export_value = etchserializer_authxcp_export_value;
    newobj->import_value = etchserializer_authxcp_import_value;

    return newobj;
}


/**
 * etch_serializer_authxcp_init() 
 * static intitializer for auth exception serializer.
 * creates the impexp serializer and installs it to the supplied type.
 * @param thistype type of the serializer (runtime exception)
 */
int etch_serializer_authxcp_init(etch_type* thistype, etch_hashtable* c2tmap)
{
    const wchar_t* keyname = str_msg; /* vf static constant */
    const unsigned thisclass 
        = ETCHMAKECLASS(ETCHTYPEB_EXCEPTION, CLASSID_AUTH_EXCEPTION);

    return etch_serializer_init(thistype, keyname, thisclass, c2tmap, 
        etchvtor_string_get(0), new_auth_exception_serializer);
}


/* - - - - - - - - - - - - - - - - - -
 * list serializer
 * - - - - - - - - - - - - - - - - - -  
 */

/**
 * etchserializer_list_export_value() 
 * virtual export value for list
 * @param objval an etch_arraylist containing values to export to tdo. 
 * caller retains ownership of this object; this method however sets the readonly
 * mark on this object, such that it will not destroy its content, which was
 * assigned to the arraylist wrapped in the returned struct. 
 * the c binding internally models a list type as an etch_arraylist.
 * @return the exported structvalue object, containing an etch_arraylist. 
 * since we can't pass around raw arrays as does the java binding in this case,
 * this serializer chooses to use an arraylist as the export object type for lists.
 * while it may seem convoluted that the exported from and exported to objects are
 * the same object type (not the same object however), the reason is the vf export
 * interface contract, i.e. caller relinquishes memory for the export from object,
 * and acquires memory for the export to objects.
 */
etch_object* etchserializer_list_export_value(etch_serializer* thisx, etch_object* objval) 
{
    const int THISINITSIZE = 2;
    int result = 0;
    
    etch_arraylist* listobj = NULL;
    etch_nativearray* param = NULL;
	etch_iterator* iterator = NULL;
    etch_structvalue* outstruct = NULL;
	etch_object* current = NULL;
	int i = 0;
    
	if (!is_etch_arraylist(objval)) return NULL;
	listobj = (etch_arraylist*) objval;
	
	param = new_etch_nativearray(CLASSID_ARRAY_OBJECT, sizeof(etch_object*),1,listobj->count,0,0);
    
	param->content_obj_type = ETCHTYPEB_PRIMITIVE;
	param->content_class_id = CLASSID_OBJECT;

    //take ownership of elements
    listobj->is_readonly = TRUE;
    param->is_content_owned = TRUE;

    iterator = new_iterator(listobj, &listobj->iterable);

	while(iterator->has_next(iterator)){
		current = iterator->current_value;

		param->put1(param,&current,i++);
		iterator->next(iterator);
	}
    etch_object_destroy(iterator);

    /* assign ownership of the new list to the returned struct */
    outstruct = new_structvalue((etch_type*) thisx->type, THISINITSIZE);

	result = structvalue_put(outstruct, clone_field(thisx->field), (etch_object*) param);
    return (etch_object*) outstruct; /* caller owns this structvalue */
}


/**
 * etchserializer_list_import_value() 
 * virtual import value for etch list.
 * @param objval an etch_structvalue of type etch list.
 * caller retains ownership of this object as with all imports.
 * @return a *disposable* etch_arraylist containing the imported data objects. 
 *
 * note that the c binding works a bit differently that does the java here.
 * java tdi populates the structvalue with an object[]. in c the tdi inserts a 
 * synchronized etch_arraylist to the structvalue. 
 */
etch_object* etchserializer_list_import_value(etch_serializer* thisx, etch_object* objval) 
{
    etch_arrayvalue* inarray = NULL;
	etch_arraylist* outarray = NULL;
    etch_structvalue* instruct = (etch_structvalue*) objval;
	etch_iterator iterator;
	etch_object* current = NULL;

    if (!is_etch_struct(instruct)) return NULL;
    if (!structvalue_is_type(instruct, thisx->type)) return NULL;

    /* fetch the arrayvalue from the struct. struct owns it. */
    inarray = (etch_arrayvalue*) structvalue_get(instruct, thisx->field);
	if (!is_etch_arrayvalue(inarray)) return NULL;

	outarray = new_etch_arraylist(0,0);
	arrayvalue_set_iterator(inarray, &iterator);
	while(iterator.has_next(&iterator)){
		current = iterator.current_value;
		etch_arraylist_add(outarray,current);
		iterator.next(&iterator);
	}
	
    /* value factory import_custom_value() will destroy() the import struct
     * which will call destructors on its content. we mark the import list
     * object such that its content will not be destroyed with it, since
     * we just copied that content to the return list */
    arrayvalue_set_static_content(inarray, TRUE);

    return (etch_object*) outarray;  /* caller owns the returned list */
}


/**
 * new_list_serializer() 
 * etch_serializer_list constructor
 * @param type  - not owned
 * @param field - not owned
 */
etch_serializer* new_list_serializer(etch_type* type, etch_field* field) 
{
    etch_serializer* newobj = new_etch_serializer(ETCH_DEFSIZE);

    ((etch_object*)newobj)->class_id = CLASSID_SERIALIZER_LIST;
    newobj->type  = type;  /* not owned */
    newobj->field = field; /* not owned */

    newobj->export_value = etchserializer_list_export_value;
    newobj->import_value = etchserializer_list_import_value;

    return newobj;
}


/**
 * etch_serializer_list_init() 
 * static intitializer for list serializer.
 * creates the list impex serializer and installs it to the supplied type.
 * @param thistype type of the serializer (list)
 */
int etch_serializer_list_init(etch_type* thistype, etch_hashtable* c2tmap)
{
    const wchar_t* key_name = str_values; /* vf static constant */
    const unsigned thisclass 
        = ETCHMAKECLASS(ETCHTYPEB_ETCHLIST, CLASSID_ETCH_LIST);

    return etch_serializer_init(thistype, key_name, thisclass, c2tmap, 
        etchvtor_object_get(1), new_list_serializer);
}


/* - - - - - - - - - - - - - - - - - -
 * map serializer
 * - - - - - - - - - - - - - - - - - -  
 */

/**
 * etchserializer_map_export_value() 
 * virtual export value for etch map
 * @param objval an etch_hashtable containing values to export. caller owns it,
 * and presumably will destroy it on return from this method. this method marks
 * this object such that its destructor will not destroy its content objects, 
 * ownership of which is transferred here to the returned arraylist.
 * @return the exported structvalue object in which the lone entry is an
 * etch_arraylist containing the map keys and values as consecutive entries.
 * caller owns this returned structvalue.
 */
etch_object* etchserializer_map_export_value(etch_serializer* thisx, etch_object* objval) 
{
    int result = 0, mapsize = 0, listsize = 0, is_expected_maptype = 0;
    etch_iterator iterator;
    const int THISINITSIZE = 2;
    const int is_this_a_set = (thisx->type->id == builtins._mt__etch_set->id);
    etch_hashtable* impmap  = NULL;
    etch_nativearray* explist = NULL;
    etch_structvalue* expstruct = NULL;
    int i = 0;
    if (!is_etch_hashtable(objval)) return NULL;

    impmap   = (etch_hashtable*) objval;
    mapsize  = etchmap_count(impmap);
    listsize = is_this_a_set? mapsize: mapsize * 2;


    /* the underlying hashtable for an etch map must be explicitly marked 
     * as having object-key, object-value content, since the export list
     * content is object-key, object-value, object-key, object value, etc. */
    is_expected_maptype = is_this_a_set?
        impmap->content_type == ETCHHASHTABLE_CONTENT_OBJECT_NONE:
        impmap->content_type == ETCHHASHTABLE_CONTENT_OBJECT_OBJECT;
    if (!is_expected_maptype) return NULL;
   
    /* create native array for export, marked such that 
     * destructors will not be called on list content */
    explist = new_etch_nativearray(CLASSID_ARRAY_OBJECT, sizeof(etch_object*),1,listsize,0,0);
    explist->content_obj_type = ETCHTYPEB_PRIMITIVE;
    explist->content_class_id = CLASSID_OBJECT;

    set_iterator(&iterator, impmap, &impmap->iterable);

    while(iterator.has_next(&iterator))  /* copy map data to array */
    {   
		explist->put1(explist,&iterator.current_key,i++);
        if  (!is_this_a_set)
		{
			explist->put1(explist,&iterator.current_value,i++);
		}
        iterator.next(&iterator);
    }

    /* mark caller's map such that it will not destroy its content, 
     * ownership of which we just transferred to the export array */
    impmap->is_readonly_keys = impmap->is_readonly_values = TRUE;
    impmap->freehook = etch_noop_clear_handler;
    
    expstruct = new_structvalue((etch_type*) thisx->type, THISINITSIZE);

    /* here we assign ownership of the new arraylist to the struct */
    result = structvalue_put(expstruct, clone_field(thisx->field), (etch_object*)explist);

    return (etch_object*) expstruct; /* caller owns this structvalue */
}


/**
 * etchserializer_map_import_value() 
 * virtual import value for etch map.
 * @param objval an etch_structvalue of type etch map, wrapping a
 * single entry of etch_arrayvalue, wrapping an etch_arraylist in which
 * consecutive entries represent a map key and a map value. this method
 * will mark the arraylist such that it will not destroy its object content.
 * caller retains ownership of this object as with all imports.
 * @return a disposable etch_hashtable containing the imported data objects.
 * caller must cast it and destroy it. 
 *
 * note that the c binding works a bit differently that does the java here.
 * java tdi populates the structvalue with an object[]. in c the tdi inserts a 
 * synchronized etch_arraylist wrapped by an etch_arrayvalue, to the structvalue. 
 */
etch_object* etchserializer_map_import_value(etch_serializer* thisx, etch_object* objval) 
{
    int i = 0, result = 0, listcount = 0;
    etch_hashtable* expmap = NULL;
    etch_arrayvalue* implist = NULL;
    etch_structvalue* impstruct = NULL;
    const int is_this_a_set = thisx->type->id == builtins._mt__etch_set->id;
    if (!is_etch_struct(objval)) return NULL;

    impstruct = (etch_structvalue*) objval;
    if (!structvalue_is_type(impstruct, thisx->type)) return NULL;

    /* get the synchronized arraylist out of the struct and mark the list 
     * such that it no longer owns its object content, ownership of which we
     * are transferring to the exported map. 
     */
    implist = (etch_arrayvalue*) structvalue_get(impstruct, thisx->field);
    if (!is_etch_arrayvalue(implist)) return NULL;
    arrayvalue_set_static_content(implist, TRUE);
	listcount = implist->list->count;

    /* instantiate a map configured appropriately for the import data */
    /* todo code a new_synchronized_map() constructor and use it here */
    if(is_this_a_set) {
        expmap = new_etch_set(listcount);
    }
    else {
        expmap = new_etch_map(listcount / 2);
    }
    expmap->content_obj_type = implist->content_obj_type;
    expmap->content_class_id = implist->content_class_id;  

    while(i < (const int) listcount) /* copy data objects to the map */ 
    {   
		etch_object* key = arrayvalue_get(implist, i++);
        if (NULL == key) continue;
        
        if  (is_this_a_set)
            result = etchmap_set_add(expmap, key);
        else
            result = etchmap_map_add(expmap, key, arrayvalue_get(implist, i++));

        if (-1 == result)
            ETCH_LOG(ETCH_SERIALIZER_CATEGORY, ETCH_LOG_ERROR, "map insert error on key %u", key);
    }
    return (etch_object*) expmap;
}

/**
 * new_map_serializer() 
 * etch_serializer_map constructor
 * @param type  - not owned
 * @param field - not owned
 */
etch_serializer* new_map_serializer(etch_type* type, etch_field* field) 
{
    etch_serializer* newobj = new_etch_serializer(ETCH_DEFSIZE);

    ((etch_object*)newobj)->class_id = CLASSID_SERIALIZER_MAP;
    newobj->type  = type;  /* not owned */
    newobj->field = field; /* not owned */

    newobj->export_value = etchserializer_map_export_value;
    newobj->import_value = etchserializer_map_import_value;

    return newobj;
}


/**
 * etch_serializer_map_init() 
 * static intitializer for map serializer.
 * creates the map serializer and installs it to the supplied type.
 * @param thistype type of the serializer (map)
 */
int etch_serializer_map_init(etch_type* thistype, etch_hashtable* c2tmap)
{
    const wchar_t* keyname = str_keys_values; /* vf static constant */
    const unsigned thisclass 
        = ETCHMAKECLASS(ETCHTYPEB_ETCHMAP, CLASSID_ETCH_MAP);
   
    return etch_serializer_init(thistype, keyname, thisclass, c2tmap, 
        etchvtor_object_get(1), new_map_serializer);
}


/* - - - - - - - - - - - - - - - - - -
 * set serializer
 * - - - - - - - - - - - - - - - - - -  
 */

/**
 * etchserializer_set_export_value() 
 * virtual export value for etch set
 * @param objval an etch_hashtable containing values to export. caller owns it,
 * and presumably will destroy it on return from this method. this method marks
 * the map object such that it will not destroy its content objects.
 * @return the exported structvalue object in which the lone entry is an
 * etch_arrayvalue containing the map keys
 * caller owns memory for the returned object.
 */
etch_object* etchserializer_set_export_value(etch_serializer* thisx, etch_object* objval) 
{
    return etchserializer_map_export_value(thisx, objval); /* caller owns */
}


/**
 * etchserializer_set_import_value() 
 * virtual import value for etch set.
 * @param objval an etch_structvalue of type etch set, wrapping a single entry
 * of etch_arrayvalue, wrapping an etch_arraylist containing the set values.
 * this method will mark the arraylist such that it will not destroy its content.
 * caller retains ownership of this object as with all imports.
 * @return a disposable etch_hashtable containing the imported data objects.
 * caller must cast it and destroy it. 
 * note that the c binding works a bit differently that does the java here.
 * java tdi populates the structvalue with an object[]. in c the tdi inserts a 
 * synchronized etch_arraylist wrapped by an etch_arrayvalue, to the structvalue. 
 */
etch_object* etchserializer_set_import_value(etch_serializer* thisx, etch_object* objval) 
{
    return etchserializer_map_import_value(thisx, objval); 
}


/**
 * new_set_serializer() 
 * etch_serializer_set constructor
 * @param type  - not owned
 * @param field - not owned
 */
etch_serializer* new_set_serializer(etch_type* type, etch_field* field) 
{
    etch_serializer* newobj = new_etch_serializer(ETCH_DEFSIZE);

    ((etch_object*)newobj)->class_id = CLASSID_SERIALIZER_SET;
    newobj->type  = type;  /* not owned */
    newobj->field = field; /* not owned */

    newobj->export_value = etchserializer_set_export_value;
    newobj->import_value = etchserializer_set_import_value;

    return newobj;
}


/**
 * etch_serializer_set_init() 
 * static intitializer for set serializer.
 * creates the set serializer and installs it to the supplied type.
 * @param thistype type of the serializer (set)
 */
int etch_serializer_set_init(etch_type* thistype, etch_hashtable* c2tmap)
{
    const wchar_t* keyname = str_keys; /* vf static constant */
    const unsigned thisclass 
        = ETCHMAKECLASS(ETCHTYPEB_HASHTABLE, CLASSID_ETCH_SET);
       
    return etch_serializer_init(thistype, keyname, thisclass, c2tmap, 
        etchvtor_object_get(1), new_set_serializer);
}


/* - - - - - - - - - - - - - - - - - -
 * date serializer
 * - - - - - - - - - - - - - - - - - -  
 */

/**
 * etchserializer_date_export_value() 
 * virtual export value for date
 * @param objval an etch_date object for export to tdo. 
 * caller retains ownership of this object. 
 * @return the exported structvalue object. caller owns it and must cast it.
 */
etch_object* etchserializer_date_export_value(etch_serializer* thisx, etch_object* objval) 
{
    const int THISINITSIZE = 2;
    etch_structvalue* expstruct = NULL;
    if (!is_etch_date(objval)) return NULL;

    expstruct = new_structvalue((etch_type*) thisx->type, THISINITSIZE);
#ifdef WIN32
    // assign ownership of the wrapped 64-bit integer to the struct
    // on win32 systems the time_t struct is a int64
    structvalue_put(expstruct, clone_field(thisx->field), 
       (etch_object*) new_int64(((etch_date*)objval)->value));
#else
    // assign ownership of the wrapped 32-bit integer to the struct
    // on linux systems the time_t struct is a int32, to get no negativ
    // value here, we cast the 32-bit value to unsigned long int
    structvalue_put(expstruct, clone_field(thisx->field), 
       (etch_object*) new_int64((unsigned long int)((etch_date*)objval)->value));
#endif
    return (etch_object*) expstruct; /* caller owns this structvalue */
}


/**
 * etchserializer_date_import_value() 
 * virtual import value for etch date.
 * @param objval an etch_structvalue of type etch date.
 * caller retains ownership of this object as with all imports.
 * @return a *disposable* etch_date object
 */
etch_object* etchserializer_date_import_value(etch_serializer* thisx, etch_object* objval) 
{
    etch_date* outdate = NULL;
    etch_int64* timeval = NULL;
    etch_structvalue* instruct = NULL;
    if (!is_etch_struct(objval)) return NULL;

    instruct = (etch_structvalue*) objval;
    if (!structvalue_is_type(instruct, thisx->type)) return NULL;

    /* fetch the arrayvalue out of the struct. struct owns it */
    timeval = (etch_int64*) structvalue_get(instruct, thisx->field);
    if (!is_etch_int64(timeval)) return NULL;

    outdate = new_date();
    outdate->value = (time_t) timeval->value;

    return (etch_object*) outdate;  /* caller owns this object */
}


/**
 * new_date_serializer() 
 * etch_serializer_date constructor
 * @param type  - not owned
 * @param field - not owned
 */
etch_serializer* new_date_serializer(etch_type* type, etch_field* field) 
{
    etch_serializer* newobj = new_etch_serializer(ETCH_DEFSIZE);

    ((etch_object*)newobj)->class_id = CLASSID_SERIALIZER_DATE;
    newobj->type  = type;  /* not owned */
    newobj->field = field; /* not owned */

    newobj->export_value = etchserializer_date_export_value;
    newobj->import_value = etchserializer_date_import_value;

    return newobj;
}


/**
 * etch_serializer_date_init() 
 * static intitializer for date serializer.
 * creates the date serializer and installs it to the supplied type.
 * @param thistype type of the serializer (date)
 */
int etch_serializer_date_init(etch_type* thistype, etch_hashtable* c2tmap)
{
    const wchar_t* keyname = str_date_time; /* vf static constant */
    const unsigned thisclass 
        = ETCHMAKECLASS(ETCHTYPEB_PRIMITIVE, CLASSID_DATE);
          
    return etch_serializer_init(thistype, keyname, thisclass, c2tmap, 
        etchvtor_int64_get(0), new_date_serializer);
}


/* - - - - - - - - - - - - - - - - - -
 * misc  
 * - - - - - - - - - - - - - - - - - -  
 */

 etch_object* etchserializer_defexportval(etch_serializer* s, etch_object* x)
{
    return NULL;
}

etch_object* etchserializer_defimportval (etch_serializer* s, etch_object* x)
{
    return NULL;
}

