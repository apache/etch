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
 * etchutl.c -- utility methods
 */

/* todo include a dummy windows.h for linux build */
#include <windows.h>
#include "etchutl.h"
#include "etch_global.h"
#include "etch_encoding.h"
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

/**
 * get_map_keys()
 * return a collection of the specified map's keys.
 * caller must invoke the destructor on the returned list. the returned list 
 * is marked as readonly content, in order that arraylist_destroy() will not 
 * attempt to free memory for the list content, which remains owned by the map.
 */
etch_arraylist* get_map_keys(etch_hashtable* map)
{    
    etch_iterator iterator; 
    etch_arraylist* list = NULL;

    const int typecount = map->vtab->count(map->realtable,0,0);
    list = new_arraylist(typecount, 0);
    list->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    list->is_readonly  = TRUE; /* content is refs to objects owned by map */

    set_iterator(&iterator, map, &map->iterable);

    while(iterator.vtab->has_next(&iterator))
    {
        arraylist_add(list, iterator.current_key);
        iterator.vtab->next(&iterator);
    }

    return list;
}


/**
 * get_map_values()
 * return a collection of the specified map's values.
 * caller must invoke the destructor on the returned list. the returned list 
 * is marked as readonly content, in order that arraylist_destroy() will not 
 * attempt to free memory for the list content, which remains owned by the map.
 */
etch_arraylist* get_map_values(etch_hashtable* map)
{    
    etch_iterator iterator; 
    etch_arraylist* list = NULL;

    const int typecount = map->vtab->count(map->realtable,0,0);
    list = new_arraylist(typecount, 0);
    list->content_type = map->content_type;
    list->is_readonly  = TRUE;  /* content is refs to objects owned by map */

    set_iterator(&iterator, map, &map->iterable);

    while(iterator.vtab->has_next(&iterator))
    {
        arraylist_add(list, iterator.current_value);
        iterator.vtab->next(&iterator);
    }

    return list;
}


/**
 * hexchar_to_int()
 */
int hexchar_to_int (const unsigned char hexchar)
{
    switch(hexchar) 
    {   case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': case 'A': return 10;
        case 'b': case 'B': return 11;
        case 'c': case 'C': return 12;
        case 'd': case 'D': return 13;
        case 'e': case 'E': return 14;
        case 'f': case 'F': return 15;
    }
    return -1;
}


/**
 * hexwchar_to_int()
 */
int hexwchar_to_int (const wchar_t hexwchar)
{
    switch(hexwchar) 
    {   case L'0': return 0;
        case L'1': return 1;
        case L'2': return 2;
        case L'3': return 3;
        case L'4': return 4;
        case L'5': return 5;
        case L'6': return 6;
        case L'7': return 7;
        case L'8': return 8;
        case L'9': return 9;
        case L'a': case L'A': return 10;
        case L'b': case L'B': return 11;
        case L'c': case L'C': return 12;
        case L'd': case L'D': return 13;
        case L'e': case L'E': return 14;
        case L'f': case L'F': return 15;
    }
    return -1;
}


/**
 * is_etcharray()
 */
#if(0) /* this method is not needed yet so is left out of the compile */

int is_etcharray(objmask* obj, etch_arrayinfo* info)
{
    union_alltypes u;
    if (info) memset(info, 0, sizeof(etch_arrayinfo));
    if (NULL == obj) return FALSE;

    switch(obj->obj_type)
    {
        case ETCHTYPEB_ARRAYLIST:
        case ETCHTYPEB_ARRAYVALUE: 
        case ETCHTYPEB_NATIVEARRAY:
             break;
        default: return FALSE;
    }

    if (NULL == info) return TRUE;

    switch(obj->obj_type)
    {
        case ETCHTYPEB_ARRAYLIST:
             u.varraylist = (etch_arraylist*) obj;
             info->array_type = ETCH_ARRAYTYPE_LIST;
             info->is_content_object 
                = u.varraylist->content_type = ETCHARRAYLIST_CONTENT_OBJECT ;
             info->numdims = 1;
             info->numitems = u.varraylist->count;
             break;
             
        case ETCHTYPEB_ARRAYVALUE: 
             u.varrayval = (etch_arrayvalue*) obj;
             info->array_type = ETCH_ARRAYTYPE_ARRAYVAL;
             info->is_content_object = TRUE;
             info->numdims  = u.varrayval->dim;
             info->numitems = u.varrayval->length;
             break;

        case ETCHTYPEB_NATIVEARRAY:
             u.vnatarray = (etch_nativearray*) obj;
             info->array_type = ETCH_ARRAYTYPE_NATIVE;
             info->is_content_object = u.vnatarray->content_class_id == CLASSID_NONE;
             info->numdims  = u.vnatarray->numdims;
             info->numitems = (int) u.vnatarray->dimension[info->numdims];
             break;
    }

    return TRUE;
}

