#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "py3c.h"
#include "py3c/fileshim.h"

#define PyFile_Check(op) PyObject_TypeCheck(op, &PyType_Type)
#define PyFile_AsFile(in) py3c_PyFile_AsFileWithMode(in, "r")

#ifdef __cplusplus
}
#endif
#endif
