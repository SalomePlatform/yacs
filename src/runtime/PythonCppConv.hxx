// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef __PYTHONCPPCONV_HXX__
#define __PYTHONCPPCONV_HXX__

#include <Python.h>
#include "CppPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    //Proxy port to adapt C++ port to Python port

    class PyCpp : public ProxyPort
    {
    public:
      PyCpp(InputCppPort* p)
         : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) ;
      void put(PyObject *data);
    };
    int isAdaptableCppPyObject(const TypeCode *t1, const TypeCode *t2);
  }
}

#endif
