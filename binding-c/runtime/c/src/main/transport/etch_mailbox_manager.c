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
 * etch_mailboxmgr.c
 * i_mailbox manager interface
 */

#include "etch_mailbox_manager.h"
#include "etch_message.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
/*
static const char* LOG_CATEGORY = "etch_mailbox_manager";
*/

int destroy_mailbox_manager_interface(void*);
int etchmailboxmgr_def_headersize(i_mailbox_manager*);
int etchmailboxmgr_def_transport_call(i_mailbox_manager*, etch_who*, etch_message*, i_mailbox**);
int etchmailboxmgr_def_redeliver (void*, etch_who*, etch_message*);
int etchmailboxmgr_def_unregister(void*, i_mailbox*);


/**
 * new_mailboxmgr_interface()
 * i_mailbox_manager constructor.
 * @param thisx the mailbox manager object.
 * @param itm transportmesssage interface, caller retains, can be null.
 * @param ism sessionmessage interface, caller retains, can be null.
 */
i_mailbox_manager* new_mailboxmgr_interface(void* thisx, 
    i_transportmessage* itm, i_sessionmessage* ism)  
{
    i_mailbox_manager* newi = (i_mailbox_manager*) new_object(sizeof(i_mailbox_manager), ETCHTYPEB_MBOX_MANAGER, CLASSID_MBOX_MANAGER);

    newi->thisx   = thisx;
    ((etch_object*)newi)->clone   = clone_null;
    ((etch_object*)newi)->destroy = destroy_mailbox_manager_interface;  

    newi->transport_call = (etch_mbm_transport_call)etchmailboxmgr_def_transport_call;
    newi->redeliver      = etchmailboxmgr_def_redeliver;
    newi->unregister     = etchmailboxmgr_def_unregister;

    newi->itm = itm? itm: new_transportmsg_interface(newi, NULL, NULL); 
    newi->transport_message = newi->itm->transport_message;
    newi->transport_message = newi->itm->transport_message;
    newi->transport_control = newi->itm->transport_control;
    newi->transport_notify  = newi->itm->transport_notify;
    newi->transport_query   = newi->itm->transport_query;
    newi->get_session       = newi->itm->get_session;
    newi->set_session       = newi->itm->set_session;

    return newi;
}


/**
 * destroy_mailbox_manager_interface()
 * i_mailbox_manager destructor
 */
int destroy_mailbox_manager_interface(void* data)
{
  i_mailbox_manager* mgr = (i_mailbox_manager*)data;
    if (NULL == mgr) return -1;

    if (!is_etchobj_static_content(mgr))
    {    
        etch_object_destroy(mgr->ism);

        etch_object_destroy(mgr->itm);
    }
            
    return destroy_objectex((etch_object*)mgr);
}


/**
 * etchmailboxmgr_def_transport_call()
 * default implementation of i_mailbox_manager::transport_call() 
 */
int etchmailboxmgr_def_transport_call(i_mailbox_manager* imbm, etch_who* whoto, etch_message* msg, i_mailbox** out)
{
    return -1;
} 


/**
 * etchmailboxmgr_def_redeliver()
 * default implementation of i_mailbox_manager::redeliver() 
 */
int etchmailboxmgr_def_redeliver(void* mailbox, etch_who* whofrom, etch_message* msg)
{
    return -1;
}


/**
 * etchmailboxmgr_def_unregister()
 * default implementation of i_mailbox_manager::unregister() 
 */
int etchmailboxmgr_def_unregister(void* imbm, i_mailbox* mbox)
{
    return -1;
}

