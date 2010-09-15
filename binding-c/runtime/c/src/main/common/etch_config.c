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
 * etch_config.c
 * config items and config file parse.
 * config file is formatted as a java-style properties file expected as ansi or utf-8.
 */

#include "etch.h"
#include "etch_config.h"
#include "etch_general.h"

#define ETCH_CONFIG_LINE_MAX 1024

/*
static const char* ETCH_CONFIG_CATEGORY = "etch_config";
*/
struct etch_config_t
{
    struct etch_config_entry*   data;
    uint16                      length;
    uint16                      size;
};

struct etch_config_entry
{
    char* name;
    char* value;
};

etch_status_t etch_config_create(etch_config_t** config)
{
    etch_config_t* newconfig = NULL;

    if(config == NULL) {
        return ETCH_EINVAL;
    }

    newconfig = etch_malloc(sizeof(etch_config_t), 0);
    if(newconfig == NULL) {
        return ETCH_ENOMEM;
    }
    memset(newconfig, 0, sizeof(etch_config_t));
    *config = newconfig;

    // default properties
    etch_config_set_property(*config, "etch.validate.write", "1");
    etch_config_set_property(*config, "etch.validate.read", "1");
    etch_config_set_property(*config, "etch.mailbox.timeout.read", "3000");
    etch_config_set_property(*config, "etch.mailbox.timeout.write", "3000");
    // destroy message with mailbox
    etch_config_set_property(*config, "etch.mailbox.destroy.message", "1");
    // TODO: add default values
    etch_config_set_property(*config, "etch.log", "error");
    
    etch_config_set_property(*config, "etch.log.consoleappender", "consoleappender");
    
    etch_config_set_property(*config, "etch.log.fileappender", "fileappender");
    etch_config_set_property(*config, "etch.log.fileappender.file", "C:/dfdr.log");

    etch_config_set_property(*config, "etch.maxconnections", "40");

    return ETCH_SUCCESS;
}

etch_status_t etch_config_open(struct etch_config_t* config, const char* filepath)
{
    FILE *file = NULL;
    int   c         = 0;
    char  linebuf[ETCH_CONFIG_LINE_MAX];
    int16 linepos   = 0;
    char  lineend   = 0;

    if(config == NULL) {
        return ETCH_EINVAL;
    }
    
    file = fopen(filepath, "r");
    if(file == NULL) {
        return ETCH_EFILENOTFOUND;
    }

    while(c != EOF) {
        char ch = 0;
        c = fgetc(file);
        ch = c;
        // determine end of config line
        if(c == EOF || c == '\n') {
            ch = '\0';
            if(linepos > 0 && linebuf[linepos - 1] == '\r') {
                linepos--;
            }
            lineend = 1;
        }
        if(linepos >= ETCH_CONFIG_LINE_MAX - 1) {
            // error
            lineend = 0;
            linepos = 0;
            continue;
        }

        linebuf[linepos++] = ch;
        if(lineend) {
            char *name   = NULL;
            char *value = NULL;
            char *line = strtrim(linebuf);
            
            // check for comments or empty lines
            if(line[0] != '#' && line[0] != '\0') {
                // parse line
                name = strtok(line, "=");
                name = strtrim(name);
                if(name != NULL) {
                    value = strtok(NULL, "=");
                    value = strtrim(value);
                }
                etch_config_set_property(config, name, value);
            }
            lineend = 0;
            linepos = 0;
        }
    }

    fclose(file);
    return ETCH_SUCCESS;
}

