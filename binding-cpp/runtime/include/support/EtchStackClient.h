/* $Id$
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

#ifndef __ETCHSTACKCLIENT_H__
#define __ETCHSTACKCLIENT_H__

#include "support/EtchStack.h"
#include "util/EtchResources.h"

class EtchRuntime;

/* *
 * This class serves as a container for all layers created during the setup of a new stack (connection, packetizer, messagizer, ...).
 */
class EtchStackClient : public EtchStack {
public:

  /**
   * EtchObjectType for EtchStackClient.
   */
  static const EtchObjectType* TYPE();

  /**
   * Default Constructor
   */
  EtchStackClient(EtchRuntime* runtime);

  /**
   * Copy Constructor
   */
  EtchStackClient(const EtchStackClient& other);

  /**
   * Destructor which cleans the whole stack
   */
  virtual ~EtchStackClient();

public:
  EtchRuntime* mRuntime;
  EtchResources* mStaticResources;
};

#endif /* __ETCHSTACKCLIENT_H__ */
