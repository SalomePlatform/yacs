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

#ifndef __NEUTRALPYTHONCONV_HXX__
#define __NEUTRALPYTHONCONV_HXX__

#include "PythonPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // --- adaptator ports Neutral->Python for several types

    class NeutralPyDouble : public ProxyPort
    {
    public:
      NeutralPyDouble(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPyInt : public ProxyPort
    {
    public:
      NeutralPyInt(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPyString : public ProxyPort
    {
    public:
      NeutralPyString(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPyBool : public ProxyPort
    {
    public:
      NeutralPyBool(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPyObjref : public ProxyPort
    {
    public:
      NeutralPyObjref(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPySequence : public ProxyPort
    {
    public:
      NeutralPySequence(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPyStruct : public ProxyPort
    {
    public:
      NeutralPyStruct(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };
  }
}
#endif
