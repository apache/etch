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

#ifndef __ETCHDELIVERYSERVICE_H__
#define __ETCHDELIVERYSERVICE_H__

#include "serialization/EtchType.h"
#include "support/EtchMailbox.h"
#include "transport/EtchSessionMessage.h"
#include "transport/EtchTransportMessage.h"

/**
 * Adapter between remote and message source.
 */
class EtchDeliveryService : public EtchSessionMessage, public EtchTransportMessage
{
public:
  /**
   * Sends the message which begins a call sequence.
   *
   * @param msg The message to send.
   * @param result The mailbox which can be used to read the response
   * @return ETCH_OK if the message was send.
   *         ETCH_ERROR if there is a problem sending.
   */
  virtual status_t begincall(capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchMailbox>& result) = 0;

  /**
   * Finishes a call sequence by waiting for the response message.
   *
   * @param mb A mailbox which will be used to read the response
   * @param responseType The type of the expected response.
   * @param result The value of the response field if it isn't an exception.
   * @return ETCH_OK if the response message is successful received.
   *         ETCH_TIMEOUT if there is a timeout.
   */
  virtual status_t endcall(EtchMailbox* mb, EtchType* responseType, capu::SmartPointer<EtchObject>& result) = 0;
};

#endif
