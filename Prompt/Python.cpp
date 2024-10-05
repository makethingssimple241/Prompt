//
//  Python.cpp
//  Prompt
//
//  Created by MakeThingsSimple on 2/10/2024.
//

#include "Python.hpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

static const char *s_pythonExceptionWhat[] = {
    [PythonException::ConstructPythonCallableWithNonCallablePyObject] = "Attempted to construct PythonCallable with PyObject which fails callable check",
    [PythonException::FailedToRunString] = "Failed to run string",
    [PythonException::UnableToFindFunction] = "Unable to find function",
};

PythonException::PythonException(Type type)
    : std::runtime_error(s_pythonExceptionWhat[type]) {}

PythonObject::PythonObject(PyObject *value)
    : PythonObject(value, true) {}

PythonObject::PythonObject(const PythonObject &other)
    : m_pyObject(other.m_pyObject), m_shouldDecRef(true)
{
    Py_XINCREF(other.m_pyObject);
}

PythonObject::~PythonObject()
{
    if (m_shouldDecRef) {
        Py_XDECREF(m_pyObject);
    }
}

PythonStr::PythonStr(const char *s)
    : PythonObject(PyUnicode_FromString(s)) {}

void PythonTupleElement::operator=(PyObject *object)
{
    PyTuple_SetItem(m_pyObject, m_index, object);
}

PythonTuple::PythonTuple()
    : PythonObject(PyTuple_New(0)) {}

PythonTuple::PythonTuple(const std::vector<PyObject *> &objects)
    : PythonObject(PyTuple_New(objects.size()))
{
    for (size_t i = 0; i < objects.size(); ++i) {
        PyTuple_SetItem(m_pyObject, i, objects[i]);
    }
}

PythonCallable::PythonCallable(PyObject *object, bool shouldDecRef)
    : PythonObject(object, shouldDecRef)
{
    if (!PyCallable_Check(object))
    {
        throw PythonException(PythonException::ConstructPythonCallableWithNonCallablePyObject);
    }
}

PythonObject PythonCallable::operator()(const PythonTuple &args)
{
    assert(PyCallable_Check(m_pyObject) && "Object that originally pass the callable check mutated, now unable to pass the check");
    
    return PyObject_CallObject(m_pyObject, args);
}

PythonList::PythonList(PyObject *object)
    : PythonObject(object) {}

void PythonList::append(PyObject *object)
{
    PyList_Append(m_pyObject, object);
}

void PythonInterpreter::initialize(const char *root)
{
    Py_Initialize();
    
    PythonList sysPath = PySys_GetObject("path");
    PythonStr pathStr = root;
    sysPath.append(pathStr);
    
    s_module = PyImport_ImportModule("main");
}

void PythonInterpreter::finalize()
{
    if (Py_FinalizeEx() < 0) {
        abort();
    }
}

void PythonInterpreter::runString(const char *string)
{
    if (PyRun_SimpleString(string) >= 0) {
        return;
    }
    
    throw PythonException(PythonException::FailedToRunString);
}

PythonCallable PythonInterpreter::getFunction(const char *name)
{
    PyObject *function = PyObject_GetAttrString(s_module, name);
    if (!function) {
        throw PythonException(PythonException::UnableToFindFunction);
    }
    
    return PythonCallable(function, false);
}
