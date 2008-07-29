/* 
 * objtypes.h -- constants for internal object types.
 * a type can stand alone outside a class, so the higher levels of the 
 * hierarchy identify whether the item is or is not an object, etc.
 * for example a type code could identify an item as a native array
 * of primitive integers, for example, which would be distinct from an 
 * object which wrapped a native array of primitive integers. so if we
 * want for example to use a type code to pair with a pointer to say
 * this pointer points at a hashtable, we can do so, rather than always
 * saying the pointer points at object, and get the type from the object.
 */

#ifndef ETCHOBJTYPES_H
#define ETCHOBJTYPES_H

/** 
 * byte 3 of a 32-bit object type where: | b3 | b2 | b1 | b0 |
 * This subtype identifies the logical object's class-ness.
 * It is an 8-bit biststring of boolean bitflags.
 */
typedef enum objtype_a   
{  
   ETCHTYPEA_IS_ARRAY      = 0x1,  /* non-wrapped native array (1) or scalar (0) */  
   ETCHTYPEA_IS_COLLECTION = 0x2,  /* object is a non-wrapped collection of type */  
   ETCHTYPEA_IS_OBJECT     = 0x4,  /* object is or is not an etch object */ 
   ETCHTYPEA_IS_CLASS      = 0x8,  /* object is or is not a class object */ 
   ETCHTYPEA_IS_PRIMITIVE  = 0x10, /* object is primitive (1) e.g. int or whchar_t */ 
   ETCHTYPEA_IS_CONT_ARRAY = 0x20, /* object or collection contains native array of type */  
}  objtype_a;   


/** 
 * byte 2 of a 32-bit object type where: | b3 | b2 | b1 | b0 |
 * This subtype identifies the item's data type, or an array's content type
 * It is an 8-bit value 0-255
 */
typedef enum objtype_b   
{  
   ETCHTYPEB_UNDEFINED    = 0x0, 
   ETCHTYPEB_BYTE         = 0x1, 
   ETCHTYPEB_BOOL         = 0x2, 
   ETCHTYPEB_INT8         = 0x3,    
   ETCHTYPEB_INT16        = 0x4,
   ETCHTYPEB_INT32        = 0x5,
   ETCHTYPEB_INT64        = 0x6,
   ETCHTYPEB_IEEE32       = 0x7,
   ETCHTYPEB_IEEE64       = 0x8,
   ETCHTYPEB_STRING       = 0xa,   
   ETCHTYPEB_CUSTOM       = 0xb,   
   ETCHTYPEB_EXTERN       = 0xc,  
   ETCHTYPEB_ETCHOBJECT   = 0xd,  
   ETCHTYPEB_HASHTABLE    = 0xe,  
   ETCHTYPEB_ARRAYLIST    = 0xf, 
   ETCHTYPEB_VTABLE       = 0x10,  
   ETCHTYPEB_EXCEPTION    = 0x11,  
   ETCHTYPEB_CACHEREC     = 0x12,  
   ETCHTYPEB_BYTES        = 0x13,  
   ETCHTYPEB_ID_NAME      = 0x14,
   ETCHTYPEB_FIELD        = 0x15,
   ETCHTYPEB_TYPE         = 0x16,
   ETCHTYPEB_STRUCTVAL    = 0x17,
   ETCHTYPEB_ARRAYVAL     = 0x18,
   ETCHTYPEB_VALUEFACTORY = 0x18,
   ETCHTYPEB_RESULT       = 0x20,
   ETCHTYPEB_TAGDATA      = 0x21,
   ETCHTYPEB_TAGDATAINP   = 0x22,
   ETCHTYPEB_TAGDATAOUT   = 0x23,
   ETCHTYPEB_TDIOBJ       = 0x23,
   ETCHTYPEB_TDOOBJ       = 0x24,
   ETCHTYPEB_ARRAYELEMENT = 0x25,
   ETCHTYPEB_STRUCTELEMENT= 0x26,
   ETCHTYPEB_INSTANCEDATA = 0x27,
   ETCHTYPEB_COLLECTION   = 0x28,
   ETCHTYPEB_ITERATOR     = 0x29,
   ETCHTYPEB_USER         = 0x80,
}  objtype_b;   


/** 
 * byte 1 of a 32-bit object type where: | b3 | b2 | b1 | b0 |
 * This subtype identifies the item's custom data type
 * It is an 8-bit value 0-255
 */
typedef enum objtype_c  /* this is second NIBBLE */
{  
   ETCHTYPEC_NONE = 0x0,   
}  objtype_c;   


/** 
 * byte 0 of a 32-bit object type where: | b3 | b2 | b1 | b0 |
 * This subtype identifies the item's sub-type
 * It is largely a catch-all for object discrimination
 */
typedef enum objtype_d  /* this is second NIBBLE */
{  
   ETCHTYPED_NONE     = 0x0,   
   ETCHTYPED_JENKINS  = 0x1,  
}  objtype_d;   


/** 
 * global next custom object type available for assignment.
 * This should be initialized to ETCHTYPED_USER either in startup code or lazyload.
 * this should probably go into etch_global.h, and be initialized in etch_global.c.
 */
int next_avail_objtype;  /* = ETCHTYPED_USER */


/** 
 * macros to construct object type | b3 | b2 | b1 | b0 | 
 */
#define MAKETYPE_ABCD(a,b,c,d) ((a << 24) | (b << 16) | (c << 8) | d)
#define MAKETYPE_ACD(a,c,d)    ((a << 24) | (c << 8)  | d)
#define MAKETYPE_AC(a,c)       ((a << 24) | (c << 8)) 
#define MAKETYPE_AD(a,d)       ((a << 24) | d)
#define MAKETYPE_BD(b,d)       ((b << 16) | d)
#define MAKETYPE_BB(b,bd)      ((b << 16) | bd)
#define MAKETYPE_B(b)           (b << 16) 

