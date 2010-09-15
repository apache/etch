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
 * etch_mailboxint.c
 * mailbox interface
 */

#include "etch_mailbox.h"
#include "etch_plain_mailbox.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"

/*
static const char* LOG_CATEGORY = "etch_mailbox";
*/

int etchmbi_def_message(void* thisx, etch_who* whofrom, etch_message* msg);
int etchmbi_def_read   (void* thisx, etch_mailbox_element** out);
int etchmbi_def_read_withwait (void* thisx, const int maxdelay, etch_mailbox_element** out);
int etchmbi_def_close_delivery (void* thisx);
int etchmbi_def_close_read (void* thisx);
int etchmbi_def_register_notify (void* thisx, etch_mailbox_notify, etch_object* state, const int maxdelay);
int etchmbi_def_unregister_notify (void* thisx, etch_mailbox_notify);
int etchmbi_def_is_empty  (void* thisx);
int etchmbi_def_is_closed (void* thisx);
int etchmbi_def_is_full   (void* thisx);
int64 etchmbi_def_get_message_id (void* thisx);
int destroy_mailbox_interface(void*);


/**
 * new_default_mailbox_interface
 * return a mailbox interface populated with defaults for virtuals.
 */
i_mailbox* new_default_mailbox_interface(void* thisx)
{
    i_mailbox* newi = (i_mailbox*) etch_malloc(sizeof(i_mailbox), ETCHTYPEB_RAWOBJECT); 

    newi->thisx = thisx;
    newi->destroy = destroy_mailbox_interface;
    newi->mailbox = etchmbox_get_implobj;
    newi->manager = etchmbox_get_manager;

    newi->message           = etchmbi_def_message;
    newi->read              = etchmbi_def_read;
    newi->read_withwait     = etchmbi_def_read_withwait;
    newi->close_delivery    = etchmbi_def_close_delivery;
    newi->close_read        = etchmbi_def_close_read;
    newi->register_notify   = etchmbi_def_register_notify;
    newi->unregister_notify = etchmbi_def_unregister_notify;
    newi->is_empty          = etchmbi_def_is_empty;
    newi->is_closed         = etchmbi_def_is_closed;
    newi->is_full           = etchmbi_def_is_full;
    newi->get_message_id    = etchmbi_def_get_message_id;

    return newi;    
}


/**
 * new_mailbox_interface
 * return a mailbox interface populated with specified virtuals.
 */
i_mailbox* new_mailbox_interface(void* thisx,
    etch_mailbox_message mbm,
    etch_mailbox_read    mbr,
    etch_mailbox_read_withwait     mbrd,
    etch_mailbox_close_delivery    mbcd,
    etch_mailbox_close_read        mbcr,
    etch_mailbox_register_notify   mbrn,
    etch_mailbox_unregister_notify mbun,
    etch_mailbox_is_empty  mbise,
    etch_mailbox_is_closed mbisc,
    etch_mailbox_is_full   mbisf,
    etch_mailbox_get_message_id    mbgmi) 
{
    i_mailbox* newi = (i_mailbox*) new_object
       (sizeof(i_mailbox), ETCHTYPEB_MAILBOXINT, CLASSID_MAILBOXINT);

    newi->thisx = thisx;
    newi->destroy = destroy_mailbox_interface;
    newi->mailbox = etchmbox_get_implobj;
    newi->manager = etchmbox_get_manager;

    newi->message           = mbm?   mbm:   etchmbi_def_message;
    newi->read              = mbr?   mbr:   etchmbi_def_read;
    newi->read_withwait     = mbrd?  mbrd:  etchmbi_def_read_withwait;
    newi->close_delivery    = mbcd?  mbcd:  etchmbi_def_close_delivery;
    newi->close_read        = mbcr?  mbcr:  etchmbi_def_close_read;
    newi->register_notify   = mbrn?  mbrn:  etchmbi_def_register_notify;
    newi->unregister_notify = mbun?  mbun:  etchmbi_def_unregister_notify;
    newi->is_empty          = mbise? mbise: etchmbi_def_is_empty;
    newi->is_closed         = mbisc? mbisc: etchmbi_def_is_closed;
    newi->is_full           = mbisf? mbisf: etchmbi_def_is_full;
    newi->get_message_id    = mbgmi? mbgmi: etchmbi_def_get_message_id;

    return newi;    
}


