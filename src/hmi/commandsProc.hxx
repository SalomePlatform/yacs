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
#ifndef _COMMANDSPROC_HXX_
#define _COMMANDSPROC_HXX_

#include "commands.hxx"

#include <list>
#include <stack>
#include <string>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
    class Node;
    class TypeCode;
    class Catalog;
    class DataPort;
    class InputPort;
    class OutputPort;
    class InputDataStreamPort;
    class OutputDataStreamPort;
    class Container;
    class ComponentInstance;
  }

  namespace HMI
  {

    typedef enum
      {
        SALOMEPROC,
        BLOC,
        FOREACHLOOP,
        OPTIMIZERLOOP,
        FORLOOP,
        WHILELOOP,
        SWITCH,
        PYTHONNODE,
        PYFUNCNODE,
        CORBANODE,
        SALOMENODE,
        CPPNODE,
        SALOMEPYTHONNODE,
        XMLNODE,
        SPLITTERNODE,
        DFTODSFORLOOPNODE,
        DSTODFFORLOOPNODE,
        PRESETNODE,
        OUTNODE,
        STUDYINNODE,
        STUDYOUTNODE,
        INPUTPORT,
        OUTPUTPORT,
        INPUTDATASTREAMPORT,
        OUTPUTDATASTREAMPORT,
	DATALINK,
	CONTROLLINK,
        CONTAINER,
        COMPONENT,
        REFERENCE,
        DATATYPE,
        UNKNOWN
      } TypeOfElem;
   
    class ProcInvoc: public Invocator
    {
    public:
      ProcInvoc();
      static TypeOfElem getTypeOfNode(YACS::ENGINE::Node* node);
      static TypeOfElem getTypeOfPort(YACS::ENGINE::DataPort* port);
      static std::string getTypeName(TypeOfElem type);
    protected:
      static std::map<int, std::string> _typeNameMap;
    };
    

    class CommandAddNodeFromCatalog: public Command
    {
    public:
      CommandAddNodeFromCatalog(YACS::ENGINE::Catalog *catalog,
                                std::string compo,
                                std::string type,
                                std::string position,
                                std::string name,
                                int swCase =0);
      YACS::ENGINE::Node *getNode();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      YACS::ENGINE::Catalog* _catalog;
      YACS::ENGINE::Node *_nodeToClone;
      TypeOfElem  _typeNode;
      std::string _compoName;
      std::string _typeName;
      std::string _position;
      std::string _name;
      int _swCase;
      YACS::ENGINE::Node *_node;
    };

    class CommandReparentNode: public Command
    {
    public:
      CommandReparentNode(std::string position,
                          std::string newParent);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _position;
      std::string _newParent;
    };

    class CommandCopyNode: public Command
    {
    public:
      CommandCopyNode(std::string position,
                      std::string newParent);
      YACS::ENGINE::Node *getNode();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _position;
      std::string _newParent;
      YACS::ENGINE::Node *_clone;
    };

    class CommandRenameNode: public Command
    {
    public:
      CommandRenameNode(std::string position, std::string name);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _position;
      std::string _name;
    };

    class CommandRenameContainer: public Command
    {
    public:
      CommandRenameContainer(std::string oldName, std::string newName);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _oldName;
      std::string _newName;
    };

    class CommandRenameInDataPort: public Command
    {
    public:
      CommandRenameInDataPort(std::string position,
                              std::string oldName,
                              std::string newName);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _position;
      std::string _oldName;
      std::string _newName;
    };

    class CommandRenameOutDataPort: public Command
    {
    public:
      CommandRenameOutDataPort(std::string position,
                               std::string oldName,
                               std::string newName);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _position;
      std::string _oldName;
      std::string _newName;
    };

    class CommandAddDataTypeFromCatalog: public Command
    {
    public:
      CommandAddDataTypeFromCatalog(YACS::ENGINE::Catalog* catalog,
                                    std::string typeName);
      YACS::ENGINE::TypeCode *getTypeCode();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      YACS::ENGINE::Catalog* _catalog;
      std::string _typeName;
    };

    class CommandAddInputPortFromCatalog: public Command
    {
    public:
      CommandAddInputPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                     std::string type,
                                     std::string node,
                                     std::string name);
      YACS::ENGINE::InputPort *getInputPort();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      YACS::ENGINE::Catalog* _catalog;
      std::string _typePort;
      std::string _node;
      std::string _name;
      YACS::ENGINE::InputPort *_inputPort;
    };

    class CommandAddOutputPortFromCatalog: public Command
    {
    public:
      CommandAddOutputPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                      std::string type,
                                      std::string node,
                                      std::string name);
      YACS::ENGINE::OutputPort *getOutputPort();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      YACS::ENGINE::Catalog* _catalog;
      std::string _typePort;
      std::string _node;
      std::string _name;
      YACS::ENGINE::OutputPort *_outputPort;
    };

    class CommandAddIDSPortFromCatalog: public Command
    {
    public:
      CommandAddIDSPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                   std::string type,
                                   std::string node,
                                   std::string name);
      YACS::ENGINE::InputDataStreamPort *getIDSPort();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      YACS::ENGINE::Catalog* _catalog;
      std::string _typePort;
      std::string _node;
      std::string _name;
      YACS::ENGINE::InputDataStreamPort *_IDSPort;
    };

    class CommandAddODSPortFromCatalog: public Command
    {
    public:
      CommandAddODSPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                   std::string type,
                                   std::string node,
                                   std::string name);
      YACS::ENGINE::OutputDataStreamPort *getODSPort();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      YACS::ENGINE::Catalog* _catalog;
      std::string _typePort;
      std::string _node;
      std::string _name;
      YACS::ENGINE::OutputDataStreamPort *_ODSPort;
    };

    class CommandOrderInputPorts: public Command
    {
    public:
      CommandOrderInputPorts(std::string node,
                             std::string port,
                             int isUp);
      int getRank() {return _rank; };
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
      std::string _port;
      int _isUp;
      int _rank;
    };

    class CommandOrderOutputPorts: public Command
    {
    public:
      CommandOrderOutputPorts(std::string node,
                              std::string port,
                              int isUp);
      int getRank() {return _rank; };
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
      std::string _port;
      int _isUp;
      int _rank;
    };

    class CommandSetInPortValue: public Command
    {
    public:
      CommandSetInPortValue(std::string node,
                            std::string port,
                            std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
      std::string _port;
      std::string _value;
    };

    class CommandSetOutPortValue: public Command
    {
    public:
      CommandSetOutPortValue(std::string node,
                             std::string port,
                             std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
      std::string _port;
      std::string _value;
    };

    class CommandSetSwitchSelect: public Command
    {
    public:
      CommandSetSwitchSelect(std::string aSwitch,
                             std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _switch;
      std::string _value;
    };

    class CommandSetSwitchCase: public Command
    {
    public:
      CommandSetSwitchCase(std::string aSwitch,
                           std::string node,
                           std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _switch;
      std::string _node;
      std::string _value;
    };

    class CommandSetForLoopSteps: public Command
    {
    public:
      CommandSetForLoopSteps(std::string forLoop,
                             std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _forLoop;
      std::string _value;
    };

    class CommandSetWhileCondition: public Command
    {
    public:
      CommandSetWhileCondition(std::string whileLoop,
                               std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _whileLoop;
      std::string _value;
    };

    class CommandSetForEachBranch: public Command
    {
    public:
      CommandSetForEachBranch(std::string forEach,
                              std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _forEach;
      std::string _value;
    };

    class CommandAddLink: public Command
    {
    public:
      CommandAddLink(std::string outNode, std::string outPort,
                     std::string inNode, std::string inPort,bool control=true);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _outNode;
      std::string _outPort;
      std::string _inNode;
      std::string _inPort;
      bool _control;
    };

    class CommandAddControlLink: public Command
    {
    public:
      CommandAddControlLink(std::string outNode, std::string inNode);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _outNode;
      std::string _inNode;
    };

    class CommandAddContainer: public Command
    {
    public:
      CommandAddContainer(std::string name,
                          std::string refContainer ="");
      virtual YACS::ENGINE::Container* getContainer();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _name;
      std::string _containerToClone;
      YACS::ENGINE::Container* _container;
    };

    class CommandSetContainerProperties: public Command
    {
    public:
      CommandSetContainerProperties(std::string container,
                                    std::map<std::string,std::string> properties);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _container;
      std::map<std::string,std::string> _properties;
    };

    class CommandSetDSPortProperties: public Command
    {
    public:
      CommandSetDSPortProperties(std::string node, std::string port, bool isInport,
                                 std::map<std::string,std::string> properties);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _nodeName;
      std::string _portName;
      bool _isInport;
      std::map<std::string,std::string> _properties;
    };

    class CommandSetFuncNodeFunctionName: public Command
    {
    public:
      CommandSetFuncNodeFunctionName(std::string node, std::string funcName);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _nodeName;
      std::string _funcName;
    };

    class CommandSetInlineNodeScript: public Command
    {
    public:
      CommandSetInlineNodeScript(std::string node, std::string script);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _nodeName;
      std::string _script;
    };

    class CommandAddComponentInstance: public Command
    {
    public:
      CommandAddComponentInstance(std::string compoName);
      virtual YACS::ENGINE::ComponentInstance* getComponentInstance();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _compoName;
      YACS::ENGINE::ComponentInstance *_compoInst;
    };

    class CommandAssociateComponentToContainer: public Command
    {
    public:
      CommandAssociateComponentToContainer(std::pair<std::string,int> key,
                                           std::string container);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::pair<std::string,int>  _key;
      std::string _container;
    };

    class CommandAssociateServiceToComponent: public Command
    {
    public:
      CommandAssociateServiceToComponent(std::string service,
                                         std::pair<std::string,int> key);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _service;
      std::pair<std::string,int>  _key;
    };

    class Subject;
    class CommandDestroy: public Command
    {
    public:
      CommandDestroy(std::string position, Subject* subject);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
    protected:
      std::string _position;
      Subject* _subject;
    };

  }
}
#endif
