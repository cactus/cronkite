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

typedef enum CK_Option {
    CK_OPT_AURURL,
    CK_OPT_HTTP_PROXY
} CK_Option;

typedef enum CK_Error {
    CK_ERR_OK,
    CK_ERR_PARSE,
    CK_ERR_ALLOC,
    CK_ERR_URLUNSET,
    CK_ERR_RESP,
    CK_ERR_EMPTY,
    CK_ERR_CURL_INIT,
    // sigil. always at the bottom. required for curl error nums.
    CK_ERR_CURL_OFFSET
} CK_Error;

typedef enum CK_Pgkval {
    CK_PKG_ID,
    CK_PKG_URL,
    CK_PKG_NAME,
    CK_PKG_VERSION,
    CK_PKG_URLPATH,
    CK_PKG_LICENSE,
    CK_PKG_NUMVOTES,
    CK_PKG_OUTOFDATE,
    CK_PKG_CATEGORYID,
    CK_PKG_DESCRIPTION,
    // sigil. always at the bottom. required for looping/sizing later.
    CK_PKG_MAX
} CK_Pkgval;

typedef struct CKPackage {
    char *values[CK_PKG_MAX];
    struct CKPackage *next;
} CKPackage;

extern int ck_errno;
CKPackage *cronkite_get(const char t, const char *term);
CKPackage *cronkite_json_conv(char *jsondata);
void cronkite_cleanup(CKPackage *ckresult);
const char *cronkite_strerror(int ck_err_val);
void cronkite_setopt(int opt, const char *val);
