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

#ifndef _COMMANDSPROC_HXX_
#define _COMMANDSPROC_HXX_

#include "HMIExport.hxx"
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
    class SubjectNode;
    class SubjectInputPort;
    class SubjectOutputPort;
    class SubjectInputDataStreamPort;
    class SubjectOutputDataStreamPort;
    class SubjectLink;
    class SubjectControlLink;
    class SubjectContainerBase;
    class SubjectComponent;

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
   
    class HMI_EXPORT ProcInvoc: public Invocator
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
                                bool newCompoInst=true,
                                int swCase =0);
      YACS::ENGINE::Node *getNode();
      YACS::HMI::SubjectNode *getSubjectNode();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      YACS::ENGINE::Catalog* _catalog;
      TypeOfElem  _typeNode;
      std::string _compoName;
      std::string _typeName;
      std::string _position;
      std::string _name;
      bool _newCompoInst;
      int _swCase;
      YACS::ENGINE::Node *_node;
      YACS::HMI::SubjectNode *_snode;
    };

    class CommandReparentNode: public Command
    {
    public:
      CommandReparentNode(std::string position,
                          std::string newParent);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _position;
      std::string _newParent;
      std::string _oldParent;
      std::string _newpos;
    };

    class CommandPutInComposedNode: public Command
    {
    public:
      CommandPutInComposedNode(std::string position,
			       std::string newParent,
			       std::string type,
			       bool toSaveRestoreLinks=true);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _position;
      std::string _newParent;
      std::string _type;
      std::string _newpos;
      bool _toSaveRestoreLinks;
    };

    class CommandCopyNode: public Command
    {
    public:
      CommandCopyNode(YACS::ENGINE::Proc* fromproc,
                      std::string position,
                      std::string newParent,
                      int acase=0);
      YACS::ENGINE::Node *getNode();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _position;
      std::string _newParent;
      std::string _newName;
      int         _case;
      YACS::ENGINE::Node *_clone;
      YACS::ENGINE::Proc *_fromproc;
    };

    class CommandRenameNode: public Command
    {
    public:
      CommandRenameNode(std::string position, std::string name);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _position;
      std::string _name;
      std::string _oldName;
      std::string _newpos;
    };

    class CommandRenameContainer: public Command
    {
    public:
      CommandRenameContainer(std::string oldName, std::string newName);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _oldName;
      std::string _newName;
    };

    class CommandRenameInDataPort: public Command
    {
    public:
      CommandRenameInDataPort(std::string position,
                              std::string oldName,
                              std::string newName, TypeOfElem portType);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _position;
      std::string _oldName;
      std::string _newName;
      TypeOfElem _portType;
    };

    class CommandRenameOutDataPort: public Command
    {
    public:
      CommandRenameOutDataPort(std::string position,
                               std::string oldName,
                               std::string newName, TypeOfElem portType);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _position;
      std::string _oldName;
      std::string _newName;
      TypeOfElem _portType;
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
      virtual std::string dump();
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
      SubjectInputPort* getSubjectInputPort();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      YACS::ENGINE::Catalog* _catalog;
      std::string _typePort;
      std::string _node;
      std::string _name;
      YACS::ENGINE::InputPort *_inputPort;
      SubjectInputPort* _sip;
    };

    class CommandAddOutputPortFromCatalog: public Command
    {
    public:
      CommandAddOutputPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                      std::string type,
                                      std::string node,
                                      std::string name);
      YACS::ENGINE::OutputPort *getOutputPort();
      SubjectOutputPort* getSubjectOutputPort();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      YACS::ENGINE::Catalog* _catalog;
      std::string _typePort;
      std::string _node;
      std::string _name;
      YACS::ENGINE::OutputPort *_outputPort;
      SubjectOutputPort* _sop;
    };

    class CommandAddIDSPortFromCatalog: public Command
    {
    public:
      CommandAddIDSPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                   std::string type,
                                   std::string node,
                                   std::string name);
      YACS::ENGINE::InputDataStreamPort *getIDSPort();
      SubjectInputDataStreamPort* getSubjectIDSPort();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      YACS::ENGINE::Catalog* _catalog;
      std::string _typePort;
      std::string _node;
      std::string _name;
      YACS::ENGINE::InputDataStreamPort *_IDSPort;
      SubjectInputDataStreamPort* _sip;
    };

    class CommandAddODSPortFromCatalog: public Command
    {
    public:
      CommandAddODSPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                   std::string type,
                                   std::string node,
                                   std::string name);
      YACS::ENGINE::OutputDataStreamPort *getODSPort();
      SubjectOutputDataStreamPort* getSubjectODSPort();
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      YACS::ENGINE::Catalog* _catalog;
      std::string _typePort;
      std::string _node;
      std::string _name;
      YACS::ENGINE::OutputDataStreamPort *_ODSPort;
      SubjectOutputDataStreamPort* _sop;
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
      virtual std::string dump();
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
      virtual std::string dump();
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
      virtual std::string dump();
      std::string _node;
      std::string _port;
      std::string _value;
      std::string _oldValue;
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
      virtual std::string dump();
      std::string _node;
      std::string _port;
      std::string _value;
      std::string _oldValue;
    };

    class CommandSetSwitchSelect: public Command
    {
    public:
      CommandSetSwitchSelect(std::string aSwitch,
                             std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _switch;
      std::string _value;
      std::string _oldValue;
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
      virtual std::string dump();
      std::string _switch;
      std::string _node;
      std::string _oldNode;
      std::string _value;
      int _oldValue;
    };

    class CommandSetForLoopSteps: public Command
    {
    public:
      CommandSetForLoopSteps(std::string forLoop,
                             std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _forLoop;
      std::string _value;
      int _oldValue;
    };

    class CommandSetWhileCondition: public Command
    {
    public:
      CommandSetWhileCondition(std::string whileLoop,
                               std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _whileLoop;
      std::string _value;
      bool _oldValue;
    };

    class CommandSetForEachBranch: public Command
    {
    public:
      CommandSetForEachBranch(std::string forEach,
                              std::string value);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _forEach;
      std::string _value;
      int _oldValue;
    };

    class CommandSetAlgo: public Command
    {
    public:
      CommandSetAlgo(std::string optimizer, std::string alglib, std::string symbol);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _optimizer;
      std::string _alglib;
      std::string _symbol;
      std::string _oldAlglib;
      std::string _oldSymbol;
    };

    class CommandAddLink: public Command
    {
    public:
      CommandAddLink(std::string outNode, std::string outPort, TypeOfElem outPortType,
                     std::string inNode, std::string inPort, TypeOfElem inPortType, bool control=true);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _outNode;
      std::string _outPort;
      TypeOfElem _outPortType;
      std::string _inNode;
      std::string _inPort;
      TypeOfElem _inPortType;
      bool _control;
      bool _controlCreatedWithDF;
    };

    class CommandAddControlLink: public Command
    {
    public:
      CommandAddControlLink(std::string outNode, std::string inNode);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _outNode;
      std::string _inNode;
    };

    class CommandAddContainerBase : public Command
    {
    public:
      CommandAddContainerBase(std::string name, std::string refContainer);
      virtual ~CommandAddContainerBase();
      SubjectContainerBase* getSubjectContainer() { return _subcont; }
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual YACS::ENGINE::Container *createNewInstance() const = 0;
      std::string _name;
      std::string _containerToClone;
      SubjectContainerBase *_subcont;
    };

    class CommandAddContainer : public CommandAddContainerBase
    {
    public:
      CommandAddContainer(std::string name, std::string refContainer ="");
    protected:
      std::string dump();
      YACS::ENGINE::Container *createNewInstance() const;
    };

    class CommandAddHPContainer : public CommandAddContainerBase
    {
    public:
      CommandAddHPContainer(std::string name, std::string refContainer ="");
    protected:
      std::string dump();
      YACS::ENGINE::Container *createNewInstance() const;
    };

    class CommandSetContainerProperties: public Command
    {
    public:
      CommandSetContainerProperties(std::string container,
                                    std::map<std::string,std::string> properties);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _container;
      std::map<std::string,std::string> _properties;
      std::map<std::string,std::string> _oldProp;
    };

    class CommandSetNodeProperties: public Command
    {
    public:
      CommandSetNodeProperties(std::string position,
                                    std::map<std::string,std::string> properties);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _position;
      std::map<std::string,std::string> _properties;
      std::map<std::string,std::string> _oldProp;
    };

    class CommandSetComponentInstanceProperties: public Command
    {
    public:
      CommandSetComponentInstanceProperties(std::string compoinstance,
                                            std::map<std::string,std::string> properties);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _compoinstance;
      std::map<std::string,std::string> _properties;
      std::map<std::string,std::string> _oldProp;
      bool _oldAnon;
    };

    class CommandSetDSPortProperties: public Command
    {
    public:
      CommandSetDSPortProperties(std::string node, std::string port, bool isInport,
                                 std::map<std::string,std::string> properties);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _nodeName;
      std::string _portName;
      bool _isInport;
      std::map<std::string,std::string> _properties;
      std::map<std::string,std::string> _oldProp;
    };

    class CommandSetLinkProperties: public Command
    {
    public:
      CommandSetLinkProperties(std::string startnode, std::string startport, 
                               std::string endnode, std::string endport,
                               std::map<std::string,std::string> properties);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _startNodeName;
      std::string _startPortName;
      std::string _endNodeName;
      std::string _endPortName;
      std::map<std::string,std::string> _properties;
      std::map<std::string,std::string> _oldProp;
    };

    class CommandSetFuncNodeFunctionName: public Command
    {
    public:
      CommandSetFuncNodeFunctionName(std::string node, std::string funcName);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _nodeName;
      std::string _funcName;
      std::string _oldName;
    };

    class CommandSetInlineNodeScript: public Command
    {
    public:
      CommandSetInlineNodeScript(std::string node, std::string script);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _nodeName;
      std::string _script;
      std::string _oldScript;
    };

    class CommandAddComponentInstance: public Command
    {
    public:
      CommandAddComponentInstance(std::string compoName,
                                  std::string container,
                                  std::string name ="");
      SubjectComponent* getSubjectComponent() {return _subcompo; };
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _compoName;
      std::string _container;
      std::string _name;
      SubjectComponent *_subcompo;
    };

    class CommandSetExecutionMode: public Command
    {
    public:
      CommandSetExecutionMode(std::string nodeName, std::string mode);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _mode;
      std::string _nodeName;
      std::string _oldmode;
    };

    class CommandSetContainer: public Command
    {
    public:
      CommandSetContainer(std::string nodeName, std::string container);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _container;
      std::string _nodeName;
      std::string _oldcont;
    };

    class CommandAssociateComponentToContainer: public Command
    {
    public:
      CommandAssociateComponentToContainer(std::string instanceName,
                                           std::string container);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _container;
      std::string _instanceName;
      std::string _oldcont;
    };

    class CommandAssociateServiceToComponent: public Command
    {
    public:
      CommandAssociateServiceToComponent(std::string service,
                                         std::string instanceName);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      std::string _service;
      std::string _instanceName;
      std::string _oldInstance;
      std::string _oldcont;
    };

    class CommandAddComponentFromCatalog: public Command
    {
    public:
      CommandAddComponentFromCatalog(YACS::ENGINE::Catalog* catalog,
                                     std::string position,
                                     std::string compo,
                                     std::string service);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
      YACS::ENGINE::Catalog* _catalog;
      std::string _position;
      std::string _compo;
      std::string _service;
      std::string _nameInProc;
      bool _createdInstance;
    };


    class Subject;
    class CommandDestroy: public Command
    {
    public:
      CommandDestroy(TypeOfElem elemType,
                     std::string startnode, std::string startport, TypeOfElem startportType,
                     std::string endnode, std::string endport, TypeOfElem endportType);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      virtual std::string dump();
    protected:
      TypeOfElem _elemType;
      std::string _startnode;
      std::string _startport; 
      TypeOfElem _startportType;
      std::string _endnode;
      std::string _endport;
      TypeOfElem _endportType;

//       std::string _position;
//       Subject* _subject;
//       std::string _name;
    };

  }
}
#endif
