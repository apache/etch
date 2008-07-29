/**
 * arrayvaluetest.c -- test etch_array_value object
 */

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_global.h"
#include "etch_objects.h"
#include "etch_arrayval.h"
#include "etchexcp.h"


int suite_init(void)    /* pre-suite init */
{
    int result = etch_init();
    return result;
}

int suite_cleanup(void) /* post-suite cleanup */
{
   return etch_runtime_cleanup(0,0); /* free memtable and cache etc */
}

#define OBJTYPE_FAKETDI_IMPL ETCHTYPEB_INSTANCEDATA
#define CLASSID_FAKETDI_IMPL 0xf0
#define OBJTYPE_FAKETDO_IMPL ETCHTYPEB_INSTANCEDATA
#define CLASSID_FAKETDO_IMPL 0xf1
#define OBJTYPE_FAKETDI_VTABLE 0x88
#define OBJTYPE_FAKETDO_VTABLE 0x89

#define EXCPTEST_UNCHECKED_STATICTEXT 1
#define EXCPTEST_CHECKED_COPYTEXT     2
#define EXCPTEST_CHECKED_STATICTEXT   3

#define IS_DEBUG_CONSOLE FALSE  /* TRUE to display test diagnostics */

etch_arraylist* testdata;
int g_bytes_allocated, g_which_exception_test;
wchar_t* local_excp_text = L"global text";


/**
 * fake_tdi_impl
 * instance data for a TDI implementation
 */
typedef struct fake_tdi_impl
{
    unsigned short obj_type; 
    unsigned short class_id;
    impl_destroy_handler destroy_handler;

    int index;
    byte started, done, ended;
    ETCH_ARRAYVALUE* arrray;
} fake_tdi_impl;


/**
 * fake_tdo_impl
 * instance data for a TDO implementation
 */
typedef struct fake_tdo_impl
{
    unsigned short obj_type; 
    unsigned short class_id;
    impl_destroy_handler destroy_handler;

    byte started, ended;
    etch_arraylist* list;   
    ETCH_ARRAYVALUE* arrray;
} fake_tdo_impl;


/**
 * fake_tdi_impl_destroy_handler
 * memory cleanup handler for fake_tdi_impl
 */
int fake_tdi_impl_destroy_handler(fake_tdi_impl* impl)
{
    impl_destroy_handler destroy = NULL;
    int result = verify_object((objmask*)impl, OBJTYPE_FAKETDI_IMPL, CLASSID_FAKETDI_IMPL, NULL);
    if (result == -1) return -1; /* object passed was not expected object */

    if (impl->arrray)
        impl->arrray->vtab->destroy(impl->arrray); 

    etch_free(impl);    
    return 0;
}


/**
 * fake_tdo_impl_destroy_handler
 * memory cleanup handler for fake_tdo_impl
 */
int fake_tdo_impl_destroy_handler(fake_tdo_impl* impl)
{
    impl_destroy_handler destroy = NULL;
    int result = verify_object((objmask*)impl, OBJTYPE_FAKETDO_IMPL, CLASSID_FAKETDO_IMPL, NULL);
    if (result == -1) return -1; /* object passed was not expected object */

    arraylist_destroy(impl->list, TRUE); /* destroy list, free content */
    impl->arrray->vtab->destroy(impl->arrray);

    etch_free(impl);    
    return 0;
}


/**
 * new_fake_tdi_impl()
 * constructor for TDI implementation instance data
 */
fake_tdi_impl* new_fake_tdi_impl()
{
    fake_tdi_impl* instance_data = etch_malloc(sizeof(fake_tdi_impl), ETCHTYPEB_INSTANCEDATA);
    memset(instance_data, 0, sizeof(fake_tdi_impl));
    instance_data->obj_type = OBJTYPE_FAKETDI_IMPL;
    instance_data->class_id = CLASSID_FAKETDI_IMPL;

    /* set destructor for fake TDI instance data */
    instance_data->destroy_handler = fake_tdi_impl_destroy_handler;
    
    return instance_data;
}


/**
 * new_fake_tdo_impl()
 * constructor for TDO implementation instance data
 */
