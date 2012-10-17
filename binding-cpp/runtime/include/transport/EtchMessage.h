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


#ifndef __ETCHMESSAGE_H__
#define __ETCHMESSAGE_H__

#include "serialization/EtchType.h"
#include "serialization/EtchValueFactory.h"
#include "serialization/EtchStructValue.h"

class EtchMessage : public EtchStructValue {
public:

  /**
   * Constructor
   */
  EtchMessage(EtchType *type, EtchValueFactory* vf);

  /**
   * Constructor
   */
  EtchMessage(EtchType *type, EtchValueFactory* vf, capu::uint32_t length);

  /**
   * Copy Constructor
   */
  EtchMessage(const EtchMessage& other);

  /**
   * Destructor
   */
  virtual ~EtchMessage();

  /**
   * @return the value factory.
   */
  EtchValueFactory* getValueFactory();

  /**
   * Creates a message which is a reply to the current message.
   * The current message's value factory is copied to the new
   * message. The message-id of the current message (if any) is
   * copied into the in-reply-to field of the new message.
   * @param rType the type of the reply.
   * @param a reply message.
   * @return ETCH_OK if reply is created
   *         ETCH_EINVAL if rType is null
   *         ETCH_ERROR otherwise
   */
  status_t createReplyMessage(EtchType* rType, capu::SmartPointer<EtchMessage> &message);

  /**
   *
   * @param a reply message.
   * @return ETCH_OK if reply is created
   *         ETCH_ERROR otherwise
   */
  status_t createReplyMessage(capu::SmartPointer<EtchMessage> &message);

  /**
   * @param the connection specific unique identifier of this
   * message, or null if there was no such identifier.
   */
  status_t getMessageId(capu::int64_t &result);

  /**
   * Sets the message-id field of this message.
   * @param msgid the connection specific unique identifier of this
   * message. Null if the message has not been sent yet. NOTE: the
   * send process overwrites any value the user might set here. So
   * don't bother trying to set it.
   */
  status_t setMessageId(capu::int64_t msgid);

  /**
   * @param the message-id of the message that this is a response to.
   * Null if this is an original message or if the original message did
   * not have a message-id.
   */
  status_t getInReplyToMessageId(capu::int64_t &result);

  /**
   * Sets the in-reply-to field of this message.
   * @param msgid the message-id of the message that this is a response to.
   */
  void setInReplyToMessageId(capu::int64_t msgid);

private:
  EtchValueFactory* mVf;
};

typedef capu::SmartPointer<EtchMessage> EtchMessagePtr;

#endif /* ETCHMESSAGE_H */