/** 
 * macros to deconstruct object type | b3 | b2 | b1 | b0 | 
 */
#define SUBTYPEA(n) ((n & 0xff000000) >> 24)
#define SUBTYPEB(n) ((n & 0xff0000) >> 16)
#define SUBTYPEC(n) ((n & 0xff00) >> 8)
#define SUBTYPED(n)  (n & 0xff) 
#define PARSETYPE(n,a) do {a[0]=((n & 0xff000000)>>24); a[1]=((n & 0xff0000)>>16); \
  a[2]=((n & 0xff00)>>8); a[3]=(n & 0xff);} while(0);


/** 
 * convenience object type constructors
 */
typedef enum objtyp
{ ETCHTYPE_NONE = 0x0,  
  ETCHTYPE_HASHTABLE_JENK     = MAKETYPE_BD (ETCHTYPEB_HASHTABLE,ETCHTYPED_JENKINS),
  ETCHTYPE_VTABLE_JENK        = MAKETYPE_BD (ETCHTYPEB_VTABLE,ETCHTYPED_JENKINS), 
  ETCHTYPE_VTABLE_ETCHOBJECT  = MAKETYPE_BB (ETCHTYPEB_VTABLE,ETCHTYPEB_ETCHOBJECT), 
  ETCHTYPE_VTABLE_ID_NAME     = MAKETYPE_BB (ETCHTYPEB_VTABLE,ETCHTYPEB_ID_NAME), 
  ETCHTYPE_VTABLE_TAGDATAINP  = MAKETYPE_BB (ETCHTYPEB_VTABLE,ETCHTYPEB_TAGDATAINP), 
  ETCHTYPE_VTABLE_TAGDATAOUT  = MAKETYPE_BB (ETCHTYPEB_VTABLE,ETCHTYPEB_TAGDATAOUT), 
  ETCHTYPE_VTABLE_TDIOBJ      = MAKETYPE_BB (ETCHTYPEB_VTABLE,ETCHTYPEB_TDIOBJ), 
  ETCHTYPE_VTABLE_TDOOBJ      = MAKETYPE_BB (ETCHTYPEB_VTABLE,ETCHTYPEB_TDOOBJ), 
  ETCHTYPE_VTABLE_ARRAYVALUE  = MAKETYPE_BB (ETCHTYPEB_VTABLE,ETCHTYPEB_ARRAYVAL), 
  ETCHTYPE_VTABLE_STRUCTVALUE = MAKETYPE_BB (ETCHTYPEB_VTABLE,ETCHTYPEB_STRUCTVAL),
  ETCHTYPE_VTABLE_VALUEFACTORY= MAKETYPE_BB (ETCHTYPEB_VTABLE,ETCHTYPEB_VALUEFACTORY),
  ETCHTYPE_ARRAYVALUE         = MAKETYPE_AC (ETCHTYPEA_IS_PRIMITIVE,ETCHTYPEB_ARRAYVAL), 
  ETCHTYPE_SCALAR_INT32       = MAKETYPE_AC (ETCHTYPEA_IS_PRIMITIVE,ETCHTYPEB_INT32), 
  ETCHTYPE_TAGDATA            = MAKETYPE_B  (ETCHTYPEB_TAGDATA), 
  ETCHTYPE_TAGDATAINP         = MAKETYPE_B  (ETCHTYPEB_TAGDATAINP),
  ETCHTYPE_TAGDATAOUT         = MAKETYPE_B  (ETCHTYPEB_TAGDATAOUT),
  ETCHTYPE_BOXED_UNDEFINED    = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_UNDEFINED), 
  ETCHTYPE_BOXED_BYTE         = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_BYTE), 
  ETCHTYPE_BOXED_BOOL         = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_BOOL), 
  ETCHTYPE_BOXED_INT32        = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_INT32), 
  ETCHTYPE_BOXED_INT8         = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_INT8), 
  ETCHTYPE_BOXED_INT16        = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_INT16), 
  ETCHTYPE_BOXED_INT64        = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_INT64), 
  ETCHTYPE_BOXED_IEEE32       = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_IEEE32), 
  ETCHTYPE_BOXED_IEEE64       = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_IEEE64), 
  ETCHTYPE_NATIVE_STRING      = MAKETYPE_AC (ETCHTYPEA_IS_PRIMITIVE,ETCHTYPEB_STRING), 
  ETCHTYPE_BOXED_STRING       = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_STRING),
  ETCHTYPE_BOXED_ID_NAME      = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_ID_NAME),
  ETCHTYPE_BOXED_EXCEPTION    = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_EXCEPTION),
  ETCHTYPE_BOXED_ARRAYLIST    = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_ARRAYLIST),
  ETCHTYPE_BOXED_ARRAYVALUE   = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_ARRAYVAL),
  ETCHTYPE_BOXED_STRUCTVALUE  = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_STRUCTVAL),
  ETCHTYPE_BOXED_TAGDATAINP   = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_TAGDATAINP),
  ETCHTYPE_BOXED_TAGDATAOUT   = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_TAGDATAOUT),
  ETCHTYPE_BOXED_VALUEFACTORY = MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_VALUEFACTORY),
  ETCHTYPE_BOXED_STRUCTELEMENT= MAKETYPE_AC (ETCHTYPEA_IS_OBJECT,ETCHTYPEB_STRUCTELEMENT),
  ETCHTYPE_RESULT             = MAKETYPE_B  (ETCHTYPEB_RESULT),
} objtyp; 

#endif  /* ETCHOBJTYPES_H */ 