fake_tdo_impl* new_fake_tdo_impl()
{
    fake_tdo_impl* instance_data = etch_malloc(sizeof(fake_tdo_impl), ETCHTYPEB_INSTANCEDATA);
    memset(instance_data, 0, sizeof(fake_tdo_impl));
    instance_data->obj_type = OBJTYPE_FAKETDO_IMPL;
    instance_data->class_id = CLASSID_FAKETDO_IMPL;

    /* set destructor for fake TDO instance data */
    instance_data->destroy_handler = fake_tdo_impl_destroy_handler;
    
    return instance_data;
}


enum objtyp ETCHTYPE_VTABLE_FAKETDI = MAKETYPE_BB(ETCHTYPEB_VTABLE,ETCHTYPEB_USER+0x1); 
enum objtyp ETCHTYPE_VTABLE_FAKETDO = MAKETYPE_BB(ETCHTYPEB_VTABLE,ETCHTYPEB_USER+0x2); 


/**
 * faketdi_start_array() overrides tdi_start_array()
 * Starts reading an array from the stream.
 * @return the array that we are reading.
 * @throws IOException if there is a problem with the stream.
 * TODO instantiate and percolate exception
 */
ETCH_ARRAYVALUE* faketdi_start_array(tagged_data_input* tdi)
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    impl_destroy_handler destroy = NULL;
    ETCH_ARRAYVALUE* newarrayval = NULL;
    const int DIM_ZERO = 0, READONLY = TRUE, DEFSIZE = 0, DEFDELTA = 0;
    byte fake_typecode = 0;
    etch_type* fake_structtype = NULL;
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    data = tdi->impl; /* validate instance data */
    result = verify_object((objmask*)data, OBJTYPE_FAKETDI_IMPL, 0, (void**) &destroy);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(destroy); 

    result = destroy(NULL); /* ensure we can call into instance data destructor */
    CU_ASSERT_EQUAL(result,-1);

    CU_ASSERT_EQUAL(data->started,FALSE);
    CU_ASSERT_EQUAL(data->done,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);

    data->started = TRUE;
    data->index = 0;

    /* create instance array */
    newarrayval = new_boxed_array_value 
        (fake_typecode, fake_structtype, DIM_ZERO, DEFSIZE, DEFDELTA, READONLY); 

    data->arrray = newarrayval;
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->arrray);

    return data->arrray;
}


/**
 * faketdo_start_array() overrides tdo_start_array()
 * TODO instantiate and percolate exception
 */
ETCH_ARRAYVALUE* faketdo_start_array(tagged_data_output* tdo, ETCH_ARRAYVALUE* thisp)
{
    int result = 0;
    fake_tdo_impl* data = NULL;
    impl_destroy_handler destroy = NULL;
  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = tdo->impl; /* validate instance data */
    result = verify_object((objmask*)data, OBJTYPE_FAKETDO_IMPL, 0, (void**) &destroy);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(destroy); 

    result = destroy(NULL); /* ensure we can call into instance data destructor */
    CU_ASSERT_EQUAL(result,-1);

    CU_ASSERT_EQUAL(data->started,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);

    data->arrray = thisp;
    data->list = new_arraylist(0,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->arrray);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->list);

    /**
     * TODO a way to ensure that the arraylist is created with readonly set
     * (meaning the list when destroyed will not attempt to free values)
     */
    data->list->is_readonly = TRUE;

    data->started = TRUE;
    return data->arrray;
}


/**
 * faketdi_read_array_element() overrides tdi_read_array_element()
 * TODO instantiate and percolate exception
 */
int faketdi_read_array_element(tagged_data_input* tdi, ETCH_ARRAY_ELEMENT** out_ae)   
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    impl_destroy_handler destroy = NULL;    
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    data = tdi->impl; /* validate instance data */
    result = verify_object((objmask*)data, OBJTYPE_FAKETDI_IMPL, 0, (void**) &destroy);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(destroy); 

    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->done,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);

	if (data->index < (int)testdata->count)
	{   
		*out_ae = arraylist_get(testdata, data->index++);
		return TRUE;
	}
	
	data->done = TRUE;
	return FALSE;
}


