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

/* 
 * etch_config.h -- configuration items and file
 */
 
#ifndef _ETCH_CONFIG_H_
#define _ETCH_CONFIG_H_

#include "etch_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct etch_config_t etch_config_t;

/**
 * create a new configuration.
 * default config properties:
 * TODO: document default properties
 * @param config that will be created.
 */
etch_status_t etch_config_create(etch_config_t** config);

/**
 * reads the given config file.
 * @config handle.
 * @param config configuration handle.
 * @param filepath to the file which holds the configuration.
 */
etch_status_t etch_config_open(etch_config_t* config, const char* filepath);

/**
 * sets a new property value
 * @config handle.
 * @name of the property.
 * @value of the property.
 */
etch_status_t etch_config_set_property(etch_config_t* config, const char* name, const char* value);

/**
 * check if property with name existis.
 * @config handle.
 * @param name of the property.
 * @return true if exists, else false.
 */
int etch_config_has_property(etch_config_t* config, const char* name);

/**
 * get the string property by name.
 * @config handle.
 * @name of the property.
 * @value char** where the value will be written
 * @return etch_status ETCH_SUCCESS or ETCH_EINVAL
 */
etch_status_t etch_config_get_property_string(etch_config_t* config, const char* name, char** value);

/**
 * get the int32 property by name.
 * @config handle.
 * @name of the property.
 * @value where the value will be written
 * @return etch_status ETCH_SUCCESS or ETCH_EINVAL
 */
etch_status_t etch_config_get_property_int(etch_config_t* config, const char* name, int32* value);

/**
 * gets the property by index.
 * @config handle.
 * @name of the property.
 * @value char** where the value will be written; the value has to be freed with etch_free after use.
 * @return etch_status ETCH_SUCCESS or ETCH_EINVAL
 */
etch_status_t etch_config_get_property_by_index(etch_config_t* config, uint16 index, char** value);

/**
 * clear the property list.
 * @config handle.
 * @return etch_status_t
 */
etch_status_t etch_config_clear(etch_config_t* config);

/**
 * gets the length of the property list.
 * @config handle.
 * @return length
 */
uint16 etch_config_get_length(etch_config_t* config);

/**
 * gets the size of the property list.
 * @config handle.
 * @return length
 */
uint16 etch_config_get_size(etch_config_t* config);

/**
 * destroy the configure instance.
 * @config handle.
 */
etch_status_t etch_config_destroy(etch_config_t* config);

#ifdef __cplusplus
}
#endif

#endif /* _ETCH_CONFIG_H_ */
