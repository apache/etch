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
 * etch_url.c -- URL class, ported from etch java binding
 * only methods needed for the c binding were ported.
 */
#include "etch_url.h"
#include "etch_exception.h"
#include "etch_map.h"
#include "etch_log.h"
#include "etch_objecttypes.h"
#include "etch_general.h"
#include "etch_mem.h"
#include <wchar.h>

int      etchurl_parse      (etch_url*, wchar_t*);
int      etchurl_parseterms (etch_url*, wchar_t*);
int      etchurl_parseparams(etch_url*, wchar_t*);
void     etchurl_parsehost  (etch_url*, wchar_t*);
wchar_t* etchurl_unescape   (etch_url*, wchar_t*);
int      etchurl_is_emptystring(wchar_t*);
etch_arraylist* new_etchurl_paramlist();
etch_hashtable* new_etchurl_termmap();
etch_set* new_etch_term_consolidator_set();


/**
 * destroy_url()
 * etch_url destructor. 
 */
int destroy_url(void* data)
{
    etch_url* url = (etch_url*)data;
    if (!is_etchobj_static_content(url))
    {   if (url->raw)       etch_free(url->raw);
        if (url->scheme)    etch_free(url->scheme);
        if (url->user)      etch_free(url->user);
        if (url->password)  etch_free(url->password);
        if (url->fragment)  etch_free(url->fragment);
        if (url->host)      etch_free(url->host);
        if (url->uri)       etch_free(url->uri);

        etch_object_destroy(url->params);
        etch_object_destroy(url->terms);
    }

    return destroy_objectex((etch_object*)url);
}


/**
 * new_url()
 * etch_url constructor. 
 * @param urlstring the raw url string. caller retains ownership.
 */
etch_url* new_url(wchar_t* urlstring) 
{
    etch_url* newurl = (etch_url*) new_object(sizeof(etch_url), ETCHTYPEB_URL, CLASSID_URL);

    ((etch_object*)newurl)->destroy = destroy_url;  
    ((etch_object*)newurl)->clone   = clone_null;   

    newurl->raw = new_wchar(urlstring);
    etchurl_parse(newurl, urlstring);
 
    return newurl;
}




/**
 * etchurl_get_params()
 * @return an iterator over the URL parameters. caller owns and must destroy the iterator.
 */
etch_iterator* etchurl_get_params(etch_url* url)
{
    etch_iterator* iterator = url->params?
        new_iterator(url->params, &url->params->iterable):
        new_empty_iterator();

    return iterator;
}


/**
 * etchurl_paramcount()
 * @return count of parameters present in the URL
 */
int etchurl_paramcount(etch_url* url)
{
    return url->params? url->params->count: 0;
}


/**
 * etchurl_termcount()
 * @return count of terms present in the URL
 */
int etchurl_termcount(etch_url* url)
{
    return etchmap_count(url->terms);
}


/**
 * etchurl_parse()
 * parse out a raw URL
 */
int etchurl_parse(etch_url* thisx, wchar_t* rawurl)
{
    wchar_t *q = 0, *p = 0, *urlcopy = 0, *ss = 0; 
    int result = -1;
    if (NULL == rawurl) return -1;
    urlcopy = new_wchar(rawurl); p = urlcopy;

    do
    {   thisx->charcount = wcslen(p);
        thisx->bytecount = sizeof(wchar_t) * thisx->charcount; /* minus nullterm */
        if (0 == thisx->bytecount) break;
   
        /* scheme:[//[user[:password]@]host[:port]/]uri[;params][?terms][#frag] */
        ss = wcsstr(p, L"//");
        q  = wcschr(p, L':');

        if (!ss || !q)   
            thisx->scheme = new_wchar(ETCH_URL_DEFAULT_SCHEME);
        else
        if (q && (ss > q))
        {  *q++ = L'\0';
            thisx->scheme = etchurl_unescape(thisx, p); 
            p = q;  
        }
        else break;

        /* s is [//[user[:password]@]host[:port]/]uri[;params][?terms][#frag] */
        if (NULL != (q = wcschr(p, L'#')))  
        {  *q++ = L'\0';
            thisx->fragment = etchurl_unescape(thisx, q); 
        } 

        /* [//[user[:password]@]host[:port]/]uri[;params][?terms] */
        if (NULL != (q = wcschr(p, L'?')))  
        {  *q++ = L'\0';
            etchurl_parseterms(thisx, q); 
        } 

        /* [//[user[:password]@]host[:port]/]uri[;params] */
        if (NULL != (q = wcschr(p, L';')))  
        {  *q++ = L'\0';
            etchurl_parseparams(thisx, q); 
        } 

        /* [//[user[:password]@]host[:port]/]uri */
        if (p == wcsstr(p, L"//"))  /* begins with "//"? */   
        {   p++; p++; 
        }

        /* [user[:password]@]host[:port]/]uri */
        if (NULL != (q = wcschr(p, L'/'))) /* if slash found ... */
        {   /* [user[:password]@]host[:port]/uri */                
           *q++ = L'\0';
            etchurl_parsehost(thisx, p);   
            p = q;
        }
        else 
        {   /* [user[:password]@]host[:port] */
            etchurl_parsehost(thisx, p);   
            *p = L'\0';  /* p now empty string */      
        }   
     
        thisx->uri = etchurl_unescape(thisx, p);
  
        result = 0;

    } while(0);

    etch_free(urlcopy);
    return result;
}


