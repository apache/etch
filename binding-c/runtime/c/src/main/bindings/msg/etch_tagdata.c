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
 * etch_tagdata.c -- tagged data implementation.
 */

#include "etch_tagged_data.h"
#include "etch_type.h"
#include "etch_arrayval.h"
#include "etch_nativearray.h"
#include "etch_objecttypes.h"


/* the only instance data in the java version is the value factory 
 * for C, inheritors can implement that, in order that we don't need
 * any instance data here and can just implement methods. 
 */

/*
 * etchtagdata_get_number()
 * java.Lang.Number emulation support.
 * given an anonymous wrapped primitive, return its value represented as
 * a 64-bit integer.
 */
int etchtagdata_get_number(etch_object* valobj, 
    const double fmin, const double fmax, int64* out)
{
    int64 longval = 0;
    double dval;
    if (!is_etch_primitive_number(valobj)) return -1; 

    switch(((etch_object*)valobj)->class_id)
    { 
        case CLASSID_PRIMITIVE_INT32:
             longval = ((etch_int32*)valobj)->value;
             break; 

        case CLASSID_PRIMITIVE_INT64:
             longval = ((etch_int64*)valobj)->value;
             break;

        case CLASSID_PRIMITIVE_BYTE:
        case CLASSID_PRIMITIVE_BOOL: 
        case CLASSID_PRIMITIVE_INT8: 
             longval = ((etch_byte*)valobj)->value;
             break;

        case CLASSID_PRIMITIVE_INT16:
             longval = ((etch_int16*)valobj)->value;
             break;
         
        case CLASSID_PRIMITIVE_DOUBLE: 
        case CLASSID_PRIMITIVE_FLOAT:  
        {   
            switch(((etch_object*)valobj)->class_id)
            { case CLASSID_PRIMITIVE_DOUBLE:
                   dval = ((etch_double*)valobj)->value;
                   break;
              case CLASSID_PRIMITIVE_FLOAT:
                   dval = (double) ((etch_float*)valobj)->value;
                   break;
            }

            /* round ieee value if necessary */
            if (dval >= 0.0 && dval <= fmax) 
                dval += 0.5;
            else
            if (dval  < 0.0 && dval >= fmin)
                dval -= 0.5;

            longval = (int64) dval;   
            break;
        }
    }

    *out = longval;
    return 0;
}


/*
 * etchtagdata_get_fnumber()
 * java.Lang.Number emulation support.
 * given an anonymous wrapped primitive, return its value represented as
 * an IEEE number of 64 bits.
 */
int etchtagdata_get_double_number(etch_object* valobj,  
    const double fmin, const double fmax, double* outd)
{
    float fval = 0.0; double dval = 0.0; int64 longval = 0;

    if (-1 == etchtagdata_get_number(valobj, fmin, fmax, &longval))
        return -1;
  
    switch(((etch_object*)valobj)->class_id)
    { 
        case CLASSID_PRIMITIVE_DOUBLE: 
           dval = ((etch_double*)valobj)->value; 
           break;

        case CLASSID_PRIMITIVE_FLOAT:  
            fval = ((etch_float*)valobj)->value;
            dval = (double) fval;
            break;

        default:
            dval = (double) longval;
    }

    *outd = dval;
    return 0;
}


/*
 * etchtagdata_get_float_number()
 * java.Lang.Number emulation support.
 * given an anonymous wrapped primitive, return its value represented as
 * an IEEE number of 32 bits.
 */
int etchtagdata_get_float_number(etch_object* valobj,
    const double fmin, const double fmax, float* outf)
{
    float fval = 0.0; double dval = 0.0; int64 longval = 0;

    if (-1 == etchtagdata_get_number(valobj, fmin, fmax, &longval))
        return -1;
   
    switch(((etch_object*)valobj)->class_id)
    { 
        case CLASSID_PRIMITIVE_DOUBLE: 
           dval = ((etch_double*)valobj)->value; 
           fval = (float) dval;
           break;

        case CLASSID_PRIMITIVE_FLOAT:  
            fval = ((etch_float*)valobj)->value;
            break;

        default:
            fval = (float) longval;
    }

    *outf = fval;
    return 0;
}


/*
 * etchtagdata_byte_value()
 * mimic java.Lang.Number.byteValue()
 */
