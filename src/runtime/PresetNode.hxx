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

#ifndef _PRESETNODE_HXX_
#define _PRESETNODE_HXX_

#include "YACSRuntimeSALOMEExport.hxx"
#include "DataNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class YACSRUNTIMESALOME_EXPORT PresetNode: public DataNode
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      PresetNode(const std::string& name);
      PresetNode(const PresetNode& other, ComposedNode *father);
      virtual void execute();
      virtual void accept(Visitor *visitor);
      virtual void checkBasicConsistency() const throw(Exception);
      virtual void setData(OutputPort* port, const std::string& data);
      virtual OutputPort *createOutputPort(const std::string& outputPortName, TypeCode* type);
      virtual std::string typeName() {return "YACS__ENGINE__PresetNode";}
    public:
      static const char IMPL_NAME[];
    };
  }
}

#endif
