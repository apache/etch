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
 * etch_tdformat.c
 * a factory used to abstract tagged data handler constructors
 */
#include "etch_tdformat.h" 
#include "etch_objecttypes.h"
#include <wchar.h>

void etchtdf_init();
int  etchtdf_name_to_id(wchar_t*);
tagged_data_input*  etchtdf_defnewtdi (etch_value_factory*);
tagged_data_output* etchtdf_defnewtdo (etch_value_factory*);

const wchar_t* tdfname_binary = L"binary";
const wchar_t* tdfname_xml    = L"xml";


/*
 * tagdata_format_factory()
 * @param format_name name of the tagged data format, e.g., L"binary", 
 * caller retains ownership of this string.
 * @return a format factory (interface to tagged data constructors) for specified format.
 * caller assumes ownership of this object.
 */
tagdata_format_factory* get_format_factory(wchar_t* format_name)
{
    tagdata_format_factory* ff = (tagdata_format_factory*) new_object
        (sizeof(tagdata_format_factory), ETCHTYPEB_FORMATFACT, CLASSID_FORMATFACT);

    ff->new_tagdata_input  = etchtdf_defnewtdi;
    ff->new_tagdata_output = etchtdf_defnewtdo; 

    switch(etchtdf_name_to_id(format_name))
    {   case ETCH_TAGDATA_FORMAT_BINARY:
             ((etch_object*)ff)->class_id = CLASSID_FORMATFACT_BINARY;       
             ff->new_tagdata_input  = new_binary_tdi; 
             ff->new_tagdata_output = new_binary_tdo; 
             break;
        case ETCH_TAGDATA_FORMAT_XML:
             ((etch_object*)ff)->class_id = CLASSID_FORMATFACT_XML;
             /* overrides will be plugged in here at such time as we have xml tdi/tdo */
             break;
        default: /* name not recognized */
             break;
    }

    return ff;
}


/*
 * etchtdf_init()
 * static data lazy initializer 
 */
void etchtdf_init()
{
    if (is_tdf_initialized) return;
    tdformat_names[ETCH_TAGDATA_FORMAT_BINARY] = tdfname_binary;
    tdformat_names[ETCH_TAGDATA_FORMAT_XML]    = tdfname_xml;
    is_tdf_initialized = TRUE;
}

 
/*
 * etchtdf_name_to_id()
 * translate name of tagdata format to format ID (index)
 */
int etchtdf_name_to_id(wchar_t* format_name)
{
    int i = 0;
    etchtdf_init();
    if (NULL == format_name) return -1;

    for(; i < ETCH_TAGDATA_FORMAT_COUNT; i++)
        if (0 == wcscmp(format_name, tdformat_names[i]))
            return i;

    return -1;
}


/*
 * tdf_defnewtdi()
 * default virtual new_tagdata_input()
 */
tagged_data_input* etchtdf_defnewtdi (etch_value_factory* vf)
{
    return NULL;
} 


/*
 * tdf_defnewtdi()
 * default virtual new_tagdata_output()
 */
tagged_data_output* etchtdf_defnewtdo (etch_value_factory* vf)
{
    return NULL;
} 