/**
 * faketdo_write_array_element() overrides tdi_read_array_element()
 * TODO instantiate and percolate exception
 */
int faketdo_write_array_element(tagged_data_output* tdo, etchobject* newitem)   
{
    int result = 0;
    fake_tdo_impl* data = NULL;
    impl_destroy_handler destroy = NULL;    
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = tdo->impl; /* validate instance data */
    result = verify_object((objmask*)data, OBJTYPE_FAKETDO_IMPL, 0, (void**) &destroy);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(destroy)
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->list);  

    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
   
	result = arraylist_add(data->list, newitem);
	return 0;
}


/**
 * faketdi_end_array() overrides tdi_end_array()
 * Ends an array that we are reading.
 * @param array the array that we read.
 * @throws IOException if there is a problem with the stream.
 * TODO instantiate and percolate exception
 */
int faketdi_end_array(tagged_data_input* tdi, ETCH_ARRAYVALUE* av)
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    impl_destroy_handler destroy = NULL;    
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    data = tdi->impl; /* validate instance data */
    result = verify_object((objmask*)data, OBJTYPE_FAKETDI_IMPL, 0, (void**) &destroy);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(destroy)
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->arrray); 
 
    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->done,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
    CU_ASSERT_EQUAL(data->arrray,av);
			
	data->ended = TRUE;
    return 0;
}


/**
 * faketdo_end_array() overrides tdo_end_array()
 * TODO instantiate and percolate exception
 */
int faketdo_end_array(tagged_data_output* tdo, ETCH_ARRAYVALUE* av)
{
    int result = 0;
    fake_tdo_impl* data = NULL;
    impl_destroy_handler destroy = NULL;    
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = tdo->impl; /* validate instance data */
    result = verify_object((objmask*)data, OBJTYPE_FAKETDO_IMPL, 0, (void**) &destroy);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(destroy)
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->list); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->arrray); 
 
    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
    CU_ASSERT_EQUAL(data->arrray,av);
			
	data->ended = TRUE;
    return 0;
}


/**
 * faketdi_close() port from java test
 * also tested here is that we can call base methods on a derived object. we invoke 
 * the TDI destructor here, which is an overide of the etchobject destructor.
 * the TDI destructor walks the vtable chain to its parent, and invokes the parent
 * destructor to destroy etchobject content such as any exception, and finally
 * the object itself.
 */
void faketdi_close(ETCH_TAGDATAINPUT* tdi)  
{
    tdi->vtab->destroy(tdi); /* destroy object */
}


/**
 * faketdo_close() port from java test
 */
void faketdo_close(ETCH_TAGDATAOUTPUT* tdo)  
{
    tdo->vtab->destroy(tdo); /* destroy object */
}


/**
 * new_fake_tdi()
 * constructor for TDI implementation  
 */
ETCH_TAGDATAINPUT* new_fake_tdi()
{
    tagged_data_input* faketdi = NULL;
    i_tagged_data_input*  vtab = NULL;
    const int VTABSIZE = sizeof(i_tagged_data_input);
    const int VTAB_KEY = ETCHTYPE_VTABLE_FAKETDI;

    ETCH_TAGDATAINPUT* boxtdi = new_boxed_tagged_data_input();
   
    faketdi = boxtdi->value_ptr_to;

    vtab = cache_find(VTAB_KEY);

    if(!vtab)  
    {   /* we do not track cached allocations */            
        vtab = malloc(VTABSIZE);
        /* default all methods to base methods */
        memcpy(vtab, faketdi->vtab, VTABSIZE);

        vtab->obj_type = ETCHTYPEB_VTABLE;
        vtab->class_id = OBJTYPE_FAKETDI_VTABLE;

        /* override three i_tagged_data_input methods */
        vtab->start_array = faketdi_start_array;
        vtab->end_array   = faketdi_end_array;
        vtab->read_array_element = faketdi_read_array_element;

        vtab->vtab = faketdi->vtab; /* chain parent vtable to override vtable */

        cache_add(VTAB_KEY, vtab);  /* cache the new vtable */
    } 

    CU_ASSERT_EQUAL_FATAL(vtab->class_id, OBJTYPE_FAKETDI_VTABLE);

    faketdi->vtab = vtab;          /* set override vtable */

    faketdi->impl = new_fake_tdi_impl(); /* set TDI instance data */

    switch(g_which_exception_test)
    {   case EXCPTEST_UNCHECKED_STATICTEXT: 
             etch_throw(boxtdi, EXCPTYPE_NULLPTR, NULL, 0);  
             break;   
        case EXCPTEST_CHECKED_COPYTEXT:   
             etch_throw(boxtdi, EXCPTYPE_CHECKED_BOGUS, L"copied text", ETCHEXCP_COPYTEXT | ETCHEXCP_FREETEXT);  
             break; 
        case EXCPTEST_CHECKED_STATICTEXT:   
             etch_throw(boxtdi, EXCPTYPE_CHECKED_BOGUS, local_excp_text, ETCHEXCP_STATICTEXT);  
             break;       
    }

    return boxtdi;
}