/**
 * destroy_mailbox_interface()
 * i_mailbox destructor
 */
int destroy_mailbox_interface (void* data)
{
  i_mailbox* mb = (i_mailbox*)data;
    int result = 0;
    if (mb && !is_etchobj_static_content(mb))
    {
        i_mailbox* temp = mb->thisx; //destroying thisx will also destroy mb...
	//        ETCHOBJ_DESTROY();	
	if(temp)
	  ((etch_object*)temp)->destroy((etch_object*)temp);
	temp = NULL;

    }
    else result = -1;
    return result;
}  


/**
 * etch_mailbox_get_implobj()
 * verify and return the concrete mailbox implementation
 */
etch_mailbox* etchmbox_get_implobj(i_mailbox* imb) 
{
    etch_mailbox* mbox = imb? imb->thisx: NULL;
    return is_etch_mailbox(mbox)? mbox: NULL;
}


/**
 * etch_mailbox_get_manager()
 * verify and return the mailbox's mailbox manager
 */
i_mailbox_manager* etchmbox_get_manager(i_mailbox* imb)
{
    etch_mailbox* mbox = imb? imb->thisx: NULL;
    i_mailbox_manager* mgr = mbox? mbox->manager: NULL;
    return is_etch_mailboxmgr(mgr)? mgr: NULL;
} 


int etchmbi_def_message(void* thisx, etch_who* whofrom, etch_message* msg)
{
    return -1;
}


int etchmbi_def_read   (void* thisx, etch_mailbox_element** out)
{
    return -1;
}


int etchmbi_def_read_withwait (void* thisx, const int maxdelay, etch_mailbox_element** out)
{
    return -1;
}


int etchmbi_def_close_delivery (void* thisx)
{
    return -1;
}


int etchmbi_def_close_read (void* thisx)
{
    return -1;
}


int etchmbi_def_register_notify (void* thisx, etch_mailbox_notify fn, etch_object* state, const int maxdelay)
{
    return -1;
}

int etchmbi_def_unregister_notify (void* thisx, etch_mailbox_notify fn)
{
    return -1;
}


int etchmbi_def_is_empty  (void* thisx) 
{
    return FALSE;
}


int etchmbi_def_is_closed (void* thisx)
{
    return FALSE;
}


int etchmbi_def_is_full   (void* thisx)
{
    return FALSE;
}


int64 etchmbi_def_get_message_id (void* thisx)
{
    return 0;
}


/* - - - - - - - - - - - - - - - - 
 * etch_mailbox_element
 * - - - - - - - - - - - - - - - -
 */

/**
 * destroy_mailbox_element()
 * etch_mailbox_element destructor
 */
int destroy_mailbox_element(void* data)
{
    etch_mailbox_element* thisx = (etch_mailbox_element*)data;

    if (!is_etchobj_static_content(thisx))
    {
        etch_object_destroy(thisx->msg);
        thisx->msg = NULL;
    }

   return destroy_objectex((etch_object*) thisx);
}


/**
 * new_mailbox_element()
 * etch_mailbox_element constructor
 * @param msg todo document ownership
 * @param whofrom todo document ownership
 */
etch_mailbox_element* new_mailbox_element(etch_message* msg, etch_who* who)
{
    etch_mailbox_element* elt = (etch_mailbox_element*) new_object
        (sizeof(etch_mailbox_element), ETCHTYPEB_MBOX_ELEMENT, CLASSID_MBOX_ELEMENT); 
    elt->msg     = msg;
    elt->whofrom = who;
    elt->destroy = destroy_mailbox_element;
    return elt;
}
