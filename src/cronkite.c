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
static char *cronkite_ifetch(const char *qtype, const char *term);
static char *cronkite_get_obj(cJSON *elem, char *name);
static CKPackage *cronkite_pack_result(cJSON *result);
static CKPackage *cronkite_json_to_packlist(char *jsondata);
/* end local declarations */

/* initialize some globals (dirty hacks! *cough*) */
int ck_errno = CK_ERR_OK;
char *default_urlfmt = NULL;
/* end initialize */

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
    ck_errno = CK_ERR_OK;
    CURL *curl_handle;
    CURLcode status;
    long result_code;

    curl_global_init(CURL_GLOBAL_NOTHING);
    curl_handle = curl_easy_init();
    if (!curl_handle) {
        ck_errno = CK_ERR_CURL_INIT;
        return 1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)response);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UAGENT);
    curl_easy_setopt(curl_handle, CURLOPT_ENCODING, "deflate, gzip");

    status = curl_easy_perform(curl_handle);
    if (status != 0) {
        // add ck_curl_offset (+10) to curlcode for later parsing
        ck_errno = CK_ERR_CURL_OFFSET + status;
        return 1;
    }

    status = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &result_code);
    if (status != 0) {
        // add ck_curl_offset (+10) to curlcode for later parsing
        ck_errno = CK_ERR_CURL_OFFSET + status;
        return 1;
    }
    if (result_code != 200) {
        ck_errno = CK_ERR_RESP;
        return 1;
    }

    curl_easy_cleanup(curl_handle);
    return 0;
}

static char *
cronkite_ifetch(const char *qtype, const char *term) {
    ck_errno = CK_ERR_OK;
    char *url;
    CURL *curl_handle;
    char *esc_term;
    char *result;
    int reqlen = 0;

    struct CKMemoryStruct jdata;
    jdata.memory = NULL;
    jdata.size = 0;

    curl_global_init(CURL_GLOBAL_NOTHING);
    curl_handle = curl_easy_init();
    esc_term = curl_easy_escape(curl_handle, term, 0);
    reqlen = strlen(default_urlfmt) + strlen(qtype) + strlen(esc_term);
    // since the placeholders in urlfmt exist, no need to pad the
    // length for later snprintf
    url = calloc(reqlen, sizeof(char));
    if (url == NULL) {
        // failed to allocate memory..just bail
        ck_errno = CK_ERR_ALLOC;
        return NULL;
    }
    snprintf(url, reqlen, default_urlfmt, qtype, esc_term);
    curl_free(esc_term);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    if (0 != cronkite_request(url, &jdata)) {
        // ck_errno should be set by cronkite_request
        return NULL;
    }
    
    result = calloc(jdata.size + 1, sizeof(char));
    int len = strlen(jdata.memory);
    strncpy(result, jdata.memory, len);
    /* cleanup */
    free(jdata.memory);
    free(url);
    return result;
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
cronkite_pack_result(cJSON *pkg_p) {
    CKPackage *pkg = (CKPackage *) calloc(1, sizeof(CKPackage));
    pkg->values[CKPKG_ID] = cronkite_get_obj(pkg_p, "id");
    pkg->values[CKPKG_URL] = cronkite_get_obj(pkg_p, "url");
    pkg->values[CKPKG_NAME] = cronkite_get_obj(pkg_p, "name");
    pkg->values[CKPKG_VERSION] = cronkite_get_obj(pkg_p, "version");
    pkg->values[CKPKG_URLPATH] = cronkite_get_obj(pkg_p, "urlpath");
    pkg->values[CKPKG_LICENSE] = cronkite_get_obj(pkg_p, "license");
    pkg->values[CKPKG_NUMVOTES] = cronkite_get_obj(pkg_p, "numvotes");
    pkg->values[CKPKG_OUTOFDATE] = cronkite_get_obj(pkg_p, "outofdate");
    pkg->values[CKPKG_CATEGORYID] = cronkite_get_obj(pkg_p, "categoryid");
    pkg->values[CKPKG_DESCRIPTION] = cronkite_get_obj(pkg_p, "description");
    pkg->next = NULL;
    return pkg;
}

static CKPackage *
cronkite_json_to_packlist(char *jsondata) {
    ck_errno = CK_ERR_OK;
    cJSON *root;
    cJSON *results;
    CKPackage *ckpkg = NULL;
    CKPackage *head = NULL;

    root = cJSON_Parse(jsondata);
    if (!root) {
        ck_errno = CK_ERR_PARSE;
        return NULL;
    }

    results = cJSON_GetObjectItem(root, "results");
    int iter = 0;
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

void 
cronkite_cleanup(CKPackage *ckpackage) {
    ck_errno = CK_ERR_OK;
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
    if (default_urlfmt != NULL) {
        free(default_urlfmt);
    }
}

CKPackage *
cronkite_get(const char t, const char *term) {
    ck_errno = CK_ERR_OK;
    char *qtype;
    CKPackage *pkg_list = NULL;
    char *json;

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

    if (default_urlfmt == NULL) {
        default_urlfmt = calloc(strlen(DEFAULT_AUR_URL) + 1, sizeof(char));
        strcpy(default_urlfmt, DEFAULT_AUR_URL);
    }

    json = cronkite_ifetch(qtype, term);
    if (!json) {
        // ck_errno should be set by cronkite_ifetch
        return NULL;
    }

    pkg_list = cronkite_json_to_packlist(json);
    free(json);

    if (!pkg_list) {
        // ck_errno should be set by json_to_packlist
        return NULL;
    }

    return pkg_list;
}

CKPackage *
cronkite_json_conv(char *jsondata) {
    ck_errno = CK_ERR_OK;
    CKPackage *pkg_list = cronkite_json_to_packlist(jsondata);
    
    if (!pkg_list) {
        // ck_err should be set by json_to_packlist
        // fprintf(stderr, "error parsing json data\n");
        return NULL;
    }

    return pkg_list;
}

const char *
cronkite_strerror(int ck_err_val) {
    if (ck_err_val < CK_ERR_CURL_OFFSET) {
        switch (ck_err_val) {
            case CK_ERR_CURL_INIT:
                return "Error initializing CURL library calls.";
            case CK_ERR_RESP:
                return "Response code not 200 ok.";
            case CK_ERR_ALLOC:
                return "Failed to allocate memory.";
            case CK_ERR_PARSE:
                return "Error parsing JSON result.";
            case CK_ERR_OK:
                return "No error. Ok!";
                break;
            default:
                return "No error string for that code.";
                break;
        }
    }
    else {
        return curl_easy_strerror(ck_err_val - CK_ERR_CURL_OFFSET);
    }
    return "Hurrrrrrrr....."; //should never be reached
}

void
cronkite_seturl(const char *urlfmt) {
    default_urlfmt = calloc(strlen(urlfmt) + 1, sizeof(char));
    strcpy(default_urlfmt, urlfmt);
}

