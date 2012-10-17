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

#ifndef __ETCHTRANSPORTDATA_H__
#define __ETCHTRANSPORTDATA_H__

#include "transport/EtchSessionData.h"
#include "transport/EtchTransport.h"
#include "transport/EtchFlexBuffer.h"
#include "transport/EtchWho.h"

class EtchTransportData : public virtual EtchTransport<EtchSessionData> {

public:

  /**
   * Delivers the data to the transport.
   * @param recipient specifies the recipient when there is the possibility
   * of more than one.
   * @param buf the FlexBuffer positioned on the data.
   * @return ETCH_OK if the sent is successful
   *         ETCH_EINVAL if the buffer is NULL
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  virtual status_t transportData(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchFlexBuffer> buf) = 0;

  /**
   * Destructor
   */
  virtual ~EtchTransportData() {

  }

};

#endif /* ETCHTRANSPORTDATA_H */