#endif /* this method is not needed yet so is left out of the compile */


/**
 * etch_existsfile()
 * determine if specified path exists on the file system.
 * @return boolean valued indicator.
 */
int etch_existsfile (char* path)
{
    struct stat fs; 
    return 0 == stat(path, &fs);
}


/**
 * etchlog_make_logfilepatha()
 * build and return a path to specified log file.
 * @return an etch_malloc'ed path string, caller owns it.
 */
char* etchlog_make_logfilepatha (char* dirpath, char* filename)
{                 
    const size_t pathlen = strlen(dirpath), namelen = strlen(filename);
    const size_t pathbuflen = pathlen + namelen + sizeof('/') + sizeof('\0');
    char* outpath = etch_malloc (pathbuflen, 0), *p = outpath;
    memcpy (p, dirpath, pathlen);  p += pathlen;
    *p++ = '/';
    memcpy (p, filename, namelen); p += namelen;
    *p = '\0'; 
    return outpath;  /* caller must etch_free */
}


/**
 * etchlog_make_logfilepathw()
 * build and return a path to specified log file.
 * @return an etch_malloc'ed path string, caller owns it.
 */
char* etchlog_make_logfilepathw (char* dirpath, wchar_t* filename)
{
    char* aname = NULL, *outpath = NULL;
    etch_unicode_to_utf8 (&aname, filename);
    outpath = etchlog_make_logfilepatha (dirpath, aname);
    etch_free (aname);
    return outpath;  /* caller must etch_free */
}


/**
 * etchlog_is_logfilew()
 * determine if specified path identifies a log file.
 */
int is_etch_logfilew (const wchar_t* filepath)
{
    int is_logfile = 0, extndx = 0;
    const wchar_t* ETCHLOG_WIDEEXT = L".log";
    const size_t extlen = wcslen(ETCHLOG_WIDEEXT);
    const int wstrlen = (int) wcslen (filepath);
    if (wstrlen < 1 || wstrlen > MAXPATHSIZE) return FALSE;
    extndx = (int) wstrlen - (int) extlen;  /* point at .log extension */
    is_logfile = (0 == wcscmp (&filepath[extndx], ETCHLOG_WIDEEXT));
    return is_logfile;
}


/**
 * etchlog_is_logfilea()
 * determine if specified path identifies a log file.
 */
int is_etch_logfilea (const char* filepath)
{
    int is_logfile = 0, extndx = 0;
    const char* ETCHLOG_LOGEXT = ".log";
    const size_t extlen = strlen(ETCHLOG_LOGEXT);
    const int astrlen = (int) strlen (filepath);
    if (astrlen < 1 || astrlen > MAXPATHSIZE) return FALSE;
    extndx = (int) astrlen - (int) extlen;  /* point at .log extension */
    is_logfile = (0 == strcmp (&filepath[extndx], ETCHLOG_LOGEXT));
    return is_logfile;
}



/**
 * etchlog_countfiles()
 * count log files in specified directory.
 * @param dirpath narrow directory string, may be relative, can't be root or net.
 * @return count of files in directory having .log file extension.
 * @remarks do not invoke this until after etch runtime is initialized, since 
 * encoding functions use etch_malloc(), which requires the global cache. 
 * @remarks this uses the linux/unix idiom for enumerating files in a directory.
 * etch emulations of those routines for windows are provided following.
 */
