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
    [PythonException::PyObjectIsNull] = "PyObject is null",
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

PythonObject::PythonObject(PyObject *object, bool shouldDecRef)
    : m_pyObject(object), m_shouldDecRef(shouldDecRef)
{
    if (!object) {
        throw PythonException(PythonException::PyObjectIsNull);
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

PythonTupleElement PythonTuple::operator[](size_t index)
{
    return {m_pyObject, index};
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

PythonDict::PythonDict()
    : PythonDict(PyDict_New()) {}

PythonDict::PythonDict(PyObject *object, bool shouldDecRef)
    : PythonObject(object, shouldDecRef) {}

void PythonDict::mergeWith(const PythonDict &dict)
{
    PyDict_Merge(m_pyObject, dict, true);
}

PythonDictElement PythonDict::operator[](PyObject *key)
{
    return {m_pyObject, key};
}

PythonModule::PythonModule(const char *moduleName)
    : m_module(PyModule_New(moduleName))
{
    PyModule_AddStringConstant(m_module, "__file__", moduleName);
}

PythonModule::PythonModule(PyObject *mod)
    : m_module(mod) {}

bool PythonModule::import(const char *moduleName)
{
    try {
        PythonModule mod = PyImport_ImportModule(moduleName);
        return PyModule_AddObjectRef(m_module, moduleName, mod) >= 0;
    } catch (const std::runtime_error &error) {
        return false;
    }
}

bool PythonModule::importAllFrom(const char *moduleName)
{
    try {
        PythonModule mod = PyImport_ImportModule(moduleName);
        getDict().mergeWith(mod.getDict());
        return true;
    } catch (const std::runtime_error &error) {
        return false;
    }
}

PythonObject PythonModule::run(const char *str)
{
    PyObject *locals = PyModule_GetDict(m_module);
    return PyRun_String(str, Py_file_input, m_globals, locals);
}

PythonDict PythonModule::getDict() const
{
    return PythonDict(PyModule_GetDict(m_module), false);
}

void PythonInterpreter::initialize(const char *root)
{
    Py_Initialize();
    
    s_module = new PythonModule("Prompt");
    
    PythonList sysPath = PySys_GetObject("path");
    PythonStr pathStr = root;
    sysPath.append(pathStr);
    
    s_module->importAllFrom("prompt");
}

void PythonInterpreter::finalize()
{
    delete s_module;
    
    if (Py_FinalizeEx() < 0) {
        abort();
    }
}

bool PythonInterpreter::runString(const char *string)
{
    return s_module->run(string);
}
