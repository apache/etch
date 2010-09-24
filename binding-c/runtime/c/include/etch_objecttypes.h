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
 * etch_objecttypes.h -- constants for internal object types.
 */

#ifndef ETCHOBJTYPES_H
#define ETCHOBJTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * identifies a class object's data type or content type
 */
typedef enum objtype_b
{  
    ETCHTYPEB_UNDEFINED       = 0x0,
    ETCHTYPEB_NONE            = 0x0,
    ETCHTYPEB_BYTE            = 0x1,
    ETCHTYPEB_BOOL            = 0x2,
    ETCHTYPEB_INT8            = 0x3,
    ETCHTYPEB_INT16           = 0x4,
    ETCHTYPEB_INT32           = 0x5,
    ETCHTYPEB_INT64           = 0x6,
    ETCHTYPEB_IEEE32          = 0x7,
    ETCHTYPEB_IEEE64          = 0x8,
    ETCHTYPEB_STRING          = 0x9,
    ETCHTYPEB_CLASS           = 0xa,
    ETCHTYPEB_RAWOBJECT       = 0xb,
    ETCHTYPEB_CUSTOM          = 0xc,
    ETCHTYPEB_EXTERN          = 0xd,
    ETCHTYPEB_ETCHOBJECT      = 0xe,
    ETCHTYPEB_HASHTABLE       = 0xf,
    ETCHTYPEB_VTABLE          = 0x10,
    ETCHTYPEB_EXCEPTION       = 0x11,
    ETCHTYPEB_CACHEREC        = 0x12,
    ETCHTYPEB_BYTES           = 0x13,
    ETCHTYPEB_ID_NAME         = 0x14,
    ETCHTYPEB_FIELD           = 0x15,
    ETCHTYPEB_TYPE            = 0x16,
    ETCHTYPEB_STRUCTVAL       = 0x17,
    ETCHTYPEB_ARRAYVAL        = 0x18,
    ETCHTYPEB_VALUEFACTORY    = 0x19,
    ETCHTYPEB_VALUEFACTOBJ    = 0x1a,
    ETCHTYPEB_VALUEFACTIMP    = 0x1b,
    ETCHTYPEB_MESSAGE         = 0x1c,
    ETCHTYPEB_TAGDATA         = 0x1d,
    ETCHTYPEB_TAGDATAINP      = 0x1e,
    ETCHTYPEB_TAGDATAOUT      = 0x1f,
    ETCHTYPEB_TDIOBJ          = 0x20,
    ETCHTYPEB_TDOOBJ          = 0x21,
    ETCHTYPEB_ARRAYELEMENT    = 0x22,
    ETCHTYPEB_STRUCTELEMENT   = 0x23,
    ETCHTYPEB_INSTANCEDATA    = 0x24,
    ETCHTYPEB_COLLECTION      = 0x25,
    ETCHTYPEB_LINKLIST        = 0x26,
    ETCHTYPEB_ITERATOR        = 0x27,
    ETCHTYPEB_RESULT          = 0x28,
    ETCHTYPEB_PRIMITIVE       = 0x29,
    ETCHTYPEB_DATE            = 0x2a,
    ETCHTYPEB_URL             = 0x2b,
    ETCHTYPEB_XPORTFACT       = 0x2c,
    ETCHTYPEB_CONNECTION      = 0x2d,
    ETCHTYPEB_TCPSERVER       = 0x2e,
    ETCHTYPEB_TCPCLIENT       = 0x2f,
    ETCHTYPEB_SOCKET          = 0x30,
    ETCHTYPEB_FLEXBUF         = 0x31,
    ETCHTYPEB_WHO             = 0x32,
    ETCHTYPEB_ARRAYLIST       = 0x33, 
    ETCHTYPEB_BINARYTDI       = 0x34,
    ETCHTYPEB_BINARYTDO       = 0x35,
    ETCHTYPEB_IDNAMEIMPL      = 0x36,
    ETCHTYPEB_SERIALIZER      = 0x37,
    ETCHTYPEB_FORMATFACT      = 0x38,
    ETCHTYPEB_VALIDATOR       = 0x39,
    ETCHTYPEB_NATIVEARRAY     = 0x3a, 
    ETCHTYPEB_SESSIONMSG      = 0x3b,
    ETCHTYPEB_TRANSPORTMSG    = 0x3c,
    ETCHTYPEB_SESSIONDATA     = 0x3d,
    ETCHTYPEB_SESSIONPKT      = 0x3e,
    ETCHTYPEB_SESSIONLXR      = 0x3f,
    ETCHTYPEB_DEFAULT_VF      = 0x40,
    ETCHTYPEB_DEFAULT_VFOBJ   = 0x41,
    ETCHTYPEB_DEFAULT_VFIMP   = 0x42,
    ETCHTYPEB_THREAD          = 0x43,
    ETCHTYPEB_THREADPOOL      = 0x44,
    ETCHTYPEB_MUTEX           = 0x45,
    ETCHTYPEB_THREADPARAMS    = 0x46,
    ETCHTYPEB_WAIT            = 0x47, 
    ETCHTYPEB_OBJSESSION      = 0x48,
    ETCHTYPEB_MSGHANDLER      = 0x49,
    ETCHTYPEB_SOURCE          = 0x4a,
    ETCHTYPEB_SOURCEHDLR      = 0x4b,
    ETCHTYPEB_MSGSOURCE       = 0x4c,
    ETCHTYPEB_PACKETIZER      = 0x4d,
    ETCHTYPEB_MESSAGIZER      = 0x4e,
    ETCHTYPEB_PACKETHANDLER   = 0x4f,
    ETCHTYPEB_ETCHLIST        = ETCHTYPEB_ARRAYLIST,
    ETCHTYPEB_ETCHMAP         = ETCHTYPEB_HASHTABLE,
    ETCHTYPEB_ETCHSET         = 0x50,
    ETCHTYPEB_ETCHQUEUE       = 0x51,
    ETCHTYPEB_UNUSED1         = 0x52,
    ETCHTYPEB_SERVERIMPL      = 0x53,
    ETCHTYPEB_EVENT           = 0x54,
    ETCHTYPEB_MAILBOX         = 0x55,
    ETCHTYPEB_MBOX_ELEMENT    = 0x56,
    ETCHTYPEB_MBOX_MANAGER    = 0x57,
    ETCHTYPEB_MBOXMGR_IMPL    = 0x58,
    ETCHTYPEB_MAILBOXINT      = 0x59,
    ETCHTYPEB_TRANSPORTDATA   = 0x5b,
    ETCHTYPEB_TRANSPORTPKT    = 0x5c,
    ETCHTYPEB_DELIVERYSVC     = 0x5d,
    ETCHTYPEB_DELIVERYSVCINT  = 0x5e,
    ETCHTYPEB_DELIVERYSVC_IMPL= 0x5f,
    ETCHTYPEB_SERVERFACT      = 0x60,
    ETCHTYPEB_CLIENTFACT      = 0x61,
    ETCHTYPEB_SERVERFACT_IMPL = 0x62,
    ETCHTYPEB_CLIENTFACT_IMPL = 0x63,
    ETCHTYPEB_SVCINTERFACE    = 0x64,
    ETCHTYPEB_EXESERVERBASE   = 0x66,
    ETCHTYPEB_EXESERVERIMPL   = 0x67,
    ETCHTYPEB_EXECLIENTBASE   = 0x68,
    ETCHTYPEB_EXECLIENTIMPL   = 0x69,
    ETCHTYPEB_REMOTE          = 0x6a,
    ETCHTYPEB_REMOTECLIENT    = 0x6b,
    ETCHTYPEB_REMOTESERVER    = 0x6c,
    ETCHTYPEB_CLIENT_SESSION  = 0x74,
    ETCHTYPEB_STUB            = 0x70,
    ETCHTYPEB_CLIENTSTUB      = 0x71,
    ETCHTYPEB_SERVERSTUB      = 0x72,
    ETCHTYPEB_FACTORYPARAMS   = 0x73,

    ETCHTYPEB_EODMARK         = 0x7f,

    ETCHTYPEB_DYNAMIC         = 0x80,

    ETCHTYPEB_USER            = 0xa0,

}  objtype_b;

