
#ifndef _GUIOBSERVERS_HXX_
#define _GUIOBSERVERS_HXX_

#include <set>
#include <string>
#include <map>
#include <list>

#define _DEVDEBUG_
#include "YacsTrace.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class ComposedNode;
    class Bloc;
    class Proc;
    class ForLoop;
    class ForEachLoop;
    class WhileLoop;
    class Switch;
    class OptimizerLoop;
    class ElementaryNode;
    class InlineNode;
    class ServiceNode;
    class PythonNode;
    class PyFuncNode;
    class CORBANode;
    class CppNode;
    class SalomeNode;
    class SalomePythonNode;
    class XmlNode;
    class DataPort;
    class InputPort;
    class OutputPort;
    class InputDataStreamPort;
    class OutputDataStreamPort;
    class Catalog;
  }

  namespace HMI
  {

    typedef enum
      {
        ADD,
        REMOVE,
        CUT,
        PASTE,
        EDIT,
        RENAME
      } GuiEvent;
    
    typedef enum
      {
        INPUTPORT,
        OUTPUTPORT,
        INPUTDATASTREAMPORT,
        OUTPUTDATASTREAMPORT
      } PortKind;

    class ProcInvoc;
    class GuiObserver;
    
    class Subject
    {
    public:
      Subject(Subject *parent);
      virtual ~Subject();
      virtual void attach(GuiObserver *obs);
      virtual void detach(GuiObserver *obs);
      virtual void select(bool isSelected);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual std::string getName();
      virtual bool setName(std::string name);
      virtual Subject* getParent();
      virtual bool destroy(Subject *son);
    protected:
      std::set<GuiObserver *> _setObs;
      Subject *_parent;
    };
    
    class GuiObserver
    {
    public:
      GuiObserver();
      virtual ~GuiObserver();
      virtual void select(bool isSelected);
      virtual void update(GuiEvent event, int type, Subject* son);
    protected:
    };
    
    
    class SubjectNode: public Subject
    {
    public:
      SubjectNode(YACS::ENGINE::Node *node, Subject *parent);
      virtual ~SubjectNode();
      virtual std::string getName();
      virtual bool setName(std::string name);
      virtual YACS::ENGINE::Node* getNode();
    protected:
      YACS::ENGINE::Node *_node;
    };
    
    class SubjectComposedNode: public SubjectNode
    {
    public:
      SubjectComposedNode(YACS::ENGINE::ComposedNode *composedNode, Subject *parent);
      virtual ~SubjectComposedNode();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
    protected:
      SubjectNode *createNode(YACS::ENGINE::Catalog *catalog,
                              std::string type,
                              std::string name,
                              int swCase=0);
      YACS::ENGINE::ComposedNode *_composedNode;
    };

    class SubjectBloc: public SubjectComposedNode
    {
    public:
      SubjectBloc(YACS::ENGINE::Bloc *bloc, Subject *parent);
      virtual ~SubjectBloc();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual void removeNode(SubjectNode* child);
    protected:
      YACS::ENGINE::Bloc *_bloc;
      std::set<SubjectNode*> _children;
    };

    class SubjectProc: public SubjectBloc
    {
    public:
      SubjectProc(YACS::ENGINE::Proc *proc, Subject *parent);
      virtual ~SubjectProc();
    protected:
      YACS::ENGINE::Proc *_proc;
    };

    class SubjectForLoop: public SubjectComposedNode
    {
    public:
      SubjectForLoop(YACS::ENGINE::ForLoop *forLoop, Subject *parent);
      virtual ~SubjectForLoop();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
    protected:
      YACS::ENGINE::ForLoop *_forLoop;
      SubjectNode* _body;
    };

    class SubjectWhileLoop: public SubjectComposedNode
    {
    public:
      SubjectWhileLoop(YACS::ENGINE::WhileLoop *whileLoop, Subject *parent);
      virtual ~SubjectWhileLoop();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
    protected:
      YACS::ENGINE::WhileLoop *_whileLoop;
      SubjectNode* _body;
    };

    class SubjectSwitch: public SubjectComposedNode
    {
    public:
      SubjectSwitch(YACS::ENGINE::Switch *aSwitch, Subject *parent);
      virtual ~SubjectSwitch();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name,
                           int swCase);
      std::map<int, SubjectNode*> getBodyMap();
    protected:
      YACS::ENGINE::Switch *_switch;
      std::map<int, SubjectNode*> _bodyMap;
    };

    class SubjectForEachLoop: public SubjectComposedNode
    {
    public:
      SubjectForEachLoop(YACS::ENGINE::ForEachLoop *forEachLoop, Subject *parent);
      virtual ~SubjectForEachLoop();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
    protected:
      YACS::ENGINE::ForEachLoop *_forEachLoop;
      SubjectNode* _body;
    };

    class SubjectOptimizerLoop: public SubjectComposedNode
    {
    public:
      SubjectOptimizerLoop(YACS::ENGINE::OptimizerLoop *optimizerLoop, Subject *parent);
      virtual ~SubjectOptimizerLoop();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
    protected:
      YACS::ENGINE::OptimizerLoop *_optimizerLoop;
      SubjectNode* _body;
    };

    class SubjectDataPort: public Subject
    {
    public:
      SubjectDataPort(YACS::ENGINE::DataPort* port, Subject *parent);
      virtual ~SubjectDataPort();
      virtual std::string getName();
    protected:
      YACS::ENGINE::DataPort *_dataPort;
    };

    class SubjectInputPort: public SubjectDataPort
    {
    public:
      SubjectInputPort(YACS::ENGINE::InputPort *port, Subject *parent);
      virtual ~SubjectInputPort();
    protected:
      YACS::ENGINE::InputPort *_inputPort;
    };
    
    class SubjectOutputPort: public SubjectDataPort
    {
    public:
      SubjectOutputPort(YACS::ENGINE::OutputPort *port, Subject *parent);
      virtual ~SubjectOutputPort();
    protected:
      YACS::ENGINE::OutputPort *_outputPort;
    };
    
    class SubjectInputDataStreamPort: public SubjectDataPort
    {
    public:
      SubjectInputDataStreamPort(YACS::ENGINE::InputDataStreamPort *port, Subject *parent);
      virtual ~SubjectInputDataStreamPort();
    protected:
      YACS::ENGINE::InputDataStreamPort *_inputDataStreamPort;
    };
    
    class SubjectOutputDataStreamPort: public SubjectDataPort
    {
    public:
      SubjectOutputDataStreamPort(YACS::ENGINE::OutputDataStreamPort *port, Subject *parent);
      virtual ~SubjectOutputDataStreamPort();
    protected:
      YACS::ENGINE::OutputDataStreamPort *_outputDataStreamPort;
    };

    class SubjectElementaryNode: public SubjectNode
    {
    public:
      SubjectElementaryNode(YACS::ENGINE::ElementaryNode *elementaryNode, Subject *parent);
      virtual ~SubjectElementaryNode();
      virtual bool addInputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual bool addOutputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual void removePort(SubjectDataPort* port);
    protected:
      YACS::ENGINE::ElementaryNode *_elementaryNode;
      std::list<SubjectInputPort*> _listSubjectInputPort;
      std::list<SubjectOutputPort*> _listSubjectOutputPort;
    };

    class SubjectInlineNode: public SubjectElementaryNode
    {
    public:
      SubjectInlineNode(YACS::ENGINE::InlineNode *inlineNode, Subject *parent);
      virtual ~SubjectInlineNode();
    protected:
      YACS::ENGINE::InlineNode *_inlineNode;
    };

    class SubjectPythonNode: public SubjectInlineNode
    {
    public:
      SubjectPythonNode(YACS::ENGINE::PythonNode *pythonNode, Subject *parent);
      virtual ~SubjectPythonNode();
    protected:
      YACS::ENGINE::PythonNode *_pythonNode;
    };

    class SubjectPyFuncNode: public SubjectInlineNode
    {
    public:
      SubjectPyFuncNode(YACS::ENGINE::PyFuncNode *pyFuncNode, Subject *parent);
      virtual ~SubjectPyFuncNode();
    protected:
      YACS::ENGINE::PyFuncNode *_pyFuncNode;
    };

    class SubjectServiceNode: public SubjectElementaryNode
    {
    public:
      SubjectServiceNode(YACS::ENGINE::ServiceNode *serviceNode, Subject *parent);
      virtual ~SubjectServiceNode();
    protected:
      YACS::ENGINE::ServiceNode *_serviceNode;
    };

    class SubjectCORBANode: public SubjectServiceNode
    {
    public:
      SubjectCORBANode(YACS::ENGINE::CORBANode *corbaNode, Subject *parent);
      virtual ~SubjectCORBANode();
    protected:
      YACS::ENGINE::CORBANode *_corbaNode;
    };

    class SubjectCppNode: public SubjectServiceNode
    {
    public:
      SubjectCppNode(YACS::ENGINE::CppNode *cppNode, Subject *parent);
      virtual ~SubjectCppNode();
    protected:
      YACS::ENGINE::CppNode *_cppNode;
    };

    class SubjectSalomeNode: public SubjectServiceNode
    {
    public:
      SubjectSalomeNode(YACS::ENGINE::SalomeNode *salomeNode, Subject *parent);
      virtual ~SubjectSalomeNode();
    protected:
      YACS::ENGINE::SalomeNode *_salomeNode;
    };

    class SubjectSalomePythonNode: public SubjectServiceNode
    {
    public:
      SubjectSalomePythonNode(YACS::ENGINE::SalomePythonNode *salomePythonNode, Subject *parent);
      virtual ~SubjectSalomePythonNode();
    protected:
      YACS::ENGINE::SalomePythonNode *_salomePythonNode;
    };

    class SubjectXmlNode: public SubjectServiceNode
    {
    public:
      SubjectXmlNode(YACS::ENGINE::XmlNode *xmlNode, Subject *parent);
      virtual ~SubjectXmlNode();
    protected:
      YACS::ENGINE::XmlNode *_xmlNode;
    };
    
  }
}
#endif