int etchlog_countfiles(char* dirpath)
{    
    int logfilecount = 0;
    DIR* dirstream = NULL;
    dirent* direntry = NULL;
    assert(is_runtime_initialized);

    dirstream = opendir(dirpath);

    while(1)
    {   if (NULL == (direntry = readdir (dirstream))) break;
        if (is_etch_logfilew (direntry->d_name))
            logfilecount++;
    }

    closedir(dirstream);  /* free DIR memory */
    return logfilecount;
}


/**
 * etchlog_purgefiles()
 * purge up tp specified number of log files from specified directory.
 * @param dirpath narrow directory string, can be relative, can't be root or net.
 * @param count max number of files to purge.
 * @param option not currently used.
 * @return count of files so purged.
 * @remarks do not invoke this until after etch runtime is initialized, since 
 * encoding functions use etch_malloc(), which requires the global cache. 
 * @remarks this uses the linux/unix idiom for enumerating files in a directory.
 * etch emulations of those routines for windows are provided following.
 */
int etchlog_purgefiles (char* dirpath, const int count, const int option)
{    
    int purgedcount = 0;
    DIR* dirstream = NULL;
    dirent* direntry = NULL;
    assert(is_runtime_initialized);
    /* TODO not sure if we will be dealing with narrow or wide paths in
     * linux dirent - if narrow, we'll need to modify code a bit here. 
     */
    dirstream = opendir(dirpath);

    while(purgedcount < count)
    {  
        if (NULL == (direntry = readdir (dirstream))) break;

        if (is_etch_logfilew (direntry->d_name)) 
        {   
            char* filepath = etchlog_make_logfilepathw (dirpath,  direntry->d_name);

            if (0 == etch_deletefile (filepath)) /* delete this log file */
                purgedcount++;

            etch_free (filepath);
        }
    }

    closedir(dirstream);  /* free DIR memory */
    return purgedcount;
}


#if IS_WINDOWS_ETCH
 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * etch windows emulation of POSIX directory browsing functions
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/* turn off potentially unsafe function warnings for strcpy strcat wcscpy etc */
#pragma warning (disable:4996)  

/*
 * opendir()
 * etch win32 limited emulation of POSIX opendir(). 
 * opens the specified "directory stream". the stream is positioned at the first entry. 
 * @param dirname the narrow character directory path. assumed not a root or net path.
 * @return a DIR*, or NULL if error, in which case system errno indicates the specific 
 * error. caller owns memory referenced by returned address, and is expected to
 * eventually relinquish this memory p by calling closedir(p).
 * @remarks closedir() must eventually be called on the returned DIR*, or the DIR will 
 * leak. it is assumed herein that _UNICODE is #defined ahead of windows.h, such that 
 * windows' FindFirstFile and FindNextFile operate on and return wide character strings.
 */
DIR* opendir (char* dirname)
{
    DIR* dir = NULL;
    int myerrno = 0;
    wchar_t* wdirname = NULL;
    const char* SLASHASTER = "/*";
    char *dircopy = NULL, *p = NULL;
    size_t namelen = dirname? strlen(dirname): 0;
    size_t dircopylen = 0, widestrbytelen = 0, dirstructlen = 0;
    struct stat st; memset(&st, 0, sizeof(struct stat));

    do {

    if (0 == namelen)
    {   myerrno = EINVAL;
        break;
    }

    /* clone caller's directory string */
    dircopylen = namelen + strlen(SLASHASTER) + sizeof('\0'); 
    dircopy = etch_malloc(dircopylen, 0);  
    memset(dircopy, 0, dircopylen);
    memcpy(dircopy, dirname, namelen);

    /* lose trailing slash if present */
    p = dircopy + namelen - 1;
    if (*p == '/' || *p == '\\') 
    {   *p = '/0';
        namelen--;
    } 

    /* verify specified directory exists */
    if (0 != stat (dircopy, &st) || 0 == (st.st_mode & S_IFDIR))
    {   myerrno = ENOTDIR;
        break;
    }

    /* add slash-asterisk expected by FindFirstFile() */
    strcat(dircopy, SLASHASTER);

    /* get an etch_malloc'ed unicode version of new path string 
     * since we're assuming _UNICODE was defined to windows.h */
    if (0 != etch_ansi_to_unicode (&wdirname, dircopy))
    {   myerrno = EIO;
        break;
    }

    /* allocate DIR big enough to contain unicode directory string */
    widestrbytelen = etch_get_unicode_bytecount (wdirname);
    dirstructlen = sizeof(DIR) + widestrbytelen;
    dir = etch_malloc (dirstructlen, 0);
    memset(dir, 0, dirstructlen);
    dir->dirhandle = INVALID_HANDLE_VALUE; /* first file indicator */
    wcscpy(dir->dirname, wdirname);

    } while(0);

    /* free work memory, fyi OK if null */
    etch_free(dircopy);
    etch_free(wdirname);
    errno = myerrno;

    return dir;  /* caller owns dir and must closedir() to free it */
}


