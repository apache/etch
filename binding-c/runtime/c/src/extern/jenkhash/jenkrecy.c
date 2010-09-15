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
--------------------------------------------------------------------
By Bob Jenkins, September 1996.  recycle.c
You may use this code in any way you wish, and it is free.  No warranty.

This manages memory for commonly-allocated structures.
It allocates RESTART to REMAX items at a time.
Timings have shown that, if malloc is used for every new structure,
malloc will consume about 90% of the time in a program.  
This module cuts down the number of mallocs by an order of magnitude.
This also decreases memory fragmentation, and freeing structures
only requires freeing the root.
--------------------------------------------------------------------
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jenkstd.h"
#include "jenkrecy.h"


reroot *remkroot(size_t size)
{
   reroot *r = (reroot*) remalloc(sizeof(reroot), "recycle.c, root");
   r->list  = (recycle*)0;
   r->trash = (recycle*)0;
   r->size  = align(size);
   r->logsize = RESTART;
   r->numleft = 0;
   return r;
}



void refree(struct reroot *r)
{
   recycle *temp;
   if (temp = r->list) while (r->list)
   {
      temp = r->list->next;
      free((char *)r->list);
      r->list = temp;
   }
   free((char *)r);
   return;
}



/* to be called from the macro renew only */
char *renewx(struct reroot *r)
{
   recycle *temp;
   if (r->trash)
   {  /* pull a node off the trash heap */
      temp = r->trash;
      r->trash = temp->next;
      (void)memset((void *)temp, 0, r->size);
   }
   else
   {  /* allocate a new block of nodes */
      r->numleft = (int) r->size*((ub4)1<<r->logsize);
      if (r->numleft < REMAX) ++r->logsize;
      temp = (recycle *)remalloc(sizeof(recycle) + r->numleft, 
				 "recycle.c, data");
      temp->next = r->list;
      r->list = temp;
      r->numleft -= (int) r->size;
      temp = (recycle *)((char *)(r->list+1)+r->numleft);
   }
   return (char *)temp;
}


char *remalloc(size_t len, char* purpose)
{
  char *x = (char*) malloc(len);
  if (!x)
  {
    fprintf(stderr, "malloc %lu failed for %s\n", (unsigned long)len, purpose);
    exit(SUCCESS);
  }
  return x;
}

