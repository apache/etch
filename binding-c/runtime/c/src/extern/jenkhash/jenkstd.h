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
------------------------------------------------------------------------------
Standard definitions and types, Bob Jenkins
------------------------------------------------------------------------------
*/
#ifndef STANDARD
#define STANDARD

/*
   JLD cisco systems: ensure VS 2005 wide character support is turned on.
   These definitions are enabled in the VS project, however for Linux and etc.
   they may serve as a reminder that an etch binding should be unicode enabled.
*/
#ifndef _UNICODE    /* defined by default in VS2005 */
#define _UNICODE
#endif

#if defined(WIN32) && !defined(_WIN32_WCE)
#include <tchar.h>  /* wide char support, wmain() vs main() */
#else
#include <wchar.h>
#endif

#ifndef UNICODE     /* defined by default in VS2005 */
#define UNICODE
#endif
/*
   end VS 2005 wide character support -- JLD
*/
 
#include <stdio.h>
#include <stddef.h>

typedef  unsigned long long  ub8;
#define UB8MAXVAL 0xffffffffffffffffLL
#define UB8BITS 64
typedef    signed long long  sb8;
#define SB8MAXVAL 0x7fffffffffffffffLL
typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
#define UB4MAXVAL 0xffffffff
typedef    signed long  int  sb4;
#define UB4BITS 32
#define SB4MAXVAL 0x7fffffff
typedef  unsigned short int  ub2;
#define UB2MAXVAL 0xffff
#define UB2BITS 16
typedef    signed short int  sb2;
#define SB2MAXVAL 0x7fff
typedef  unsigned       char ub1;
#define UB1MAXVAL 0xff
#define UB1BITS 8
typedef    signed       char sb1;   /* signed 1-byte quantities */
#define SB1MAXVAL 0x7f

/* JLD replaced Jenkins' 'word' typdef with 'intx'. 'word' is too 
   likely to be confused by the reader with a Windows 16-bit integer.
   intx is brief, and is more readable as 'the register size on the
   host OS". It remains unclear whether simply changing this typedef to 
   a 64-bit integer, on a 64-bit OS, is sufficient for the hashtable
   to work as advertised in 64 bits. See macro renew() for example,
   where it appears as if sizeof(char*) is assumed same as sizeof(int).   
*/
typedef int intx; 
/* typedef int  word fastest type available */

#define bis(target,mask)  ((target) |=  (mask))
#define bic(target,mask)  ((target) &= ~(mask))
#define bit(target,mask)  ((target) &   (mask))

/* JLD commented this stuff out
#ifndef min
#define min(a,b) (((a)<(b)) ? (a) : (b))
#endif  

#ifndef max
#define max(a,b) (((a)<(b)) ? (b) : (a))
#endif  
*/

#ifndef align
#define align(a) (((ub4)a+(sizeof(void *)-1))&(~(sizeof(void *)-1)))
#endif  

#ifndef abs
#define abs(a)   (((a)>0) ? (a) : -(a))
#endif

#define TRUE    1
#define FALSE   0
#define SUCCESS 0  /* 1 on VAX */

#endif /* STANDARD */