/**
 * new_fake_tdo()
 * constructor for TDO implementation  
 */
ETCH_TAGDATAOUTPUT* new_fake_tdo()
{
    tagged_data_output* faketdo = NULL;
    i_tagged_data_output*  vtab = NULL;
    const int VTABSIZE = sizeof(i_tagged_data_output);
    const int VTAB_KEY = ETCHTYPE_VTABLE_FAKETDO;

    ETCH_TAGDATAOUTPUT* boxtdo = new_boxed_tagged_data_output();
     
    faketdo = boxtdo->value_ptr_to;

    vtab = cache_find(VTAB_KEY);

    if(!vtab)  
    {   /* we do not track cached allocations */            
        vtab = malloc(sizeof(i_tagged_data_output));
        /* default all methods to base methods */
        memcpy(vtab, faketdo->vtab, VTABSIZE);

        vtab->obj_type = ETCHTYPEB_VTABLE;
        vtab->class_id = OBJTYPE_FAKETDO_VTABLE;

        /* override three i_tagged_data_output methods */
        vtab->start_array = faketdo_start_array;
        vtab->end_array   = faketdo_end_array;
        vtab->write_array_element = faketdo_write_array_element;

        vtab->vtab = faketdo->vtab; /* chain parent vtable to override vtable */
    
        cache_add(VTAB_KEY, vtab);  /* cache the new vtable */
    } 

    CU_ASSERT_EQUAL_FATAL(vtab->class_id, OBJTYPE_FAKETDO_VTABLE);

    faketdo->vtab = vtab;           /* set override vtable */

    faketdo->impl = new_fake_tdo_impl(); /* set TDO instance data */

    return boxtdo;
}


/* 
 * load_testdata_string()
 * load testdata array with some ETCH_STRING objects
 */
int load_testdata_string()
{
    int i = 0, numitems = 4;
    wchar_t* testval = NULL;
    ETCH_STRING* newobj = NULL;
    wchar_t* str0 = L"now ", *str1 = L"is  ", *str2 = L"the ", *str3 = L"time";
    wchar_t* strings[4] = { str0, str1, str2, str3 };
    const size_t bytelen = (wcslen(str0) + 1) * sizeof(wchar_t);

    for(; i < numitems; i++)
    {
        testval = etch_malloc(bytelen, 0);
        memcpy(testval, strings[i], bytelen);
        newobj = new_etch_string(testval);
        arraylist_add(testdata, newobj);
    }

    return numitems;
}


/* 
 * load_testdata_int()
 * load testdata array with some ETCH_INT objects
 */
int load_testdata_int()
{
    int i = 0, numitems = 4;
    ETCH_INT32* newobj = NULL;
    int ints[4] = { 1, 2, 3, 4 };

    for(; i < numitems; i++)
    {
        newobj = new_etch_int32(ints[i]);
        arraylist_add(testdata, newobj);
    }

    return numitems;
}


/* 
 * new_testdata()
 * create testdata array and load it up with data objects
 */
