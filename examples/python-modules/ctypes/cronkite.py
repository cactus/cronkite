##
## Copyright 2009 elij
##
## Licensed under the Apache License, Version 2.0 (the "License"); you may
## not use this file except in compliance with the License. You may obtain
## a copy of the License at
##
## http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
## WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
## License for the specific language governing permissions and limitations
## under the License.
##
from ctypes import *
from ctypes.util import find_library
import sys
import os
import exceptions

class CronkiteException(Exception):
    pass

class CKPackage(Structure):
    pass

CKPackage._fields_ = [("values", c_char_p * 10), ("next", POINTER(CKPackage))]

_libcronkite = cdll.LoadLibrary(find_library("cronkite"))
_libcronkite.cronkite_get.argtypes = [c_char_p, c_char, c_char_p]
_libcronkite.cronkite_get.restype = POINTER(CKPackage)
_libcronkite.cronkite_cleanup.argtypes = [POINTER(CKPackage)]
_libcronkite.cronkite_cleanup.restype = c_void_p

def q_unpack(cpkg):
    return {"id":          cpkg.contents.values[0],
            "url":         cpkg.contents.values[1],
            "name":        cpkg.contents.values[2],
            "version":     cpkg.contents.values[3],
            "urlpath":     cpkg.contents.values[4],
            "license":     cpkg.contents.values[5],
            "numvotes":    cpkg.contents.values[6],
            "outofdate":   cpkg.contents.values[7],
            "categoryid":  cpkg.contents.values[8],
            "description": cpkg.contents.values[9]}

def query(qtype, qstring):
    if not qtype == 's' and not qtype == 'i' and not qtype == 'm':
        raise exceptions.TypeError("argument 1 must be 'i', 's', or 'm'")
        return
    results = []
    pkg = _libcronkite.cronkite_get(
        "http://aur.archlinux.org/rpc.php?type=%s&arg=%s", qtype, qstring)
    if bool(pkg) == False:
        raise CronkiteException("Error communicating with server.")
        return results
    if pkg:
        results.append(q_unpack(pkg))
        next = pkg.contents.next
        while next:
            results.append(q_unpack(next))
            next = next.contents.next
    _libcronkite.cronkite_cleanup(pkg)
    return results
