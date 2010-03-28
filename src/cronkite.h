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

#define CKPKG_VAL_CNT 10
#define DEFAULT_AUR_URL "http://aur.archlinux.org/rpc.php?type=%s&arg=%s"

typedef enum CKPKG_VAL {
    CKPKG_ID = 0,
    CKPKG_URL = 1,
    CKPKG_NAME = 2,
    CKPKG_VERSION = 3,
    CKPKG_URLPATH = 4,
    CKPKG_LICENSE = 5,
    CKPKG_NUMVOTES = 6,
    CKPKG_OUTOFDATE = 7,
    CKPKG_CATEGORYID = 8,
    CKPKG_DESCRIPTION = 9
} CKPKG_VAL;

typedef struct CKPackage {
    char *values[CKPKG_VAL_CNT];
    struct CKPackage *next;
} CKPackage;

typedef enum ck_errors {
    CK_ERR_OK,
    CK_ERR_PARSE,
    CK_ERR_ALLOC,
    CK_ERR_RESP,
    CK_ERR_CURL_INIT,
    CK_ERR_CURL_OFFSET
} CK_ERRORS;

extern int ck_errno;
CKPackage *cronkite_get(const char t, const char *term);
CKPackage *cronkite_json_conv(char *jsondata);
void cronkite_cleanup(CKPackage *ckresult);
const char *cronkite_strerror(int ck_err_val);
void cronkite_seturl(const char *urlfmt);

