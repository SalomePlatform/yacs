
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

    class CommandAddLink: public Command
    {
    public:
      CommandAddLink(std::string outNode, std::string outPort,
                     std::string inNode, std::string inPort);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _outNode;
      std::string _outPort;
      std::string _inNode;
      std::string _inPort;
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

    class CommandDeleteNode: public Command
    {
    public:
      CommandDeleteNode(std::string node);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
    };

    class CommandCutNode: public Command
    {
    public:
      CommandCutNode(std::string node);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
    };

    class CommandCopyNode: public Command
    {
    public:
      CommandCopyNode(std::string node);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
    };

    class CommandPasteNode: public Command
    {
    public:
      CommandPasteNode(std::string position);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
    };

    class CommandDeleteInPort: public Command
    {
    public:
      CommandDeleteInPort(std::string node, std::string port);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
      std::string _port;
    };

    class CommandDeleteOutPort: public Command
    {
    public:
      CommandDeleteOutPort(std::string node, std::string port);
    protected:
      virtual bool localExecute();
      virtual bool localReverse();
      std::string _node;
      std::string _port;
    };

  }
}
#endif
