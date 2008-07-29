/**
 * objtest.c -- test the etch object 
 * -- this tests whataver state of development the current object is currently
 * -- in, so the test should not be formalized until the object is formalized.
 */

#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include "etchobj.h"
#include "etch_objects.h"
#include "etch_global.h"

/**
 * Example of a generated list of function pointers to generated functions.
 * Implementation gets such a list in lieu of having to code possibly very
 * long qualified generated function names.
 */

typedef struct fnlist   
{
    int  (*add) (int a, int b);
    int  (*sub) (int a, int b);
    void (*show)(char* buf);
    etchobject* (*idiv) (etchobject* x, etchobject* y);

} fnlist;


int  __cdecl impl_add(int n1, int n2);
int  __cdecl impl_sub(int n1, int n2);
void __cdecl impl_show(char* p);
etchobject* __cdecl impl_idiv(etchobject* x, etchobject* y);


/**
 * Example of populating a function list. Such a populated list of function
 * pointers will be created by the framework and made available to the
 * implementation, in order that implementation can write: 
 *    svc->add(x,y);
 * as opposed to coding to the possibly very long generated function name 
 * and its qualifiers.
 */
void init_fnstruct(fnlist* fs)
{  
    memset(fs, 0, sizeof(fnlist));
    fs->add  = impl_add;
    fs->sub  = impl_sub;
    fs->show = impl_show;
    fs->idiv = impl_idiv;
}


/**
 * these are concrete functions to demonstrate that the interface is pointing at
 * real functions, doing work, and returning a value. in the real world we would   
 * not be performing business logic here as these examples do, we would be calling  
 * into generated message building and result retrieval functions 
 */

int impl_add(int n1, int n2)
{
    int result = n1 + n2;
    return result;
}

int impl_sub(int n1, int n2)
{
    int result = n1 - n2;
    return result;
}

void impl_show(char* p)
{
    if (p) printf(p);
}


ETCH_INT32* impl_idiv(ETCH_INT32* x, ETCH_INT32* y)
{   
    /* example of an integer divide using boxed integers */
    etchobject* result = NULL;
    const unsigned expected_type = ETCHTYPE_BOXED_INT32;
    if (x == NULL  || y == NULL) return NULL;  /* 'throw' excp maybe */
    if (x->vtab == NULL || y->vtab == NULL) return NULL; 
    if (x->is_null || y->is_null) return NULL;  
    if (x->type != expected_type || y->type != expected_type) return NULL;
    if (y->value_int32 == 0) return NULL;      /* intercept div by zero */
    if (NULL == (result = x->vtab->clone(x))) return NULL; /* freed by caller */

    /* disclaimer: if this were real code we would need to check the type of the 
     * object here as well. not only might it not be integer, but it could be an
     * integer but of different width. Here we're simply assuming two 32-bit ints.
     * In the real world we might widen all args to 64 bits and narrow the result. 
     */      
    result->value_int32 = x->value_int32 / y->value_int32;  /* do div & assign */

    /* TODO populate result int the return object. "throw" exceptions. */
    /* TODO generic facility for populating result into an object . */
    /* TODO generic facility for populating exception into an object . */
    /* TODO in object destroy, free result and free exception */

    return result;      
}


/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{
    ETCH_INT32* Int1 = NULL;
    ETCH_INT32* Int2 = NULL;
    ETCH_INT32* Int3 = NULL;
    int result = 0, testresult = TRUE;
    char buf[32], c=0;
    fnlist f; fnlist* functions = &f;
    init_fnstruct(functions);  /* load up the function ptrs list */
    wprintf(L"start object test\n\n");

    /* Call the add, sub, and show functions via function ptrs list */
    result = (*functions->add) (1,2);
    result = (*functions->sub) (result,1);
    sprintf_s(buf, sizeof(buf), "(1+2) - 1 = %d\n", result);
    (*functions->show) (buf); 

    /* Box its args and call the idiv function from function ptrs list.
     * We might do: #define NewInt(a,b) ctor_etchobject_int(a,b)
     * We might do: typedef Int etchobject*; Int Int1 = 0;
     */
    Int1 = new_etch_int32(result);             /* box variable result */
    Int2 = new_etch_int32(2);                  /* box value 2 */
    Int3 = (*functions->idiv) (Int1, Int2);    /* do the divide */

    sprintf_s(buf, sizeof(buf), "result / 2 = %d\n", Int3->value_int32);
    (*functions->show) (buf); 

    etch_dumpmem();

    Int1->vtab->destroy(Int1); /* deallocate all the etchobject*s we created. */
    Int2->vtab->destroy(Int2); /* should we pass etchobject.destroy **this_p  */ 
    Int3->vtab->destroy(Int3); /* so we can null out its containing variable? */ 
    
    debug_showmem(TRUE,TRUE);  /* free leaks, console out */
    cache_clear();  /* must not clear cache before final hashtable access */   

    if (testresult) wprintf(L"\ntest succeeded\n"); else wprintf(L"\ntest failed\n");
    wprintf(L"end object test\n\n");
    wprintf(L"any key ..."); while(!c) c = _getch(); wprintf(L"\n"); 

	return 0;
}