int etchtagdata_byte_value(etch_object* valobj, byte* out)
{
    int64 longval = 0;

    const int result = etchtagdata_get_number(valobj, 0, 0, &longval);

    if (0 == result && out) 
       (*out) = (byte) longval;  /* truncate and return value */
 
    return result;  
}


/*
 * etchtagdata_bool_value()
 * if there was a java.Lang.Number.booleanValue(), this would mimic it
 */
int etchtagdata_bool_value(etch_object* valobj, boolean* out)
{
    int64 longval = 0;

    const int result = etchtagdata_get_number(valobj, 0, 0, &longval);

    if (0 == result && out) 
    {   
        if (longval != 1) {
            *out = 1;
        }
        else {
            *out = 0;
        }
    } 
 
    return result;  
}


/*
 * etchtagdata_int16_value()
 * mimic java.Lang.Number.shortValue()
 */
int etchtagdata_int16_value(etch_object* valobj, short* out)
{
    int64 longval = 0;

    const int result = etchtagdata_get_number(valobj, 0, 0, &longval);

    if (0 == result && out) 
       (*out) = (short) longval;  /* truncate and return value */
 
    return result;
}


/*
 * etchtagdata_int32_value()
 * mimic java.Lang.Number.intValue()
 */
int etchtagdata_int32_value(etch_object* valobj, int* out)
{
    int64 longval = 0;

    const int result = etchtagdata_get_number(valobj, 0, 0, &longval);

    if (0 == result && out) 
       (*out) = (int) longval;  /* truncate and return value */
 
    return result;
}


/*
 * etchtagdata_int64_value()
 * mimic java.Lang.Number.longValue()
 */
int etchtagdata_int64_value(etch_object* valobj, int64* out)
{
    int64 longval = 0;

    const int result = etchtagdata_get_number(valobj, 0, 0,  &longval);

    if (0 == result && out) 
       (*out) = longval; 
 
    return result;
}


/*
 * etchtagdata_float_value()
 * mimic java.Lang.Number.floatValue()
 */
int etchtagdata_float_value(etch_object* valobj, float* out)
{
    const static double ETCHTYPE_MAX_FLOATF = ETCHTYPE_MAX_FLOAT + 0.4999;
    const static double ETCHTYPE_MIN_FLOATF = ETCHTYPE_MIN_FLOAT - 0.4999;
    float floatval = 0.0;

    const int result = etchtagdata_get_float_number(valobj, 
          ETCHTYPE_MIN_FLOATF, ETCHTYPE_MAX_FLOATF, &floatval);

    if (0 == result && out) 
       (*out) = floatval; 
 
    return result;
}


/*
 * etchtagdata_double_value()
 * mimic java.Lang.Number.doubleValue()
 */
int etchtagdata_double_value(etch_object* valobj, double* out)
{
    double dval = 0.0;

    const int result = etchtagdata_get_double_number(valobj, 
          ETCHTYPE_MIN_DOUBLE, ETCHTYPE_MAX_DOUBLE, &dval);

    if (0 == result && out) 
       (*out) = dval; 
 
    return result;
}


/*
 * etchtagdata_adjust_tiny_int()
 * if target value is one byte and value is in range of tiny int,
 * adjust the type code accordingly, returning it in out parameter
 */
int etchtagdata_adjust_tiny_int(const signed char target_type, etch_object* valobj, signed char* out)
{
    signed char type_out = target_type;
    signed char byteval = 0;
    int result = 0;

    if (target_type == ETCH_XTRNL_TYPECODE_BYTE) {
        if (0 == (result = etchtagdata_byte_value(valobj, (byte*)&byteval))) {
            if (is_inrange_tiny_for_signed_chars(byteval)) {
                type_out = byteval; 
	    }
        }
    }
    if (out) {
        (*out = type_out);
    }
    return result;
}


/*
 * etchtagdata_validate_value()
 * in java binding this is checkValue(object, validator);
 * defers to validator to determine type code for the value.
 * adjusts the type to tiny integer if applicable and possible.
 * returns the type code in the out parameter.
 */
