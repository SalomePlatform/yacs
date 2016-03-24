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

#ifndef __CONDITIONINPUTPORT_HXX__
#define __CONDITIONINPUTPORT_HXX__

#include "YACSlibEngineExport.hxx"
#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class WhileLoop;

    class YACSLIBENGINE_EXPORT ConditionInputPort : public InputPort 
    {
      friend class WhileLoop;
    private:
      OutPort *_outOfScopeBackLink;
      ConditionInputPort(const std::string& name, WhileLoop *node);
      ConditionInputPort(const ConditionInputPort& other, Node *newHelder);
      virtual ~ConditionInputPort();
      InputPort *clone(Node *newHelder) const;
    public:
      void exSaveInit();
      void exRestoreInit();
      bool isLinkedOutOfScope() const;
      void edNotifyReferencedBy(OutPort *fromPort);
      void edNotifyDereferencedBy(OutPort *fromPort);
      void *get() const;
      virtual void put(const void *data) throw(ConversionException);
      void put(Any *data) throw(ConversionException);
      std::string dump();
      virtual std::string getAsString();
    protected:
      Any *_value;
    public:
      bool getValue() const { return (_value ? _value->getBoolValue() : false); }
      virtual std::string typeName() {return "YACS__ENGINE__ConditionInputPort";}
    };
  }
}

#endif
