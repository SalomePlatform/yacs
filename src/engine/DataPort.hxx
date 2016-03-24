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

#ifndef __DATAPORT_HXX__
#define __DATAPORT_HXX__

#include "YACSlibEngineExport.hxx"
#include "Port.hxx"
#include "Exception.hxx"

#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class TypeCode;

    typedef enum
      {
        DATAFLOW,
        DATASTREAM
      } TypeOfChannel;

    class YACSLIBENGINE_EXPORT DataPort : public virtual Port
    {
    protected:
      TypeCode *_type;
      std::string _name;
    public:
      static const char NAME[];
    protected:
      virtual ~DataPort();
      DataPort(const DataPort& other, Node *newHelder);
      DataPort(const std::string& name, Node *node, TypeCode* type);
    public:
      TypeCode* edGetType() const { return _type; }
      void edSetType(TypeCode* type);
      std::string getName() const { return _name; }
      void setName( std::string theName ) { _name = theName; }
      std::string getNameOfTypeOfCurrentInstance() const;
      //! returns type of channel the port will use for data exchange on runtime : DATAFLOW or DATASTREAM.
      virtual TypeOfChannel getTypeOfChannel() const = 0;
      bool isDifferentTypeOf(const DataPort *other) const;
      virtual void edRemoveAllLinksLinkedWithMe() throw(Exception) = 0;
      virtual std::string typeName() {return "YACS__ENGINE__DataPort";}
      virtual std::string getAsString();
    public:
      static DataPort *isCrossingType(const std::vector<DataPort *>& historyOfLink);
    };
  }
}

#endif
