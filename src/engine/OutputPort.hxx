// Copyright (C) 2006-2023  CEA/DEN, EDF R&D
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

#ifndef __OUTPUTPORT_HXX__
#define __OUTPUTPORT_HXX__

#include "YACSlibEngineExport.hxx"
#include "OutPort.hxx"
#include "DataFlowPort.hxx"
#include "ConversionException.hxx"

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class InPort;
    class Runtime;
    class InputPort;
    class InPropertyPort;
    class OptimizerLoop;
    class ElementaryNode;
    class CollectorSwOutputPort;

    class YACSLIBENGINE_EXPORT OutputPort : public DataFlowPort, public OutPort
    {
      friend class CollectorSwOutputPort; // for conect
      friend class ElementaryNode;        // for disconnect...
      friend class OptimizerLoop;         // for interceptors
      friend class InputPort;
      friend class Runtime;               // for port creation
      friend class ForLoop;               // for port creation
    public:
      virtual ~OutputPort();
      std::set<InPort *> edSetInPort() const;
      bool isAlreadyLinkedWith(InPort *withp) const;
      bool isAlreadyInSet(InputPort *inputPort) const;
      bool isConnected() const;
      std::string getNameOfTypeOfCurrentInstance() const;
      int removeInPort(InPort *inPort, bool forward) ;
      virtual bool edAddInputPort(InputPort *phyPort) ;
      virtual bool edAddInPropertyPort(InPropertyPort *phyPort) ;
      virtual int edRemoveInputPort(InputPort *inputPort, bool forward) ;
      bool addInPort(InPort *inPort) ;
      void edRemoveAllLinksLinkedWithMe() ;//entry point for forward port deletion
      virtual void exInit();
      virtual void checkBasicConsistency() const ;
      virtual OutputPort *clone(Node *newHelder) const = 0;
      virtual std::string dump();

      virtual void put(const void *data);
      virtual std::string typeName() {return "YACS__ENGINE__OutputPort";}

    protected:
      OutputPort(const OutputPort& other, Node *newHelder);
      OutputPort(const std::string& name, Node *node, TypeCode* type);
    protected:
      const std::set<InputPort *>& getSetOfPhyLinks() const;
    protected:
      std::set<InputPort *> _setOfInputPort;//Key is for physical Data link
    public:
      static const char NAME[];
    };
  }
}

#endif
