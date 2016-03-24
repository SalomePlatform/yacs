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

#include "CppPythonConv.hxx"
#include "TypeConversions.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS 
{
  namespace ENGINE
  {

    PyObject *convertCppPyObject(const TypeCode *t, Any *a)
    {
      return convertNeutralPyObject(t, (Any *) a);
    }

    CppPy::CppPy(InputPyPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
    }

    //!Convert the received C++ value to PyObject * and send it to proxy port
    /*!
     *   \param data : C++ value received 
     */
    void CppPy::put(const void *data) throw(ConversionException)
    {
      put((Any *) data);
    }

    void CppPy::put(Any *data) throw(ConversionException)
    {
      DEBTRACE(" CppPython::put(Any *data)");
      PyObject *ob;
      InterpreterUnlocker l;
      ob=convertCppPyObject(edGetType(), data);
#ifdef _DEVDEBUG_
      PyObject_Print(ob,stderr,Py_PRINT_RAW);
      std::cerr << std::endl;
#endif
      DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
      _port->put(ob);
      Py_DECREF(ob);
      DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
    }

    int isAdaptablePyObjectCpp(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptablePyObjectNeutral(t1, t2);
    }

  }
}
