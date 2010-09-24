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
 * etch_sessionint.h
 * session interface
 */
#ifndef ETCHISESSION_H
#define ETCHISESSION_H

#include "etch_object.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char*   ETCHDSIF;
#define ETCHEVT_SESSION_UP   0x80
#define ETCHEVT_SESSION_DOWN 0x81

/*
 * the memory ownership contracts for control, notify, and query, are:
 * caller relinquishes all arguments except of course the this pointer,
 * regardless of outcome. it follows that only simple and temporal
 * objects are passed as parameters.
 */
typedef int   (*etch_session_control)(void* thisx, etch_event* evt, etch_object* value);
typedef int   (*etch_session_notify) (void* thisx, etch_event* evt);
typedef etch_object* (*etch_session_query)  (void* thisx, etch_query* query);

/**
 * i_session
 * session interface
 * not an etch object, ergo no ->desroy(), no ->clone()
 */
typedef struct i_session
{
    etch_session_control session_control;
    etch_session_notify  session_notify;
    etch_session_query   session_query;

    void* thisx;

} i_session;

/**
 * i_session_mask
 * mask over any implemented session interface
 */
typedef struct i_session_mask
{
    etch_object object;

    void* thisx; /* object which is the interface implementor */

    /* session interface */
    i_session* isession;
    etch_session_control session_control;
    etch_session_notify  session_notify;
    etch_session_query   session_query;

    void* main_session_method;

} i_session_mask;

i_session* new_default_session_interface(void* thisx);
i_session* new_session_interface(void* thisx, etch_session_control sc, etch_session_notify sn, etch_session_query sq);
i_session* clone_session(void* thisx, const i_session*);

/**
 * i_objsession
 * obj_session interface
 * same interface, names changed to avoid collisions.
 * not determined whether this is necessary in the c binding.
 * not an etch object, ergo no ->destroy(), no ->clone()
 */
typedef struct i_objsession
{
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

    void* thisx;

} i_objsession;

/**
 * etch_objsession_objinfo
 * session methods parameter info struct
 */
typedef struct etch_objsession_objinfo
{
  etch_object*  obj;
  etch_object*  resobj;
  etch_object*  msg;
  etch_who* whofrom;
  char*     msg_aname;
  etch_exception*     exception;
  unsigned short obj_type;
  unsigned short class_id;
  unsigned char  is_message;
  unsigned char  is_exception;

} etch_objsession_objinfo;

i_objsession* new_default_objsession_interface(void* thisx);
i_objsession* new_objsession_interface(void* thisx, etch_session_control sc, etch_session_notify sn, etch_session_query sq);
void etchsession_get_objinfo(etch_objsession_objinfo* objinfo, void* evt);

#ifdef __cplusplus
}
#endif

#endif /* ETCHISESSION_H */
