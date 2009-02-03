//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __DATAFLOWPORT_HXX__
#define __DATAFLOWPORT_HXX__

#include "DataPort.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class DataFlowPort : public virtual DataPort
    {
    public:
      static const char NAME[];
    protected:
      DataFlowPort(const DataFlowPort& other, Node *newHelder);
      DataFlowPort(const std::string& name, Node *node, TypeCode* type);
    public:
      std::string getNameOfTypeOfCurrentInstance() const;
      TypeOfChannel getTypeOfChannel() const { return DATAFLOW; }
      virtual ~DataFlowPort();
      virtual std::string typeName() {return "YACS__ENGINE__DataFlowPort";}
      virtual std::string valToStr();
      virtual void valFromStr(std::string valstr);
    };
  }
}

#endif
