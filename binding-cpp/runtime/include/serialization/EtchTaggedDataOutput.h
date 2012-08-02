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


#ifndef __ETCHTAGGEDDATAOUTPUT_H__
#define __ETCHTAGGEDDATAOUTPUT_H__
#include "transport/EtchFlexBuffer.h"
#include "transport/EtchMessage.h"

class EtchTaggedDataOutput {
public:

  /**
   * Writes the message to the buf.
   * @param msg the message to be written.
   * @param buf the buffer to write to.
   */
  virtual status_t writeMessage(capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchFlexBuffer> buf) = 0;

  virtual ~EtchTaggedDataOutput() {

  }

};


#endif /* ETCHTAGGEDDATAOUTPUT_H */

