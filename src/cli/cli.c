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

