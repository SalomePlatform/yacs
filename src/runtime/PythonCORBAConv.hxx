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

#ifndef __PYTHONCORBACONV_HXX__
#define __PYTHONCORBACONV_HXX__

#include <Python.h>

#include "CORBAPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {

    // --- adaptator ports Python->Corba for several types

    class PyCorbaInt : public ProxyPort
    {
    public:
      PyCorbaInt(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data);
      void put(PyObject *data);
    };

    class PyCorbaDouble : public ProxyPort
    {
    public:
      PyCorbaDouble(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data);
      void put(PyObject *data);
    };

    class PyCorbaString : public ProxyPort
    {
    public:
      PyCorbaString(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data);
      void put(PyObject *data);
    };

    class PyCorbaBool : public ProxyPort
    {
    public:
      PyCorbaBool(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data);
      void put(PyObject *data);
    };

    class PyCorbaObjref : public ProxyPort
    {
    public:
      PyCorbaObjref(InputCorbaPort* p);
      virtual void put(const void *data);
      void put(PyObject *data);
    protected:
      PyObject * _pyorb;
      CORBA::ORB_ptr _orb;
    };

    class PyCorbaSequence : public ProxyPort
    {
    public:
      PyCorbaSequence(InputCorbaPort* p);
      virtual void put(const void *data);
      void put(PyObject *data);
    };

    class PyCorbaStruct : public ProxyPort
    {
    public:
      PyCorbaStruct(InputCorbaPort* p);
      virtual void put(const void *data);
      void put(PyObject *data);
    };

  }
}
#endif
