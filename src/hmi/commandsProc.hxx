
#ifndef _COMMANDSPROC_HXX_
#define _COMMANDSPROC_HXX_

#include "commands.hxx"

#include <list>
#include <stack>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
    class Node;
    class TypeCode;
    class Catalog;
    class InputPort;
    class OutputPort;
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
        INPUTPORT,
        OUTPUTPORT,
        INPUTDATASTREAMPORT,
        OUTPUTDATASTREAMPORT,
        UNKNOWN
      } TypeOfElem;
   
    class ProcInvoc: public Invocator
    {
    public:
      ProcInvoc();
      static TypeOfElem getTypeOfNode(YACS::ENGINE::Node* node);
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