/**
 * etchurl_parsepass()
 */
void etchurl_parsepass(etch_url* thisx, wchar_t* s)
{
    wchar_t *p = s, *q = 0;
    /* user[:password] */

    if  (NULL != (q = wcschr(p, L':'))) 
    {    *q++ = L'\0';
         thisx->password = etchurl_unescape(thisx, q);
    }

    thisx->user = etchurl_unescape(thisx, p);                
}


/**
 * etchurl_parseport()
 */
void etchurl_parseport(etch_url* thisx, wchar_t* s)
{
    wchar_t *p = s, *q = 0, *a = 0;
    /* host[:port] */

    if  (NULL != (q = wcschr(p, L':'))) 
    {    *q++ = L'\0';
         a = etchurl_unescape(thisx, q);
		 //thisx->port = _wtoi(a);
#ifdef WIN32
		thisx->port = _wtoi(a);
#else
		//TODO: check this
		thisx->port = wcstol(a, NULL, 10);
#endif
		 
         etch_free(a);
    }

    a = etchurl_unescape(thisx, p); 
    thisx->host = a;   
}


/**
 * etchurl_parsehost()
 */
void etchurl_parsehost(etch_url* thisx, wchar_t* s)
{
    wchar_t *p = s, *q = 0;

    /* [user[:password]@]host[:port] */
    if (NULL != (q = wcschr(p, L'@')))  
    {  *q++ = L'\0'; 
        etchurl_parsepass(thisx, p);
        etchurl_parseport(thisx, q);
    } 
    else etchurl_parseport(thisx, p);     
}


/**
 * etchurl_remove_term()
 * remove term from term map, destroying term key string and returning term value object.
 * @param key a raw string representing the key of the pair to be removed from the map.
 * caller retains ownership of this string.
 * @return the etch object value of the key/value pair removed from the map, or null.
 * caller assumes ownership of this object.
 */
etch_object* etchurl_remove_term(etch_url* url, wchar_t* key)
{
    int result = 0;
    etch_hashitem bucket, *removeditem = &bucket;
    const int keylen = (int) wcslen(key) * sizeof(wchar_t);
    memset(removeditem, 0, sizeof(etch_hashitem));

    result = ((struct i_hashtable*)((etch_object*)url->terms)->vtab)->remove(url->terms->realtable, 
									     key, keylen, url->terms, (void*)&removeditem);
                               
    etch_free(removeditem->key);    

    return removeditem->value;
}


/**
 * etchurl_add_term()
 * @param termname a disposable string, caller relinquishes ownership
 * @param termval  a disposable string, caller relinquishes ownership
 * @return 0 or -1
 */
int etchurl_add_term(etch_url* url, wchar_t* termname, wchar_t* termval)
{
    int result = 0;
    etch_string* newvalobj = new_stringw(termval);
    etch_object* foundvalobj = etchmap_findxw (url->terms, termname, NULL);

    /* zero is insertxw error return */
    if (NULL == foundvalobj) {
        result = etchmap_insertxw(url->terms, termname, newvalobj, FALSE)? 0: -1;
    }
    else
    if(is_etch_set(foundvalobj)) {
        /* -1 is set_add error return */
        result = etchmap_set_add((etch_set*)foundvalobj, (etch_object*) newvalobj)? -1: 0;
    }
    else {
        /* consolidate terms for this key */
        etch_set* thisset = new_etch_term_consolidator_set();  
        etch_object* removedvalobj = NULL;

        etchmap_set_add (thisset, foundvalobj);
        etchmap_set_add (thisset, (etch_object*) newvalobj);

        removedvalobj = etchurl_remove_term(url, termname);  
        ETCH_ASSERT(removedvalobj == foundvalobj); 
 
        /* replace the single term with the new term consolidator set */
        result = etchmap_insertxw (url->terms, termname, thisset, FALSE)? 0: -1;
    }

    etch_free(termname);
    etch_free(termval);
    return result;
}


/**
 * etchurl_add_double_term()
 * todo: remove this if we don't use it
 */
