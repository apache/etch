/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#ifndef SIGNALS_CLIENT_MAIN_H
#define SIGNALS_CLIENT_MAIN_H

#include "etch_runtime.h"
#include "signals_helper.h"
#include "signals_client_impl.h"
#include "signals_remote_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Start the listener at the given uri, waiting waitupms microseconds for listener startup.
 * The created listener is saved at address pointed to by pplistener.
 */
extern etch_status_t signals_listener_start(i_sessionlistener** pplistener, wchar_t* uri, int waitupms);

/*
 * signals_listener_stop(plistener, waitupms)
 * Stop the listener given by plistener, waiting waitupms microseconds to stop.
 */
extern etch_status_t signals_listener_stop(i_sessionlistener* plistener, int waitupms);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* SIGNALS_CLIENT_MAIN.H */
