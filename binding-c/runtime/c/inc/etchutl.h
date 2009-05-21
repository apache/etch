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
 * etchutl.h -- utility methods
 */

#ifndef ETCHUTL_H
#define ETCHUTL_H

#include "etch_arrayval.h"
#include "etchhash.h"

#define ETCH_ARRAYTYPE_NONE     0
#define ETCH_ARRAYTYPE_NATIVE   1
#define ETCH_ARRAYTYPE_LIST     2
#define ETCH_ARRAYTYPE_ARRAYVAL 3

typedef struct etch_arrayinfo
{
    int numdims;
    int numitems;
    unsigned char array_type;
    unsigned char is_content_object;

} etch_arrayinfo;


etch_arraylist* get_map_keys(etch_hashtable*);

etch_arraylist* get_map_values(etch_hashtable*);

int is_etcharray(objmask*, etch_arrayinfo*);

int hexchar_to_int (const unsigned char hexchar);
int hexwchar_to_int(const wchar_t hexwchar);

int etchlog_countfiles(char* dirpath);  
int etchlog_purgefiles(char* dirpath, const int, const int);
int etch_existsfile(char* path);


#if IS_WINDOWS_ETCH
 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * etch windows emulation of POSIX directory browsing functions  
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

typedef struct dirent  /* directory entry descriptor */
{ /* *nix includes various other fields but dirent.d_name is usual reference 
   * and using it as our only field lets us cast a string to a dirent* 
   */
  wchar_t d_name [MAXPATHSIZE+1];
} dirent;


typedef struct DIR  /* "directory stream" descriptor */
{
  void*   dirhandle;  /* HANDLE */
  wchar_t dirname[1];
} DIR;


DIR*    opendir  (char *dirname);
dirent* readdir  (DIR*);
int     closedir (DIR*);
void    rewinddir(DIR*);

#else  /* IS_WINDOWS_ETCH */

#include "dirent.h" /* linux opendir, readir, etc. */

#endif /* IS_WINDOWS_ETCH */


#endif /* #ifndef ETCHUTL_H */