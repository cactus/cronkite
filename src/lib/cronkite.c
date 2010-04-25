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

/* local struct declarations -- not exported */
struct CKoptions {
    char *aururl;
    char *proxyurl;
};

struct CKMemoryStruct {
    char *memory;
    size_t size;
};

/* local function declarations -- not exported */
static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data);
static int cronkite_request(const char *url, struct CKMemoryStruct *response);
static char *cronkite_ifetch(const char *qtype, const char *term);
static char *cronkite_get_obj(cJSON *elem, char *name);
static CKPackage *cronkite_pack_result(cJSON *result);
static CKPackage *cronkite_json_to_packlist(char *jsondata);
/* end local declarations */

/* initialize local globals -- not exported */
static struct CKoptions *g_options;

/* initialize exported globals -- exported -- extern in header */
int ck_errno = CK_ERR_OK;
/* end initialize */

/* callback write function. used by curl. */
static size_t 
write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
    size_t realsize = size * nmemb;
    struct CKMemoryStruct *mem = (struct CKMemoryStruct *)data;
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

/* uses libcurl to perform a request. */
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
        curl_global_cleanup();
        return 1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)response);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, UAGENT);
    curl_easy_setopt(curl_handle, CURLOPT_ENCODING, "deflate, gzip");
    if (g_options && g_options->proxyurl) {
        curl_easy_setopt(curl_handle, CURLOPT_PROXY, g_options->proxyurl);
    }

    status = curl_easy_perform(curl_handle);
    if (status != 0) {
        // add ck_curl_offset (+10) to curlcode for later parsing
        ck_errno = CK_ERR_CURL_OFFSET + status;
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
        return 1;
    }

    status = curl_easy_getinfo(
        curl_handle, CURLINFO_RESPONSE_CODE, &result_code);
    if (status != 0) {
        // add ck_curl_offset (+10) to curlcode for later parsing
        ck_errno = CK_ERR_CURL_OFFSET + status;
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
        return 1;
    }
    if (result_code != 0 && result_code != 200) {
        ck_errno = CK_ERR_RESP;
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
        return 1;
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    return 0;
}

/* wrapper that performs the aur fetch functionality.
   takes a query-type and a query term, and returns
   a string of json data (returned from the aur) */
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
    reqlen = strlen(g_options->aururl) + strlen(qtype) + strlen(esc_term);
    // since the placeholders in g_options->aururl exist, no need to pad the
    // length for later snprintf
    url = calloc(reqlen, sizeof(char));
    if (url == NULL) {
        // failed to allocate memory..just bail
        ck_errno = CK_ERR_ALLOC;
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
        return NULL;
    }
    snprintf(url, reqlen, g_options->aururl, qtype, esc_term);
    curl_free(esc_term);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    if (0 != cronkite_request(url, &jdata)) {
        // ck_errno should be set by cronkite_request
        return NULL;
    }

    if (jdata.size == 0) {
        ck_errno = CK_ERR_EMPTY;
        return NULL;
    }
    result = calloc(jdata.size, sizeof(char));
    strncpy(result, jdata.memory, jdata.size);

    /* cleanup */
    free(jdata.memory);
    free(url);
    return result;
}

/* returns a json object parse partial string */
static char *
cronkite_get_obj(cJSON *elem, char *name) {
    char *rval;
    cJSON *element;
    int len=0;

    element = cJSON_GetObjectItem(elem, name);
    if (element && element->valuestring) {
        len = strlen(element->valuestring);
        rval = calloc(len+1, sizeof(char));
        strncpy(rval, element->valuestring, len);
        rval[len] = '\0'; /* make sure it is null terminated */
    }
    else {
        rval = "NULL";
    }
    return rval;
}