etch_status_t etch_config_set_property(etch_config_t* config, const char* name, const char* value)
{
    int16 i = 0;
    int16 index = -1;

    if(config == NULL) {
        return ETCH_EINVAL;
    }
    if(name == NULL || value == NULL) {
        return ETCH_EINVAL;
    }

    // check if resize is neccesary
    if(config->length == config->size) {
        uint16 newsize = config->size + 10;
        struct etch_config_entry* temp = etch_malloc(sizeof(struct etch_config_entry) * newsize, 0);
        memset(temp, 0, sizeof(struct etch_config_entry) * newsize);
        if(config->size > 0) {
            memcpy(temp, config->data, sizeof(struct etch_config_entry) * config->size);
            etch_free(config->data);
        }
        config->data = temp;
        config->size = newsize;
    }

    // check if key exists
    for(i = 0; i < config->length; i++) {
        if(strcmp(config->data[i].name, name) == 0) {
            index = i;
            break;
        }
    }

    // if index == -1 append new entry
    if(index == -1) {
        // write key
        config->data[config->length].name = etch_malloc(strlen(name) + 1, 0);
        memset(config->data[config->length].name, 0, (size_t)(strlen(name) + 1));
        strcpy(config->data[config->length].name, name);
        index = config->length++;
    } else {
        etch_free(config->data[index].value);
        config->data[index].value = NULL;
    }

    // write value
    config->data[index].value = etch_malloc(strlen(value) + 1, 0);
    memset(config->data[index].value, 0, (size_t)(strlen(value) + 1));
    strcpy(config->data[index].value, value);

    return ETCH_SUCCESS;
}

int etch_config_has_property(struct etch_config_t* config, const char* name)
{
    int16 i = 0;

    if(config == NULL || name == NULL) {
        return 0;
    }

    // check if key exists
    for(i = 0; i < config->length; i++) {
        if(strcmp(config->data[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

etch_status_t etch_config_get_property_string(struct etch_config_t* config, const char* name, char** value)
{
    int16 i = 0;
    int16 index = -1;
    
    if(config == NULL || name == NULL|| value == NULL) {
        return ETCH_EINVAL;
    }

    // check if key exists
    for(i = 0; i < config->length; i++) {
        if(strcmp(config->data[i].name, name) == 0) {
            index = i;
            break;
        }
    }

    if(index != -1) {
        *value = config->data[i].value;
    } else {
        *value = NULL;
    }
    return ETCH_SUCCESS;
}

etch_status_t etch_config_get_property_int(struct etch_config_t* config, const char* name, int32* value)
{
    int16 i = 0;
    int16 index = -1;
    
    if(config == NULL || name == NULL|| value == NULL) {
        return ETCH_EINVAL;
    }

    // check if key exists
    for(i = 0; i < config->length; i++) {
        if(strcmp(config->data[i].name, name) == 0) {
            index = i;
            break;
        }
    }

    if(index != -1) {
        *value = atoi(config->data[i].value);
    } else {
        *value = 0;
    }
    return ETCH_SUCCESS;
}

etch_status_t etch_config_get_property_by_index(struct etch_config_t* config, uint16 index, char** value)
{
    if(config == NULL || value == NULL) {
        return ETCH_EINVAL;
    }

    if(index < config->length) {
        *value = config->data[index].value;
    } else {
        *value = NULL;
    }
    return ETCH_SUCCESS;
}

etch_status_t etch_config_clear(etch_config_t* config)
{
    int16 i = 0;

    if(config == NULL) {
        return ETCH_EINVAL;
    }

    // check if key exists
    for(i = 0; i < config->length; i++) {
        etch_free(config->data[i].name);
        etch_free(config->data[i].value);
    }
    config->length = 0;


    return ETCH_SUCCESS;
}

uint16 etch_config_get_length(struct etch_config_t* config)
{
    if(config == NULL) {
        return 0;
    }
    return config->length;
}

uint16 etch_config_get_size(struct etch_config_t* config)
{
    if(config == NULL) {
        return 0;
    }
    return config->size;
}

etch_status_t etch_config_destroy(etch_config_t* config)
{
    if(config == NULL) {
        return ETCH_EINVAL;
    }
    etch_config_clear(config);
    etch_free(config->data);
    etch_free(config);
    return ETCH_SUCCESS;
}
