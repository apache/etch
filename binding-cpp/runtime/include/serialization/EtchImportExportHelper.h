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

#ifndef __ETCHIMPORTEXPORTHELPER_H__
#define __ETCHIMPORTEXPORTHELPER_H__
#include "serialization/EtchValueFactory.h"
#include "support/EtchRuntime.h"

class EtchStructValue;

/**
 * Interface used to implement ValueFactory based import / export code
 * for service defined types.
 */
class EtchImportExportHelper {
public:

  /**
   * Constructor
   */
  EtchImportExportHelper(EtchRuntime* runtime) : mRuntime(runtime) {
  }

  /**
   * Destructor
   */
  virtual ~EtchImportExportHelper() {
  }

  /**
   * Exports a value by creating an equivalent StructValue.
   * @param vf the value factory for this connection.
   * @param value a service defined type to export.
   * @param result as out parameter where the exported value will be stored.
   * @return ETCH_OK if successfully exported
   *         ETCH_EINVAL if it is locked
   *         ETCH_ERROR otherwise
   */
  virtual status_t exportValue(EtchValueFactory* vf, capu::SmartPointer<EtchObject> value, EtchStructValue *&result) = 0;

  /**
   * Imports a value from an equivalent StructValue.
   * @param struct a StructValue representing the exported value.
   * @param result as out parameter where the imported value will be stored.
   * @return ETCH_OK if successfully imported
   *         ETCH_EINVAL if it is locked
   *         ETCH_ERROR otherwise
   */
  virtual status_t importValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result) = 0;

protected:
  EtchRuntime* mRuntime;
};

#endif /* ETCHIMPORTEXPORTHELPER_H */

