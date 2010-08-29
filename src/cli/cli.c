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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <cronkite.h>
#include "cli.h"

static void print_pkgs(CKPackage *pkg);
static void print_version();
static void print_help();
static void cleanup();
int main(int argc, char *argv[]);

static void 
print_pkgs(CKPackage *pkg) {
    char *delimiter;
    char *default_delimiter = "\t";
    int rsize = (int) (sizeof(pkg->values)/sizeof(char *));

    delimiter = getenv("CRONKITE_DELIMITER");
    if (delimiter == NULL) {
        delimiter = default_delimiter;
    }

    for (int i=0; i<rsize; i++) {
        if (i < (rsize - 1)) {
            printf("%s%s", pkg->values[i], delimiter);
        }
        else {
            printf("%s\n", pkg->values[i]);
        }
    }
}

static void 
print_version() {
    fprintf(stderr, "%s %s\n", NAME, VERSION);
}

static void 
print_help() {
    fprintf(stderr, "Usage: %s [OPTION] [OPTION-ARGUMENT]\n\n", NAME);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-msearch <search-term>\n");
    fprintf(stderr, "\t\tPerform an aurjson msearch operation.\n");
    fprintf(stderr, "\t-search <search-term>\n");
    fprintf(stderr, "\t\tPerform an aurjson search operation.\n");
    fprintf(stderr, "\t-info <package-name>\n");
    fprintf(stderr, "\t\tPerform an aurjson info operation.\n");
    fprintf(stderr, "\t-version\n\t\tShow version and exit.\n");
    fprintf(stderr, "\t-help\n");
    fprintf(stderr, "\t\tShow simple help and exit.\n");
    fprintf(stderr, "\n");
}

static void 
cleanup() {
    /* flush and close fds */
    fflush(NULL); /* flush all open streams */
    close(0);
    close(1);
    close(2);
}

int 
main(int argc, char *argv[]) {
    CKPackage *results;
    char qtype='s';

    atexit(cleanup);
    /* handle command line arguments */
    if (argc == 2 && strcmp(argv[1],"-help") == 0) {
        print_help();
        return 0;
    }
    else if (argc == 2 && strcmp(argv[1],"-version") == 0) {
        print_version();
        return 0;
    }
    else if (argc == 3 && strcmp(argv[1],"-msearch") == 0) {
        qtype='m';
    }
    else if (argc == 3 && strcmp(argv[1],"-search") == 0) {
        qtype='s';
    }
    else if (argc == 3 && strcmp(argv[1],"-info") == 0) {
        qtype='i';
    }
    else {
        print_help();
        return 1;
    }

    char *aur_url = getenv("CRONKITE_AURURL");
    if (aur_url == NULL) {
        aur_url = "http://aur.archlinux.org/rpc.php?type=%s&arg=%s";
    }
    cronkite_setopt(CK_OPT_AURURL, aur_url);

    char *proxy = getenv("HTTP_PROXY");
    if (proxy) {
        cronkite_setopt(CK_OPT_HTTP_PROXY, proxy);
    }

    results = cronkite_get(qtype, argv[2]);

    if (results) {
        CKPackage *pkg = results;
        while (pkg) {
            print_pkgs(pkg);
            pkg = pkg->next;
        }
    }
    else {
        /* no results or results not readable. Just exit. */
        if (ck_errno == CK_ERR_OK) {
            fprintf(stderr, "%s\n", "No results found.");
            cronkite_cleanup(results);
            return 2;
        }
        else {
            fprintf(stderr, "%s\n", cronkite_strerror(ck_errno));
            cronkite_cleanup(results);
            return 1;
        }
    }

    /* cleanup */
    cronkite_cleanup(results);
    return 0;
}

