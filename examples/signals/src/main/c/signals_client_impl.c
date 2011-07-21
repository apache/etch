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

#include "signals_client_impl.h"
#include "signals_valufact.h"
#include "etch_url.h"
#include "etch_arrayval.h"
#include "etch_binary_tdo.h"
#include "etch_exception.h"
#include "etch_general.h"
#include "etch_log.h"
#include "etch_map.h"
#include <stdio.h>

unsigned short CLASSID_SIGNALS_CLIENT_IMPL;

char* SIGNALS_ETCHCIMP = "CIMP";
static char *client_name = "";

int destroy_signals_client_implx(void*);
void* example_signal_impl(void* thisx, etch_int32* count);

signals_client_impl* init_signals_client_impl(struct signals_remote_server*, etch_object_destructor);

void set_signals_client_name(char *name) {
	client_name = name;
}

/**
 * signals_client_impl constructor.
 */
signals_client_impl* new_signals_client_impl(struct signals_remote_server* server)
{
	signals_client_impl* pclient = init_signals_client_impl(server, destroy_signals_client_implx);
	i_signals_client* pclient_base = pclient->signals_client_base;
	/* virtual method overrides */
  etchmap_insertxw(pclient_base->virtuals, signals_valufact_get_static()->str_signals_example_signal, example_signal_impl, FALSE);

	return pclient;
}

/**
 * destructor for any user allocated memory.
 */
int destroy_signals_client_implx(void* data) {
	return 0;
}

/**
 * implementation method to handle signals from server
 */
void* example_signal_impl(void* thisx, etch_int32* count) {
	printf("%s: example signal, count = %d\n", client_name, count->value);
	etch_object_destroy(count);
	return NULL;
}
