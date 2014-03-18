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

#ifndef __ETCHPLAINMAILBOXMANAGER_H__
#define __ETCHPLAINMAILBOXMANAGER_H__

#include "capu/os/Mutex.h"

#include "common/EtchString.h"
#include "transport/EtchMailboxManager.h"
#include "support/EtchPlainMailbox.h"
#include "util/EtchURL.h"
#include "util/EtchResources.h"
#include "util/EtchIdGenerator.h"

class EtchRuntime;

class EtchPlainMailboxManager : public EtchMailboxManager {
public:
  /**
   * @param transport used to deliver messages to our peer.
   * @param uri
   * @param resources
   */
  EtchPlainMailboxManager(EtchRuntime* runtime, EtchTransportMessage* transport, const EtchString& url, EtchResources* resources);

  /**
   * Destructure
   */
  virtual ~EtchPlainMailboxManager();

  /**
   * @return the transport.
   */
  EtchTransportMessage* getTransport();

  /**
   * @see EtchMailboxManager
   */
  status_t redeliver(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg);

  /**
   * Adds a mailbox to the set of mailbox receiving responses
   * to messages.
   * @param mb
   */
  status_t registerMailbox(capu::SmartPointer<EtchMailbox> mb);

  /**
   * @see EtchMailboxManager
   */
  status_t unregisterMailbox(EtchLong mailboxId);

  /**
   * Returns the mailbox for the specified msgid. This is a testing api.
   * @param msgid
   * @return the mailbox for the specified msgid.
   */
  status_t getMailbox(EtchLong msgid, capu::SmartPointer<EtchMailbox>& result);

  ////////////////////////////
  // MessageHandler methods //
  ////////////////////////////
  status_t sessionMessage(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg);

  ///////////////////////////
  // MessageSource methods //
  ///////////////////////////
  status_t transportCall(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchMailbox>& result);
  status_t transportMessage(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> message);

  ///////////////////////////
  // SourceHandler methods //
  ///////////////////////////

  status_t sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result);
  status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);
  status_t sessionNotify(capu::SmartPointer<EtchObject> event);

  ////////////////////
  // Source methods //
  ////////////////////
  status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result);
  status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);
  status_t transportNotify(capu::SmartPointer<EtchObject> event);
  EtchSessionMessage* getSession();
  void setSession(EtchSessionMessage* session);

  capu::uint64_t count();


private:
  EtchRuntime* mRuntime;
  EtchSessionMessage* mSession;
  EtchTransportMessage* mTransport;
  capu::bool_t mUp;
  EtchHashTable<EtchLong, capu::SmartPointer<EtchMailbox> > mMailboxes;
  capu::Mutex mMailboxesMutex;
  EtchIdGenerator mIdGen;
};

#endif
