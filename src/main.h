/*
 * Copyright 2009 elij
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
**/

struct MemoryStruct {
    char *memory;
    size_t size;
};

static void *myrealloc(void *ptr, size_t size);
static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data);
static int cronkite_request(const char *url, struct MemoryStruct *response);
json_t *cronkite_get(const char qtype, const char *term);
static void print_objs(json_t *result);
static void print_objs_header();
static void print_version();
static void print_help();
