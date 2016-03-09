// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "PyStdout.hxx"
#include "Exception.hxx"
#include "AutoGIL.hxx"

#include <structmember.h>

#include <string>
#include <sstream>

#ifdef WIN32
#include <process.h>
#define getpid _getpid
#endif

namespace YACS
{
  namespace ENGINE
    {

    typedef struct {
      PyObject_HEAD
      int softspace;
      std::string *out;
    } PyStdOut;

static void
PyStdOut_dealloc(PyStdOut *self)
{
  PyObject_Del(self);
}

static PyObject *
PyStdOut_write(PyStdOut *self, PyObject *args)
{
  char *c;
  int l;
  if (!PyArg_ParseTuple(args, "t#:write",&c, &l))
    return NULL;

  //std::cerr << c ;
  *(self->out)=*(self->out)+std::string(c);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyStdOut_methods[] = {
  {"write",  (PyCFunction)PyStdOut_write,  METH_VARARGS,
    PyDoc_STR("write(string) -> None")},
  {NULL,    NULL}   /* sentinel */
};

static PyMemberDef PyStdOut_memberlist[] = {
  {(char*)"softspace", T_INT,  offsetof(PyStdOut, softspace), 0,
   (char*)"flag indicating that a space needs to be printed; used by print"},
  {NULL} /* Sentinel */
};

static PyTypeObject PyStdOut_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyObject_HEAD_INIT(NULL)
  0,      /*ob_size*/
  "PyOut",   /*tp_name*/
  sizeof(PyStdOut),  /*tp_basicsize*/
  0,      /*tp_itemsize*/
  /* methods */
  (destructor)PyStdOut_dealloc, /*tp_dealloc*/
  0,      /*tp_print*/
  0, /*tp_getattr*/
  0, /*tp_setattr*/
  0,      /*tp_compare*/
  0,      /*tp_repr*/
  0,      /*tp_as_number*/
  0,      /*tp_as_sequence*/
  0,      /*tp_as_mapping*/
  0,      /*tp_hash*/
        0,                      /*tp_call*/
        0,                      /*tp_str*/
        PyObject_GenericGetAttr,                      /*tp_getattro*/
        /* softspace is writable:  we must supply tp_setattro */
        PyObject_GenericSetAttr,    /* tp_setattro */
        0,                      /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT,     /*tp_flags*/
        0,                      /*tp_doc*/
        0,                      /*tp_traverse*/
        0,                      /*tp_clear*/
        0,                      /*tp_richcompare*/
        0,                      /*tp_weaklistoffset*/
        0,                      /*tp_iter*/
        0,                      /*tp_iternext*/
        PyStdOut_methods,                      /*tp_methods*/
        PyStdOut_memberlist,                      /*tp_members*/
        0,                      /*tp_getset*/
        0,                      /*tp_base*/
        0,                      /*tp_dict*/
        0,                      /*tp_descr_get*/
        0,                      /*tp_descr_set*/
        0,                      /*tp_dictoffset*/
        0,                      /*tp_init*/
        0,                      /*tp_alloc*/
        0,                      /*tp_new*/
        0,                      /*tp_free*/
        0,                      /*tp_is_gc*/
};


#define PyStdOut_Check(v)  ((v)->ob_type == &PyStdOut_Type)

PyObject * newPyStdOut( std::string& out )
{
  PyStdOut *self;
  self = PyObject_New(PyStdOut, &PyStdOut_Type);
  if (self == NULL)
    return NULL;
  self->softspace = 0;
  self->out=&out;
  return (PyObject*)self;
}

PyObject *evalPy(const std::string& funcName, const std::string& strToEval)
{
  std::ostringstream oss0; oss0 << "def " << funcName << "():\n";
  std::string::size_type i0(0);
  while(i0<strToEval.length() && i0!=std::string::npos)
    {
      std::string::size_type i2(strToEval.find('\n',i0));
      std::string::size_type lgth(i2!=std::string::npos?i2-i0:std::string::npos);
      std::string part(strToEval.substr(i0,lgth));
      if(!part.empty())
        oss0 << "  " << part << "\n";
      i0=i2!=std::string::npos?i2+1:std::string::npos;
    }
  std::string zeCodeStr(oss0.str());
  std::ostringstream stream;
  stream << "/tmp/PythonNode_";
  stream << getpid();
  AutoPyRef context(PyDict_New());
  PyDict_SetItemString( context, "__builtins__", PyEval_GetBuiltins() );
  AutoPyRef code(Py_CompileString(zeCodeStr.c_str(), "kkkk", Py_file_input));
  if(code.isNull())
    {
      std::string errorDetails;
      PyObject *new_stderr(newPyStdOut(errorDetails));
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);
      std::ostringstream oss; oss << "evalPy failed : " << errorDetails;
      throw Exception(oss.str());
    }
  AutoPyRef res(PyEval_EvalCode(reinterpret_cast<PyCodeObject *>((PyObject *)code),context,context));
  PyObject *ret(PyDict_GetItemString(context,funcName.c_str())); //borrowed ref
  if(!ret)
    throw YACS::Exception("evalPy : Error on returned func !");
  Py_XINCREF(ret);
  return ret;
}

PyObject *evalFuncPyWithNoParams(PyObject *func)
{
  if(!func)
    throw YACS::Exception("evalFuncPyWithNoParams : input func is NULL !");
  AutoPyRef args(PyTuple_New(0));
  AutoPyRef ret(PyObject_CallObject(func,args));
  if(ret.isNull())
    throw YACS::Exception("evalFuncPyWithNoParams : ret is null !");
  return ret.retn();
}

}
}

