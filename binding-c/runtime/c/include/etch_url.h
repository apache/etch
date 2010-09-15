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
 * etchurl.h -- URL class
 * modeled after java binding URL class
 */

#ifndef ETCHURL_H
#define ETCHURL_H

#include "etch_collection.h"
#include "etch_arraylist.h"
#include "etch_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_URL_DEFAULT_SCHEME L"http"
#define ETCH_URL_DEFNUMPARMS 4
#define ETCH_URL_DEFNUMTERMS 4
#define ETCH_URL_DEFSUBTERMS 4


typedef struct etch_url
{
    etch_object object;

    unsigned port;

    wchar_t* raw;
    wchar_t* scheme;  
    wchar_t* user;  
    wchar_t* password; 
    wchar_t* fragment; 
    wchar_t* host;  
    wchar_t* uri;  

    etch_arraylist* params;
    etch_hashtable* terms;  

    size_t charcount;
    size_t bytecount;

} etch_url;


etch_url* new_url(wchar_t* urlstring);
etch_object* etchurl_get_term (etch_url*, const wchar_t* termname);
boolean etchurl_get_boolean_term(etch_url*, const wchar_t* termname, boolean* retval);
etch_iterator* etchurl_get_params(etch_url*);
etch_object* etchurl_remove_term(etch_url*, wchar_t* key);
int etchurl_get_integer_term(etch_url*, const wchar_t* termname, int* retval);
int etchurl_add_term(etch_url*, wchar_t* termname, wchar_t* termval);
int etchurl_add_double_term( etch_url*, wchar_t* termname, const double termval);
int etchurl_add_integer_term(etch_url*, wchar_t* termname, const int termval);
int etchurl_paramcount(etch_url*);
int etchurl_termcount (etch_url*);

int is_url_scheme_http (etch_url*); 
int is_url_scheme_udp  (etch_url*);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHURL_H */
