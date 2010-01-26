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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "config.h"
#include "main.h"


static void *myrealloc(void *ptr, size_t size) {
    if(ptr) {
        return realloc(ptr, size);
    }
    else {
        return malloc(size);
    }
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;
    mem->memory = myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

static int cronkite_request(const char *url, struct MemoryStruct *response) {
    CURL *curl_handle;
    CURLcode status;
    long result_code;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    if(!curl_handle) {
        return 1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)response);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UAGENT);

    status = curl_easy_perform(curl_handle);
    if(status != 0) {
        fprintf(stderr, "error: unable to request data from %s:\n", url);
        fprintf(stderr, "%s\n", curl_easy_strerror(status));
        return 1;
    }

    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &result_code);
    if(result_code != 200) {
        fprintf(stderr, "error: server responded with code %ld\n", result_code);
        return 1;
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    return 0;
}

json_t *cronkite_get(const char qtype, const char *term) {
    int result;
    char url[URL_SIZE];
    json_t *root;
	json_t *jresults;
    json_error_t error;

    struct MemoryStruct jdata;
    jdata.memory = NULL;
    jdata.size = 0;

    if (qtype == 'i') {
        snprintf(url, URL_SIZE, URL_FORMAT, "info", term);
    }
    else if (qtype == 's') {
        snprintf(url, URL_SIZE, URL_FORMAT, "search", term);
    }
    else {
        return NULL;
    }

    result = cronkite_request(url, &jdata);
    if (result != 0) {
        return NULL;
    }

    root = json_loads(jdata.memory, &error);
    free(jdata.memory);

    if(!root) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return NULL;
    }

    jresults = json_object_get(root, "results");
    return jresults;
}


static void print_objs(json_t *result) {
    int i=0;
    char *rnames[] = PKG_VALUES;
    char *delimiter;
    char *default_delimiter = "\t";
    int size = sizeof(rnames) / sizeof(char *);

    delimiter = getenv("CRONKITE_DELIMITER");
    if (delimiter == NULL) {
        delimiter = default_delimiter;
    }

    for (i=0; i<size; i++) {
        json_t *rez = json_object_get(result, rnames[i]);
        if(!json_is_string(rez)) {
            fprintf(stderr, "error: %s is not a string\n", rnames[i]);
        }

        if (i < (size - 1)) {
            printf("%s%s", json_string_value(rez), delimiter);
        }
        else {
            printf("%s\n", json_string_value(rez));
        }

        json_decref(rez);
    }
}

static void print_version() {
    fprintf(stderr, "%s-%s\n", NAME, VERSION);
}

static void print_help() {
    fprintf(stderr, "Usage: %s [OPTION] [OPTION-ARGUMENT]\n\n", NAME);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-search <search-term>\n");
    fprintf(stderr, "\t\tPerform an aurjson search operation.\n");
    fprintf(stderr, "\t-info <package-name>\n");
    fprintf(stderr, "\t\tPerform an aurjson info operation.\n");
    fprintf(stderr, "\t-version\n\t\tShow version and exit.\n");
    fprintf(stderr, "\t-help\n");
    fprintf(stderr, "\t\tShow simple help and exit.\n");
    fprintf(stderr, "\n");
}

int main(int argc, char *argv[]) {
    json_t *results;

    /* handle command line arguments */
    if (argc == 2 && strcmp(argv[1],"-help") == 0) {
        print_help();
        return 0;
    }
    else if (argc == 2 && strcmp(argv[1],"-version") == 0) {
        print_version();
        return 0;
    }
    else if (argc == 3 && strcmp(argv[1],"-search") == 0) {
        results = cronkite_get('s', argv[2]);
    }
    else if (argc == 3 && strcmp(argv[1],"-info") == 0) {
        results = cronkite_get('i', argv[2]);
    }
    else {
        print_help();
        return 1;
    }

    if(json_is_array(results)) {
        int i;
        for(i = 0; i < json_array_size(results); i++) {
            json_t *pkg;

            pkg = json_array_get(results, i);
            if(!json_is_object(pkg)) {
                fprintf(stderr, "error: pkg %d is not an object\n", i + 1);
                continue;
            }
            else {
                print_objs(pkg);
            }
        }
    }
    else if (json_is_object(results)) {
        print_objs(results);
    }
    else {
        /* no results or results not readable. Just exit. */
        return 2;
    }

    return 0;
}

