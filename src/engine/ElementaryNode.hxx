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

#ifndef __ELEMENTARYNODE_HXX__
#define __ELEMENTARYNODE_HXX__

#include "YACSlibEngineExport.hxx"
#include "Node.hxx"
#include "Task.hxx"
#include "define.hxx"

#include <string>
#include <algorithm>

namespace YACS
{
  namespace ENGINE
  {
    class TypeCode;
    class Port;
    class InputPort;
    class OutputPort;
    class ComposedNode;
    class InputDataStreamPort;
    class OutputDataStreamPort;

    class YACSLIBENGINE_EXPORT ElementaryNode : public Node, public Task
    {
      friend class ComposedNode;
    protected:
      std::list<InputPort *> _setOfInputPort;
      std::list<OutputPort *> _setOfOutputPort;
      std::list<InputDataStreamPort *> _setOfInputDataStreamPort;
      std::list<OutputDataStreamPort *> _setOfOutputDataStreamPort;

      // Management of multi property
      bool _createDatastreamPorts;
      bool _multi_port_node;
    protected:
      ElementaryNode(const std::string& name);
      ElementaryNode(const ElementaryNode& other, ComposedNode *father);
      void performDuplicationOfPlacement(const Node& other);
      void performShallowDuplicationOfPlacement(const Node& other);
    public:
      virtual ~ElementaryNode();
      void exUpdateState();
      void init(bool start=true);
      bool isDeployable() const;
      ComponentInstance *getComponent();
      const ComponentInstance *getComponent() const;
      Container *getContainer();
      YACS::StatesForNode getState() const;
      void getReadyTasks(std::vector<Task *>& tasks);
      void edRemovePort(Port *port) throw(Exception);
      std::list<ElementaryNode *> getRecursiveConstituents() const;
      std::list<ProgressWeight> getProgressWeight() const;
      Node *getChildByName(const std::string& name) const throw(Exception);
      virtual void checkBasicConsistency() const throw(Exception);
      ComposedNode *getDynClonerIfExists(const ComposedNode *levelToStop) const;
      int getNumberOfInputPorts() const;
      int getNumberOfOutputPorts() const;
      std::string getInPortName(const InPort *) const throw (Exception);
      std::string getOutPortName(const OutPort *) const throw (Exception);
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      std::list<InputPort *> getSetOfInputPort() const { return _setOfInputPort; }
      std::list<OutputPort *> getSetOfOutputPort() const { return _setOfOutputPort; }
      std::list<InputPort *> getLocalInputPorts() const { return _setOfInputPort; }
      std::list<OutputPort *> getLocalOutputPorts() const { return _setOfOutputPort; }
      std::set<OutPort *> getAllOutPortsLeavingCurrentScope() const;
      std::set<InPort *> getAllInPortsComingFromOutsideOfCurrentScope() const;
      virtual std::vector< std::pair<OutPort *, InPort *> > getSetOfLinksLeavingCurrentScope() const;
      virtual std::vector< std::pair<InPort *, OutPort *> > getSetOfLinksComingInCurrentScope() const;
      std::list<InputDataStreamPort *> getSetOfInputDataStreamPort() const { return _setOfInputDataStreamPort; }
      std::list<OutputDataStreamPort *> getSetOfOutputDataStreamPort() const { return _setOfOutputDataStreamPort; }
      InputDataStreamPort *getInputDataStreamPort(const std::string& name) const throw(Exception);
      OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const throw(Exception);
      virtual InputPort *createInputPort(const std::string& inputPortName, TypeCode* type);
      virtual OutputPort *createOutputPort(const std::string& outputPortName, TypeCode* type);
      virtual InputDataStreamPort *createInputDataStreamPort(const std::string& inputPortDSName, TypeCode* type);
      virtual OutputDataStreamPort *createOutputDataStreamPort(const std::string& outputPortDSName, TypeCode* type);
      virtual InputPort *edAddInputPort(const std::string& inputPortName, TypeCode* type) throw(Exception);
      virtual OutputPort *edAddOutputPort(const std::string& outputPortName, TypeCode* type) throw(Exception);
      virtual InputDataStreamPort *edAddInputDataStreamPort(const std::string& inputPortDSName, TypeCode* type) throw(Exception);
      virtual OutputDataStreamPort *edAddOutputDataStreamPort(const std::string& outputPortDSName, TypeCode* type) throw(Exception);
      virtual void edOrderInputPorts(const std::list<InputPort*>& ports);
      virtual void edOrderOutputPorts(const std::list<OutputPort*>& ports);

      virtual std::string typeName() {return "YACS__ENGINE__ElementaryNode";}
      virtual void edUpdateState();
      virtual void ensureLoading();

