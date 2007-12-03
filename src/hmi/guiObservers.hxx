
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
    class SplitterNode;
    class DataPort;
    class InputPort;
    class OutputPort;
    class InPort;
    class OutPort;
    class InputDataStreamPort;
    class OutputDataStreamPort;
    class Catalog;
    class ComponentInstance;
    class Container;
    class TypeCode;
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
	UPDATE,
	UP,
	DOWN,
        RENAME,
        NEWROOT,
        ADDLINK,
        ADDCONTROLLINK,
        ADDREF,
      } GuiEvent;
    
    class ProcInvoc;
    class GuiObserver;
    
    class SubjectReference;
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
      virtual void loadChildren();
      virtual void addSubjectReference(Subject *ref);
      virtual void clean();
      void localClean();
      bool isDestructible() { return _destructible; };
      static void erase(Subject* sub);
    protected:
      std::set<GuiObserver *> _setObs;
      Subject *_parent;
      bool _destructible;
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
    
    class SubjectReference: public Subject
    {
    public:
      SubjectReference(Subject* ref, Subject *parent);
      virtual ~SubjectReference();
      virtual std::string getName();
      virtual Subject* getReference() const;
      virtual void clean();
      void localClean();
    protected:
      Subject* _reference;
    };

    class SubjectLink;
    class SubjectControlLink;
    class SubjectDataPort: public Subject
    {
    public:
      SubjectDataPort(YACS::ENGINE::DataPort* port, Subject *parent);
      virtual ~SubjectDataPort();
      virtual std::string getName();
      virtual YACS::ENGINE::DataPort* getPort();
      static void tryCreateLink(SubjectDataPort *subOutport, SubjectDataPort *subInport);
      virtual void clean();
      void localClean();
      void addSubjectLink(SubjectLink* subject) { _listSubjectLink.push_back(subject); };
      void removeSubjectLink(SubjectLink* subject) { _listSubjectLink.remove(subject); };
      std::list<SubjectLink*> getListOfSubjectLink() { return _listSubjectLink; };
    protected:
      YACS::ENGINE::DataPort *_dataPort;
      std::list<SubjectLink*> _listSubjectLink;
    };

    class SubjectInputPort: public SubjectDataPort
    {
    public:
      SubjectInputPort(YACS::ENGINE::InputPort *port, Subject *parent);
      virtual ~SubjectInputPort();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::InputPort *_inputPort;
    };
    
    class SubjectOutputPort: public SubjectDataPort
    {
    public:
      SubjectOutputPort(YACS::ENGINE::OutputPort *port, Subject *parent);
      virtual ~SubjectOutputPort();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::OutputPort *_outputPort;
    };
    
    class SubjectInputDataStreamPort: public SubjectDataPort
    {
    public:
      SubjectInputDataStreamPort(YACS::ENGINE::InputDataStreamPort *port, Subject *parent);
      virtual ~SubjectInputDataStreamPort();
      virtual bool setProperties(std::map<std::string, std::string> properties);
      virtual std::map<std::string, std::string> getProperties();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::InputDataStreamPort *_inputDataStreamPort;
    };
    
    class SubjectOutputDataStreamPort: public SubjectDataPort
    {
    public:
      SubjectOutputDataStreamPort(YACS::ENGINE::OutputDataStreamPort *port, Subject *parent);
      virtual ~SubjectOutputDataStreamPort();
      virtual bool setProperties(std::map<std::string, std::string> properties);
      virtual std::map<std::string, std::string> getProperties();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::OutputDataStreamPort *_outputDataStreamPort;
    };

    
    class SubjectNode: public Subject
    {
    public:
      SubjectNode(YACS::ENGINE::Node *node, Subject *parent);
      virtual ~SubjectNode();
      virtual void reparent(Subject* parent);
      virtual std::string getName();
      virtual bool setName(std::string name);
      virtual YACS::ENGINE::Node* getNode();
      virtual void clean();
      SubjectControlLink* addSubjectControlLink(SubjectControlLink *sub) { _listSubjectControlLink.push_back(sub); };
      void removeSubjectControlLink(SubjectControlLink* sub) { _listSubjectControlLink.remove(sub); };
      void localClean();
      static void tryCreateLink(SubjectNode *subOutNode, SubjectNode *subInNode);
    //protected: // a temporary solution while SessionCataLoader use loadTypesOld(...)
                 // method instead of a new loadTypes(...) method
      virtual SubjectInputPort* addSubjectInputPort(YACS::ENGINE::InputPort *port,
                                                    std::string name = "");
      virtual SubjectOutputPort* addSubjectOutputPort(YACS::ENGINE::OutputPort *port,
                                                      std::string name = "");
      virtual SubjectInputDataStreamPort* addSubjectIDSPort(YACS::ENGINE::InputDataStreamPort *port,
                                                            std::string name = "");
      virtual SubjectOutputDataStreamPort* addSubjectODSPort(YACS::ENGINE::OutputDataStreamPort *port,
                                                             std::string name = "");
    protected:
      virtual void removeExternalLinks();
      YACS::ENGINE::Node *_node;
      std::list<SubjectInputPort*> _listSubjectInputPort;
      std::list<SubjectOutputPort*> _listSubjectOutputPort;
      std::list<SubjectInputDataStreamPort*> _listSubjectIDSPort;
      std::list<SubjectOutputDataStreamPort*> _listSubjectODSPort;
      std::list<SubjectLink*> _listSubjectLink;
      std::list<SubjectControlLink*> _listSubjectControlLink;
    };
    
    class SubjectComposedNode: public SubjectNode
    {
    public:
      SubjectComposedNode(YACS::ENGINE::ComposedNode *composedNode, Subject *parent);
      virtual ~SubjectComposedNode();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog,
                           std::string compo,
                           std::string type,
                           std::string name);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const  { return 0; }
      virtual void loadChildren();
      virtual void loadLinks();
      virtual void completeChildrenSubjectList(SubjectNode *son);
      SubjectLink* addSubjectLink(SubjectNode *sno,
                                  SubjectDataPort *spo,
                                  SubjectNode *sni,
                                  SubjectDataPort *spi);
      SubjectControlLink* addSubjectControlLink(SubjectNode *sno,
                                                SubjectNode *sni);
      virtual void removeLink(SubjectLink* link);
      virtual void removeControlLink(SubjectControlLink* link);
      virtual void clean();
      void localClean();
    protected:
      virtual SubjectNode *createNode(YACS::ENGINE::Catalog *catalog,
                                      std::string compo,
                                      std::string type,
                                      std::string name,
                                      int swCase=0);
      virtual SubjectNode* addSubjectNode(YACS::ENGINE::Node * node,
                                          std::string name = "",
                                          YACS::ENGINE::Catalog *catalog = 0,
                                          std::string compo = "",
                                          std::string type ="");
      YACS::ENGINE::ComposedNode *_composedNode;
    };

    class SubjectBloc: public SubjectComposedNode
    {
    public:
      SubjectBloc(YACS::ENGINE::Bloc *bloc, Subject *parent);
      virtual ~SubjectBloc();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog,
                           std::string compo,
                           std::string type,
                           std::string name);
      virtual void removeNode(SubjectNode* child);
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const;
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::Bloc *_bloc;
      std::set<SubjectNode*> _children;
    };

    class SubjectContainer: public Subject
    {
    public:
      SubjectContainer(YACS::ENGINE::Container* container, Subject *parent);
      virtual ~SubjectContainer();
      virtual std::string getName();
      virtual std::map<std::string, std::string> getProperties();
      virtual bool setProperties(std::map<std::string, std::string> properties);
      virtual void clean();
      void localClean();
      YACS::ENGINE::Container* getContainer() const;
    protected:
      YACS::ENGINE::Container* _container;
    };

    class SubjectComponent: public Subject
    {
    public:
      SubjectComponent(YACS::ENGINE::ComponentInstance* component, int id, Subject *parent);
      virtual ~SubjectComponent();
      virtual std::string getName();
      virtual void setContainer();
      virtual void associateToContainer(SubjectContainer* subcont);
      virtual std::pair<std::string, int> getKey();
      virtual void clean();
      void localClean();
      YACS::ENGINE::ComponentInstance* getComponent() const;
    protected:
      int _id;
      YACS::ENGINE::ComponentInstance* _compoInst;
    };

    class SubjectDataType: public Subject
    {
    public:
      SubjectDataType(std::string typeName, Subject *parent);
      virtual ~SubjectDataType();
      virtual std::string getName();
      virtual void clean();
      void localClean();
    protected:
      std::string _typeName;
    };

    class SubjectProc: public SubjectBloc
    {
    public:
      SubjectProc(YACS::ENGINE::Proc *proc, Subject *parent);
      virtual ~SubjectProc();
      void loadProc();
      virtual bool addComponent(std::string name);
      virtual bool addContainer(std::string name, std::string ref="");
      virtual bool addDataType(YACS::ENGINE::Catalog* catalog, std::string typeName);
      SubjectComponent* addSubjectComponent(YACS::ENGINE::ComponentInstance* compo,
                                            std::pair<std::string,int> key);
      SubjectContainer* addSubjectContainer(YACS::ENGINE::Container* cont,
                                            std::string name = "");
      SubjectDataType* addSubjectDataType(YACS::ENGINE::TypeCode *type);
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::Proc *_proc;
    };

    class SubjectForLoop: public SubjectComposedNode
    {
    public:
      SubjectForLoop(YACS::ENGINE::ForLoop *forLoop, Subject *parent);
      virtual ~SubjectForLoop();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog,
                           std::string compo,
                           std::string type,
                           std::string name);
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const { return _body; }
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::ForLoop *_forLoop;
      SubjectNode* _body;
    };

    class SubjectWhileLoop: public SubjectComposedNode
    {
    public:
      SubjectWhileLoop(YACS::ENGINE::WhileLoop *whileLoop, Subject *parent);
      virtual ~SubjectWhileLoop();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog,
                           std::string compo,
                           std::string type,
                           std::string name);
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const { return _body; }
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::WhileLoop *_whileLoop;
      SubjectNode* _body;
    };

    class SubjectSwitch: public SubjectComposedNode
    {
    public:
      SubjectSwitch(YACS::ENGINE::Switch *aSwitch, Subject *parent);
      virtual ~SubjectSwitch();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog,
                           std::string compo,
                           std::string type,
                           std::string name,
                           int swCase);
      std::map<int, SubjectNode*> getBodyMap();
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const;
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::Switch *_switch;
      std::map<int, SubjectNode*> _bodyMap;
    };

    class SubjectForEachLoop: public SubjectComposedNode
    {
    public:
      SubjectForEachLoop(YACS::ENGINE::ForEachLoop *forEachLoop, Subject *parent);
      virtual ~SubjectForEachLoop();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog,
                           std::string compo,
                           std::string type,
                           std::string name);
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const { return _body; }
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::ForEachLoop *_forEachLoop;
      SubjectNode* _body;
      SubjectNode* _splitter;
    };

    class SubjectOptimizerLoop: public SubjectComposedNode
    {
    public:
      SubjectOptimizerLoop(YACS::ENGINE::OptimizerLoop *optimizerLoop, Subject *parent);
      virtual ~SubjectOptimizerLoop();
      virtual bool addNode(YACS::ENGINE::Catalog *catalog,
                           std::string compo,
                           std::string type,
                           std::string name);
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const { return _body; }
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::OptimizerLoop *_optimizerLoop;
      SubjectNode* _body;
    };

    class SubjectElementaryNode: public SubjectNode
    {
    public:
      SubjectElementaryNode(YACS::ENGINE::ElementaryNode *elementaryNode, Subject *parent);
      virtual ~SubjectElementaryNode();
      virtual SubjectDataPort* addInputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual SubjectDataPort* addOutputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual bool addIDSPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual bool addODSPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual void removePort(SubjectDataPort* port);
      virtual void loadChildren();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::ElementaryNode *_elementaryNode;
    };

    class SubjectInlineNode: public SubjectElementaryNode
    {
    public:
      SubjectInlineNode(YACS::ENGINE::InlineNode *inlineNode, Subject *parent);
      virtual ~SubjectInlineNode();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::InlineNode *_inlineNode;
    };

    class SubjectPythonNode: public SubjectInlineNode
    {
    public:
      SubjectPythonNode(YACS::ENGINE::PythonNode *pythonNode, Subject *parent);
      virtual ~SubjectPythonNode();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::PythonNode *_pythonNode;
    };

    class SubjectPyFuncNode: public SubjectInlineNode
    {
    public:
      SubjectPyFuncNode(YACS::ENGINE::PyFuncNode *pyFuncNode, Subject *parent);
      virtual ~SubjectPyFuncNode();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::PyFuncNode *_pyFuncNode;
    };

    class SubjectServiceNode: public SubjectElementaryNode
    {
    public:
      SubjectServiceNode(YACS::ENGINE::ServiceNode *serviceNode, Subject *parent);
      virtual ~SubjectServiceNode();
      virtual void setComponentFromCatalog(YACS::ENGINE::Catalog *catalog,
                                           std::string compo,
                                           std::string service);
      virtual void setComponent();
      virtual void associateToComponent(SubjectComponent *subcomp);
      virtual void addSubjectReference(Subject *ref);
      virtual SubjectReference* getSubjectReference(Subject *ref);
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::ServiceNode *_serviceNode;
      std::list<SubjectReference*> _listSubjectReference;
    };

    class SubjectCORBANode: public SubjectServiceNode
    {
    public:
      SubjectCORBANode(YACS::ENGINE::CORBANode *corbaNode, Subject *parent);
      virtual ~SubjectCORBANode();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::CORBANode *_corbaNode;
    };

    class SubjectCppNode: public SubjectServiceNode
    {
    public:
      SubjectCppNode(YACS::ENGINE::CppNode *cppNode, Subject *parent);
      virtual ~SubjectCppNode();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::CppNode *_cppNode;
    };

    class SubjectSalomeNode: public SubjectServiceNode
    {
    public:
      SubjectSalomeNode(YACS::ENGINE::SalomeNode *salomeNode, Subject *parent);
      virtual ~SubjectSalomeNode();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::SalomeNode *_salomeNode;
    };

    class SubjectSalomePythonNode: public SubjectServiceNode
    {
    public:
      SubjectSalomePythonNode(YACS::ENGINE::SalomePythonNode *salomePythonNode,
                              Subject *parent);
      virtual ~SubjectSalomePythonNode();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::SalomePythonNode *_salomePythonNode;
    };

    class SubjectXmlNode: public SubjectServiceNode
    {
    public:
      SubjectXmlNode(YACS::ENGINE::XmlNode *xmlNode, Subject *parent);
      virtual ~SubjectXmlNode();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::XmlNode *_xmlNode;
    };

    class SubjectSplitterNode: public SubjectElementaryNode
    {
    public:
      SubjectSplitterNode(YACS::ENGINE::SplitterNode *splitterNode, Subject *parent);
      virtual ~SubjectSplitterNode();
      virtual std::string getName();
      virtual void clean();
      void localClean();
    protected:
      YACS::ENGINE::SplitterNode *_splitterNode;
    };

    class SubjectLink: public Subject
    {
    public:
      SubjectLink(SubjectNode* subOutNode,
                  SubjectDataPort* outPort,
                  SubjectNode* subInNode,
                  SubjectDataPort* inPort,
                  Subject *parent);
      virtual ~SubjectLink();
      virtual std::string getName();
      virtual void clean();
      void localClean();
      SubjectNode* getSubjectOutNode() { return _subOutNode; };
      SubjectNode* getSubjectInNode() { return _subInNode; };
      SubjectDataPort* getSubjectOutPort() { return _outPort; };
      SubjectDataPort* getSubjectInPort() { return _inPort; };
    protected:
      SubjectNode* _subOutNode;
      SubjectDataPort* _outPort;
      SubjectNode* _subInNode;
      SubjectDataPort* _inPort;
      std::string _name;
      YACS::ENGINE::ComposedNode *_cla;
      YACS::ENGINE::OutPort *_outp;
      YACS::ENGINE::InPort *_inp;
    };

    class SubjectControlLink: public Subject
    {
    public:
      SubjectControlLink(SubjectNode* subOutNode,
                         SubjectNode* subInNode,
                         Subject *parent);
      virtual ~SubjectControlLink();
      virtual std::string getName();
      virtual void clean();
      void localClean();
      SubjectNode* getSubjectOutNode() { return _subOutNode; };
      SubjectNode* getSubjectInNode() { return _subInNode; };
    protected:
      SubjectNode* _subOutNode;
      SubjectNode* _subInNode;
      std::string _name;
      YACS::ENGINE::ComposedNode *_cla;
    };

  }
}
#endif
