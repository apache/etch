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
 
#include "signals_client_main.h"
#include "etch_objecttypes.h"
#include "etch_runtime.h"
#include "etch_arrayval.h"
#include "etch_nativearray.h"
#include "etch_binary_tdo.h"
#include "etch_general.h"

#define WAIT_UP_MS 4000

static signals_client_impl *signals_client = NULL;

/**
 * callback constructor for client implementation object.
 */
static i_signals_client* signals_client_create(void* factory_thisx, signals_remote_server* server)
{
	signals_client = new_signals_client_impl(server);
	return signals_client ? signals_client->signals_client_base:NULL;
}


/**
 * main()
 */
int main(int argc, char* argv[])
{
	etch_status_t etch_status = ETCH_SUCCESS;
	signals_remote_server* remote = NULL;
	char *signals_client_name = NULL;

	wchar_t* uri = L"udp://127.0.0.1:4001";

	// init etch
	etch_config_t* config = NULL;
	etch_config_create(&config);

	etch_status = etch_runtime_initialize(config);
	if(etch_status != ETCH_SUCCESS) {
		return 1;
	}

	// init etch client
	etch_status = signals_helper_remote_server_create(&remote, uri, NULL, (main_client_create_func)signals_client_create);
	if(etch_status != ETCH_SUCCESS) {
		// error
	}

	etch_status = signals_helper_remote_server_start_wait(remote, WAIT_UP_MS);
	if(etch_status != ETCH_SUCCESS) {
		// error
	}

	// client implementation
	{
		etch_string *_signals_client_name = remote->get_free_client_name(remote);
		if (is_etch_exception(_signals_client_name)) {
			printf("Error retrieving client name. Is server running?\n");
			return 1;
		}
		etch_encoding_transcode_wchar(&signals_client_name, ETCH_ENCODING_UTF8, _signals_client_name->v.valw, NULL);
		set_signals_client_name(signals_client_name);
		printf("Got client name: %s\n", signals_client_name);
		etch_object_destroy(_signals_client_name);
	}

	printf("%s: Subscribing to example signal ...\n", signals_client_name);
	remote->_SubscribeExampleSignal(remote);

	printf("Press enter to unsubscribe\n" );
	waitkey();
	remote->_UnsubscribeExampleSignal(remote);

	printf("%s: Press enter to disconnect\n", signals_client_name);
	waitkey();

	// shutdown client
	etch_free(signals_client_name);

	etch_status = signals_helper_remote_server_stop_wait(remote, WAIT_UP_MS);
	if(etch_status != ETCH_SUCCESS) {
		// error
	}

	etch_status = signals_helper_remote_server_destroy(remote);
	if(etch_status != ETCH_SUCCESS) {
		// error
	}
	etch_config_destroy(config);
	return 0;
}
