/*
** By Bob Jenkins, February 22, 1997, Public Domain
** Contains various modifications by j l decocq cisco systems 2007
** This is an example of how to use the hash table.
**
** Given an input (stdin) with lines in any order
** produce an output (stdout) with duplicate lines removed.
** Lines may not be longer than 4096 characters.
*/
#pragma warning(disable: 4996) /* quash VS compiler unsafe function warning JLD */

/*
 * JLD comments: the hash table appears to be fine with wide character support.
 * _UNICODE is for C runtime TCHAR support, i.e. _tcscpy (--> wcscpy vs strcpy), 
 * distinct from UNICODE, which turns, say, CreateWindowEx into CreateWindowExW 
 * instead of CreateWindowExA. 
*/

#ifndef _UNICODE    /* defined by default in VS2005 */
#define _UNICODE
#endif
#include "tchar.h"  /* wide char support, wmain() vs main() */

#ifndef UNICODE     /* defined by default in VS2005 */
#define UNICODE
#endif
 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "jenkstd.h"
#include "jenkhtab.h"
#include <conio.h>
#define TESTDATAPATH "jenktest.txt"
#define MAXLINELEN 64
#define INITIAL_TABLE_SIZE_LOG2 2


int _tmain(int argc, _TCHAR* argv[])
{
  /* when _UNICODE is defined _tmain expands to wmain, otherwise it expands to main */
  char  buf[MAXLINELEN];
  char* pkey;
  int   keylen, myincount=0, myhashcount=0, myfreecount=0;
  htab* myhashtable;
  char  c = 0;
  FILE* f;
  printf("** jenktest.c start\n\n");

  do {

  f = fopen(TESTDATAPATH,"r");
  if (!f)
  {   printf("could not open test data file %s\n",TESTDATAPATH);
      break;
  }

  myhashtable = hcreate(INITIAL_TABLE_SIZE_LOG2); /* create hash table */
  hstat(myhashtable);  /* show table stats */

  /* read in all the lines */
  while (fgets((char*)buf, MAXLINELEN, f))  
  {
    keylen = (ub4) strlen(buf);

    if (hadd(myhashtable, buf, keylen, (void *)0)) /* if not a duplicate */
    {
      pkey = (char*) malloc(keylen);      /* dumb use of malloc */
      memcpy(pkey, buf, keylen);          /* copy buf into pkey */
      hkey(myhashtable) = pkey;           /* replace buf with pkey */
      myincount++;
    }
  }

  fclose(f); f = 0;
  hstat(myhashtable);                  /* show table stats */

  if  (hfirst(myhashtable))            /* go to first element */  
       printf("%.*s\n", hkeyl(myhashtable), hkey(myhashtable));  
  else break;
  myhashcount = 1;
   
  while(hnext(myhashtable))           /* go to next element */
  {  printf("%.*s\n", hkeyl(myhashtable), hkey(myhashtable));  
     myhashcount++;
  }

  while (hcount(myhashtable))          /* while the table is not empty */
  {
    free(hkey(myhashtable));           /* free memory for the line */
    hdel(myhashtable);                 /* delete from hash table */
    myfreecount++;
  }

  hstat(myhashtable);                  /* show table stats */

  hdestroy(myhashtable);               /* destroy hash table */

  } while(0);

  printf("** keys read %d -- hashed %d -- removed %d\n", 
         myincount, myhashcount, myfreecount);
  printf("\n** jenktest.c end\n\n");
  printf("any key ..."); while(!c) c = _getch(); printf("\n"); 
  return 0;
}
