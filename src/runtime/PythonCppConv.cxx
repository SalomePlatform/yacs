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

#include "PythonCppConv.hxx"
#include "TypeConversions.hxx"
#include "RuntimeSALOME.hxx"
#include "PythonPorts.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS
{
  namespace ENGINE
  {
    Any * convertPyObjectCpp(const TypeCode *t, PyObject *data)
    {
      return convertPyObjectNeutral(t, data);
    }

    void PyCpp::put(const void *data)  throw(ConversionException)
    {
      put((PyObject *)data);
    }

    //!Convertit un PyObject de type entier en YACS::ENGINE::Any entier
    /*!
     *   \param data : python object
     */

    void PyCpp::put(PyObject *data)  throw(ConversionException)
    {
      YACS::ENGINE::Any *a;
      //Do not need to take the Python GIL as put is called from Python node
      a = convertPyObjectCpp(edGetType(), data);
      YACS::ENGINE::InterpreterSaveThread _loc;
      DEBTRACE( "before put refcnt: " << a->getRefCnt() );
      _port->put(a);
      //_port has called incRef
      a->decrRef();
      DEBTRACE( "after put refcnt: " << a->getRefCnt() );
    }

    int isAdaptableCppPyObject(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableNeutralPyObject(t1, t2);
    }
  }
}