int new_testdata(const int datatype)
{
    int count = 0;
    g_which_exception_test = 0;
    testdata = new_arraylist(0,0); /* list can call desto*/
    testdata->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
 
    switch(datatype)
    { case 1: count = load_testdata_int();    break;
      case 2: count = load_testdata_string(); break;
      default: return -1;
    }
  
    return count;
}


/* 
 * destroy_testdata()
 * destroy testdata array and content
 */
void destroy_testdata()
{
    arraylist_destroy(testdata, TRUE);
}


/* 
 * compare_lists()
 * compares testdata list with the arrayvalue's list
 * returns boolean indicating equal or not
 */
int compare_lists(ETCH_ARRAYVALUE* avobj)
{
    int testcount = 0, result = 0, i = 0, eqcount = 0;

    etch_array_value* avx = avobj->value_ptr_to;
    CU_ASSERT_PTR_NOT_NULL_FATAL(avx);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(avx->list);

    testcount = avx->list->count; 
    CU_ASSERT_EQUAL(testcount, testdata->count);
    if (testcount != testdata->count) return FALSE;

    for(; i < (const int) testcount; i++)     
        if (arraylist_get(testdata, i) == arraylist_get(avx->list, i))
            eqcount++;  

    CU_ASSERT_EQUAL(testcount, eqcount);     
    return testcount == eqcount;
}


/* 
 * run_read_test
 * create an array value to read current test data
 * and verify that content matches test data
 */
void run_read_test()
{
    int result = 0;
    tagged_data_input* tdi = NULL;
    ETCH_ARRAYVALUE* avobj = NULL;

    /* create a bogus TDI inheriting from tagged data input */
    ETCH_TAGDATAINPUT* tdiobj = new_fake_tdi();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdiobj);
    CU_ASSERT_FALSE_FATAL(is_exception(tdiobj));
    CU_ASSERT_EQUAL_FATAL(get_result(tdiobj),0);

    tdi = tdiobj->value_ptr_to; 
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);

    /* create array value, reading from test values */
    avobj = arrayvalue_read(tdi);

    result = compare_lists(avobj);
    CU_ASSERT_EQUAL(result, TRUE);

    /* clean up TDI */
    faketdi_close(tdiobj);

    /* destroy testdata list and content */
    arraylist_destroy(testdata, TRUE);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * run_write_test
 * create an array value to write current test data
 * and verify that content matches test data
 */
void run_write_test()
{
    int result = 0, i = 0;
    ETCH_TAGDATAOUTPUT* tdoobj = NULL;
    ETCH_ARRAYVALUE*    arrayvalobj = NULL;
    etch_array_value*   arrayval = NULL;
    tagged_data_output* tdo = NULL;
    const int DIM_ZERO = 0, NOT_READONLY = FALSE, DEFSIZE = 0, DEFDELTA = 0;
    etch_type* fake_structtype = NULL;
    byte  fake_typecode = 0;
    void* thisitem = NULL;
                                         
    arrayvalobj = new_boxed_array_value /* create array value instance */
        (fake_typecode, fake_structtype, DIM_ZERO, DEFSIZE, DEFDELTA, NOT_READONLY);

    CU_ASSERT_PTR_NOT_NULL_FATAL(arrayvalobj);
    CU_ASSERT_FALSE_FATAL(is_exception(arrayvalobj));
    CU_ASSERT_EQUAL_FATAL(get_result(arrayvalobj),0);

    for(; i < (const int) testdata->count; i++)
    {     
        thisitem = arraylist_get(testdata, i);
        result = arrayvalue_add(arrayvalobj, (ETCH_ARRAY_ELEMENT*) thisitem);
        CU_ASSERT_EQUAL(result,0);
    }

    /* create a bogus TDO inheriting from tagged data output */
    tdoobj = new_fake_tdo();

    tdo = tdoobj->value_ptr_to; 
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);

    result = arrayvalue_write(arrayvalobj, tdo);
    CU_ASSERT_EQUAL(result,0);

    faketdo_close(tdoobj);

    /* destroy testdata list but not content */
    arraylist_destroy(testdata, FALSE);   

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * run_exception_test
 *  
 */
