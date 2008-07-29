/**
* fieldtest.c
* Tests the C implementation of the etch_field object.
* This is the same as the id_name test, but using the etch_field typedef.
*/

#include "field.h"
#include "etchhash.h"
#include "etchmem.h"
#include <conio.h>
#define ID2 987654321


int _tmain(int argc, _TCHAR* argv[])
{
    char c=0;
    int  testresult = FALSE;
    errno_t result1 = 0, result2 = 0;
    size_t  actlen1 = 0, actlen2 = 0;
    unsigned hash1  = 0, hash2   = 0;
    etch_field static_field;

    etch_hashtable* myhash = NULL;    /* define a hashtable */
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;

    wchar_t* wstr1 = L"abracadabra"; /* is L macro Windows only? */
    wchar_t* wstr2 = L"gilgamesh";

    const size_t numElements1 = wcslen(wstr1);
    const size_t numElements2 = wcslen(wstr2);

    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;

    wchar_t *key1 = NULL, *key2 = NULL;
    wchar_t *foundkey1 = NULL,  *foundkey2 = NULL; 
    etch_field *field1 = NULL, *field2 = NULL;

    wprintf(L"begin field test\n\n");

    do {

    /* verify wcscpy_s: allocate new keys on heap and copy into them keys from stack.  
     * This illustrates some important considerations not obvious from the MSDN docs. 
     * wcscpy_s parameter 2 is documented as "numberOfElements: size of the buffer".   
     * Trial and error shows that we must allocate a (wcslen(s)+1)*2) byte buffer, and 
     * specify the wcscpy_s numberOfElements parameter as wcslen(s)+1. The MSDN docs   
     * don't explain this because the examples are all templated C++ in which there is 
     * no length parameter to wcscpy_s(). */
    key1 = malloc(numBytes1 + 2);  /* NOTE 1: wchar_t buffer size will be */
    key2 = malloc(numBytes2 + 2);  /* (number of characters + 1) * 2      */
    if (key1 == NULL || key2 == NULL) break;
        
    result1 = wcscpy_s(key1, numElements1+1, wstr1);  /* NOTE 2: wcscpy_s param 2 is */
    result2 = wcscpy_s(key2, numElements1+1, wstr2);  /* number of characters + 1    */

    if (result1 != 0 || result2 != 0) 
    {   wprintf(L"wscpy_s error %d %d\n",result1,result2);
        break;
    }
    actlen1 = wcslen(key1); actlen2 = wcslen(key2);
    wprintf(L"key 1 %s length %d bytes %d\n",key1,actlen1,numBytes1);
    wprintf(L"key 2 %s length %d bytes %d\n",key2,actlen2,numBytes2);

    /* create an etch_id_name object for each of the keys */
    field1 = field_ctor1(key1, (int)numBytes1, NULL);
    field2 = field_ctor2(ID2, key2, (int)numBytes2, &static_field);
    if (!field1 || !field2)
    {   wprintf(L"field_ctor error\n");
        break;
    }          
    wprintf(L"field 1: %s %d %d\n",field1->name,field1->namebytelen,field1->id);
    wprintf(L"field 2: %s %d %d\n",field2->name,field2->namebytelen,field2->id);

    /* do an ad hoc hash of the two keys and ensure that the hashkeys match */
    hash1 = field_hashfunc(key1, numBytes1);
    hash2 = field_hashfunc(key2, numBytes2);
    if (!hash1 || !hash2)
    {   wprintf(L"field_hashfunc error\n");
        break;
    } 
    wprintf(L"ad hoc hash of key 1: %d key 2: %d\n",hash1,hash2);  

    myhash = new_hashtable(16);  /* create a hash table */
    if (!myhash) 
    {   wprintf(L"new_hashtable error\n");
        break;
    } 

    /* insert the two keys into the hashtable allowing the hashtable to hash the keys */
    if (0 != myhash->vtbl->insert(myhash->realtable, key1, (int)numBytes1, NULL,0,0,0))         
    {   wprintf(L"hashtable->insert %s %d error\n", key1, numBytes1);
        break;
    }
    if (0 != myhash->vtbl->insert(myhash->realtable, key2, (int)numBytes2, NULL,0,0,0))         
    {   wprintf(L"hashtable->insert %s %d error\n", key2, numBytes2);
        break;
    }

    /* do a hashtable lookup on our ad-hoc hashed keys, which should be identical  
     * to the hashkeys created by the hashtable */
    if (0 == myhash->vtbl->findh(myhash->realtable, hash1, NULL, &myentry))
        foundkey1 = (wchar_t*)myentry->key;    
    else
    {   wprintf(L"hashtable->findh %d error\n", hash1);
        break;
    }

    if (0 == myhash->vtbl->findh(myhash->realtable, hash2, NULL, &myentry))
        foundkey2 = (wchar_t*)myentry->key;
    else
    {   wprintf(L"hashtable->findh %d error\n", hash2);
        break;
    }

    /* verify that the lookup returned good key pointers */
    if (!foundkey1 || !foundkey2)
    {   wprintf(L"hashtable->findh %d error\n", hash2);
        break;
    }

    /* display keys found on lookup */
    wprintf(L"key %s found on hash %d lookup\n",foundkey1, hash1);
    wprintf(L"key %s found on hash %d lookup\n",foundkey2, hash2);

    if (0 != memcmp(foundkey1, key1, numBytes1) || 
        0 != memcmp(foundkey2, key2, numBytes2))  
    {   wprintf(L"error: keys inserted do not match keys located\n");
        break;
    }
 
    testresult = TRUE;

    } while(0);

    if (key1) 
        free(key1); 
    if (key2) 
        free(key2);  
    field_destroy(field1);
    /* field_destroy(field2); */    /* on stack */
                 
    /* destroy hashtable *without* freeing its contents */
    delete_hashtable(myhash, FALSE);  

    debug_showmem();

    if (testresult) wprintf(L"\ntest succeeded\n"); else wprintf(L"\ntest failed\n");
    wprintf(L"end field test\n\n");
    wprintf(L"any key ..."); while(!c) c = _getch(); wprintf(L"\n"); 
	return testresult;
}

