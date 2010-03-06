from ctypes import *
from ctypes.util import find_library
import sys
import os
import exceptions

class CKPackage(Structure):
    pass

CKPackage._fields_ = [("values", c_char_p * 10), ("next", POINTER(CKPackage))]

_libcronkite = cdll.LoadLibrary(find_library("cronkite"))
_libcronkite.cronkite_get.argtypes = [c_char, c_char_p]
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
    pkg = _libcronkite.cronkite_get(qtype, qstring)
    if pkg:
        results.append(q_unpack(pkg))
        next = pkg.contents.next
        while next:
            results.append(q_unpack(next))
            next = next.contents.next
    _libcronkite.cronkite_cleanup(pkg)
    return results
