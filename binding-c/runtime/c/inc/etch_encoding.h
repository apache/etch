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
 * etch_encoding.h -- character encoding
 */

#ifndef ETCH_ENCODING_H
#define ETCH_ENCODING_H

int etch_unicode_to_utf8(char** utf8_string_out, wchar_t* unicode_string_in);
int etch_unicode_to_ansi(char** ansi_string_out, wchar_t* unicode_string_in);

int etch_utf8_to_unicode(wchar_t** unicode_string_out, char* utf8_string_in);
int etch_ansi_to_unicode(wchar_t** unicode_string_out, char* ansi_string_in);

size_t etch_get_unicode_bytecount (wchar_t*);


#endif /* #ifndef ETCH_ENCODING_H */