int etchtagdata_validate_value(etch_object* valobj, etch_validator* vtor, signed char* out)
{
    signed char xtype = 0;
    int result = 0;

    if (NULL == valobj) {
        xtype = ETCH_XTRNL_TYPECODE_NULL;
    } else {
        if (etchtagdata_is_eod(valobj)) {
            xtype = ETCH_XTRNL_TYPECODE_NONE;
	} else {
	    if (0 == (result = vtor->check_value(vtor, valobj, (byte*)&xtype)))  {
                result = etchtagdata_adjust_tiny_int(xtype, valobj, &xtype);
	    }
	}
    }
    if (out) {
        (*out = xtype);
    }
    return result;
}


/**
 * etchtagdata_check_value()
 * returns a type code for the specified value
 * @param valobj an etch value object, caller retains ownership.
 */
signed char etchtagdata_check_value (etch_object* valobj)
{
    signed char xtype = 0;
    const unsigned int obj_type = valobj? ((etch_object*)valobj)->obj_type: 0;
    const unsigned int class_id = valobj? ((etch_object*)valobj)->class_id: 0;

    if (NULL == valobj)
        xtype = ETCH_XTRNL_TYPECODE_NULL;
    else
    if (etchtagdata_is_eod(valobj))
        xtype = ETCH_XTRNL_TYPECODE_NONE; 
    else
    if (is_etch_primitive(valobj))
    {
        switch(class_id)
        {
           case CLASSID_PRIMITIVE_INT32:  
                 xtype = etchtagdata_check_integer(((etch_int32*)valobj)->value);
                 break;
            case CLASSID_PRIMITIVE_INT16:  
                 xtype = etchtagdata_check_short(((etch_int16*)valobj)->value);
                 break;
            case CLASSID_PRIMITIVE_INT64:  
                 xtype = etchtagdata_check_long(((etch_int64*)valobj)->value);
                 break;
            case CLASSID_PRIMITIVE_BYTE: 
            case CLASSID_PRIMITIVE_INT8:  
                 xtype = etchtagdata_check_byte(((etch_byte*)valobj)->value);     
                 break;
            case CLASSID_PRIMITIVE_DOUBLE:
                 xtype = ETCH_XTRNL_TYPECODE_DOUBLE; 
                 break;      
            case CLASSID_PRIMITIVE_FLOAT: 
                 xtype = ETCH_XTRNL_TYPECODE_FLOAT;
                 break;                 
            case CLASSID_PRIMITIVE_BOOL:  
                 xtype = ((etch_boolean*)valobj)->value?   
                     ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE:  
                     ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE;  
                 break; 
           case CLASSID_STRING:         
                 xtype = ((etch_string*)valobj)->char_count == 0?
                     ETCH_XTRNL_TYPECODE_EMPTY_STRING:
                     ETCH_XTRNL_TYPECODE_STRING;     
                 break;
            default: 
                 xtype = ETCH_XTRNL_TYPECODE_CUSTOM;   
                 break;
        }  /* switch(class_id) */
    }    
    else switch(obj_type)
    { 
        case ETCHTYPEB_STRUCTVAL:
             xtype = ETCH_XTRNL_TYPECODE_CUSTOM;   
             break;
        case ETCHTYPEB_ARRAYVAL:
             xtype = ((etch_arrayvalue*)valobj)->content_obj_type == ETCHTYPEB_BYTE?
                 ETCH_XTRNL_TYPECODE_BYTES: ETCH_XTRNL_TYPECODE_ARRAY;  
             break;
        case ETCHTYPEB_NATIVEARRAY:
             if(((etch_nativearray*)valobj)->numdims > 1) {
                 xtype = ETCH_XTRNL_TYPECODE_ARRAY;
                 break;
             }
             xtype = ((etch_nativearray*)valobj)->content_obj_type == ETCHTYPEB_BYTE?
                 ETCH_XTRNL_TYPECODE_BYTES: ETCH_XTRNL_TYPECODE_ARRAY;   
             break;
        default: 
             xtype = ETCH_XTRNL_TYPECODE_CUSTOM;   
    }
    
    return xtype;
}


/*
 * etchtagdata_get_native_typecode()
 * returns the external type code corresponding to internal type.
 * see etch_binary_tdi.c for implementation
 */
byte etchtagdata_get_native_typecode
    (const unsigned short obj_type, const unsigned short class_id)
{
    return ETCH_XTRNL_TYPECODE_NONE;
}


/*
 * etchtagdata_get_native_type()
 * returns the internal type and class ids corresponding to external typecode.
 * see etch_binary_tdi.c for implementation
 */
