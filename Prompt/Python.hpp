//
//  Python.hpp
//  Prompt
//
//  Created by MakeThingsSimple on 1/10/2024.
//

#ifndef Python_hpp
#define Python_hpp

#include <vector>

typedef struct _object PyObject;

class PythonException : public std::runtime_error {
public:
    enum Type {
        ConstructPythonCallableWithNonCallablePyObject,
        FailedToRunString,
        UnableToFindFunction,
    };
    
    PythonException(Type type);
};

class PythonObject {
public:
    PythonObject(PyObject *object);
    PythonObject(const PythonObject &other);
    ~PythonObject();
    
    operator PyObject *() const { return m_pyObject; }
protected:
    PythonObject(PyObject *object, bool shouldDecRef)
        : m_pyObject(object), m_shouldDecRef(shouldDecRef) {}
    
    PyObject *m_pyObject = nullptr;
    bool m_shouldDecRef = false;
};

class PythonStr : public PythonObject {
public:
    PythonStr(const char *s);
};

class PythonTupleElement {
public:
    void operator=(PyObject *object);
private:
    PythonTupleElement(PyObject *tuple, size_t index)
        : m_pyObject(tuple), m_index(index) {}
    
    PyObject *m_pyObject = nullptr;
    size_t m_index = 0;
    
    friend class PythonTuple;
};

class PythonTuple : public PythonObject {
public:
    PythonTuple();
    PythonTuple(const std::vector<PyObject *> &objects);
    
    PythonTupleElement operator[](size_t index);
};

class PythonCallable : public PythonObject {
public:
    PythonCallable(PyObject *object, bool shouldDecRef = true);
    
    PythonObject operator()(const PythonTuple &args = {});
};

class PythonList : public PythonObject {
public:
    PythonList(PyObject *object);
    
    void append(PyObject *object);
};

class PythonInterpreter {
public:
    static void initialize(const char *root);
    static void finalize();
    
    static void runString(const char *string);
    
    static PythonCallable getFunction(const char *name);
private:
    inline static PyObject *s_module;
};

#endif /* Python_hpp */
