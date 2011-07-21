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

#include "signals_listener_main.h"
#include "etch_objecttypes.h"
#include "etch_runtime.h"
#include "etch_arrayval.h"
#include "etch_nativearray.h"
#include "etch_binary_tdo.h"
#include "etch_general.h"

#define WAIT_UP_MS 4000

static void* signals_server_create(void* factoryData, void* sessionData)
{
	etch_session* session = (etch_session*)sessionData;
	signals_remote_client* client  = (signals_remote_client*) session->client;

	signals_server_impl* newserver = new_signals_server_impl(client);

	return newserver->signals_server_base;
}

etch_status_t signals_listener_start(i_sessionlistener** pplistener, wchar_t* uri, int waitupms)
{
	etch_status_t etch_status = ETCH_SUCCESS;

	etch_status = signals_helper_listener_create(pplistener, uri, NULL, signals_server_create);
	if(etch_status == ETCH_SUCCESS) 
	{
		etch_status = signals_helper_listener_start_wait(*pplistener, waitupms);
	}

	return etch_status;
}

etch_status_t signals_listener_stop(i_sessionlistener* plistener, int waitupms)
{
	etch_status_t etch_status = ETCH_SUCCESS;

	etch_status = signals_helper_listener_stop_wait(plistener, waitupms);
	if(etch_status == ETCH_SUCCESS) 
	{
		signals_helper_listener_destroy(plistener);
	}

	return etch_status;
}

/**
 * main()
 */
int main(int argc, char* argv[])
{
	etch_status_t etch_status = ETCH_SUCCESS;
	i_sessionlistener* listener = NULL;

	wchar_t* uri = L"udp://127.0.0.1:4001?UdpListener.singleSession=true";

	etch_config_t* config = NULL;
	etch_config_create(&config);

	etch_status = etch_runtime_initialize(config);
	if(etch_status != ETCH_SUCCESS) {
		// error
		return 1;
	}

	etch_status = signals_listener_start(&listener, uri, WAIT_UP_MS);
	if(etch_status != ETCH_SUCCESS) {
		// error
	}

	printf("Server started\n");
	printf("Press enter to stop server\n");
	// wait for keypress
	waitkey();

	etch_status = signals_listener_stop(listener, WAIT_UP_MS);
	if(etch_status != ETCH_SUCCESS) {
		// error
	}

	etch_status = etch_runtime_shutdown();
	if(etch_status != ETCH_SUCCESS) {
		// error
		return 1;
	}
	etch_config_destroy(config);

	return 0;
}
