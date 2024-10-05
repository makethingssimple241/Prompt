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
        PyObjectIsNull,
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
    PythonObject(PyObject *object, bool shouldDecRef);
    
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

class PythonDictElement {
public:
    void operator=(PyObject *object);
private:
    PythonDictElement(PyObject *dict, PyObject *key)
        : m_pyObject(dict), m_key(key) {}

    PyObject *m_pyObject = nullptr, *m_key = nullptr;
    
    friend class PythonDict;
};

class PythonDict : public PythonObject {
public:
    PythonDict();
    PythonDict(PyObject *object, bool shouldDecRef = true);
    
    void mergeWith(const PythonDict &dict);
    
    PythonDictElement operator[](PyObject *key);
};

class PythonModule {
public:
    PythonModule(const char *moduleName);
    PythonModule(PyObject *mod);
    
    bool import(const char *moduleName);
    bool importAllFrom(const char *moduleName);
    PythonObject run(const char *str);
    
    PythonDict getDict() const;
    
    operator PyObject *() const { return m_module; }
private:
    PyObject *m_module;
    PythonDict m_globals;
};

class PythonInterpreter {
public:
    static void initialize(const char *root);
    static void finalize();
    
    static bool runString(const char *string);
private:
    inline static PythonModule *s_module = nullptr;
};

#endif /* Python_hpp */
