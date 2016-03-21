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

#ifndef __RUNTIMEFORENGINEINTEGRATIONTEST_HXX__
#define __RUNTIMEFORENGINEINTEGRATIONTEST_HXX__

#include "Runtime.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class RuntimeForEngineIntegrationTest : public Runtime
    {
    public:
      static void setRuntime();
      ElementaryNode* createNode(const std::string& implementation, const std::string& name) throw(Exception);
      InputPort* createInputPort(const std::string& name, const std::string& impl, Node * node, TypeCode * type);
      OutputPort* createOutputPort(const std::string& name, const std::string& impl, Node * node, TypeCode * type);
      InputPort* adapt(InputPort* source, const std::string& impl,TypeCode * type,bool init) throw (ConversionException);
      InputPort* adapt(InPropertyPort* source, const std::string& impl, TypeCode * type,bool init) throw (ConversionException);
      ComponentInstance* createComponentInstance(const std::string& name, const std::string& kind="");
    };
  }
}

#endif
