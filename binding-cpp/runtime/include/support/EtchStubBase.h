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

#ifndef __ETCHSTUBBASE_H__
#define __ETCHSTUBBASE_H__

#include "common/EtchTypes.h"
#include "support/EtchDeliveryService.h"

/**
* Base class of stub implementations.
* @param <T> The type of object used to implement stub.
*/
template<typename T>
class EtchStubBase {


protected:
  /**
   * The message source.
   */
  const capu::SmartPointer<EtchDeliveryService> svc;

  /**
   * The object used to implement stub.
   */
  const capu::SmartPointer<T> obj;

public:
  /**
   * Constructs the StubBase.
   * @param svc the message source.
   * @param obj the target of decoded messages.
   */
  EtchStubBase(capu::SmartPointer<EtchDeliveryService> svc, capu::SmartPointer<T> obj);

  status_t sessionMessage(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg, capu::bool_t &result );

private:
    /**
     * A PoolRunnable used by the stub to dispatch asynchronous
     * receiver methods from messages.
     */
    class EtchStubPoolRunnable //: EtchPoolRunnable
    {
      /**
       * Constructs the StubPoolRunnable.
       * @param sender the sender of the message.
       * @param msg the message.
       * @param helper the StubHelper which will dispatch the message.
       */
      /*public StubPoolRunnable( Who sender, Message msg, StubHelper<T> helper )
      {
        this.sender = sender;
        this.msg = msg;
        this.helper = helper;
      }

      private final Who sender;

      private final Message msg;

      private final StubHelper<T> helper;

      public void exception( Exception e )
      {
        try
        {
          sessionNotify( e );
        }
        catch ( Exception e1 )
        {
          e1.printStackTrace();
        }
      }

      public void run() throws Exception
      {
        helper.run( svc, obj, sender, msg );
      }*/
    };
};

#endif