void run_exception_test(const int whichtest)
{
    int result = 0;
    /* global marker asks components to throw exceptions */
    tagged_data_input* tdi = NULL;
    ETCH_ARRAYVALUE* avobj = NULL;
    ETCH_TAGDATAINPUT* tdiobj = NULL;
    etchexception* excp = NULL;
    g_which_exception_test = whichtest; 

    /* create a bogus TDI inheriting from tagged data input */
    tdiobj = new_fake_tdi();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdiobj);

    do 
    {   switch(whichtest)
        {   case EXCPTEST_UNCHECKED_STATICTEXT:
            case EXCPTEST_CHECKED_COPYTEXT:
            case EXCPTEST_CHECKED_STATICTEXT:
            {   CU_ASSERT_TRUE_FATAL(is_exception(tdiobj));
                #if IS_DEBUG_CONSOLE 
                wprintf(L"\ncaught %s exception\n", tdiobj->result->exception->excptext);          
                #endif   
                break;          
            }
        }
        if (is_exception(tdiobj)) break;

        tdi = tdiobj->value_ptr_to; 
        CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);

        /* create array value, reading from test values */
        avobj = arrayvalue_read(tdi);
        /* if avobj is NULL we would throw an exception here */

        result = compare_lists(avobj);
        CU_ASSERT_EQUAL(result, TRUE); 

    } while(0);  

    /* clean up TDI */
    faketdi_close(tdiobj);

    /* destroy testdata list and content */
    arraylist_destroy(testdata, TRUE);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * read_test_integer
 */
void read_test_integer(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_read_test(); 
}


/* 
 * write_test_integer
 */
void write_test_integer(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_write_test(); 
}


/* 
 * read_test_string
 */
void read_test_string(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(2)) > 0)
        run_read_test(); 
}


/* 
 * write_test_string
 */
void write_test_string(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(2)) > 0)
        run_write_test(); 
}

/* 
 * exception_test_1
 */
void exception_test_1(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_exception_test(EXCPTEST_UNCHECKED_STATICTEXT); 
}


/* 
 * exception_test_2
 */
void exception_test_2(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_exception_test(EXCPTEST_CHECKED_COPYTEXT); 
}


/* 
 * exception_test_3
 */
void exception_test_3(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_exception_test(EXCPTEST_CHECKED_STATICTEXT); 
}


/* 
 * test_iterator_over_arraylist
 */
void test_iterator_over_arraylist(void)
{
    etch_iterator* iterator = NULL; 
    int testcount = 0;
    new_testdata(1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testdata);
    CU_ASSERT_NOT_EQUAL(testdata->count, 0);

    iterator = new_iterator(testdata, &testdata->iterable);

    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_EQUAL_FATAL(iterator->ordinal,1);
    testcount = 1;

    while(iterator->vtab->has_next(iterator))
          testcount += (iterator->vtab->next(iterator) == 0);  
        
    CU_ASSERT_EQUAL(testcount, testdata->count);

    etch_free(iterator);
    destroy_testdata();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */ 
}



/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{    
    CU_pSuite pSuite = 0; char c=0; 
    if (CUE_SUCCESS != CU_initialize_registry()) return -1;
    pSuite = CU_add_suite("suite_arrayvalue", suite_init, suite_cleanup);
    CU_set_output_filename("../test_arraylist");

    CU_add_test(pSuite, "test iterator over arraylist",   test_iterator_over_arraylist);
    CU_add_test(pSuite, "test array value read ints",     read_test_integer);
    CU_add_test(pSuite, "test array value read strings",  read_test_string);
    CU_add_test(pSuite, "test array value write ints",    write_test_integer);
    CU_add_test(pSuite, "test array value write strings", write_test_string);
    CU_add_test(pSuite, "test predefined exception",      exception_test_1);
    CU_add_test(pSuite, "test copy text exception",       exception_test_2);
    CU_add_test(pSuite, "test global text exception",     exception_test_3);


    if (argc > 1 && !wcscmp(argv[1], L"-a"))
        CU_automated_run_tests();
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    printf("any key ..."); while(!c) c = _getch(); wprintf(L"\n"); 
    CU_cleanup_registry();
    return CU_get_error(); 
}