/*
 * readdir()
 * etch win32 emulation of POSIX readdir().
 * gets the next file from the specified directory stream.
 * this implementation is not reentrant. caller does not own the memory 
 * referenced by the returned address.
 * @param dir the "directory stream" created by opendir(). caller retains.
 * @return a pointer to a structure representing the directory entry at the current 
 * position in the specified directory stream, and position the directory stream 
 * at the next entry; or NULL if no more entries in the directory stream. 
 * caller does not own this memory, function is not reentrant.
 * @remarks regarding type of entity referenced by returned dirent->d_name, 
 * at this writing we will only return files, not directories, since that is our
 * current need. not completely sure how this works in linux, presumably d_name
 * can be a file or a directory, and caller parses the name? not sure atm, but 
 * since we're writing this to scan the log directory and we have no need to 
 * recurse subdirectories, we can omit code to handle subdirectories for now. 
 */
dirent* readdir (DIR* dir)
{
    static WIN32_FIND_DATA fd;
    int is_gotfile = FALSE, myerrno = dir? 0: EBADF;

    while(!myerrno && !is_gotfile)
    {    
        if (dir->dirhandle == INVALID_HANDLE_VALUE) /* requesting first file in stream? */
            if (INVALID_HANDLE_VALUE != (dir->dirhandle = FindFirstFile(dir->dirname, &fd)))
                is_gotfile = TRUE;
            else;
        else
        if (FindNextFile (dir->dirhandle, &fd))
            is_gotfile = TRUE;

        if (is_gotfile)   /* no doubt there are more attributes we should test for here */
            if (fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN)) 
                is_gotfile = FALSE;
            else;
        else switch(GetLastError())
        {   case ERROR_NO_MORE_FILES: case ERROR_FILE_NOT_FOUND: case ERROR_PATH_NOT_FOUND:
                 myerrno = ENOENT;
                 break;
            default: myerrno = EINVAL;
        }
    } 

    errno = myerrno;
    return myerrno? NULL: (dirent*) &fd.cFileName;
} 


/*
 * closedir()
 * etch win32 emulation of POSIX function closedir().
 * @param dir the "directory stream" allocated by opendir(). this memory is freed here.
 * @return 0.
 * &remarks caller relinquishes memory referenced by dir parameter.
 */
int closedir (DIR* dir)
{
    if (dir && dir->dirhandle != INVALID_HANDLE_VALUE)
        FindClose (dir->dirhandle);

    etch_free (dir);
    return 0;
} 


/*
 * rewinddir()
 * etch win32 emulation of POSIX function rewinddir().
 * @param dir the "directory stream" allocated by opendir(). caller retains.
 * this "directory stream" is reset to the beginning, i.e. prior to first file.
 */
void rewinddir (DIR *dir)
{
    if (NULL == dir) return;
    if (dir->dirhandle && dir->dirhandle != INVALID_HANDLE_VALUE)
        FindClose(dir->dirhandle);
    dir->dirhandle = INVALID_HANDLE_VALUE;
} 


#endif /* IS_WINDOWS_ETCH */
