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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "support/EtchPlainMailbox.h"
#include "common/EtchList.h"

class MockMailboxManager2 : public virtual EtchMailboxManager {
public:

  MockMailboxManager2() {
    unregistered = false;
  }
  capu::bool_t unregistered;
  EtchList<EtchMailbox::EtchElement *> list;

  virtual status_t unregisterMailbox(EtchLong mailboxId) {
    unregistered = true;
    return ETCH_OK;
  }

  status_t redeliver(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg) {
    return list.add(new EtchMailbox::EtchElement(sender, msg));
  }

  MOCK_METHOD2(getMailbox, status_t(EtchLong msgid, capu::SmartPointer<EtchMailbox>& result));

  MOCK_METHOD2(sessionMessage, status_t(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg));

  MOCK_METHOD3(transportCall, status_t(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchMailbox>& result));

  MOCK_METHOD2(transportMessage, status_t(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> message));

  MOCK_METHOD2(transportQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result));

  MOCK_METHOD2(transportControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));

  MOCK_METHOD1(transportNotify, status_t(capu::SmartPointer<EtchObject> event));

  MOCK_METHOD0(getSession, EtchSessionMessage* ());

  MOCK_METHOD1(setSession, void(EtchSessionMessage* session));

  MOCK_METHOD2(sessionQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(sessionControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));

  MOCK_METHOD1(sessionNotify, status_t(capu::SmartPointer<EtchObject> event));

  virtual ~MockMailboxManager2() {
    EtchMailbox::EtchElement *el;
    EtchList<EtchMailbox::EtchElement*>::Iterator it = list.begin();
    while (it.hasNext()) {
      it.next(&el);
      delete el;
    }
  }
};

TEST(EtchPlainMessageBoxTest, constructorTest) {
  MockMailboxManager2 manager;
  capu::SmartPointer<EtchMailbox> mailbox = NULL;
  EtchLong id(5);
  mailbox = new EtchPlainMailbox(&manager, id);
  EXPECT_TRUE(mailbox.get() != NULL);
}

TEST(EtchPlainMessageBoxTest, closeDeliveryTest) {
  MockMailboxManager2* manager = new MockMailboxManager2();

  EtchMailbox* mailbox = NULL;
  EtchLong id(5);
  mailbox = new EtchPlainMailbox(manager, id);

  //initial configuration
  EXPECT_TRUE(true == mailbox->isEmpty());
  EXPECT_TRUE(false == mailbox->isFull());
  EXPECT_TRUE(false == mailbox->isClosed());
  EXPECT_TRUE(false == manager->unregistered);

  mailbox->message(NULL, NULL);

  mailbox->closeDelivery();
  //configuration after closing
  EXPECT_TRUE(false == mailbox->isEmpty());
  EXPECT_TRUE(true == mailbox->isFull());
  EXPECT_TRUE(true == mailbox->isClosed());
  EXPECT_TRUE(true == manager->unregistered);

  delete mailbox;
  delete manager;
}

TEST(EtchPlainMessageBoxTest, closeReadTest) {
  MockMailboxManager2* manager = new MockMailboxManager2();

  EtchMailbox* mailbox = NULL;
  EtchLong id(5);
  mailbox = new EtchPlainMailbox(manager, id);

  //initial configuration
  EXPECT_TRUE(true == mailbox->isEmpty());
  EXPECT_TRUE(false == mailbox->isFull());
  EXPECT_TRUE(false == mailbox->isClosed());
  EXPECT_TRUE(false == manager->unregistered);

  mailbox->message(NULL, NULL);


  EXPECT_TRUE(ETCH_OK == mailbox->closeRead());

  //configuration after closing
  EXPECT_TRUE(true == mailbox->isEmpty());
  EXPECT_TRUE(false == mailbox->isFull());
  EXPECT_TRUE(true == mailbox->isClosed());
  EXPECT_TRUE(true == manager->unregistered);
  EXPECT_EQ(1, manager->list.size());

  delete mailbox;
  delete manager;
}

TEST(EtchPlainMessageBoxTest, fullTest) {
  MockMailboxManager2* manager = new MockMailboxManager2();

  EtchMailbox* mailbox = NULL;
  EtchLong id(5);
  mailbox = new EtchPlainMailbox(manager, id);

  //initial configuration
  EXPECT_TRUE(true == mailbox->isEmpty());
  EXPECT_TRUE(false == mailbox->isFull());
  EXPECT_TRUE(false == mailbox->isClosed());
  EXPECT_TRUE(false == manager->unregistered);

  mailbox->message(NULL, NULL);

  //configuration after closing
  EXPECT_TRUE(false == mailbox->isEmpty());
  EXPECT_TRUE(true == mailbox->isFull());
  EXPECT_TRUE(false == mailbox->isClosed());
  EXPECT_TRUE(false == manager->unregistered);

  //mailbox is full
  EXPECT_TRUE(mailbox->isFull());

  //not handled message because it mailbox is full
  mailbox->message(NULL, NULL);
  EXPECT_TRUE(false == mailbox->isEmpty());
  EXPECT_TRUE(true == mailbox->isFull());
  EXPECT_TRUE(false == mailbox->isClosed());
  EXPECT_TRUE(false == manager->unregistered);

  delete mailbox;
  delete manager;
}

TEST(EtchPlainMessageBoxTest, readTest) {
  MockMailboxManager2* manager = new MockMailboxManager2();

  EtchMailbox* mailbox = NULL;
  EtchLong id(5);
  mailbox = new EtchPlainMailbox(manager, id);

  //initial configuration
  EXPECT_TRUE(true == mailbox->isEmpty());
  EXPECT_TRUE(false == mailbox->isFull());
  EXPECT_TRUE(false == mailbox->isClosed());
  EXPECT_TRUE(false == manager->unregistered);

  mailbox->message(NULL, NULL);

  mailbox->closeDelivery();

  //configuration after closing
  EXPECT_TRUE(false == mailbox->isEmpty());
  EXPECT_TRUE(true == mailbox->isFull());
  EXPECT_TRUE(true == mailbox->isClosed());
  EXPECT_TRUE(true == manager->unregistered);

  //try to read now
  EtchMailbox::EtchElement* element;
  //there is one element in mailbox
  EXPECT_TRUE(mailbox->read(element) == ETCH_OK);
  delete element;

  //now it is empty
  EXPECT_TRUE(mailbox->read(element) == ETCH_ERROR);
  delete mailbox;
  delete manager;
}