      int getMaxLevelOfParallelism() const { return 1; }

      //run part
      void begin();
      bool isReady();
      void finished();
      void aborted();
      void loaded();
      void connected();
      virtual std::string getErrorDetails();
      virtual void initService() { }
      virtual void connectService() { }
      virtual void disconnectService() { }
      virtual void load() { }
      virtual void getCoupledTasks(std::set<Task*>& coupledSet);
      virtual void getCoupledNodes(std::set<Task*>& coupledSet);
      void accept(Visitor *visitor);

      // Used for runtime nodes that need
      // to configure their services for the multi property
      virtual void addDatastreamPortToInitMultiService(const std::string & port_name,
                                                       int number) {}
    protected:
      void initCommonPartWithoutStateManagement(bool start);
      // Management of multi property
      virtual void createMultiDatastreamPorts();

      void edDisconnectAllLinksWithMe();
      bool areAllInputPortsValid() const;
      template<class PORT>
      PORT *getPort(const std::string& name, const std::list<PORT *>& setOfPorts) const throw(Exception);
      template<class PORT, class ENUMTYPE>
      PORT *edAddPort(const std::string& portName, std::list<PORT *>& setOfPorts, ENUMTYPE type) throw(Exception);
      template<class PORT, class ENUMTYPE>
      bool edCheckAddPort(const std::string& portName, std::list<PORT *>& setOfPorts, ENUMTYPE type) throw(Exception);
      template<class PORT>
      static void edRemovePortTypedFromSet(PORT *port, std::list<PORT *>& setOfPorts) throw(Exception);
      template<class PORT>
      static bool isPortNameAlreadyExist(const std::string& portName, const std::list<PORT *>& setOfPorts);
    };

    /**
     * protected: get a port in a list given it's name
     */

    template<class PORT>
    PORT *ElementaryNode::getPort(const std::string& name, const std::list<PORT *>& setOfPorts) const throw(Exception)
    {
      for(typename std::list<PORT *>::const_iterator iter=setOfPorts.begin();iter!=setOfPorts.end();iter++)
        {
          if((*iter)->getName()==name)
            return *iter;
        }
      std::string what="ElementaryNode::getPort : unexisting "; what+=PORT::NAME;
      what+=" with name ";
      what+=name;
      throw Exception(what);
    }

    /**
     * protected: add a port given it's name and type, in a given list of ports
     * WHY TEMPLATE PARAMETER ENUMTYPE?
     */

    template<class PORT, class ENUMTYPE>
    PORT *ElementaryNode::edAddPort(const std::string& portName, std::list<PORT *>& setOfPorts, ENUMTYPE type) throw(Exception)
    {
      checkValidityOfPortName(portName);
      if(isPortNameAlreadyExist<PORT>(portName, setOfPorts))
        {
          std::string what="Port of type "; what+=PORT::NAME; what += " with name : "; what+=portName; what+=" already exists";
          throw Exception(what);
        }
      PORT *ret=new PORT(portName,this,type);
      setOfPorts.push_back(ret);
      return ret;
    }

    template<class PORT, class ENUMTYPE>
    bool ElementaryNode::edCheckAddPort(const std::string& portName, std::list<PORT *>& setOfPorts, ENUMTYPE type) throw(Exception)
    {
      checkValidityOfPortName(portName);
      if(isPortNameAlreadyExist<PORT>(portName, setOfPorts))
        {
          std::string what="Port of type "; what+=PORT::NAME; what += " with name : "; what+=portName; what+=" already exists";
          throw Exception(what);
        }
      return true;
    }

    /**
     * protected: remove a port from a given list
     */

    template<class PORT>
    void ElementaryNode::edRemovePortTypedFromSet(PORT *port, std::list<PORT *>& setOfPorts) throw(Exception)
    {
      if(!isPortNameAlreadyExist<PORT>(port->getName(), setOfPorts))
        throw Exception("Port is not part of the list : unable to remove it");
      typename std::list<PORT *>::iterator iter=std::find(setOfPorts.begin(),setOfPorts.end(),port);
      if(iter!=setOfPorts.end())
        {
          (*iter)->edRemoveAllLinksLinkedWithMe();
          setOfPorts.erase(iter);
        }
    }

    /**
     * protected: checks existence of a port, given it's name, in a list
     */

    template<class PORT>
    bool ElementaryNode::isPortNameAlreadyExist(const std::string& portName, const std::list<PORT *>& setOfPorts)
    {
      for(typename std::list<PORT *>::const_iterator iter=setOfPorts.begin();iter!=setOfPorts.end();iter++)
        {
          if((*iter)->getName()==portName)
            return true;
        }
      return false;
    }
  }
}

#endif
