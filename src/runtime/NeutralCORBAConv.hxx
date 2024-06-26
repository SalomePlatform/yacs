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

#ifndef __NEUTRALCORBACONV_HXX__
#define __NEUTRALCORBACONV_HXX__

#include "ConversionException.hxx"
#include "CORBAPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Any;

    // --- adaptator ports Neutral->Corba for several types

    class NeutralCorbaInt : public ProxyPort
    {
    public:
      NeutralCorbaInt(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data);
      void put(YACS::ENGINE::Any *data);
    };

    class NeutralCorbaDouble : public ProxyPort
    {
    public:
      NeutralCorbaDouble(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data);
      void put(YACS::ENGINE::Any *data);
    };

    class NeutralCorbaBool : public ProxyPort
    {
    public:
      NeutralCorbaBool(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data);
      void put(YACS::ENGINE::Any *data);
    };

    class NeutralCorbaSequence : public ProxyPort
    {
    public:
      NeutralCorbaSequence(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data);
      void put(YACS::ENGINE::Any *data);
    };

    class NeutralCorbaString : public ProxyPort
    {
    public:
      NeutralCorbaString(InputCorbaPort* p);
      virtual void put(const void *data);
      void put(YACS::ENGINE::Any *data);
    };

    class NeutralCorbaObjref : public ProxyPort
    {
    public:
      NeutralCorbaObjref(InputCorbaPort* p);
      virtual void put(const void *data);
      void put(YACS::ENGINE::Any *data);
    };

    class NeutralCorbaStruct : public ProxyPort
    {
    public:
      NeutralCorbaStruct(InputCorbaPort* p);
      virtual void put(const void *data);
      void put(YACS::ENGINE::Any *data);
    };
  }
}
#endif
