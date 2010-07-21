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
 * etch_tagdata.h
 * tagged data
 */

#ifndef ETCH_TAGDATA_H
#define ETCH_TAGDATA_H

#include "etch_object.h"
#include "etch_type.h"
#include "etch_validator.h"

#ifdef __cplusplus
extern "C" {
#endif

struct etch_arrayvalue;

#define ETCH_BINTAGDATA_CURRENT_VERSION 3

/** 
 *  i_tagdata virtual function signatures
 */
typedef signed char (*tagdata_check_value) (etch_object* value);
typedef signed char (*tagdata_get_native_type_code) (const unsigned short obj_type, const unsigned short class_id);
typedef int  (*tagdata_get_native_type)(const signed char, etch_array_id_params*);
typedef etch_type* (*tagdata_get_custom_structtype) (etch_object* thisx, const unsigned short obj_type, const unsigned short class_id);

/** 
 *  tagged data static function signatures
 */
struct etch_arrayvalue* etchtagdata_to_arrayvalue(etch_object* thisx, struct etch_nativearray* value, tagdata_get_native_type_code get_native_type_code);

int  etchtagdata_adjust_small_value (const byte in, etch_object* value, signed char* out); 
int  etchtagdata_validate_value(etch_object* valobj, struct etch_validator*, signed char* out);
int  etchtagdata_get_number(etch_object* valobj, const double fminval, const double fmaxval,  int64* out);
int  etchtagdata_get_double_number(etch_object* valobj, const double fmin, const double fmax, double* outd);
int  etchtagdata_get_float_number(etch_object* valobj, const double fmin, const double fmax, float* outf);
int  etchtagdata_bool_value  (etch_object* valobj, boolean* out);
int  etchtagdata_byte_value  (etch_object* valobj, byte* out);
int  etchtagdata_int16_value (etch_object* valobj, short* out);
int  etchtagdata_int32_value (etch_object* valobj, int* out);
int  etchtagdata_int64_value (etch_object* valobj, int64* out);
int  etchtagdata_float_value (etch_object* valobj, float* out);
int  etchtagdata_double_value(etch_object* valobj, double* out);
int  etchtagdata_is_eod (etch_object*);
int  etchtagdata_is_null(etch_object*);
etch_object* etchtagdata_new_eodmarker(const int is_static);
etch_object* etchtagdata_new_nullobj  (const int is_static);
etch_string* etchtagdata_new_emptystring(const int is_static);
signed char etchtagdata_check_value   (etch_object*);
signed char etchtagdata_check_byte    (const signed char);  
signed char etchtagdata_check_short   (const short); 
signed char etchtagdata_check_integer (const int); 
signed char etchtagdata_check_long    (const int64); 


/**
 * etch_typecode
 * enumeration of values denoting type of an encoded value
 * rage 1: -128 to -65  defined types
 * range 2: -64 to 127  tiny integer values
 */
typedef enum etch_typecode
{
	/**
	 * denotes a null value.
	 */
	ETCH_XTRNL_TYPECODE_NULL = -128,
	/**
	 * denotes no value, which is different than null. for example, an array is a
	 * sequence of values (some of which may be null) terminated by a NONE.
	 */
	ETCH_XTRNL_TYPECODE_NONE = -127,
    ETCH_XTRNL_TYPECODE_ENDDATA = ETCH_XTRNL_TYPECODE_NONE,

    /* - - - - - - - - - - - - - - - - - - 
     * scalars (also see tiny int below)
     * - - - - - - - - - - - - - - - - - - 
     */

	/**
	 * denotes a false boolean value.
	 */
	ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE = -126,
	/**
	 * denotes a true boolean value.
	 */
	ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE = -125,
    ETCH_XTRNL_TYPECODE_BOOLEAN_CONTENT = ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE,

	/**
	 * signed byte.
	 */
	ETCH_XTRNL_TYPECODE_BYTE = -124,
	/**
	 * two byte signed short, msb first
	 */
	ETCH_XTRNL_TYPECODE_SHORT = -123,
	/**
	 * four byte signed integer, msb first.
	 */
	ETCH_XTRNL_TYPECODE_INT = -122,    
	/**
	 * eight byte byte signed long, msb first 
	 */
	ETCH_XTRNL_TYPECODE_LONG = -121, 
	/**
	 * four byte ieee floating format number 
	 */
	ETCH_XTRNL_TYPECODE_FLOAT = -120,
	/**
	 * eight byte ieee floating format number.
	 */
	ETCH_XTRNL_TYPECODE_DOUBLE = -119,

    /* - - - - - - - - - - - - - - - - - - 
     * arrays  
     * - - - - - - - - - - - - - - - - - - 
     */

	/**
	 * an array of bytes.
	 */
	ETCH_XTRNL_TYPECODE_BYTES = -117,
	/**
	 * a sequence of values.
	 */
	ETCH_XTRNL_TYPECODE_ARRAY = -111,  

    /* - - - - - - - - - - - - - - - - - - 
     * strings  
     * - - - - - - - - - - - - - - - - - - 
     */

	/**
	 * denotes an empty string.
	 */
	ETCH_XTRNL_TYPECODE_EMPTY_STRING = -110,
	/**
	 * utf-8 encoded string
	 */
	ETCH_XTRNL_TYPECODE_STRING = -109,

    /* - - - - - - - - - - - - - - - - - - 
     * structures  
     * - - - - - - - - - - - - - - - - - - 
     */

	/**
	 * a sequence of key / value pairs.
	 */
	ETCH_XTRNL_TYPECODE_STRUCT = -108,

	/**
	 * a custom value from a value factory (struct, exception, enum, extern).
     * an associated value identifies the specific type. the wire format of
     * STRUCT and CUSTOM are the same, i.e. can use CUSTOM in place of STRUCT
     * and accept STRUCT as if CUSTOM.
	 */
	ETCH_XTRNL_TYPECODE_CUSTOM = -107,
	/**
	 * denotes that any value is ok (as long as we know how to serialize it).
	 * dynamic methods should be applied to determine type.
	 * this type never appears on the wire.
	 */
	ETCH_XTRNL_TYPECODE_ANY = -106,

    /* - - - - - - - - - - - - - - - - - - 
     * tiny integers (no value follows)
     * - - - - - - - - - - - - - - - - - - 
     */

	ETCH_XTRNL_TYPECODE_MIN_TINY_INT = -64,
	ETCH_XTRNL_TYPECODE_MAX_TINY_INT = 127,

} etch_typecode;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_TAGDATA_H */
