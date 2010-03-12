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
#include <cJSON.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "cronkite.h"
#include "config.h"

/* local declarations -- not exported */
struct CKMemoryStruct {
    char *memory;
    size_t size;
};

static void *myrealloc(void *ptr, size_t size);
static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data);
static int cronkite_request(const char *url, struct CKMemoryStruct *response);
static cJSON *cronkite_ifetch(const char *urlfmt, const char *qtype, const char *term);
static char *cronkite_get_obj(cJSON *elem, char *name);
static CKPackage *cronkite_pack_result(cJSON *result);
/* end local declarations */


static void *
myrealloc(void *ptr, size_t size) {
    if (ptr) {
        return realloc(ptr, size);
    }
    else {
        return calloc(1, size);
    }
}

static size_t 
write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
    size_t realsize = size * nmemb;
    struct CKMemoryStruct *mem = (struct CKMemoryStruct *)data;
    mem->memory = myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

static int 
cronkite_request(const char *url, struct CKMemoryStruct *response) {
    CURL *curl_handle;
    CURLcode status;
    long result_code;

    curl_handle = curl_easy_init();
    if (!curl_handle) {
        return 1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)response);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UAGENT);
    curl_easy_setopt(curl_handle, CURLOPT_ENCODING, "gzip,deflate");

    status = curl_easy_perform(curl_handle);
    if (status != 0) {
        // fprintf(stderr, "error: unable to request data from %s\n", url);
        // fprintf(stderr, " %s\n", curl_easy_strerror(status));
        return 1;
    }

    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &result_code);
    if (result_code != 200) {
        // fprintf(stderr, "error: server responded with code %ld\n", result_code);
        return 1;
    }

    curl_easy_cleanup(curl_handle);
    return 0;
}

static cJSON *
cronkite_ifetch(const char *urlfmt, const char *qtype, const char *term) {
    char *url;
    cJSON *root;
    CURL *curl_handle;
    char *esc_term;
    int reqlen = 0;

    struct CKMemoryStruct jdata;
    jdata.memory = NULL;
    jdata.size = 0;

    curl_global_init(CURL_GLOBAL_NOTHING);
    curl_handle = curl_easy_init();
    esc_term = curl_easy_escape(curl_handle, term, 0);
    reqlen = strlen(urlfmt) + strlen(qtype) + strlen(esc_term);
    url = calloc(reqlen, sizeof(char));
    if (url == NULL) {
        // failed to allocate memory..just bail
        return NULL;
    }
    snprintf(url, reqlen, urlfmt, qtype, esc_term);
    curl_free(esc_term);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    if (0 != cronkite_request(url, &jdata)) {
        return NULL;
    }

    root = cJSON_Parse(jdata.memory);
    free(jdata.memory);

    if (!root) {
        // fprintf(stderr, "error parsing json data\n");
        return NULL;
    }

    return root;
}

static char *
cronkite_get_obj(cJSON *elem, char *name) {
    char *rval;
    cJSON *element;
    int len=0;

    element = cJSON_GetObjectItem(elem, name);
    if (!element) {
        rval = "NULL";
    }
    else {
        if (!element->valuestring) {
            rval = "NULL";
        }
        else {
            len = strlen(element->valuestring);
            rval = (char *)calloc(len+1, sizeof(char *));
            strncpy(rval, element->valuestring, len);
            rval[len] = '\0'; /* make sure it is null terminated */
        }
    }
    return rval;
}

static CKPackage *
cronkite_pack_result(cJSON *result) {
    CKPackage *pkg = (CKPackage *) calloc(1, sizeof(CKPackage));
    pkg->values[CKPKG_ID] = cronkite_get_obj(result, "id");
    pkg->values[CKPKG_URL] = cronkite_get_obj(result, "url");
    pkg->values[CKPKG_NAME] = cronkite_get_obj(result, "name");
    pkg->values[CKPKG_VERSION] = cronkite_get_obj(result, "version");
    pkg->values[CKPKG_URLPATH] = cronkite_get_obj(result, "urlpath");
    pkg->values[CKPKG_LICENSE] = cronkite_get_obj(result, "license");
    pkg->values[CKPKG_NUMVOTES] = cronkite_get_obj(result, "numvotes");
    pkg->values[CKPKG_OUTOFDATE] = cronkite_get_obj(result, "outofdate");
    pkg->values[CKPKG_CATEGORYID] = cronkite_get_obj(result, "categoryid");
    pkg->values[CKPKG_DESCRIPTION] = cronkite_get_obj(result, "description");
    pkg->next = NULL;
    return pkg;
}

void 
cronkite_cleanup(CKPackage *ckpackage) {
    CKPackage *ckpkg = ckpackage;
    CKPackage *next;
    char *ptr;
    while (ckpkg) {
        for (int j=0; j<CKPKG_VAL_CNT; j++) {
            ptr = ckpkg->values[j];
            free(ptr);
        }
        next = ckpkg->next;
        free(ckpkg);
        ckpkg = next;
    }
}

CKPackage *
cronkite_get(const char *urlfmt, const char t, const char *term) {
    char *qtype;
    cJSON *root;
    cJSON *results;
    CKPackage *ckpkg = NULL;
    CKPackage *head = NULL;
    int iter = 0;

    switch (t) {
        case 'i':
            qtype = "info";
            break;
        case 's':
            qtype = "search";
            break;
        case 'm':
            qtype = "msearch";
            break;
        default:
            qtype = "search";
            break;
    }

    root = cronkite_ifetch(urlfmt, qtype, term);
    if (!root) {
        return NULL;
    }
    results = cJSON_GetObjectItem(root, "results");
    if (results->type == cJSON_Array) {
        cJSON *pkg = results->child;
        while (pkg) {
            if (pkg->type == cJSON_Object) {
                if (iter == 0) { /* means first iter */
                    ckpkg = cronkite_pack_result(pkg);
                    head = ckpkg;
                }
                else {
                    ckpkg->next = cronkite_pack_result(pkg);
                    ckpkg = ckpkg->next;
                }
            }
            pkg = pkg->next;
            iter++;
        }
    }
    else if (results->type == cJSON_Object) {
        ckpkg = cronkite_pack_result(results);
        head = ckpkg;
    }

    /* cJSON cleanup */
    cJSON_Delete(root);
    return head;
}

