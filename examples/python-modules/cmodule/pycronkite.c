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
#include <cronkite.h>
#include <Python.h>

static PyObject *cronkite(PyObject *self, PyObject *args);

static PyObject *cronkite(PyObject *self, PyObject *args) {
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
        return NULL;
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
    {"search", cronkite, METH_VARARGS, 
     "search(qtype, qstring) -> list\n\nPerform a search on the aur."},
    {NULL, NULL, 0, NULL} /* sentinel */
};

PyMODINIT_FUNC initcronkite(void) {
    PyObject *m;

    m = Py_InitModule3("cronkite", CRONKITE_METHODS, 
        "aur search library\n");

    if (m == NULL)
        return;
}

