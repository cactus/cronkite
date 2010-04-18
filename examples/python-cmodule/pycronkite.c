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

#include <stdlib.h>
#include <string.h>
#include <Python.h>
#include <cronkite.h>

static PyObject *ck_query(PyObject *self, PyObject *args);
static PyObject *ck_setopt(PyObject *self, PyObject *args);
static PyObject *CronkiteError;

static PyObject *ck_setopt(PyObject *self, PyObject *args){
    char *ovalue;
    char *ck_option;

    if (!PyArg_ParseTuple(args, "ss", &ck_option, &ovalue)) {
        return NULL;
    }

    if (strcmp(ck_option, "AURURL") == 0) {
        cronkite_setopt(CK_OPT_AURURL, ovalue);
    }
    else if (strcmp(ck_option, "HTTP_PROXY") == 0) {
        cronkite_setopt(CK_OPT_HTTP_PROXY, ovalue);
    }
    else {
        PyErr_SetString(PyExc_ValueError, "Option not valid");
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ck_query(PyObject *self, PyObject *args) {
    ck_errno = CK_ERR_OK;
    char *qtype = NULL;
    char *qstring = NULL;
    CKPackage *results = NULL;
    PyObject * list = NULL;

    if (!PyArg_ParseTuple(args, "ss", &qtype, &qstring)) {
        return NULL;
    }

    if (!(strlen(qtype) > 0) && \
        (qtype[0] != 's' || qtype[0] != 'm' || qtype[0] != 'i')) {
        return NULL;
    }
    results = cronkite_get(qtype[0], qstring);

    if (!results) {
        // get ck_error and raise exception with it
        const int ckerr = ck_errno;
        // fprintf(stderr, "%d", ckerr);
        const char *errmsg = cronkite_strerror(ckerr);
        // check to see if it a memory allocation error.
        // if so, propagate that to python as required.
        // otherwise just return a custom error
        if (ckerr == CK_ERR_ALLOC) {
            return PyErr_NoMemory();
        }
        else {
            PyErr_SetString(CronkiteError, errmsg);
            return NULL;
        }
    }

    list = PyList_New(0);
    CKPackage *pkg = results;
    while (pkg) {
        PyObject *elem = Py_BuildValue(
                "{s:s,s:s,s:s,s:s,s:s,s:s,s:s,s:s,s:s,s:s}",
                "id", pkg->values[0],
                "url", pkg->values[1],
                "name", pkg->values[2],
                "version", pkg->values[3],
                "urlpath", pkg->values[4],
                "license", pkg->values[5],
                "numvotes", pkg->values[6],
                "outofdate", pkg->values[7],
                "categoryid", pkg->values[8],
                "description", pkg->values[9]);
        PyList_Append(list, elem);
        /* I don't believe append (above) steals a reference, so should
           decrement here to release elem */
        Py_DECREF(elem);
        pkg = pkg->next;
    }
    cronkite_cleanup(results);
    return list;
}

static PyMethodDef CRONKITE_METHODS[] = {
    {"query", ck_query, METH_VARARGS, 
        "query(qtype, qstring) -> list\n\nPerform a search on the aur."},
    {"setopt", ck_setopt, METH_VARARGS,
        "setopt(option, value) -> None\n\nSet cronkite option."}, 
    {NULL, NULL, 0, NULL} /* sentinel */
};

PyMODINIT_FUNC initcronkite(void) {
    PyObject *m;

    m = Py_InitModule3("cronkite", CRONKITE_METHODS, 
        "aur search library\n");

    if (m == NULL)
        return;

    CronkiteError = PyErr_NewException("cronkite.CronkiteException", NULL, NULL);
    Py_INCREF(CronkiteError);
    PyModule_AddObject(m, "CronkiteException", CronkiteError);
}