unsigned etchtagdata_get_native_type (const byte typecode) 
{
    return (CLASSID_NONE << 16) | ETCHTYPEB_NONE;
}


/*
 * etchtagdata_get_custom_structtype()
 * returns a struct type for the specified class
 * see etch_binary_tdi for implementation
 */
etch_type* etchtagdata_get_custom_structtype (etch_object* thisx, 
   const unsigned short obj_type, const unsigned short class_id)
{
    /* we will return a non-disposable (static) type */
    etch_type* type = NULL; 
    return type;    
}




/**
 * etchtagdata_from_arrayvalue()
 * return an etch_nativearray representation of the specified etch_arrayvalue.
 * note that an arrayvalue, if created from an etch_nativearray, has retained 
 * the source nativearray as an instance member. is_force indicates if caller
 * wants to build the nativearray regardless of whether one currently exists.
 */
etch_nativearray* etchtagdata_from_arrayvalue(etch_arrayvalue* av, const int is_force)
{
    if (av->natarray && !is_force) return av->natarray;
    return (-1 == arrayvalue_to_nativearray(av))? NULL: av->natarray;    
}


/**
 * etchtagdata_alloc_arrayvalue()
 * creates and returns an empty arrayvalue object. this method is here for
 * for compatibility with the java binding, however it is superfluous.
 * we can safely substitute the new_arrayvalue call below and lose this method. 
 */
etch_arrayvalue* etchtagdata_alloc_arrayvalue(const byte type_code, 
    etch_type* custom_struct_type, const int dim, const int initsize)
{
     return new_arrayvalue(type_code, custom_struct_type, dim, 
        initsize, initsize, FALSE, ETCHARRAYLIST_SYNCHRONIZED);
}


/*
 * etchtagdata_check_byte()
 */
signed char etchtagdata_check_byte(const signed char val) 
{
    byte result = ETCH_XTRNL_TYPECODE_BYTE;
    if (is_inrange_tiny_for_signed_chars(val))
        result = val;   
    return result;
}  


/*
 * etchtagdata_check_short()
 */
signed char etchtagdata_check_short(const short val) 
{
    byte result = ETCH_XTRNL_TYPECODE_SHORT;
    if (is_inrange_byte(val))
        result = etchtagdata_check_byte((byte)val);
    return result;
}  


/*
 * etchtagdata_check_integer()
 */
signed char etchtagdata_check_integer(const int val) 
{
    byte result = ETCH_XTRNL_TYPECODE_INT;
    if  (is_inrange_int16(val)) 
         result = etchtagdata_check_short((short)val);
    return result;
}  


/*
 * etchtagdata_check_long()
 */
signed char etchtagdata_check_long(const int64 val) 
{
    byte result = ETCH_XTRNL_TYPECODE_LONG;
    if  (is_inrange_int32(val))
         result = etchtagdata_check_integer((int)val);
    return result;
} 


/**
 * etchtagdata_is_null()
 * indicate whether this object represents a null value
 */
int etchtagdata_is_null(etch_object* obj)
{
    return obj == NULL || obj->is_null;
} 


/**
 * etchtagdata_is_eod()
 * indicate whether this object is the end of stream sentinel
 */
int etchtagdata_is_eod(etch_object* obj)
{
    return obj && ((etch_object*)obj)->obj_type == ETCHTYPEB_EODMARK;
}


/**
 * etchtagdata_new_nullobj()
 * instantiate and return a logically null object
 */
etch_object* etchtagdata_new_nullobj(const int is_static)
{
   etch_object* obj = new_nullobj();
   if (is_static) set_etchobj_static_all(obj);
   return obj;
}


/**
 * etchtagdata_new_eodmarker()
 * instantiate and return an end of data marker object
 */
etch_object* etchtagdata_new_eodmarker(const int is_static)
{
   etch_object* obj = new_object(sizeof(etch_object), ETCHTYPEB_EODMARK, CLASSID_NONE);
   if (is_static) set_etchobj_static_all(obj);
   return obj;
}


/**
 * etchtagdata_new_emptystring()
 * instantiate and return an empty string object
 */
etch_string* etchtagdata_new_emptystring(const int is_static)
{
   etch_string* obj = new_stringw(L"");
   if (is_static) set_etchobj_static_all(obj);
   return obj;
}






