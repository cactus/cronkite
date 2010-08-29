/* 
 * Copyright (c) 2009-2010 elij
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
