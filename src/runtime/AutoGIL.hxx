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

#ifndef __AUTOGIL_HXX__
#define __AUTOGIL_HXX__

#include "Exception.hxx"
#include <Python.h>

namespace YACS
{
  namespace ENGINE
  {
    class AutoGIL
    {
    public:
      AutoGIL():_gstate(PyGILState_Ensure()) { }
      ~AutoGIL() { PyGILState_Release(_gstate); }
    private:
      PyGILState_STATE _gstate;
    };

    class AutoPyRef
    {
    public:
      AutoPyRef(PyObject *pyobj=0):_pyobj(pyobj) { }
      ~AutoPyRef() { release(); }
      AutoPyRef(const AutoPyRef& other):_pyobj(other._pyobj) { if(_pyobj) Py_XINCREF(_pyobj); }
      AutoPyRef& operator=(const AutoPyRef& other) { if(_pyobj==other._pyobj) return *this; release(); _pyobj=other._pyobj; Py_XINCREF(_pyobj); return *this; }
      operator PyObject *() { return _pyobj; }
      void set(PyObject *pyobj) { if(pyobj==_pyobj) return ; release(); _pyobj=pyobj; }
      PyObject *get() { return _pyobj; }
      bool isNull() const { return _pyobj==0; }
      PyObject *retn() { if(_pyobj) Py_XINCREF(_pyobj); return _pyobj; }
    private:
      void release() { if(_pyobj) Py_XDECREF(_pyobj); _pyobj=0; }
    private:
      PyObject *_pyobj;
    };
  }
}

#endif