const char* etch_object_type_get_name(objtype_b type);

/** 
 * class IDs
 */
typedef enum etch_classid
{   
    CLASSID_NONE               = 0x0, 
    CLASSID_ANY                = 0x0, 
    CLASSID_UNWRAPPED          = 0x0, 
    CLASSID_PRIMITIVE_BYTE     = 0x1, /* primitive class IDs must start at 1 */
    CLASSID_PRIMITIVE_BOOL     = 0x2,  
    CLASSID_PRIMITIVE_INT8     = 0x3,
    CLASSID_PRIMITIVE_INT16    = 0x4,
    CLASSID_PRIMITIVE_INT32    = 0x5,
    CLASSID_PRIMITIVE_INT64    = 0x6,
    CLASSID_PRIMITIVE_FLOAT    = 0x7,
    CLASSID_PRIMITIVE_DOUBLE   = 0x8,
    CLASSID_STRING             = 0x9,
    CLASSID_DATE               = 0xa,
    CLASSID_OBJECT             = 0xf,
    CLASSID_DEF_VF             = 0x10,  
    CLASSID_DEF_VF_OBJ         = 0x11,  
    CLASSID_DEF_VF_IMPL        = 0x12,  
    CLASSID_DEF_VF_VTAB        = 0x13, 
    CLASSID_BINARYTDI_VTAB     = 0x14,
    CLASSID_BINARYTDO_VTAB     = 0x15,
    CLASSID_TAGDATA            = 0x16,
    CLASSID_BINARYTDI          = 0x17,
    CLASSID_BINARYTDO          = 0x18,
    CLASSID_ETCHMESSAGE        = 0x1a,
    CLASSID_HASHTABLE          = 0x1b,
    CLASSID_ETCH_MAP           = CLASSID_HASHTABLE,
    CLASSID_HASHTABLE_VTAB     = 0x20, 
    CLASSID_ITERABLE_VTAB      = 0x21, 
    CLASSID_ITERATOR           = 0x22, 
    CLASSID_ID_NAME            = 0x23, 
    CLASSID_ID_FIELD           = 0x24, 
    CLASSID_ID_TYPE            = 0x25, 
    CLASSID_TDI_VTAB           = 0x26,
    CLASSID_TDO_VTAB           = 0x27,
    CLASSID_BINTDI_VTAB        = 0x28,
    CLASSID_BINTDO_VTAB        = 0x29,
    CLASSID_AVAILABLE_VTAB     = 0x2a,
    CLASSID_TYPEIMPL           = 0x2b,
    CLASSID_VTAB_FIELD         = 0x2c,
    CLASSID_VTAB_TYPE          = 0x2d,
    CLASSID_ETCH_ARRAYLIST     = 0x2e,
    CLASSID_ETCH_LIST          = CLASSID_ETCH_ARRAYLIST,
    CLASSID_ETCH_SET           = 0x2f,
    CLASSID_STRUCTVALUE        = 0x30,
    CLASSID_ARRAYVALUE         = 0x31,
    CLASSID_ARRAYELEMENT       = 0x32,
    CLASSID_VALUEFACTORY       = 0x33,
    CLASSID_TAGDATAINP         = 0x34,
    CLASSID_TAGDATAOUT         = 0x35,
    CLASSID_EXCEPTION          = 0x36,
    CLASSID_THREAD             = 0x39,
    CLASSID_THREADPOOL         = 0x3a,
    CLASSID_MUTEX              = 0x3b,
    CLASSID_WAIT               = 0x3c,
    CLASSID_STUB               = 0x3d,
    CLASSID_SERVERIMPL         = 0x3e,
    CLASSID_MSGSOURCE          = 0x3f,
    CLASSID_MESSAGIZER         = 0x40,
    CLASSID_MSGHANDLER         = 0x41,
    CLASSID_SOURCE             = 0x42,
    CLASSID_SOURCEHDLR         = 0x43,
    CLASSID_URL                = 0x49,
    CLASSID_PACKETIZER         = 0x4a,
    CLASSID_PACKETHANDLER      = 0x4b,
    CLASSID_FLEXBUF            = 0x4e,
    CLASSID_WHO                = 0x4f,
    CLASSID_ARRAY_OBJECT       = 0x50,
    CLASSID_ARRAY_BYTE         = 0x51,
    CLASSID_ARRAY_BOOL         = 0x52,
    CLASSID_ARRAY_INT8         = 0x53,
    CLASSID_ARRAY_INT16        = 0x54,
    CLASSID_ARRAY_INT32        = 0x55,
    CLASSID_ARRAY_INT64        = 0x56,
    CLASSID_ARRAY_FLOAT        = 0x57,
    CLASSID_ARRAY_DOUBLE       = 0x58,
    CLASSID_ARRAY_STRING       = 0x59,
    CLASSID_ARRAY_STRUCT       = 0x5a,
    CLASSID_ETCHQUEUE          = 0x5b,
    CLASSID_ETCHSOCKET         = 0x5c,
    CLASSID_CLIENT_SESSION     = 0x5d,
    CLASSID_UNUSED_1           = 0x5e,
    CLASSID_XPORTFACT          = 0x5f,
    CLASSID_FORMATFACT         = 0x60,
    CLASSID_FORMATFACT_BINARY  = 0x61,
    CLASSID_FORMATFACT_XML     = 0x62,
    CLASSID_TCP_CONNECTION     = 0x63,  
    CLASSID_TCP_LISTENER       = 0x64, 
    CLASSID_TCP_CLIENT         = 0x65, 
    CLASSID_SOCKET             = 0x66,
    CLASSID_UNUSED_2           = 0x67,
    CLASSID_UNUSED_3           = 0x68,
    CLASSID_SESSIONMSG         = 0x69,
    CLASSID_TRANSPORTMSG       = 0x6a,
    CLASSID_TRANSPORTPKT       = 0x6b,
    CLASSID_SESSIONDATA        = 0x6c,
    CLASSID_TRANSPORTDATA      = 0x6d,
    CLASSID_SESSIONPKT         = 0x6e,
    CLASSID_SESSIONLXR         = 0x6f,
    CLASSID_VALIDATOR          = 0x70,
    CLASSID_COMBO_VALIDATOR    = 0x71,
    CLASSID_VALIDATOR_BOOL     = 0x72, 
    CLASSID_VALIDATOR_BYTE     = 0x73, 
    CLASSID_VALIDATOR_INT8     = 0x74, 
    CLASSID_VALIDATOR_INT16    = 0x75, 
    CLASSID_VALIDATOR_INT32    = 0x76, 
    CLASSID_VALIDATOR_INT64    = 0x77, 
    CLASSID_VALIDATOR_FLOAT    = 0x78, 
    CLASSID_VALIDATOR_DOUBLE   = 0x79, 
    CLASSID_VALIDATOR_STRING   = 0x7a,
    CLASSID_VALIDATOR_OBJECT   = 0x7b,
    CLASSID_VALIDATOR_EXCEPTION= 0x7c,
    CLASSID_VALIDATOR_STRUCT   = 0x7d,
    CLASSID_VALIDATOR_EOD      = 0x7e,
    CLASSID_VALIDATOR_CUSTOM   = 0x7f,

    CLASSID_RUNTIME_EXCEPTION  = 0x80, 
    CLASSID_AUTH_EXCEPTION     = 0x81,
    CLASSID_SERIALIZER_EXCP    = 0x82, 
    CLASSID_SERIALIZER_RTXCP   = 0x83,
    CLASSID_SERIALIZER_AUTHXCP = 0x84,
    CLASSID_SERIALIZER_LIST    = 0x85,
    CLASSID_SERIALIZER_MAP     = 0x86,
    CLASSID_SERIALIZER_SET     = 0x87,
    CLASSID_SERIALIZER_DATE    = 0x88,

    CLASSID_EVENT_UNWANTMSG    = 0x90,
    CLASSID_MAILBOX            = 0x91,
    CLASSID_MAILBOXINT         = 0x92,
    CLASSID_MBOX_ELEMENT       = 0x93,
    CLASSID_PLAIN_MAILBOX      = 0x94,
    CLASSID_MBOX_MANAGER       = 0x95,
    CLASSID_PLAIN_MBOXMGR      = 0x96,
    CLASSID_DELIVERYSVC        = 0x97,
    CLASSID_TCP_DELIVERYSVC    = 0x98,
    CLASSID_CLIENTSTUB         = 0x99,
    CLASSID_SERVERSTUB         = 0x9a,
    CLASSID_SERVERFACTORY      = 0x9b,
    CLASSID_CLIENTFACTORY      = 0x9c,

    CLASSID_TCP_XPORTFACT      = 0xb0,
    CLASSID_SERVERFACT         = 0xb1,
    CLASSID_CLIENTFACT         = 0xb2,
    CLASSID_EXECLIENT_IMPL     = 0xb3,
    CLASSID_EXECLIENTBASE_IMPL = 0xb4,
    CLASSID_EXESERVER_IMPL     = 0xb5,
    CLASSID_EXESERVERBASE_IMPL = 0xb6,

    CLASSID_CONTROL_START         = 0x101, 
    CLASSID_CONTROL_START_WAITUP  = 0x102, 
    CLASSID_CONTROL_STOP          = 0x103, 
    CLASSID_CONTROL_STOP_WAITDOWN = 0x104, 
    CLASSID_WAITUP                = 0x105,
    CLASSID_WAITDOWN              = 0x106, 
 
    CLASSID_QUERY_IS_UP           = 0x110, 
    CLASSID_QUERY_LOCALADDR       = 0x111, 
    CLASSID_QUERY_REMOTEADDR      = 0x112, 
    CLASSID_QUERY_WAITUP          = 0x113, 
    CLASSID_QUERY_WAITDOWN        = 0x114, 

    CLASSID_DYNAMIC_START         = 0x400, 

} etch_classid;

