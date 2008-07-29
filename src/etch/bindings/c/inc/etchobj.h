/**
  etchobj.h 
  etch object definition
*/

#ifndef ETCHOBJ_H
#define ETCHOBJ_H

#include "etch.h"


/**
 * objmask
 * mask over all etch objects
 */
typedef struct objmask
{
  unsigned short obj_type; 
  unsigned short class_id;
  void* addr;
} objmask;


/**
 * impl_destroy_handler
 * function signature for an instance data destructor
 */
typedef int (*impl_destroy_handler) (void*);


struct etchexception; /* declared in etchexcp.h which we can't include here */

/**
 * when an etchobject is returned from a request such as a method, it will contain
 * an etchresult to indicate the result of the request, and any possible exception.
 */ 
typedef struct etchresult
{
    int resultcode;   
    int reasoncode;
    struct etchexception* exception; 

} etchresult;


/**
 * macros to interpret etchobject etchresult and etchexception content
 */
#define is_exception(etchobj)  ( etchobj->result && etchobj->result->exception )
#define get_exception(etchobj) ( etchobj->result? etchobj->result->exception: NULL ) 
#define get_result(etchobj)    ( etchobj->result? etchobj->result->resultcode: 0 )
#define get_reason(etchobj)    ( etchobj->result? etchobj->result->reasoncode: 0 )
#define is_resulterror(etchobj)( etchobj->result? etchobj->result->resultcode < 0: 0 )


struct etchobject; /* forward declare so we can reference in vtable definition */


/**
 * i_etchobject interface (etchobject virtual function table)
 * all etchobjects implement this set of methods.
 */
struct i_etchobject
{
    unsigned short obj_type;    /* must be ID_ETCHOBJECT */
    unsigned short class_id;    /* zero */
    struct i_etchobject* vtab;   

    struct etchobject* (*clone) (struct etchobject* thisp);  /* object copy */ 
    void  (*destroy) (struct etchobject* thisp);             /* object destructor */
    int64 (*hashcode)(struct etchobject* thisp);             /* hashcode */
    void  (*destroy_value)(struct etchobject*, void* value); /* object value free */
    void  (*clone_value)  (struct etchobject*, void* value); /* object value copy */
};

typedef struct i_etchobject i_etchobject;

#define ETCHOBJ_VTABSIZE sizeof(struct i_etchobject) 



/**
 * etchobject  
 * object wrapper for any etch type 
 */
struct etchobject
{
    unsigned short obj_type;  
    unsigned short class_id;  
    i_etchobject*  vtab;  
    /* virtual methods are clone(), destroy(), clone_value(), destroy_value() */      
 
    union objval_t     /* map storage for object value or pointer to value */
    {
        unsigned char value_byte;
        unsigned char value_bool;
        signed char   value_int8;
        short         value_int16;
        int           value_int32;
        int64         value_int64;    
        float         value_ieee32;
        double        value_ieee64;
        void*         value_ptr_to; /* allocated value e.g. string */           
    };

    /* 
     * objval: value of object (if primitive), or a pointer to the allocated    
     * value such as a string, array, etc.
     */
    union objval_t objval;  
    /* 
     * size: data size in bytes, e.g. total bytes in array, string byte   
     * length (not character count) including terminator, etc.
     */
    int64 size;  
    /* 
     * type: type of this object. type is a compound item of subtypes
     * packed into 32 bits. see objtypes.h. this is used for internal C
     * object identification, distinct from the etch (java) object type
     */
    enum objtype type;      

    unsigned char is_null;          /* boolean: object value is null */
    unsigned char is_static_object; /* boolean: object not to be freed */
    unsigned char is_static_data;   /* boolean: value not to be freed/cloned */
    unsigned char is_free_asap;     /* boolean: destroy this object after use */ 
    /* 
     * tag: arbitrary user data
     */
    void* tag;
    /* 
     * result: result plus exception. result is populated only when object 
     * is a returned object such as a function return value.
     */
    etchresult* result;  

};  /* struct etchobject */

typedef struct etchobject etchobject;


/* 
 * default i_etchobject virtual method implementations
 */
void   destroy_etchobject(etchobject* this_p);
void   destroy_etchobject_value(etchobject* this_p, void* etchobject_value);
void   copy_etchobject_value(etchobject* this_p, void* etchobject_value);
int64  hashcode_etchobject  (etchobject* this_p);
etchobject* clone_etchobject(etchobject* this_p);


/* 
 * public utility methods
 */

int   verify_object(objmask* obj, const unsigned short type, const unsigned short id, void** out);
int   destroy_instancedata(void* impl, const int objtype);
void* new_vtable(const void* parentvtab, const size_t size, const short type, const short classid);
short short_type(unsigned i, unsigned j); 
void* get_base_vtable(objmask* obj);


/* 
 * "protected" methods  
 */
etchobject* new_etchobject(const int object_type);  
wchar_t*    etchobject_clone_string_value(etchobject* thisp);
void*       etchobject_clone_arraylist(etchobject* thisp);
etchresult* new_etchresult(const int result, const int reason);


#endif /* #ifndef ETCHOBJ_H */