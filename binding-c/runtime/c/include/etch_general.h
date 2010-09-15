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
 * etch_general.h -- general file
 */

#ifndef _ETCH_GENERAL_H_
#define _ETCH_GENERAL_H_

#include "etch.h"
#include "etch_mem.h"
#include "etch_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * get_dynamic_classid()
 * get a class ID for objects not known to the binding,
 * class ID sequence for generated classes.
 */
unsigned short get_dynamic_classid(); 

/**
 * get_dynamic_classid_unique()
 * if specified ID already assigned, return it; otherwise generate and return. 
 * assignment is currently non-atomic.
 */
unsigned short get_dynamic_classid_unique(unsigned short* globalid); 

/**
 * remove spaces and tabs on left and right side
 * from given string.
 * @param str to trim
 * @return trimed string.
 */
char* strtrim(char* str);

/**
 * waitkey()
 * method to wait for a keypress.
 */
void waitkey();

int etch_snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...);

//todo: refactor this stuff
int hexchar_to_int (const unsigned char hexchar);
int hexwchar_to_int(const wchar_t hexwchar);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* _ETCH_GENERAL_H_ */