/*
 * ranges of numeric types 
 */
#define ETCHTYPE_MIN_TINY   ((signed char)(0xc0))      /* -64 */
#define ETCHTYPE_MAX_TINY   ((signed char)(0x7f))      /* 127 */
#define ETCHTYPE_MIN_BYTE   ((signed char)(0x80))      /* -128 (-(2^7))  */
#define ETCHTYPE_MAX_BYTE   ((signed char)(0x7f))      /* 127  ((2^7)-1) */
#define ETCHTYPE_MIN_INT16  ((signed short)(0x8000))   /* -65536 (-(2^15))  */
#define ETCHTYPE_MAX_INT16  ((signed short)(0x7fff))   /* 65535  ((2^15)-1) */
#define ETCHTYPE_MIN_INT32  ((signed int)(0x80000000)) /* (-(2^31))  */
#define ETCHTYPE_MAX_INT32  ((signed int)(0x7fffffff)) /* ((2^31)-1) */
#define ETCHTYPE_MIN_INT64  ((signed long long)(0x8000000000000000LL))  /* (-(2^63))  */ 
#define ETCHTYPE_MAX_INT64  ((signed long long)(0x7fffffffffffffffLL))  /* ((2^63)-1) */ 
#define ETCHTYPE_MIN_FLOAT  (1.4e-45f) 
#define ETCHTYPE_MAX_FLOAT  (3.40282346e+38f)  
#define ETCHTYPE_MIN_DOUBLE (4.9e-324) 
#define ETCHTYPE_MAX_DOUBLE (1.7976931348623157e+308) 

#define is_inrange_bool(n) (n == 0 || n == 1)
#define is_inrange_tiny(n) (n >= ETCHTYPE_MIN_TINY  && n <= ETCHTYPE_MAX_TINY)
#define is_inrange_tiny_for_signed_chars(n) (n >= ETCHTYPE_MIN_TINY)
#define is_inrange_byte(n) (n >= ETCHTYPE_MIN_BYTE  && n <= ETCHTYPE_MAX_BYTE)
#define is_inrange_int8(n) (n >= ETCHTYPE_MIN_BYTE  && n <= ETCHTYPE_MAX_BYTE)
#define is_inrange_int16(n)(n >= ETCHTYPE_MIN_INT16 && n <= ETCHTYPE_MAX_INT16)
#define is_inrange_int32(n)(n >= ETCHTYPE_MIN_INT32 && n <= ETCHTYPE_MAX_INT32)
#define is_inrange_int64(n)(n >= ETCHTYPE_MIN_INT64 && n <= ETCHTYPE_MAX_INT64)

#ifdef __cplusplus
}
#endif

#endif  /* ETCHOBJTYPES_H */ 
