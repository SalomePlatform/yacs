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

#ifndef __ANYOUTPUTPORT_HXX__
#define __ANYOUTPUTPORT_HXX__

#include "YACSlibEngineExport.hxx"
#include "OutputPort.hxx"
#include "Mutex.hxx"

namespace YACS
{
  namespace ENGINE
  { 
    class Any;

    class YACSLIBENGINE_EXPORT AnyOutputPort : public OutputPort
    {
    public:
      AnyOutputPort(const std::string& name, Node *node, TypeCode *type);
      AnyOutputPort(const AnyOutputPort& other, Node *newHelder);
      virtual ~AnyOutputPort();
      virtual void setValue(Any *data);
      virtual void put(const void *data) throw(ConversionException);
      void put(Any *data) throw(ConversionException);
      //! get the current dispatched value for update port value 
      Any* getValue() const { return _data; }
      virtual std::string getAsString();
      virtual std::string typeName() {return "YACS__ENGINE__AnyOutputPort";}
    private:
      Any* _data; // the data dispatched from port on the current moment
    private:
      OutputPort *clone(Node *newHelder) const;
      YACS::BASES::Mutex _mutex;
    };
  }
}

#endif