int etchurl_add_double_term(etch_url* url, wchar_t* termname, const double termval)
{
    wchar_t buf[128], *newstr = NULL;
    memset(buf, 0, sizeof(buf));
    etch_snwprintf(buf, 127, L"%f", termval);
    newstr = new_wchar(buf); /* we relinquish newstr to etchurl_add_term() */
    return etchurl_add_term(url, termname, newstr);
}


/**
 * etchurl_add_integer_term()
 */
int etchurl_add_integer_term(etch_url* url, wchar_t* termname, const int termval)
{
    wchar_t buf[36], *newstr = NULL;
    memset(buf, 0, sizeof(buf));
#ifdef WIN32
    _itow(termval, buf, 10);
#else
    etch_snwprintf(buf, sizeof(buf), L"%d", termval);
	//swprintf(buf, L"%d", termval);
#endif
    newstr = new_wchar(buf); /* we relinquish newstr to etchurl_add_term() */
    return etchurl_add_term(url, termname, newstr);
}


/**
 * etchurl_add_param()
 * @param param a disposable string, caller relinquishes 
 */
int etchurl_add_param(etch_url* url, wchar_t* param) 
{
    if (etchurl_is_emptystring(param)) 
    {   etch_free(param);
        return -1;
    }

    if (NULL == url->params)  /* lazy-allocate param map */
        url->params = new_etchurl_paramlist(); 
  
    return etch_arraylist_add (url->params, new_stringw(param));
}


/**
 * etchurl_unescape()
 * @param s a string from which escapes are to be removed. caller retains ownership.
 * @return a newly-allocated unescaped string for which caller assumes ownership.
 */
wchar_t* etchurl_unescape(etch_url* url, wchar_t* s)
{
    int n1, n2;
    wchar_t *p = s, *q = 0, *newstr = 0, c0, c1, c2;
    const size_t charlen = wcslen(s), bytecount = (charlen + 1) * sizeof(wchar_t);

    newstr = etch_malloc(bytecount, ETCHTYPEB_STRING);
    memset(newstr, 0, bytecount);
    q = newstr;

    while(*p)
    {
        switch(*p)
        {   case L'%':  
                 if (c1 = *++p) break;
                 if (c2 = *++p) break;
                 n1 = hexwchar_to_int(c1);
                 n2 = hexwchar_to_int(c2);
                 c0 = (n1 << 4) | n2;
                 *q++ = c0;
                 break;

            case L'+': *q++ = L' '; break;

            default:   *q++ = *p;
        }
        
        if (*p) ++p;
    }

    return newstr; /* caller owns */ 
}


/**
 * etchurl_parseterm()
 * @param s, a url string. caller retains ownership, however the string is modified herein.
 * s content is expected as name[=value] 
 */
int etchurl_parseterm(etch_url* url, wchar_t* s)
{
    wchar_t *p = s, *q = 0, *key = 0, *val = 0; 
    if (etchurl_is_emptystring(s)) return 0;

    if (NULL == url->terms)  /* lazy-allocate term map */
        url->terms = new_etchurl_termmap();  

    q = wcschr(p, L'=');
    if (q) *q = L'\0';

    key = etchurl_unescape(url, p);      /* acquire key memory */

    if(q && !etchurl_is_emptystring(++q)) {
        val = etchurl_unescape(url, q);  /* acquire val memory */
        etchurl_add_term(url, key, val); /* relinquish key&val */
    }
    else {
        /* relinquish key */
        etchurl_add_term(url, key, new_wchar(L""));
    }

    return 0;  
}


/**
 * etchurl_parseterms()
 * @param s a string containing delimited URL terms.
 * caller retains ownership of this string, however it is modified herein.
 */
int etchurl_parseterms(etch_url* url, wchar_t* s)
{
    wchar_t *p = s, *q = 0;

    while(1)
    {   if (NULL == (q = wcschr(p, L'&'))) break;
        *q = L'\0';
        etchurl_parseterm(url, p);
        p = ++q;
    }

    etchurl_parseterm(url, p); 

    return 0;  
}


/**
 * etchurl_parseparams()
 * @param s a string containing delimited URL parameters.
 * caller retains ownership of this string, however it is modified herein.
 */
int etchurl_parseparams(etch_url* url, wchar_t* s)
{
    wchar_t *p = s, *q = 0, *a = 0;
    if (etchurl_is_emptystring(s)) return 0;

    while(1)
    {   if (NULL == (q = wcschr(p, L';'))) break;
        *q = L'\0';
        a = etchurl_unescape(url, p);  /* acquire string a */
        etchurl_add_param(url, a);  /* relinquish string a */
        p = ++q;
    }

    if (!etchurl_is_emptystring(p))  /* clone final substring */
        etchurl_add_param(url, new_wchar(p));  

    return 0;  
}


/**
 * etchurl_get_integer_term()
 * returns integer value of specified query term in out parameter.
 * @return 0 success, -1 not found or error.
 */
