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

#ifndef _GUIOBSERVERS_HXX_
#define _GUIOBSERVERS_HXX_

#include <set>
#include <string>
#include <map>
#include <vector>
#include <list>
#include "HMIExport.hxx"
#include "Dispatcher.hxx"
#include "commandsProc.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class ComposedNode;
    class Bloc;
    class Proc;
    class ForLoop;
    class DynParaLoop;
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
    class DataNode;
    class PresetNode;
    class OutNode;
    class StudyInNode;
    class StudyOutNode;
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
    class HomogeneousPoolContainer;
    class TypeCode;
    class OutGate;
    class InGate;
  }

  namespace HMI
  {

    typedef enum
      {
        ADD,
        REMOVE,
        CUT,
        PASTE,
        ORDER,
        EDIT,
        UPDATE,
        UPDATEPROGRESS,
        SYNCHRO,
        UP,
        DOWN,
        RENAME,
        NEWROOT,
        ENDLOAD,
        ADDLINK,
        ADDCONTROLLINK,
        ADDREF,
        ADDCHILDREF,
        REMOVECHILDREF,
        ASSOCIATE,
        SETVALUE,
        SETCASE,
        SETSELECT,
        GEOMETRY,
        EMPHASIZE,
        SWITCHSHAPE,
        PROGRESS
      } GuiEvent;
    
    class ProcInvoc;
    class GuiObserver;
    
    class SubjectReference;
    class HMI_EXPORT Subject: public YACS::ENGINE::Observer
    {
    public:
      friend class CommandReparentNode;
      Subject(Subject *parent=0);
      virtual ~Subject();
      virtual void attach(GuiObserver *obs);
      virtual void detach(GuiObserver *obs);
      virtual void select(bool isSelected);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual std::string getName();
      virtual bool setName(std::string name);
      virtual bool setProperties(std::map<std::string, std::string> properties);
      virtual std::map<std::string, std::string> getProperties();
      virtual std::vector<std::string> knownProperties();
      virtual Subject* getParent();
      virtual void setParent(Subject* son);
      virtual bool destroy(Subject *son);
      virtual void loadChildren();
      virtual void loadLinks();
      virtual void addSubjectReference(Subject *ref);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      void registerUndoDestroy();
      void askRegisterUndoDestroy() {_askRegisterUndo=true; };
      bool isDestructible() { return _destructible; };
      static void erase(Subject* sub, Command *command=0, bool post=false);
      virtual TypeOfElem getType(){return UNKNOWN;}
      virtual void setProgress( std::string newProgress );
      virtual std::string getProgress() { return _progress; };
    protected:
      std::set<GuiObserver *> _setObs;
      Subject *_parent;
      bool _destructible;
      bool _askRegisterUndo;
      std::string _progress;
    };
    
    class HMI_EXPORT GuiObserver
    {
    public:
      GuiObserver();
      virtual ~GuiObserver();
      virtual void select(bool isSelected);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void incrementSubjects(Subject *subject);
      virtual void decrementSubjects(Subject *subject);
      int getNbSubjects();
      bool isDestructible() { return _destructible; };
      static std::string eventName(GuiEvent event);
      static void setEventMap();
    protected:
      std::set<Subject*> _subjectSet;
      bool _destructible;
      static std::map<int, std::string> _eventNameMap;
    };
    
    class HMI_EXPORT SubjectObserver:public GuiObserver
    {
    public:
      SubjectObserver(Subject* ref);
      virtual ~SubjectObserver();
      virtual void select(bool isSelected);
      virtual void update(GuiEvent event, int type, Subject* son);
    protected:
      Subject* _reference;
    };

    class SubjectReference: public Subject
    {
    public:
      SubjectReference(Subject* ref, Subject *parent);
      virtual ~SubjectReference();
      virtual std::string getName();
      virtual Subject* getReference() const;
      virtual void reparent(Subject *parent);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return REFERENCE;}
    protected:
      Subject* _reference;
      SubjectObserver* _sobs;
    };

    class SubjectLink;
    class SubjectControlLink;
    class HMI_EXPORT SubjectDataPort: public Subject
    {
    public:
      SubjectDataPort(YACS::ENGINE::DataPort* port, Subject *parent);
      virtual ~SubjectDataPort();
      virtual std::string getName();
      virtual bool setName(std::string name);
      virtual YACS::ENGINE::DataPort* getPort();
      static bool tryCreateLink(SubjectDataPort *subOutport, SubjectDataPort *subInport,bool control=true);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      void addSubjectLink(SubjectLink* subject) { _listSubjectLink.push_back(subject); };
      void removeSubjectLink(SubjectLink* subject) { _listSubjectLink.remove(subject); };
      std::list<SubjectLink*> getListOfSubjectLink() { return _listSubjectLink; };
      virtual bool setValue(std::string value);
      void setExecValue(std::string value);
      std::string getExecValue();
      void registerUndoDestroy();
    protected:
      YACS::ENGINE::DataPort *_dataPort;
      std::list<SubjectLink*> _listSubjectLink;
      std::string _execValue;
    };

    class SubjectInputPort: public SubjectDataPort
    {
    public:
      SubjectInputPort(YACS::ENGINE::InputPort *port, Subject *parent);
      virtual ~SubjectInputPort();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual bool setValue(std::string value);
      virtual TypeOfElem getType(){return INPUTPORT;}
    protected:
      YACS::ENGINE::InputPort *_inputPort;
    };
    
    class SubjectOutputPort: public SubjectDataPort
    {
    public:
      SubjectOutputPort(YACS::ENGINE::OutputPort *port, Subject *parent);
      virtual ~SubjectOutputPort();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual bool setValue(std::string value);
      virtual TypeOfElem getType(){return OUTPUTPORT;}
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
      virtual std::vector<std::string> knownProperties();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return INPUTDATASTREAMPORT;}
      void registerUndoDestroy();
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
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return OUTPUTDATASTREAMPORT;}
      void registerUndoDestroy();
    protected:
      YACS::ENGINE::OutputDataStreamPort *_outputDataStreamPort;
    };

    
    class HMI_EXPORT SubjectNode: public Subject
    {
    public:
      friend class CommandAddInputPortFromCatalog;
      friend class CommandAddOutputPortFromCatalog;
      friend class CommandAddIDSPortFromCatalog;
      friend class CommandAddODSPortFromCatalog;
      SubjectNode(YACS::ENGINE::Node *node, Subject *parent);
      virtual ~SubjectNode();
      virtual bool setProperties(std::map<std::string, std::string> properties);
      virtual std::map<std::string, std::string> getProperties();
      virtual bool reparent(Subject* parent);
      virtual bool copy(Subject* parent);
      virtual std::string getName();
      virtual bool setName(std::string name);
      virtual YACS::ENGINE::Node* getNode();
      virtual void clean(Command *command=0);
      void registerUndoDestroy();
      SubjectControlLink* addSubjectControlLink(SubjectControlLink *sub) { _listSubjectControlLink.push_back(sub); return sub; };
      void removeSubjectControlLink(SubjectControlLink* sub) { _listSubjectControlLink.remove(sub); };
      std::list<SubjectLink*> getSubjectLinks() const { return _listSubjectLink; };
      std::list<SubjectControlLink*> getSubjectControlLinks() const { return _listSubjectControlLink; };
      std::list<SubjectInputPort*> getSubjectInputPorts() const { return _listSubjectInputPort; };
      std::list<SubjectOutputPort*> getSubjectOutputPorts() const { return _listSubjectOutputPort; };
      std::list<SubjectInputDataStreamPort*> getSubjectInputDataStreamPorts() const { return _listSubjectIDSPort; };
      std::list<SubjectOutputDataStreamPort*> getSubjectOutputDataStreamPorts() const { return _listSubjectODSPort; };
      void localclean(Command *command=0);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void recursiveUpdate(GuiEvent event, int type, Subject* son);
      virtual void removeExternalLinks();
      virtual void removeExternalControlLinks();
      virtual void saveLinks();
      virtual void restoreLinks();
      virtual bool putInComposedNode(std::string name,std::string type, bool toSaveRestoreLinks=true);
      virtual int isValid();
      void setExecState(int execState);
      static bool tryCreateLink(SubjectNode *subOutNode, SubjectNode *subInNode);

    protected:
      virtual SubjectInputPort* addSubjectInputPort(YACS::ENGINE::InputPort *port,
                                                    std::string name = "");
      virtual SubjectOutputPort* addSubjectOutputPort(YACS::ENGINE::OutputPort *port,
                                                      std::string name = "");
      virtual SubjectInputDataStreamPort* addSubjectIDSPort(YACS::ENGINE::InputDataStreamPort *port,
                                                            std::string name = "");
      virtual SubjectOutputDataStreamPort* addSubjectODSPort(YACS::ENGINE::OutputDataStreamPort *port,
                                                             std::string name = "");
      virtual void notifyObserver(YACS::ENGINE::Node* object,const std::string& event);

      YACS::ENGINE::Node *_node;
      std::list<SubjectInputPort*> _listSubjectInputPort;
      std::list<SubjectOutputPort*> _listSubjectOutputPort;
      std::list<SubjectInputDataStreamPort*> _listSubjectIDSPort;
      std::list<SubjectOutputDataStreamPort*> _listSubjectODSPort;
      std::list<SubjectLink*> _listSubjectLink;
      std::list<SubjectControlLink*> _listSubjectControlLink;
      int _execState;
      std::list<YACS::ENGINE::OutGate *> loutgate;
      std::list<YACS::ENGINE::InGate *> singate;
      std::vector< std::pair<YACS::ENGINE::OutPort *, YACS::ENGINE::InPort *> > dataLinks;
      std::vector< std::pair<YACS::ENGINE::OutPort *, YACS::ENGINE::InPort *> > dataflowLinks;
    };
    
    class SubjectComposedNode: public SubjectNode
    {
    public:
      SubjectComposedNode(YACS::ENGINE::ComposedNode *composedNode, Subject *parent);
      virtual ~SubjectComposedNode();
      virtual SubjectNode* addNode(YACS::ENGINE::Catalog *catalog,
                                   std::string compo,
                                   std::string type,
                                   std::string name,
                                   bool newCompoInst);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const  { return 0; }
      virtual void loadChildren();
      virtual void loadLinks();
      virtual void completeChildrenSubjectList(SubjectNode *son);
      SubjectNode* addSubjectNode(YACS::ENGINE::Node * node,
                                  std::string name = "",
                                  YACS::ENGINE::Catalog *catalog = 0,
                                  std::string compo = "",
                                  std::string type ="");
      SubjectLink* addSubjectLink(SubjectNode *sno,
                                  SubjectDataPort *spo,
                                  SubjectNode *sni,
                                  SubjectDataPort *spi);
      SubjectControlLink* addSubjectControlLink(SubjectNode *sno,
                                                SubjectNode *sni);
      virtual void removeLink(SubjectLink* link);
      virtual void removeControlLink(SubjectControlLink* link);
      virtual void removeExternalControlLinks();
      virtual void houseKeepingAfterCutPaste(bool isCut, SubjectNode *son);
      virtual bool hasValue();
      virtual std::string getValue();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      static SubjectComposedNode* getLowestCommonAncestor(SubjectNode* snode1, SubjectNode* snode2);
    protected:
      virtual SubjectNode *createNode(YACS::ENGINE::Catalog *catalog,
                                      std::string compo,
                                      std::string type,
                                      std::string name,
                                      bool newCompoInst,
                                      int swCase=0);
      YACS::ENGINE::ComposedNode *_composedNode;
    };

    class SubjectBloc: public SubjectComposedNode
    {
    public:
      SubjectBloc(YACS::ENGINE::Bloc *bloc, Subject *parent);
      virtual ~SubjectBloc();
      virtual SubjectNode* addNode(YACS::ENGINE::Catalog *catalog,
                                   std::string compo,
                                   std::string type,
                                   std::string name,
                                   bool newCompoInst);
      virtual void removeNode(SubjectNode* child);
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const;
      virtual void recursiveUpdate(GuiEvent event, int type, Subject* son);
      virtual void houseKeepingAfterCutPaste(bool isCut, SubjectNode *son);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return BLOC;}
    protected:
      YACS::ENGINE::Bloc *_bloc;
      std::set<SubjectNode*> _children;
    };

    class SubjectComponent;
    class HMI_EXPORT SubjectContainerBase : public Subject
    {
    public:
      static SubjectContainerBase *New(YACS::ENGINE::Container* container, Subject *parent);
      SubjectContainerBase(YACS::ENGINE::Container* container, Subject *parent);
      virtual ~SubjectContainerBase();
      virtual std::string getName();
      virtual std::string getLabelForHuman() const = 0;
      virtual bool setName(std::string name);
      virtual YACS::ENGINE::Container *getContainer() const { return _container; }
      virtual std::map<std::string, std::string> getProperties();
      virtual bool setProperties(std::map<std::string, std::string> properties);
      virtual SubjectReference* attachComponent(SubjectComponent* component);
      virtual void detachComponent(SubjectComponent* component);
      virtual void moveComponent(SubjectReference* reference);
      virtual void removeSubComponentFromSet(SubjectComponent *component);
      virtual void notifyComponentsChange(GuiEvent event, int type, Subject* son);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      bool isUsed() { return !_subComponentSet.empty(); }
      TypeOfElem getType() { return CONTAINER; }
    protected:
      YACS::ENGINE::Container* _container;
      std::set<SubjectComponent*> _subComponentSet;
      std::map<SubjectComponent*,SubjectReference*> _subReferenceMap;
    };

    class HMI_EXPORT SubjectContainer: public SubjectContainerBase
    {
    public:
      SubjectContainer(YACS::ENGINE::Container *container, Subject *parent);
      void registerUndoDestroy();
      std::string getLabelForHuman() const { return std::string("Salome Container"); }
    };

    class HMI_EXPORT SubjectHPContainer : public SubjectContainerBase
    {
    public:
      SubjectHPContainer(YACS::ENGINE::HomogeneousPoolContainer* container, Subject *parent);
      void registerUndoDestroy();
      YACS::ENGINE::Container *getContainer() const;
      std::string getLabelForHuman() const { return std::string("Salome Homogeneous Pool Container"); }
    };

    class SubjectServiceNode;
    class HMI_EXPORT SubjectComponent : public Subject
    {
    public:
      friend class SubjectNode;
      SubjectComponent(YACS::ENGINE::ComponentInstance* component, Subject *parent);
      virtual ~SubjectComponent();
      virtual std::string getName();
      virtual void setContainer();
      virtual bool associateToContainer(SubjectContainerBase *subcont);
      virtual SubjectReference* attachService(SubjectServiceNode* service);
      virtual void detachService(SubjectServiceNode* service);
      virtual void moveService(SubjectReference* reference);
      virtual void removeSubServiceFromSet(SubjectServiceNode *service);
      virtual bool setProperties(std::map<std::string, std::string> properties);
      virtual std::map<std::string, std::string> getProperties();
      virtual void notifyServicesChange(GuiEvent event, int type, Subject* son);
      virtual std::pair<std::string, int> getKey();
      virtual void clean(Command *command=0);
      bool hasServices() { return !_subServiceSet.empty(); };
      void localclean(Command *command=0);
      YACS::ENGINE::ComponentInstance* getComponent() const;
      virtual TypeOfElem getType(){return COMPONENT;}
      SubjectReference* _subRefContainer;
    protected:
      int _id;
      YACS::ENGINE::ComponentInstance* _compoInst;
      std::set<SubjectServiceNode*> _subServiceSet;
      std::map<SubjectServiceNode*,SubjectReference*> _subReferenceMap;
    };

    class SubjectDataType: public Subject
    {
    public:
      SubjectDataType(YACS::ENGINE::TypeCode *typeCode, Subject *parent, std::string alias);
      virtual ~SubjectDataType();
      virtual std::string getName();
      virtual std::string getAlias();
      virtual YACS::ENGINE::TypeCode* getTypeCode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return DATATYPE;}
    protected:
      YACS::ENGINE::TypeCode *_typeCode;
      std::string _alias;
    };

    class SubjectProc: public SubjectBloc
    {
    public:
      SubjectProc(YACS::ENGINE::Proc *proc, Subject *parent);
      virtual ~SubjectProc();
      void loadProc();
      void loadComponents();
      void loadContainers();
      void loadTypes();
      virtual SubjectComponent* addComponent(std::string compoName, std::string containerName="");
      virtual SubjectContainerBase* addContainer(std::string name, std::string ref="");
      virtual SubjectContainerBase* addHPContainer(std::string name, std::string ref="");
      virtual bool addDataType(YACS::ENGINE::Catalog* catalog, std::string typeName);
      SubjectComponent* addSubjectComponent(YACS::ENGINE::ComponentInstance* compo);
      SubjectContainerBase* addSubjectContainer(YACS::ENGINE::Container* cont, std::string name = "");
      SubjectDataType* addComSubjectDataType(YACS::ENGINE::TypeCode *type, std::string alias);
      SubjectDataType* addSubjectDataType(YACS::ENGINE::TypeCode *type, std::string alias);
      void removeSubjectDataType(std::string typeName);
      void removeSubjectContainer(SubjectContainerBase* scont);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      void addPostErase(Subject* sub) {_postEraseList.push_back(sub); };
      void cleanPostErase();
      virtual TypeOfElem getType(){return SALOMEPROC;}
    protected:
      YACS::ENGINE::Proc *_proc;
      std::vector<Subject*> _postEraseList;
    };

    class SubjectForLoop: public SubjectComposedNode
    {
    public:
      SubjectForLoop(YACS::ENGINE::ForLoop *forLoop, Subject *parent);
      virtual ~SubjectForLoop();
      virtual SubjectNode* addNode(YACS::ENGINE::Catalog *catalog,
                                   std::string compo,
                                   std::string type,
                                   std::string name,
                                   bool newCompoInst);
      virtual void recursiveUpdate(GuiEvent event, int type, Subject* son);
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const { return _body; }
      virtual bool setNbSteps(std::string nbSteps);
      virtual bool hasValue();
      virtual std::string getValue();
      virtual void houseKeepingAfterCutPaste(bool isCut, SubjectNode *son);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return FORLOOP;}
    protected:
      YACS::ENGINE::ForLoop *_forLoop;
      SubjectNode* _body;
    };

    class SubjectWhileLoop: public SubjectComposedNode
    {
    public:
      SubjectWhileLoop(YACS::ENGINE::WhileLoop *whileLoop, Subject *parent);
      virtual ~SubjectWhileLoop();
      virtual SubjectNode* addNode(YACS::ENGINE::Catalog *catalog,
                                   std::string compo,
                                   std::string type,
                                   std::string name,
                                   bool newCompoInst);
      virtual void recursiveUpdate(GuiEvent event, int type, Subject* son);
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const { return _body; }
      virtual bool setCondition(std::string condition);
      virtual bool hasValue();
      virtual std::string getValue();
      virtual void houseKeepingAfterCutPaste(bool isCut, SubjectNode *son);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return WHILELOOP;}
    protected:
      YACS::ENGINE::WhileLoop *_whileLoop;
      SubjectNode* _body;
    };

    class HMI_EXPORT SubjectSwitch: public SubjectComposedNode
    {
    public:
      SubjectSwitch(YACS::ENGINE::Switch *aSwitch, Subject *parent);
      virtual ~SubjectSwitch();
      virtual SubjectNode* addNode(YACS::ENGINE::Catalog *catalog,
                                   std::string compo,
                                   std::string type,
                                   std::string name,
                                   bool newCompoInst,
                                   int swCase,
                                   bool replace = false);
      virtual void recursiveUpdate(GuiEvent event, int type, Subject* son);
      virtual void removeNode(SubjectNode* son);
      std::map<int, SubjectNode*> getBodyMap();
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual SubjectNode* getChild(YACS::ENGINE::Node* node=0) const;
      virtual bool setSelect(std::string select);
      virtual bool setCase(std::string caseId, SubjectNode* snode);
      virtual bool hasValue();
      virtual std::string getValue();
      virtual void houseKeepingAfterCutPaste(bool isCut, SubjectNode *son);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return SWITCH;}
    protected:
      YACS::ENGINE::Switch *_switch;
      std::map<int, SubjectNode*> _bodyMap;
    };

    class SubjectDynParaLoop: public SubjectComposedNode
    {
    public:
      SubjectDynParaLoop(YACS::ENGINE::DynParaLoop * dynParaLoop, Subject * parent);
      virtual ~SubjectDynParaLoop();
      virtual SubjectNode * addNode(YACS::ENGINE::Catalog * catalog,
                                    std::string compo,
                                    std::string type,
                                    std::string name,
                                    bool newCompoInst);
      virtual void recursiveUpdate(GuiEvent event, int type, Subject * son);
      virtual void completeChildrenSubjectList(SubjectNode * son);
      virtual void removeNode(SubjectNode * child);
      virtual SubjectNode * getChild(YACS::ENGINE::Node * node = NULL) const;
      virtual bool setNbBranches(std::string nbBranches);
      virtual bool hasValue();
      virtual std::string getValue();
      virtual void houseKeepingAfterCutPaste(bool isCut, SubjectNode * son);
      virtual void clean(Command * command = NULL);
      void localclean(Command * command = NULL);
    protected:
      YACS::ENGINE::DynParaLoop * _dynParaLoop;
      SubjectNode * _subjectExecNode;
      SubjectNode * _subjectInitNode;
      SubjectNode * _subjectFinalizeNode;
    };

    class SubjectForEachLoop: public SubjectDynParaLoop
    {
    public:
      SubjectForEachLoop(YACS::ENGINE::ForEachLoop *forEachLoop, Subject *parent);
      virtual ~SubjectForEachLoop();
      virtual void completeChildrenSubjectList(SubjectNode *son);
      virtual void removeNode(SubjectNode * child);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return FOREACHLOOP;}
    protected:
      YACS::ENGINE::ForEachLoop *_forEachLoop;
      SubjectNode* _splitter;
    };

    class SubjectOptimizerLoop: public SubjectDynParaLoop
    {
    public:
      SubjectOptimizerLoop(YACS::ENGINE::OptimizerLoop *optimizerLoop, Subject *parent);
      virtual ~SubjectOptimizerLoop();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return OPTIMIZERLOOP;}
      virtual bool setAlgorithm(const std::string& alglib,const std::string& symbol);
    protected:
      YACS::ENGINE::OptimizerLoop *_optimizerLoop;
    };

    class SubjectElementaryNode: public SubjectNode
    {
    public:
      SubjectElementaryNode(YACS::ENGINE::ElementaryNode *elementaryNode, Subject *parent);
      virtual ~SubjectElementaryNode();
      virtual void recursiveUpdate(GuiEvent event, int type, Subject* son);
      virtual SubjectDataPort* addInputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual SubjectDataPort* addOutputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual SubjectDataPort* addIDSPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual SubjectDataPort* addODSPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name);
      virtual bool OrderDataPorts(SubjectDataPort* portToMove, int isUp);
      virtual void removePort(SubjectDataPort* port);
      virtual void loadChildren();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual void saveLinks();
      virtual void restoreLinks();
    protected:
      YACS::ENGINE::ElementaryNode *_elementaryNode;
    };

    class SubjectInlineNode: public SubjectElementaryNode
    {
    public:
      SubjectInlineNode(YACS::ENGINE::InlineNode *inlineNode, Subject *parent);
      virtual ~SubjectInlineNode();
      virtual bool setScript(std::string script);
      virtual std::string getScript();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual bool setContainer(SubjectContainerBase *scont);
      virtual bool setExecutionMode(const std::string& mode);
    protected:
      YACS::ENGINE::InlineNode *_inlineNode;
    };

    class SubjectPythonNode: public SubjectInlineNode
    {
    public:
      SubjectPythonNode(YACS::ENGINE::PythonNode *pythonNode, Subject *parent);
      virtual ~SubjectPythonNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return PYTHONNODE;}
    protected:
      YACS::ENGINE::PythonNode *_pythonNode;
    };

    class SubjectPyFuncNode: public SubjectInlineNode
    {
    public:
      SubjectPyFuncNode(YACS::ENGINE::PyFuncNode *pyFuncNode, Subject *parent);
      virtual ~SubjectPyFuncNode();
      virtual bool setFunctionName(std::string funcName);
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return PYFUNCNODE;}
    protected:
      YACS::ENGINE::PyFuncNode *_pyFuncNode;
    };

    class SubjectServiceNode: public SubjectElementaryNode
    {
      friend class CommandAssociateServiceToComponent;
      friend class CommandAddComponentFromCatalog;
    public:
      SubjectServiceNode(YACS::ENGINE::ServiceNode *serviceNode, Subject *parent);
      virtual ~SubjectServiceNode();
      virtual void setComponentFromCatalog(YACS::ENGINE::Catalog *catalog,
                                           std::string compo,
                                           std::string service);
      virtual void setComponent();
      virtual bool associateToComponent(SubjectComponent *subcomp);
      virtual void removeSubRefComponent() { _subRefComponent = 0; };
      virtual void removeSubjectReference(){ _subjectReference = 0; };
      virtual void addSubjectReference(Subject *ref);
      virtual SubjectReference* getSubjectReference();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
    protected:
      YACS::ENGINE::ServiceNode *_serviceNode;
      SubjectReference* _subjectReference;
      SubjectReference* _subRefComponent;
   };

    class SubjectCORBANode: public SubjectServiceNode
    {
    public:
      SubjectCORBANode(YACS::ENGINE::CORBANode *corbaNode, Subject *parent);
      virtual ~SubjectCORBANode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return CORBANODE;}
    protected:
      YACS::ENGINE::CORBANode *_corbaNode;
    };

    class SubjectCppNode: public SubjectServiceNode
    {
    public:
      SubjectCppNode(YACS::ENGINE::CppNode *cppNode, Subject *parent);
      virtual ~SubjectCppNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return CPPNODE;}
    protected:
      YACS::ENGINE::CppNode *_cppNode;
    };

    class SubjectSalomeNode: public SubjectServiceNode
    {
    public:
      SubjectSalomeNode(YACS::ENGINE::SalomeNode *salomeNode, Subject *parent);
      virtual ~SubjectSalomeNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return SALOMENODE;}
    protected:
      YACS::ENGINE::SalomeNode *_salomeNode;
    };

    class SubjectSalomePythonNode: public SubjectServiceNode
    {
    public:
      SubjectSalomePythonNode(YACS::ENGINE::SalomePythonNode *salomePythonNode,
                              Subject *parent);
      virtual ~SubjectSalomePythonNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return SALOMEPYTHONNODE;}
    protected:
      YACS::ENGINE::SalomePythonNode *_salomePythonNode;
    };

    class SubjectXmlNode: public SubjectServiceNode
    {
    public:
      SubjectXmlNode(YACS::ENGINE::XmlNode *xmlNode, Subject *parent);
      virtual ~SubjectXmlNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return XMLNODE;}
    protected:
      YACS::ENGINE::XmlNode *_xmlNode;
    };

    class SubjectSplitterNode: public SubjectElementaryNode
    {
    public:
      SubjectSplitterNode(YACS::ENGINE::SplitterNode *splitterNode, Subject *parent);
      virtual ~SubjectSplitterNode();
      virtual std::string getName();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return SPLITTERNODE;}
    protected:
      YACS::ENGINE::SplitterNode *_splitterNode;
    };

    class SubjectDataNode: public SubjectElementaryNode
    {
    public:
      SubjectDataNode(YACS::ENGINE::DataNode *dataNode, Subject *parent);
      virtual ~SubjectDataNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
    protected:
      YACS::ENGINE::DataNode *_dataNode;
    };

    class SubjectPresetNode: public SubjectDataNode
    {
    public:
      SubjectPresetNode(YACS::ENGINE::PresetNode *presetNode, Subject *parent);
      virtual ~SubjectPresetNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return PRESETNODE;}
    protected:
      YACS::ENGINE::PresetNode *_presetNode;
    };

    class SubjectOutNode: public SubjectDataNode
    {
    public:
      SubjectOutNode(YACS::ENGINE::OutNode *outNode, Subject *parent);
      virtual ~SubjectOutNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return OUTNODE;}
    protected:
      YACS::ENGINE::OutNode *_outNode;
    };

    class SubjectStudyInNode: public SubjectDataNode
    {
    public:
      SubjectStudyInNode(YACS::ENGINE::StudyInNode *studyInNode, Subject *parent);
      virtual ~SubjectStudyInNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return STUDYINNODE;}
    protected:
      YACS::ENGINE::StudyInNode *_studyInNode;
    };

    class SubjectStudyOutNode: public SubjectDataNode
    {
    public:
      SubjectStudyOutNode(YACS::ENGINE::StudyOutNode *studyOutNode, Subject *parent);
      virtual ~SubjectStudyOutNode();
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      virtual TypeOfElem getType(){return STUDYOUTNODE;}
    protected:
      YACS::ENGINE::StudyOutNode *_studyOutNode;
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
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      SubjectNode* getSubjectOutNode() { return _subOutNode; };
      SubjectNode* getSubjectInNode() { return _subInNode; };
      SubjectDataPort* getSubjectOutPort() { return _outPort; };
      SubjectDataPort* getSubjectInPort() { return _inPort; };
      virtual bool setProperties(std::map<std::string, std::string> properties);
      virtual std::map<std::string, std::string> getProperties();
      virtual std::vector<std::string> knownProperties();
      virtual TypeOfElem getType(){return DATALINK;}
      void registerUndoDestroy();
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
      virtual void clean(Command *command=0);
      void localclean(Command *command=0);
      SubjectNode* getSubjectOutNode() { return _subOutNode; };
      SubjectNode* getSubjectInNode() { return _subInNode; };
      virtual TypeOfElem getType(){return CONTROLLINK;}
      void registerUndoDestroy();
    protected:
      SubjectNode* _subOutNode;
      SubjectNode* _subInNode;
      std::string _name;
      YACS::ENGINE::ComposedNode *_cla;
    };

  }
}
#endif
