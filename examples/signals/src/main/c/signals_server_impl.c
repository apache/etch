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


#include "signals_server_impl.h"
#include "signals_valufact.h"
#include "signals_remote_client.h"
#include "etch_url.h"
#include "etch_arrayval.h"
#include "etch_binary_tdo.h"
#include "etch_exception.h"
#include "etch_general.h"
#include "etch_log.h"
#include "etch_map.h"
#include <stdio.h>

#if defined(_MSC_VER)
#define snprintf _snprintf
#endif

unsigned short CLASSID_SIGNALS_SERVER_IMPL;

char* SIGNALS_ETCHSIMP = "SIMP";

/* generated signatures */
int destroy_signals_server_implx(void*);
signals_server_impl* init_signals_server_impl(struct signals_remote_client*, etch_object_destructor);

static etch_string* get_free_client_name_impl(void* thisx);
static void* _SubscribeExampleSignal_impl(void* thisx, etch_who* subscriber);
static void* _UnsubscribeExampleSignal_impl(void* thisx, etch_who* subscriber);

/**
 * signals_server_impl constructor.
 */
signals_server_impl* new_signals_server_impl(struct signals_remote_client* client) 
{
	signals_server_impl* pserver = init_signals_server_impl(client, destroy_signals_server_implx);
	i_signals_server* pserver_base = pserver->signals_server_base;
	((etch_object*)pserver_base)->class_id = get_dynamic_classid_unique(&CLASSID_SIGNALS_SERVER_IMPL);
	/* virtual method overrides */
	etchmap_insertxw(pserver_base->virtuals, signals_valufact_get_static()->str_signals_get_free_client_name, get_free_client_name_impl, FALSE);
	etchmap_insertxw(pserver_base->virtuals, signals_valufact_get_static()->str_signals__SubscribeExampleSignal, _SubscribeExampleSignal_impl, FALSE);
	etchmap_insertxw(pserver_base->virtuals, signals_valufact_get_static()->str_signals__UnsubscribeExampleSignal, _UnsubscribeExampleSignal_impl, FALSE);

	return pserver;
}

/**
 * destructor for any user allocated memory.
 */
int destroy_signals_server_implx(void* data)
{
	return 0;
}

/***
 * server implementation
 */

#define MAX_CLIENT_NAME_LENGTH 25
static etch_string* get_free_client_name_impl(void* thisx) {
	static int client_count = 0;
	
	char client_name[MAX_CLIENT_NAME_LENGTH];
	wchar_t *client_nameW;
	etch_string *result;

	snprintf(client_name, MAX_CLIENT_NAME_LENGTH, "client%d", client_count++);
	printf("Allocated client name: %s\n", client_name);

	etch_encoding_transcode_to_wchar(&client_nameW, client_name, ETCH_ENCODING_DEFAULT, (unsigned int)strlen(client_name), NULL);
	result = new_stringw(client_nameW);
	etch_free(client_nameW);

	return result;
}

static apr_pool_t *memory_pool = NULL;
static apr_thread_t *notification_thread = NULL;

static void* APR_THREAD_FUNC notify_subscribers(apr_thread_t *thd, void *thisx)
{
	static int notify_count = 0;
	signals_server_impl* server = (signals_server_impl*)thisx;

	for (;; notify_count++) {
		int subscribers_count = etchmap_count(server->client->Example_subscribers);

		if (subscribers_count < 1) {
			printf("No subscribers, stopping notification thread\n");
			notification_thread = NULL;
			apr_thread_exit(thd, APR_SUCCESS);
		}

		printf("Sending event to %d subscribers (%d)\n", subscribers_count, notify_count);
		server->client->example_signal(server->client, new_int32(notify_count));

		apr_sleep(1000000);
	}

	return NULL;
}

static void* _SubscribeExampleSignal_impl(void* thisx, etch_who* subscriber)
{
	signals_server_impl* server = (signals_server_impl*)thisx;
	void* result;

	printf("Subscribing new client\n");
	result = signals_ex_def__SubscribeExampleSignal(thisx, subscriber);
	if (!result) {
		printf("succeeded\n");

		if (!memory_pool)
			apr_pool_create(&memory_pool, NULL);

		if (!notification_thread && memory_pool) {
			apr_threadattr_t *thd_attr;

			apr_threadattr_create(&thd_attr, memory_pool);
			if (apr_thread_create(&notification_thread, thd_attr, notify_subscribers, thisx, memory_pool) != APR_SUCCESS)
				printf("Unable to create notification thread!\n");
			printf("Notification thread started\n");
		}
	} else {
		printf("failed\n");
	}

	return result;
}

static void* _UnsubscribeExampleSignal_impl(void* thisx, etch_who* subscriber)
{
	signals_server_impl* server = (signals_server_impl*)thisx;
	void* result;

	printf("Unsubscribing one client\n");
	result = signals_ex_def__UnsubscribeExampleSignal(thisx, subscriber);
	if (!result)
		printf("succeeded\n");
	else
		printf("failed\n");

	return result;
}