/* convert a json package array into a cronkite package struct */
static CKPackage *
cronkite_pack_result(cJSON *pkg_p) {
    CKPackage *pkg = (CKPackage *) malloc(sizeof(CKPackage));
    pkg->values[CK_PKG_ID] = cronkite_get_obj(pkg_p, "id");
    pkg->values[CK_PKG_URL] = cronkite_get_obj(pkg_p, "url");
    pkg->values[CK_PKG_NAME] = cronkite_get_obj(pkg_p, "name");
    pkg->values[CK_PKG_VERSION] = cronkite_get_obj(pkg_p, "version");
    pkg->values[CK_PKG_URLPATH] = cronkite_get_obj(pkg_p, "urlpath");
    pkg->values[CK_PKG_LICENSE] = cronkite_get_obj(pkg_p, "license");
    pkg->values[CK_PKG_NUMVOTES] = cronkite_get_obj(pkg_p, "numvotes");
    pkg->values[CK_PKG_OUTOFDATE] = cronkite_get_obj(pkg_p, "outofdate");
    pkg->values[CK_PKG_CATEGORYID] = cronkite_get_obj(pkg_p, "categoryid");
    pkg->values[CK_PKG_DESCRIPTION] = cronkite_get_obj(pkg_p, "description");
    pkg->next = NULL;
    return pkg;
}

/* iterate through json data, generate package structs, and return
   a linked list of cronkite package structs */
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
    if (results->type == cJSON_Array) {
        cJSON *pkg = results->child;
        while (pkg) {
            if (pkg->type == cJSON_Object) {
                ckpkg = cronkite_pack_result(pkg);
                ckpkg->next = head;
                head = ckpkg;
            }
            pkg = pkg->next;
        }
    }
    else if (results->type == cJSON_Object) {
        ckpkg = cronkite_pack_result(results);
        head = ckpkg;
    }
    else {
        head = NULL;
    }

    /* cJSON cleanup */
    cJSON_Delete(root);
    return head;
}

/* free allocated memory descending a package struct linked list */
void 
cronkite_cleanup(CKPackage *ckpackage) {
    ck_errno = CK_ERR_OK;
    CKPackage *ckpkg = ckpackage;
    CKPackage *next = NULL;

    while (ckpkg) {
        for (int i=0; i<CK_PKG_MAX; i++) {
            if (ckpkg->values[i]) {
                free(ckpkg->values[i]);
            }
        }
        next = ckpkg->next;
        free(ckpkg);
        ckpkg = next;
    }
    if (g_options) {
        if (g_options->aururl) {
            free(g_options->aururl);
        }
        if (g_options->proxyurl) {
            free(g_options->proxyurl);
        }
        free(g_options);
    }
}

/* exported get interface. */
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

    if (!g_options || !g_options->aururl) {
        ck_errno = CK_ERR_URLUNSET;
        return NULL;
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

/* shortcut helper function that converts jsondata directly into
   a cronkite package struct linked list. useful if you just want
   json parsing and package struct building, but not fetching */
CKPackage *
cronkite_json_conv(char *jsondata) {
    ck_errno = CK_ERR_OK;
    CKPackage *pkg_list = cronkite_json_to_packlist(jsondata);
    
    if (!pkg_list) {
        // ck_err should be set by json_to_packlist
        return NULL;
    }

    return pkg_list;
}

/* looks up the error value, and returns a human readable string */
const char *
cronkite_strerror(int ck_err_val) {
    if (ck_err_val < CK_ERR_CURL_OFFSET) {
        switch (ck_err_val) {
            case CK_ERR_CURL_INIT:
                return "Error initializing CURL library calls.";
                break;
            case CK_ERR_EMPTY:
                return "No content! The glass is empty.";
            case CK_ERR_RESP:
                return "Response code not 200 ok.";
                break;
            case CK_ERR_URLUNSET:
                return "URL string is not set.";
                break;
            case CK_ERR_ALLOC:
                return "Failed to allocate memory.";
                break;
            case CK_ERR_PARSE:
                return "Error parsing JSON result.";
                break;
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

/* set options for later use in the library */
void
cronkite_setopt(int opt, const char *val) {
    if (!g_options) {
        g_options = malloc(sizeof(struct CKoptions));
        g_options->aururl = NULL;
        g_options->proxyurl = NULL;
    }
    switch (opt) {
        case CK_OPT_AURURL:
            g_options->aururl = calloc(strlen(val) + 1, sizeof(char));
            strcpy(g_options->aururl, val);
            break;
        case CK_OPT_HTTP_PROXY:
            g_options->proxyurl = calloc(strlen(val) + 1, sizeof(char));
            strcpy(g_options->proxyurl, val);
            break;
        default:
            break;
    }
}

