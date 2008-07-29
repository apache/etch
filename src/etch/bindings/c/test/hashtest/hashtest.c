/*
 * hashtest.c: test the etch interface to Jenkins hashtable
 */


#include <stdio.h>
#include <tchar.h>
#include <conio.h> 
#include "etchhash.h"
#include "etchrun.h"

#pragma warning(disable: 4996)  /* disable unsafe func wng */
#define TESTDATAPATH "./jenktest.txt"
#define MAXLINELEN 64
#define DATASIZE    4
#define DATAVALUE  "foo" 
#define INITIAL_TABLE_SIZE 64


/*
 * test_key_pointer: test using a memory address as a hash key.
 * return 1 if OK, zero if failed.
 */
int test_key_pointer(etch_hashtable* ht)
{
	etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;
    char* test_string = "it works"; 
    const int value_len = (int)strlen(test_string)+1; /* string length of value */
    const int key_len = sizeof(char*); 

    /* our hash key is a 4-byte item, which we will use to contain the 
       value of a memory address.
     */
	char* pkey = malloc(key_len);

    /* the value of the pval memory address is our hash key.
     * the string pval is our hash value. Of course, our hash table stores pointers,
     * not values, so the key is &pval, and the value is pval.
     */
    char* pval = malloc(value_len);  
    strcpy(pval, test_string); 

    /* copy the memory address into our hash key.
     */
    memcpy(pkey, &pval, sizeof(char*));

    if  (0 != ht->vtbl->insert(ht->realtable, pkey, key_len, pval, value_len, 0, 0)) 
	{	 printf("** insert of pointer key failed at %s line %d\n", __FILE__,__LINE__);
	     return 0;
	}

	if  (0 != ht->vtbl->find(ht->realtable, pkey, key_len, NULL, &myentry)) 
	{	 printf("** lookup of pointer key failed at %s line %d\n", __FILE__,__LINE__);
	     return 0;
	}

	if (myentry->value == NULL) 
	{   printf("** lookup of key value failed at %s line %d\n", __FILE__,__LINE__);
	    return 0;
	}

    /* This code demonstrates what we've done here. The key is the value of a
     * char*, and the value is that same char*. So, indirectly dereferencing  
     * the key must elicit the same result as dereferencing the value. 
     */
	if (memcmp(myentry->key, &myentry->value, sizeof(char*)) != 0)
	{   printf("** hash of pointer key failed at %s line %d\n", __FILE__,__LINE__);
	    return 0;
	}
	if (strcmp(*(char**)myentry->key, (char*)myentry->value) != 0)
	{   printf("** hash of pointer key failed at %s line %d\n", __FILE__,__LINE__);
	    return 0;
	}

	printf("hash of pointer key succeeded: found '%s'\n", myentry->value);	   
	return 1;
}


int _tmain(int argc, _TCHAR* argv[])
{
  char  buf[MAXLINELEN];
  char* pkey; void* pdata;
  int   n, keylen, myincount=0, myhashcount=0, myfreecount=0;
  etch_hashtable* myhash;
  etch_hashitem   hashbucket;
  etch_hashitem*  myentry = &hashbucket;
  unsigned hashed;
  char  c = 0, *p;
  FILE* f = 0;
  printf("** hashtable interface test start\n\n");
  p = "abracadabra"; n = (int)strlen(p); // Test the hash function
  hashed = etchhash(p, n, 0);
  printf("hash of %s is %u\n\n", p, hashed);

  do {

  f = fopen(TESTDATAPATH,"r");
  if (!f)
  {   printf("could not open test data file %s\n",TESTDATAPATH);
      break;
  }

  myhash = new_hashtable(INITIAL_TABLE_SIZE);  /* create hash table */
  if (!myhash) break;
  myhash->vtbl->stats(myhash->realtable, 0, 0); /* show table stats */
  n = myhash->vtbl->size(myhash->realtable, 0, 0);
  assert(n == INITIAL_TABLE_SIZE); 
  
  while (fgets((char*)buf, MAXLINELEN, f))   /* read the testdata file line by line */ 
  {
    unsigned char* p = buf; while(*p++) if (*p <= 0x0d) *p = 0;       /* strip crlf */
    keylen = (int)strlen(buf);   /* we use null termed key, but it is not necessary */ 
    pkey   = malloc(keylen+1); strcpy(pkey, buf);    
    pdata  = malloc(DATASIZE);  /* note that user allocates memory for key and data */
    strcpy(pdata, DATAVALUE);   /* hashtable does not make copies of key or of data */
                                /* insert the new hashtable entry */          
    if  (0 == myhash->vtbl->insert(myhash->realtable, pkey, keylen, pdata, DATASIZE, 0, 0))         
    {    myincount++;           /* point at current entry in hashtable */
         myhash->vtbl->current(myhash->realtable, 0, &myentry);  
         assert(myentry->key   == pkey); /* ensure items just inserted are there */
         assert(myentry->value == pdata);
    }
    else                        /* insert failure - probably duplicate */
    {    printf("** insert of '%s' failed\n", buf);   
         free(pkey);  
         free(pdata);
    }
  }

  fclose(f); f = 0;
  myhash->vtbl->stats(myhash->realtable, 0, 0); /* show table stats */ 

  pkey = "banana";                           /* test a hashtable lookup */
  if  (0 == myhash->vtbl->find(myhash->realtable, pkey, (int)strlen(pkey), NULL, &myentry))   
       assert(0 == strcmp(pkey, myentry->key));
  else printf("hashtable.find('banana') failed\n");  

                                    /* test hashtable sequential access */
  if  (0 == myhash->vtbl->first(myhash->realtable, NULL, &myentry))   
  {    printf("%s: %s\n", myentry->key, myentry->value);
       myhashcount = 1;
  }
  else printf("hashtable.first() failed\n");       
                                    /* move to next element */
  while(0 == myhash->vtbl->next(myhash->realtable, NULL, &myentry))          
  {  printf("%s - %s\n", (char*)myentry->key, (char*)myentry->value);
     myhashcount++;
  }

  /* test using pointer as key, adding 0 or 1 to myhashcount */
  myhashcount += test_key_pointer(myhash);

  /* destroy hash table and all its contents (free key and data memory) */
  myfreecount = myhash->vtbl->clear(myhash->realtable, TRUE, TRUE, 0, 0);
  assert(myfreecount == myhashcount);
  
  } while(0);

  printf("** keys read %d -- hashed %d -- removed %d\n", 
         myincount, myhashcount, myfreecount);

  printf("** hashtable interface test end\n\n");
  printf("any key ..."); while(!c) c = _getch(); printf("\n"); 
  return 0;
}