int etchurl_get_integer_term(etch_url* url, const wchar_t* termname, int* out)
{
    int  result = 0;
    etch_string* foundobj = (etch_string*) etchurl_get_term(url, termname);
    if (!foundobj || !is_etch_string(foundobj) || !foundobj->char_count) return -1;
  
#ifdef WIN32
    result = _wtoi(foundobj->v.valw);
#else
	result = wcstol(foundobj->v.valw, NULL, 10);
#endif

    if(out) *out = result;
     
    return result;
}


/**
 * etchurl_get_boolean_term()
 * this is not yet functional: see etchurl_get_integer_term()
 */
boolean etchurl_get_boolean_term(etch_url* url, const wchar_t* termname, boolean* retval)
{
    int result = 0;
    etch_object* foundobj = etchurl_get_term(url, termname);
     
    if  (is_etch_boolean(foundobj))
         if (retval)
            *retval = ((etch_boolean*) foundobj)->value;
         else;
    else result = -1;

    return result;
}


/**
 * etchurl_get_term()
 * returns a *reference* to a wrapped URL term. caller does not own it.
 */
etch_object* etchurl_get_term(etch_url* url, const wchar_t* termname)
{
    etch_object* founditem = NULL;

    if (url->terms)
        founditem = etchmap_findxw (url->terms, (wchar_t*) termname, NULL);

    return founditem;
}


/**
 * etchurl_is_emptystring()
 */
int etchurl_is_emptystring(wchar_t* s)
{
    wchar_t* p = s;
    size_t charlen = s? wcslen(s): 0;
    if (0 == charlen) return TRUE;
    while(*p++) if (*p != L' ') return FALSE;
    return TRUE;
}


/* 
 * etchurl_termmap_clear_handler()
 * clear callback for term map.
 */
int etchurl_termmap_clear_handler (void* keyData, void* valueData)
{
    wchar_t* key = (wchar_t*)keyData;
    etch_object* value = (etch_object*)valueData;
    etch_free(key);  /* free string key */
    etch_object_destroy(value); /* free etch object value */
    return TRUE;
}


/* 
 * etchurl_termset_clear_handler()
 * clear callback for term consolidation set.
 * since it is a set, it does not contains values, only keys which are etch objects
 */
int etchurl_termset_clear_handler (void* data, void* value)  
{
    etch_object* key = (etch_object*)data;
    ETCH_ASSERT(key);
    etch_object_destroy(key);  
    return TRUE;
}


/* 
 * new_etch_term_consolidator_set()
 * constructor for a set configured appropriately for use as a term consolidator.
 * since it is a set, it does not contains values, only keys which are etch objects.
 * the new set is configured such that these objects are owned by the set 
 * and will therefore be destroyed with the set.
 */
etch_set* new_etch_term_consolidator_set()
{
    etch_set* thisset = new_set(ETCH_URL_DEFSUBTERMS);
    thisset->is_readonly_keys = FALSE;
    thisset->freehook = etchurl_termset_clear_handler;
    return thisset;
}


/*
 * new_etch_urlparamlist()
 * list of URL parameters. list entries are etch_string objects. the list is 
 * configured such that these objects are destroyed when the list is cleared.
 */
etch_arraylist* new_etchurl_paramlist()
{
    etch_arraylist* list = new_etch_arraylist(ETCH_URL_DEFNUMPARMS, 0);
    list->content_type   = ETCHARRAYLIST_CONTENT_OBJECT; 
    list->content_obj_type = ETCHTYPEB_PRIMITIVE;
    list->content_class_id = CLASSID_STRING;
    list->is_readonly = FALSE;
    return list;
}


/*
 * new_etch_urltermmap() 
 * map of URL term name/value pairs.
 * map keys are raw strings. map values are etch objects, which will be either
 * etch_string*, or etch_set*. such a set consolidates objects for a given key.
 * the map is configured such that content keys and values are destroyed when  
 * the map is destroyed.
 */
etch_hashtable* new_etchurl_termmap()
{
    etch_hashtable* terms   = new_hashtable(ETCH_URL_DEFNUMTERMS);
    terms->content_type     = ETCHHASHTABLE_CONTENT_OBJECT; 
    terms->content_obj_type = ETCHTYPEB_PRIMITIVE;
    terms->content_class_id = CLASSID_ANY;
    terms->is_readonly_keys = terms->is_readonly_values = FALSE;
    terms->freehook = etchurl_termmap_clear_handler;  /* frees memory on clear */
    return terms;
}


int is_url_scheme_http (etch_url* url) { return url && url->scheme && 0 == wcscmp(url->scheme, L"http"); }
int is_url_scheme_tcp  (etch_url* url) { return url && url->scheme && 0 == wcscmp(url->scheme, L"tcp"); }
int is_url_scheme_udp  (etch_url* url) { return url && url->scheme && 0 == wcscmp(url->scheme, L"udp");  }




