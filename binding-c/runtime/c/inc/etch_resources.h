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
 * etch_resources.h
 * resource map
 * a string-to-etch-object-based hashtable
 */

#ifndef ETCHRESOURCES_H
#define ETCHRESOURCES_H

#include "etchhash.h"
#define ETCH_RESOURCES_DEFSIZE 16

const wchar_t* ETCH_RESXKEY_SOCKET;
const wchar_t* ETCH_RESXKEY_ACCEPTED_CONX;
const wchar_t* ETCH_RESXKEY_POOLTYPE_FREE;
const wchar_t* ETCH_RESXKEY_POOLTYPE_QUEUED;
const wchar_t* ETCH_RESXKEY_MSGIZER_FORMAT;
const wchar_t* ETCH_RESXKEY_MSGIZER_VALUFACT;
const wchar_t* ETCH_RESXVAL_XPORTFMT_BINARY;
const wchar_t* ETCH_RESXVAL_XPORTFMT_XML;
const wchar_t* ETCH_XFACTKEY_TCP;
const wchar_t* ETCH_XPORTKEY_START;
const wchar_t* ETCH_XPORTKEY_START_AND_WAIT_UP;
const wchar_t* ETCH_XPORTKEY_IS_UP;
const wchar_t* ETCH_XPORTKEY_STOP;
const wchar_t* ETCH_XPORTKEY_STOP_AND_WAIT_DOWN;


typedef etch_hashtable etch_resources;
etch_resources* new_etch_resources (const int initialsize);
objmask* etch_resources_get (etch_resources*, const wchar_t* key);
int etch_resources_add (etch_resources*, const wchar_t* key, objmask* value);
int etch_resources_replace (etch_resources*, const wchar_t* key, objmask* value);
int etch_resources_clear (etch_resources*);


#endif /* #ifndef ETCHRESOURCES_H */