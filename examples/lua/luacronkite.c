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

#define LUA_LIB
#define luacronkite_c
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <cronkite.h>

static int ck_test(lua_State *L);
static int ck_setopt(lua_State *L);
static int ck_query(lua_State *L);
LUALIB_API int luaopen_luacronkite(lua_State *L);

static int ck_test(lua_State *L) {
    int n = lua_gettop(L);
    if (n != 1) {
        lua_pushstring(L, "Wrong number of arguments");
        lua_error(L);
    }
    if (!lua_isstring(L, 1)) {
        lua_pushstring(L, "incorrect argument type");
        lua_error(L);
    }
    const char *testv = luaL_checkstring(L, 1);
    lua_pushstring(L, testv);
    return 1;
}

static int ck_setopt(lua_State *L) {
    int n = lua_gettop(L);
    if (n != 2) {
        lua_pushstring(L, "Wrong number of arguments");
        lua_error(L);
    }
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        lua_pushstring(L, "incorrect argument type");
        lua_error(L);
    }

    const char *opt = luaL_checkstring(L, 1);
    const char *val = luaL_checkstring(L, 1);

    if (strcmp(opt, "AURURL") == 0) {
        cronkite_setopt(CK_OPT_AURURL, val);
    }
    else if (strcmp(opt, "HTTP_PROXY") == 0) {
        cronkite_setopt(CK_OPT_HTTP_PROXY, val);
    }
    else {
        lua_pushstring(L, "Option not valid");
        lua_error(L);
    }

    return 0;
}

static int ck_query(lua_State *L) {
    ck_errno = CK_ERR_OK;
    int n = lua_gettop(L);
    CKPackage *results = NULL;
    if (n != 2) {
        lua_pushstring(L, "Wrong number of arguments");
        lua_error(L);
    }
    if (!lua_isstring(L, 1) || !lua_isstring(L,2)) {
        lua_pushstring(L, "incorrect argument type");
        lua_error(L);
    }
    const char *qtype = luaL_checkstring(L, 1);
    const char *qstring = luaL_checkstring(L, 2);
    
    results = cronkite_get(qtype[0], qstring);
    if (!results) {
        // get ck_error and raise exception with it
        const char *errmsg = cronkite_strerror(ck_errno);
        lua_pushstring(L, errmsg);
        lua_error(L);
    }

    CKPackage *pkg = results;
    // lua tables start at 1
    int i = 1;
    lua_newtable(L);
    int outer_top = lua_gettop(L);
    int inner_top = 0;
    // ensure stack is large enough for building inner table in loop
    lua_checkstack(L, 30);
    while (pkg) {
        // push array index for parent table
        lua_pushinteger(L, i);

        // build new package table
        lua_newtable(L);
        inner_top = lua_gettop(L);
        lua_pushstring(L, "id");
        lua_pushstring(L, pkg->values[0]);
        lua_pushstring(L, "url");
        lua_pushstring(L, pkg->values[1]);
        lua_pushstring(L, "name");
        lua_pushstring(L, pkg->values[2]);
        lua_pushstring(L, "version");
        lua_pushstring(L, pkg->values[3]);
        lua_pushstring(L, "urlpath");
        lua_pushstring(L, pkg->values[4]);
        lua_pushstring(L, "license");
        lua_pushstring(L, pkg->values[5]);
        lua_pushstring(L, "numvotes");
        lua_pushstring(L, pkg->values[6]);
        lua_pushstring(L, "outofdate");
        lua_pushstring(L, pkg->values[7]);
        lua_pushstring(L, "categoryid");
        lua_pushstring(L, pkg->values[8]);
        lua_pushstring(L, "description");
        lua_pushstring(L, pkg->values[9]);
        // build inner table
        for (int j=0; j<10; j++) {
            lua_settable(L, inner_top);
        }
        // add inner table to the outer table
        lua_settable(L, outer_top);
        i++;
        pkg = pkg->next;
    }
    // do cronkite cleanup
    cronkite_cleanup(results);
    return 1;
}

static const struct luaL_Reg ck_fcs[] = {
    {"setopt", ck_setopt},
    {"query", ck_query},
    {"test", ck_test},
    {NULL, NULL} /* sentinal */
};

LUALIB_API int luaopen_luacronkite(lua_State *L) {
    lua_newtable(L);
    luaL_register(L, NULL, ck_fcs);
    return 1;
}

