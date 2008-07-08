
#include <Python.h>
#include "guiObservers.hxx"
#include "commandsProc.hxx"
#include "Node.hxx"
#include "ComposedNode.hxx"
#include "Bloc.hxx"
#include "Proc.hxx"
#include "ElementaryNode.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "PythonNode.hxx"
#include "CORBANode.hxx"
#include "CppNode.hxx"
#include "XMLNode.hxx"
#include "SalomePythonNode.hxx"
#include "DataNode.hxx"
#include "PresetNode.hxx"
#include "OutNode.hxx"
#include "StudyNodes.hxx"
#include "ForLoop.hxx"
#include "ForEachLoop.hxx"
#include "WhileLoop.hxx"
#include "Switch.hxx"
#include "OptimizerLoop.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "SalomeContainer.hxx"
#include "SalomeComponent.hxx"
#include "ComponentDefinition.hxx"
#include "TypeCode.hxx"

#include "guiContext.hxx"

#include <string>
#include <sstream>
#include <vector>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

// ----------------------------------------------------------------------------

void Subject::erase(Subject* sub)
{
  sub->clean();
  delete sub;
}

// ----------------------------------------------------------------------------

Subject::Subject(Subject *parent) : _parent(parent)
{
  _destructible = true;
}

/*!
 *  Destructor must only be called by Subject::erase to clean
 *  completely the context (detach observers), before the delete
 *  process itself. Some actions involving dynamic_cast can't be done
 *  in destructor.
 */
Subject::~Subject()
{
  DEBTRACE("Subject::~Subject " << this << " "<< getName());
}

/*!
 * Clean process prior to delete is redefined in derived classes: a local clean
 * treatment relative to the derived class, then a call to the parent class clean
 * method.
 */
void Subject::clean()
{
  localClean();
}

/*!
 *  the local clean method of base class of subjects take care of Observers.
 *  Remaining Observers in the list are detached, if an abserver has no more
 *  Subject to observe, it can be deleted.
 */
void Subject::localClean()
{
  DEBTRACE("Subject::localClean ");
  set<GuiObserver*>::iterator it;
  set<GuiObserver*> copySet = _setObs;
  for (it = copySet.begin(); it != copySet.end(); ++it)
  {
    GuiObserver* anObs = (*it);
    detach(anObs);
    int nbsub = anObs->getNbSubjects();
    DEBTRACE("nbSubjects=" << nbsub);
    if (nbsub <= 0) delete anObs ;
  }
  _setObs.clear();
}

void Subject::attach(GuiObserver *obs)
{
  _setObs.insert(obs);
  obs->incrementSubjects(this);
}

void Subject::detach(GuiObserver *obs)
{
  DEBTRACE("Subject::detach " << obs);
  obs->decrementSubjects(this);
  _setObs.erase(obs);
}

std::string Subject::getName()
{
  return "generic";
}

bool Subject::setName(std::string name)
{
  return false;
}

void Subject::select(bool isSelected)
{
  DEBTRACE("Subject::select " << isSelected << " " << this);
  for (set<GuiObserver *>::iterator it = _setObs.begin(); it != _setObs.end(); ++it)
    {
      GuiObserver* currOb = *it;
      currOb->select(isSelected);
    }
}

void Subject::update(GuiEvent event,int type, Subject* son)
{
  DEBTRACE("Subject::update " << type << "," << event << "," << son);
  for (set<GuiObserver *>::iterator it = _setObs.begin(); it != _setObs.end(); ++it)
    {
      DEBTRACE("Subject::update " << *it);
      (*it)->update(event, type, son);
    }
}

Subject* Subject::getParent()
{
  return _parent;
}

bool Subject::destroy(Subject *son)
{
  string toDestroy = son->getName();
  DEBTRACE("Subject::destroy " << toDestroy);
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";

  if (dynamic_cast<SubjectProc*>(son))
    position = proc->getName();
  else
    {
      if (SubjectNode *subNode = dynamic_cast<SubjectNode*>(son))
        {
	  if (subNode->getNode()->getFather() )
	    position = proc->getChildName(subNode->getNode());
        }
      else if (dynamic_cast<SubjectDataPort*>(son))
        {
          SubjectNode *subNodep = dynamic_cast<SubjectNode*>(son->getParent());
          if (dynamic_cast<SubjectProc*>(subNodep))
            position = proc->getName();
          else
            position = proc->getChildName(subNodep->getNode());
        }
      else if (dynamic_cast<SubjectLink*>(son))
        {
          SubjectNode *subNodep = dynamic_cast<SubjectNode*>(son->getParent());
          if (dynamic_cast<SubjectProc*>(subNodep))
            position = proc->getName();
          else
            position = proc->getChildName(subNodep->getNode());
        }
    }
  if (son->isDestructible())
    {
      CommandDestroy* command = new CommandDestroy(position, son);
      if (command->execute())
        {
          DEBTRACE("Destruction done: " << toDestroy);
          return true;
        }
      else delete command;
    }
  return false;
}

void Subject::loadChildren()
{
}

void Subject::loadLinks()
{
}

void Subject::addSubjectReference(Subject *ref)
{
  DEBTRACE("Subject::addSubjectReference " << getName() << " " << ref->getName());
  SubjectReference *son = new SubjectReference(ref, this);
  update(ADDREF, 0, son);
}

// ----------------------------------------------------------------------------

GuiObserver::GuiObserver()
{
  DEBTRACE("GuiObserver::GuiObserver " << this);
  _subjectSet.clear();
}

GuiObserver::~GuiObserver()
{
  DEBTRACE("GuiObserver::~GuiObserver " << this);
}

void GuiObserver::select(bool isSelected)
{
  DEBTRACE("GuiObserver::select() " << isSelected);
}

void GuiObserver::update(GuiEvent event, int type,  Subject* son)
{
  DEBTRACE("GuiObserver::update, event not handled " << event << " " << type );
}

/*!
 * only called by subject when attach to subject.
 * @see Subject::attach
 */
void GuiObserver::incrementSubjects(Subject *subject)
{
  if (_subjectSet.count(subject))
    DEBTRACE("subject " << subject << " is already a subject of observer " << this << "---------------------------");
  _subjectSet.insert(subject);
  //DEBTRACE(this << " " << _subjectSet.size());
}

/*!
 * only called by subject when detach from subject.
 * @see Subject::detach
 */
void GuiObserver::decrementSubjects(Subject *subject)
{
  if (_subjectSet.count(subject))
    _subjectSet.erase(subject);
  else
    DEBTRACE("subject " << subject << " is not a subject of observer " << this << "---------------------------");
  //DEBTRACE(this << " " << _subjectSet.size());
}

/*! 
 * Gets the number of subjects observed.
 * used by subject. When the subject is erased (Subject::erase),
 * remaining observers are detached from subjects. If an observer has no
 * more subject, it can be deleted.
 * @see Subject::erase Subject::localClean
 */
int GuiObserver::getNbSubjects()
{
  return _subjectSet.size();
}

// ----------------------------------------------------------------------------

SubjectReference::SubjectReference(Subject* ref, Subject *parent)
  : Subject(parent), _reference(ref)
{
}

SubjectReference::~SubjectReference()
{
}

void SubjectReference::clean()
{
  localClean();
  Subject::clean();
}

void SubjectReference::localClean()
{
  DEBTRACE("SubjectReference::localClean ");
}

std::string SubjectReference::getName()
{
  std::stringstream name;
  name << "ref-->" << _reference->getName();
  return name.str();
}

Subject* SubjectReference::getReference() const
{
  return _reference;
}

// ----------------------------------------------------------------------------

SubjectNode::SubjectNode(YACS::ENGINE::Node *node, Subject *parent)
  : Subject(parent), _node(node)
{
  _listSubjectInputPort.clear();
  _listSubjectOutputPort.clear();
  _listSubjectIDSPort.clear();
  _listSubjectODSPort.clear();
  _listSubjectLink.clear();
  _listSubjectControlLink.clear();
  Dispatcher* d=Dispatcher::getDispatcher();
  d->addObserver(this,node,"status");
}

/*!
 * all destruction is done in generic class SubjectNode
 */
SubjectNode::~SubjectNode()
{
  DEBTRACE("SubjectNode::~SubjectNode " << getName());
  Dispatcher::getDispatcher()->removeObserver(this,_node,"status");

  ComposedNode* father = _node->getFather();
  if (father)
    try
      {
        Bloc *bloc = dynamic_cast<Bloc*>(father);
        if (bloc) bloc->edRemoveChild(_node);
        else
          {
            Loop *loop = dynamic_cast<Loop*>(father);
            if (loop) loop->edRemoveChild(_node);
            else
              {
		ForEachLoop *feloop = dynamic_cast<ForEachLoop*>(father);
		if (feloop && getName() != ForEachLoop::NAME_OF_SPLITTERNODE) {
		  DEBTRACE("SubjectNode::localClean: remove for each loop body");
		  feloop->edRemoveChild(_node);
		}
		else
		  {
		    Switch *aSwitch = dynamic_cast<Switch*>(father);
		    if (aSwitch) aSwitch->edRemoveChild(_node);
		  }
              }
          }
      }
    catch (YACS::Exception &e)
      {
        DEBTRACE("------------------------------------------------------------------------------");
        DEBTRACE("SubjectNode::localClean: father->edRemoveChild: YACS exception " << e.what());
        DEBTRACE("------------------------------------------------------------------------------");
      }
}

void SubjectNode::clean()
{
  localClean();
  Subject::clean();
}

void SubjectNode::localClean()
{
  DEBTRACE("SubjectNode::localClean ");
  removeExternalLinks();
  {
    list<SubjectLink*>::iterator its;
    list<SubjectLink*> cpll = _listSubjectLink;
    for (its = cpll.begin(); its != cpll.end(); ++its)
      erase(*its);
  }
  {
    list<SubjectControlLink*>::iterator its;
    list<SubjectControlLink*> cplcl = _listSubjectControlLink;
    for (its = cplcl.begin(); its != cplcl.end(); ++its)
      erase(*its);
  }
  {
    list<SubjectInputPort*>::iterator iti;
    list<SubjectInputPort*> cpli = _listSubjectInputPort;
    for(iti = cpli.begin(); iti != cpli.end(); ++iti)
      erase(*iti);
  }
  {
    list<SubjectOutputPort*>::iterator ito;
    list<SubjectOutputPort*> cplo = _listSubjectOutputPort;
    for(ito = cplo.begin(); ito != cplo.end(); ++ito)
      erase(*ito);
  }
  {
    list<SubjectInputDataStreamPort*>::iterator itid;
    list<SubjectInputDataStreamPort*> cplid = _listSubjectIDSPort;
    for(itid = cplid.begin(); itid != cplid.end(); ++itid)
      erase(*itid);
  }
  {
    list<SubjectOutputDataStreamPort*>::iterator itod;
    list<SubjectOutputDataStreamPort*> cplod = _listSubjectODSPort;
    for(itod = cplod.begin(); itod != cplod.end(); ++itod)
      erase(*itod);
  }
  if (_parent)
    {
      if( SubjectBloc* sb = dynamic_cast<SubjectBloc*>(_parent) )
	sb->removeNode(this);
      else if( SubjectForLoop* sfl = dynamic_cast<SubjectForLoop*>(_parent) )
	sfl->completeChildrenSubjectList( 0 );
      else if( SubjectWhileLoop* swl = dynamic_cast<SubjectWhileLoop*>(_parent) )
	swl->completeChildrenSubjectList( 0 );
      else if( SubjectForEachLoop* sfel = dynamic_cast<SubjectForEachLoop*>(_parent) )
	sfel->completeChildrenSubjectList( 0 );
    }
}

void SubjectNode::reparent(Subject* parent)
{
  // remove this node from children list of an old parent
  if (_parent)
  {
    if( SubjectBloc* sb = dynamic_cast<SubjectBloc*>(_parent) )
      sb->removeNode(this);
  }

  // reparent
  _parent = parent;
  
  // add this node into the children list of a new parent
  if (_parent)
  {
    if( SubjectBloc* sb = dynamic_cast<SubjectBloc*>(_parent) )
      sb->completeChildrenSubjectList(this);
    else if( SubjectSwitch* ss = dynamic_cast<SubjectSwitch*>(_parent) )
      ss->completeChildrenSubjectList(this);
  }  
}

std::string SubjectNode::getName()
{
  return _node->getName();
}

YACS::ENGINE::Node* SubjectNode::getNode()
{
  return _node;
}

bool SubjectNode::setName(std::string name)
{
  DEBTRACE("SubjectNode::setName " << name);
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node))
    position = proc->getChildName(_node);
  else
    position = _node->getName();
  CommandRenameNode* command = new CommandRenameNode(position, name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      update(RENAME, 0, this);
      return true;
    }
  else delete command;
  return false;
}
void SubjectNode::notifyObserver(Node* object,const std::string& event)
{
  DEBTRACE("SubjectNode::notifyObserver " << object->getName() << " " << event);
  TypeOfElem ntyp = ProcInvoc::getTypeOfNode(object);
  update(UPDATE, ntyp , 0 );
}

SubjectInputPort* SubjectNode::addSubjectInputPort(YACS::ENGINE::InputPort *port,
                                                   std::string name)
{
  string theName = name;
  if (name.empty()) theName =port->getName();
  DEBTRACE("SubjectNode::addSubjectInputPort "<< theName);
  SubjectInputPort *son = new SubjectInputPort(port, this);
  GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(port)] = son;
  _listSubjectInputPort.push_back(son);
  if (!name.empty()) son->setName(name);
  update(ADD, INPUTPORT ,son);
  YACS::ENGINE::TypeCode *typcod = port->edGetType();
  GuiContext::getCurrent()->getSubjectProc()->addSubjectDataType(typcod);
  return son;
}

void SubjectNode::update( GuiEvent event, int type, Subject* son )
{
  Subject::update( event, type, son );
  
  // remove subject data type if necessary
  YACS::HMI::SubjectDataPort* aSPort = dynamic_cast< YACS::HMI::SubjectDataPort* >( son );
  if ( aSPort && event == REMOVE )
  {
    YACS::ENGINE::DataPort* aEPort = aSPort->getPort();
    if ( aEPort )
    {
      YACS::ENGINE::TypeCode* aTypeCode = aEPort->edGetType();
      if ( aTypeCode )
        GuiContext::getCurrent()->getSubjectProc()->removeSubjectDataType( aTypeCode );
    }
  }
}

SubjectOutputPort* SubjectNode::addSubjectOutputPort(YACS::ENGINE::OutputPort *port,
                                                     std::string name)
{
  string theName = name;
  if (name.empty()) theName =port->getName();
  DEBTRACE("SubjectNode::addSubjectOutputPort "<< theName);
  SubjectOutputPort *son = new SubjectOutputPort(port, this);
  GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(port)] = son;
  _listSubjectOutputPort.push_back(son);
  if (!name.empty()) son->setName(name);
  update(ADD, OUTPUTPORT ,son);
  YACS::ENGINE::TypeCode *typcod = port->edGetType();
  GuiContext::getCurrent()->getSubjectProc()->addSubjectDataType(typcod);
  return son;
}

SubjectInputDataStreamPort* SubjectNode::addSubjectIDSPort(YACS::ENGINE::InputDataStreamPort *port,
                                                           std::string name)
{
  string theName = name;
  if (name.empty()) theName =port->getName();
  DEBTRACE("SubjectNode::addSubjectIDSPort "<< theName);
  SubjectInputDataStreamPort *son = new SubjectInputDataStreamPort(port, this);
  GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(port)] = son;
  _listSubjectIDSPort.push_back(son);
  if (!name.empty()) son->setName(name);
  update(ADD, INPUTDATASTREAMPORT ,son);
  YACS::ENGINE::TypeCode *typcod = port->edGetType();
  GuiContext::getCurrent()->getSubjectProc()->addSubjectDataType(typcod);
  return son;
}


SubjectOutputDataStreamPort* SubjectNode::addSubjectODSPort(YACS::ENGINE::OutputDataStreamPort *port,
                                                            std::string name)
{
  string theName = name;
  if (name.empty()) theName =port->getName();
  DEBTRACE("SubjectNode::addSubjectODSPort "<< theName);
  SubjectOutputDataStreamPort *son = new SubjectOutputDataStreamPort(port, this);
  GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(port)] = son;
  _listSubjectODSPort.push_back(son);
  if (!name.empty()) son->setName(name);
  update(ADD, OUTPUTDATASTREAMPORT ,son);
  YACS::ENGINE::TypeCode *typcod = port->edGetType();
  GuiContext::getCurrent()->getSubjectProc()->addSubjectDataType(typcod);
  return son;
}

bool SubjectNode::tryCreateLink(SubjectNode *subOutNode, SubjectNode *subInNode)
{
  DEBTRACE("SubjectNode::tryCreateLink " << subOutNode->getName() << " " << subInNode->getName());
  Proc *proc = GuiContext::getCurrent()->getProc();
  Node *outNode = subOutNode->getNode();
  string outNodePos = proc->getChildName(outNode);
  Node *inNode = subInNode->getNode();
  string inNodePos = proc->getChildName(inNode);
  CommandAddControlLink *command = new CommandAddControlLink(outNodePos, inNodePos);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      ComposedNode *cla = ComposedNode::getLowestCommonAncestor(outNode->getFather(),
                                                                inNode->getFather());
      SubjectComposedNode *scla = dynamic_cast<SubjectComposedNode*>(subOutNode->getParent());
      ComposedNode *ancestor = outNode->getFather();
      while (ancestor && ancestor != cla)
        {
          ancestor = ancestor->getFather();
          scla = dynamic_cast<SubjectComposedNode*>(scla->getParent());
          assert(scla);
        }
      DEBTRACE(scla->getName());
      scla->addSubjectControlLink(subOutNode,subInNode);
      return true;
    }
  else
    {
      delete command;
      return false;
    }
}

void SubjectNode::removeExternalLinks()
{
  DEBTRACE("SubjectNode::removeExternalLinks " << getName());
  std::vector< std::pair<OutPort *, InPort *> > listLeaving  = getNode()->getSetOfLinksLeavingCurrentScope();
  std::vector< std::pair<InPort *, OutPort *> > listIncoming = getNode()->getSetOfLinksComingInCurrentScope();
  std::vector< std::pair<OutPort *, InPort *> > globalList = listLeaving;
  std::vector< std::pair<InPort *, OutPort *> >::iterator it1;
  for (it1 = listIncoming.begin(); it1 != listIncoming.end(); ++it1)
    {
      std::pair<OutPort *, InPort *> outin = std::pair<OutPort *, InPort *>((*it1).second, (*it1).first);
      globalList.push_back(outin);
    }
  std::vector< std::pair<OutPort *, InPort *> >::iterator it2;
  for (it2 = globalList.begin(); it2 != globalList.end(); ++it2)
    {
      SubjectLink* subject = 0;
      if (GuiContext::getCurrent()->_mapOfSubjectLink.count(*it2))
        {
          subject = GuiContext::getCurrent()->_mapOfSubjectLink[*it2];
          assert(subject);
          DEBTRACE("link to remove " << subject->getName());
          erase(subject);
          GuiContext::getCurrent()->_mapOfSubjectLink.erase(*it2);
        }
      else
        {
          DEBTRACE("------------------------------------------------------------------------------");
          DEBTRACE("SubjectNode::removeExternalLinks(): an external link not in map...");
          DEBTRACE("------------------------------------------------------------------------------");
        }
    } 
      
}

// ----------------------------------------------------------------------------

SubjectComposedNode::SubjectComposedNode(YACS::ENGINE::ComposedNode *composedNode,
                                         Subject *parent)
  : SubjectNode(composedNode, parent), _composedNode(composedNode)
{
}

/*!
 * all generic destruction is done in generic class SubjectNode
 */
SubjectComposedNode::~SubjectComposedNode()
{
  DEBTRACE("SubjectComposedNode::~SubjectComposedNode " << getName());
}

void SubjectComposedNode::clean()
{
  localClean();
  SubjectNode::clean();
}

void SubjectComposedNode::localClean()
{
  DEBTRACE("SubjectComposedNode::localClean ");
}

SubjectNode* SubjectComposedNode::addNode(YACS::ENGINE::Catalog *catalog,
					  std::string compo,
					  std::string type,
					  std::string name)
{
  DEBTRACE("SubjectComposedNode::addNode("<<catalog<<","<<compo<<","<<type<<","<<name<<")");
  SubjectNode* body = 0;
  return body;
}

SubjectNode *SubjectComposedNode::createNode(YACS::ENGINE::Catalog *catalog,
                                             std::string compo,
                                             std::string type,
                                             std::string name,
                                             int swCase)
{
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);
  CommandAddNodeFromCatalog *command = new CommandAddNodeFromCatalog(catalog,
                                                                     compo,
                                                                     type,
                                                                     position,
                                                                     name,
                                                                     swCase);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      Node * node = command->getNode();
      SubjectNode *son = addSubjectNode(node,"",catalog,compo,type);
      son->loadChildren();
      son->loadLinks();
      return son;
    }
  else delete command;
  return 0;
}

SubjectNode *SubjectComposedNode::addSubjectNode(YACS::ENGINE::Node * node,
                                                 std::string name,
                                                 YACS::ENGINE::Catalog *catalog,
                                                 std::string compo,
                                                 std::string type)
{
  string theName = name;
  if (name.empty()) theName =node->getName();
  DEBTRACE("SubjectComposedNode::addSubjectNode "<< theName);
  TypeOfElem ntyp = ProcInvoc::getTypeOfNode(node);
  DEBTRACE("TypeOfNode: " << ntyp);
  SubjectNode *son = 0;
  switch(ntyp)
    {
    case BLOC:
      son = new SubjectBloc(dynamic_cast<YACS::ENGINE::Bloc*>(node), this);
      break;
    case PYTHONNODE:
      son = new SubjectPythonNode(dynamic_cast<YACS::ENGINE::PythonNode*>(node), this);
      break;
    case PYFUNCNODE:
      son = new SubjectPyFuncNode(dynamic_cast<YACS::ENGINE::PyFuncNode*>(node), this);
      break;
    case CORBANODE:
      son = new SubjectCORBANode(dynamic_cast<YACS::ENGINE::CORBANode*>(node), this);
      break;
    case CPPNODE:
      son = new SubjectCppNode(dynamic_cast<YACS::ENGINE::CppNode*>(node), this);
      break;
    case SALOMENODE:
      son = new SubjectSalomeNode(dynamic_cast<YACS::ENGINE::SalomeNode*>(node), this);
      break;
    case SALOMEPYTHONNODE:
      son = new SubjectSalomePythonNode(dynamic_cast<YACS::ENGINE::SalomePythonNode*>(node), this);
      break;
    case XMLNODE:
      son = new SubjectXmlNode(dynamic_cast<YACS::ENGINE::XmlNode*>(node), this);
      break;
    case SPLITTERNODE:
      son = new SubjectSplitterNode(dynamic_cast<YACS::ENGINE::SplitterNode*>(node), this);
      break;
    case PRESETNODE:
      son = new SubjectPresetNode(dynamic_cast<YACS::ENGINE::PresetNode*>(node), this);
      break;
    case OUTNODE:
      son = new SubjectOutNode(dynamic_cast<YACS::ENGINE::OutNode*>(node), this);
      break;
    case STUDYINNODE:
      son = new SubjectStudyInNode(dynamic_cast<YACS::ENGINE::StudyInNode*>(node), this);
      break;
    case STUDYOUTNODE:
      son = new SubjectStudyOutNode(dynamic_cast<YACS::ENGINE::StudyOutNode*>(node), this);
      break;
    case FORLOOP:
      son = new SubjectForLoop(dynamic_cast<YACS::ENGINE::ForLoop*>(node), this);
      break;
    case WHILELOOP:
      son = new SubjectWhileLoop(dynamic_cast<YACS::ENGINE::WhileLoop*>(node), this);
      break;
    case SWITCH:
      son = new SubjectSwitch(dynamic_cast<YACS::ENGINE::Switch*>(node), this);
      break;
    case FOREACHLOOP:
      son = new SubjectForEachLoop(dynamic_cast<YACS::ENGINE::ForEachLoop*>(node), this);
      break;
    case OPTIMIZERLOOP:
      son = new SubjectOptimizerLoop(dynamic_cast<YACS::ENGINE::OptimizerLoop*>(node), this);
      break;
    default:
      throw YACS::Exception("Not implemented");
      //assert(0);
    }
  assert(son);
  GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(node)] = son;
  if (!name.empty()) son->setName(name);
  completeChildrenSubjectList(son);
  update(ADD, ntyp ,son);
  if (SubjectServiceNode *service = dynamic_cast<SubjectServiceNode*>(son))
    if (catalog && !compo.empty() && !type.empty()) // --- clone from catalog: set component
      service->setComponentFromCatalog(catalog,compo,type);
    else
      service->setComponent();
  return son;
}

void SubjectComposedNode::completeChildrenSubjectList(SubjectNode *son)
{
}

void SubjectComposedNode::loadChildren()
{
  list<Node *> setOfNode= _composedNode->edGetDirectDescendants();
  if (ForEachLoop *feloop = dynamic_cast<ForEachLoop*>(_composedNode))
    {
      Node *node2Insert=feloop->getChildByName(ForEachLoop::NAME_OF_SPLITTERNODE);
      if(find(setOfNode.begin(),setOfNode.end(),node2Insert)==setOfNode.end())
        setOfNode.push_back(node2Insert);
    }
  for(list<Node *>::iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      try
        {
	  SubjectNode * son = addSubjectNode(*iter);
	  son->loadChildren();
        }
      catch(YACS::Exception& ex)
        {
          std::cerr << "Unknown type of node" << std::endl;
        }
    }
  list<InputPort*>  listInputPorts  = _composedNode->getLocalInputPorts();
  list<OutputPort*> listOutputPorts = _composedNode->getLocalOutputPorts();
  list<InputDataStreamPort*>  listIDSPorts = _composedNode->getSetOfInputDataStreamPort();
  list<OutputDataStreamPort*> listODSPorts = _composedNode->getSetOfOutputDataStreamPort();
  list<InputPort*>::const_iterator iti;
  for (iti = listInputPorts.begin(); iti != listInputPorts.end(); ++iti)
    addSubjectInputPort(*iti);
  list<OutputPort*>::const_iterator ito;
  for (ito = listOutputPorts.begin(); ito != listOutputPorts.end(); ++ito)
    addSubjectOutputPort(*ito);
}

SubjectLink* SubjectComposedNode::addSubjectLink(SubjectNode *sno,
                                                 SubjectDataPort *spo,
                                                 SubjectNode *sni,
                                                 SubjectDataPort *spi)
{
  DEBTRACE("SubjectComposedNode::addSubjectLink");
  SubjectLink *son = new SubjectLink(sno, spo, sni, spi, this);
  OutPort *outp = sno->getNode()->getOutPort(spo->getName());
  InPort *inp = sni->getNode()->getInPort(spi->getName());
  pair<OutPort*,InPort*> keyLink(outp,inp);
  GuiContext::getCurrent()->_mapOfSubjectLink[keyLink] = son;
  _listSubjectLink.push_back(son);
  spo->addSubjectLink(son);
  spi->addSubjectLink(son);
  update(ADDLINK, DATALINK, son);
  DEBTRACE("addSubjectLink: " << getName() << " " << son->getName());
  return son;
}

void SubjectComposedNode::removeLink(SubjectLink* link)
{
  DEBTRACE("removeSubjectLink: " << getName());
  link->getSubjectOutPort()->removeSubjectLink(link);
  link->getSubjectInPort()->removeSubjectLink(link);
  _listSubjectLink.remove(link);
}

SubjectControlLink* SubjectComposedNode::addSubjectControlLink(SubjectNode *sno,
                                                        SubjectNode *sni)
{
  SubjectControlLink *son = new SubjectControlLink(sno, sni, this);
  Node *outn = sno->getNode();
  Node *inn = sni->getNode();
  pair<Node*,Node*> keyLink(outn,inn);

  GuiContext::getCurrent()->_mapOfSubjectControlLink[keyLink] = son;
  _listSubjectControlLink.push_back(son);
  sno->addSubjectControlLink(son);
  sni->addSubjectControlLink(son);
  update(ADDCONTROLLINK, CONTROLLINK, son);
  DEBTRACE("addSubjectControlLink: " << getName() << " " << son->getName());
  return son;
}

void SubjectComposedNode::removeControlLink(SubjectControlLink* link)
{
  DEBTRACE("removeSubjectControlLink: " << getName());
  link->getSubjectOutNode()->removeSubjectControlLink(link);
  link->getSubjectInNode()->removeSubjectControlLink(link);
  _listSubjectControlLink.remove(link);
}

/*!
 * loadLinks is used when an existing scheme has been loaded in memory, to create gui representation.
 * Gui representation of links is done after node representation (loadChildren).
 * Proc is explored recursively to find the composedNodes and create the corresponding links
 * representation, from bottom to top.
 * For each composedNode, data links representation are created first and stored in a map to avoid
 * double represention. Then control links representation are created.
 */
void SubjectComposedNode::loadLinks()
{
  list<Node *> setOfNode= _composedNode->edGetDirectDescendants();
  for(list<Node *>::iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      ComposedNode *cnSon = dynamic_cast<ComposedNode*>(*iter);
      if (cnSon)
        {
          SubjectNode *subSon = GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(*iter)];
          assert(subSon);
          SubjectComposedNode *subCnSon = dynamic_cast<SubjectComposedNode*>(subSon);
          assert (subCnSon);
          subCnSon->loadLinks();
        }
    }

  std::vector<std::pair<OutPort*,InPort*> > setOfLinks = _composedNode->getSetOfInternalLinks();
  std::vector<std::pair<OutPort*,InPort*> >::iterator itp;
  for (itp = setOfLinks.begin(); itp != setOfLinks.end(); ++itp)
    if (!GuiContext::getCurrent()->_mapOfSubjectLink.count(*itp))
      {
        OutPort *outp = (*itp).first;
        InPort *inp = (*itp).second;
        Node *outn = outp->getNode();
        Node *inn = inp->getNode();
        DEBTRACE(outn->getName()<<"."<<outp->getName()<<"->"<<inn->getName()<<"."<<inp->getName());
        SubjectNode *sno = GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(outn)];
        SubjectNode *sni = GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(inn)];
        SubjectDataPort *spo = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(outp)];
        SubjectDataPort *spi = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(inp)];
        addSubjectLink(sno,spo,sni,spi);
      }

  std::list<Node*> setOfNodes = _composedNode->edGetDirectDescendants();
  std::list<Node*>::const_iterator itn;
  for(itn = setOfNodes.begin(); itn != setOfNodes.end(); ++itn)
    {
      SubjectNode* sno = GuiContext::getCurrent()->_mapOfSubjectNode[*itn];
      OutGate* outgate = (*itn)->getOutGate();
      std::set<InGate*> setIngate = outgate->edSetInGate();
      std::set<InGate*>::const_iterator itg;
      for(itg = setIngate.begin(); itg != setIngate.end(); ++itg)
        {
          Node* inNode = (*itg)->getNode();
          SubjectNode* sni = GuiContext::getCurrent()->_mapOfSubjectNode[inNode];
          addSubjectControlLink(sno,sni);
        }
    }
}

//! Retrieves the lowest common ancestor of 2 nodes
/*!
 * 
 * \note Retrieves the lowest common ancestor of 'node1' AND 'node2'. 
 *       If  'node1' or 'node2' are both or indiscriminately instances of ComposedNode and that
 *       'node1' is in descendance of 'node2' (resp. 'node2' in descendance of 'node1')
 *       'node2' is returned (resp. 'node1').
 * \exception Exception : if 'node1' and 'node2' do not share the same genealogy.
 * \return The lowest common ancestor if it exists.
 *
 */
SubjectComposedNode* SubjectComposedNode::getLowestCommonAncestor(SubjectNode* snode1, SubjectNode* snode2)
{
  Node* node1 = snode1->getNode();
  Node* node2 = snode2->getNode();

  ComposedNode *node = ComposedNode::getLowestCommonAncestor(node1->getFather(), node2->getFather());
  SubjectComposedNode* snode = dynamic_cast<SubjectComposedNode*>( GuiContext::getCurrent()->_mapOfSubjectNode[node] );
  return snode;
}

// ----------------------------------------------------------------------------

SubjectBloc::SubjectBloc(YACS::ENGINE::Bloc *bloc, Subject *parent)
  : SubjectComposedNode(bloc, parent), _bloc(bloc)
{
  _children.clear();
}

/*!
 * all generic destruction is done in generic class SubjectNode
 */
SubjectBloc::~SubjectBloc()
{
  DEBTRACE("SubjectBloc::~SubjectBloc " << getName());
}

void SubjectBloc::clean()
{
  localClean();
  SubjectComposedNode::clean();
}

void SubjectBloc::localClean()
{
  DEBTRACE("SubjectBloc::localClean ");
  set<SubjectNode*>::iterator it;
  set<SubjectNode*> copyChildren = _children;
  for (it = copyChildren.begin(); it !=copyChildren.end(); ++it)
    erase(*it);
}

SubjectNode* SubjectBloc::addNode(YACS::ENGINE::Catalog *catalog,
				  std::string compo,
				  std::string type,
				  std::string name)
{
  DEBTRACE("SubjectBloc::addNode( " << catalog << ", " << compo << ", " << type << ", " << name << " )");
  SubjectNode* child = createNode(catalog, compo, type, name);
  return child;
}

void SubjectBloc::completeChildrenSubjectList(SubjectNode *son)
{
  _children.insert(son);
}

void SubjectBloc::removeNode(SubjectNode* child)
{
  _children.erase(child);
}

SubjectNode* SubjectBloc::getChild(YACS::ENGINE::Node* node) const
{
  SubjectNode* aChild = 0;

  if (node)
  {
    set<SubjectNode*>::iterator it = _children.begin();
    for ( ; it != _children.end(); it++ )
      if ( (*it)->getNode() == node )
      {
	aChild = (*it);
	break;
      }
  }

  return aChild;
}

// ----------------------------------------------------------------------------

SubjectProc::SubjectProc(YACS::ENGINE::Proc *proc, Subject *parent)
  : SubjectBloc(proc, parent), _proc(proc)
{
}

SubjectProc::~SubjectProc()
{
  DEBTRACE("SubjectProc::~SubjectProc " << getName());
}

void SubjectProc::clean()
{
  localClean();
  SubjectBloc::clean();
}

void SubjectProc::localClean()
{
  DEBTRACE("SubjectProc::localClean ");
}

void SubjectProc::loadProc()
{
  DEBTRACE("SubjectProc::loadProc "  << getName());
  loadChildren();
  loadLinks();
  loadComponents();
  loadContainers();
}

/*!
 * loadComponents is used when an existing scheme has been loaded in memory,
 * to create subjects for components stored in the schema file, but are not
 * associated with any service nodes. Note, that if such component is associated
 * to any container, the subject for this container is also created, if it is not
 * exist yet.
 */
void SubjectProc::loadComponents()
{
  Proc* aProc = GuiContext::getCurrent()->getProc();
  for (map<pair<string,int>, ComponentInstance*>::const_iterator itComp = aProc->componentInstanceMap.begin();
       itComp != aProc->componentInstanceMap.end(); ++itComp)
    if ( GuiContext::getCurrent()->_mapOfSubjectComponent.find((*itComp).second)
	 ==
	 GuiContext::getCurrent()->_mapOfSubjectComponent.end() )
    { // engine object for component already exists => add only a subject for it
      addSubjectComponent((*itComp).second);
    }
}

/*!
 * loadContainers is used when an existing scheme has been loaded in memory,
 * to create subjects for containers stored in the schema file, but are not
 * associated with components.
 */
void SubjectProc::loadContainers()
{
  Proc* aProc = GuiContext::getCurrent()->getProc();
  for (map<string, Container*>::const_iterator itCont = aProc->containerMap.begin();
       itCont != aProc->containerMap.end(); ++itCont)
    if ( GuiContext::getCurrent()->_mapOfSubjectContainer.find((*itCont).second)
	 ==
	 GuiContext::getCurrent()->_mapOfSubjectContainer.end() )
      // engine object for container already exists => add only a subject for it
      addSubjectContainer((*itCont).second, (*itCont).second->getName());
}

SubjectComponent* SubjectProc::addComponent(std::string name)
{
  DEBTRACE("SubjectProc::addComponent " << name);
  CommandAddComponentInstance *command = new CommandAddComponentInstance(name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      ComponentInstance *compo = command->getComponentInstance();
      SubjectComponent *son = addSubjectComponent(compo);
      return son;
    }
  else delete command;
  return 0;
}

SubjectContainer* SubjectProc::addContainer(std::string name, std::string ref)
{
  DEBTRACE("SubjectProc::addContainer " << name << " " << ref);
  if (! GuiContext::getCurrent()->getProc()->containerMap.count(name))
    {
      CommandAddContainer *command = new CommandAddContainer(name,ref);
      if (command->execute())
        {
	  GuiContext::getCurrent()->getInvoc()->add(command);
          Container *cont = command->getContainer();
          SubjectContainer *son = addSubjectContainer(cont, name);
          GuiContext::getCurrent()->getProc()->containerMap[name] = cont;
          return son;
        }
      else
          delete command;
    }
  else GuiContext::getCurrent()->_lastErrorMessage = "There is already a container with that name";
  return 0;
}

SubjectDataType* SubjectProc::addDataType(YACS::ENGINE::Catalog* catalog, std::string typeName)
{
  DEBTRACE("SubjectProc::addDataType " << typeName);
  CommandAddDataTypeFromCatalog *command = new CommandAddDataTypeFromCatalog(catalog, typeName);
  if (command->execute())
    {
      DEBTRACE("new datatype " << typeName);
      GuiContext::getCurrent()->getInvoc()->add(command);
      SubjectDataType *son = addSubjectDataType(command->getTypeCode());
      return son;
    }
  else delete command;
  return 0;
}

SubjectComponent* SubjectProc::addSubjectComponent(YACS::ENGINE::ComponentInstance* compo)
{
  DEBTRACE("SubjectProc::addSubjectComponent " << compo->getInstanceName());
  SubjectComponent *son = new SubjectComponent(compo, this);
  GuiContext::getCurrent()->_mapOfSubjectComponent[compo] = son;
  update(ADD, COMPONENT, son);
  son->setContainer();
  return son;
}

SubjectContainer* SubjectProc::addSubjectContainer(YACS::ENGINE::Container* cont,
                                                   std::string name)
{
  DEBTRACE("SubjectProc::addSubjectContainer " << name);
  SubjectContainer *son = new SubjectContainer(cont, this);
  GuiContext::getCurrent()->_mapOfSubjectContainer[cont] = son;
  update(ADD, CONTAINER, son);
  return son;
}

SubjectDataType* SubjectProc::addSubjectDataType(YACS::ENGINE::TypeCode *type)
{
  string typeName = type->name();
  DEBTRACE("SubjectProc::addSubjectDataType " << typeName);
  Proc* proc = GuiContext::getCurrent()->getProc();
  SubjectDataType* son = 0;
  if (! proc->typeMap.count(typeName))
    proc->typeMap[ typeName ] = type->clone();
  else 
    proc->typeMap[ typeName ]->incrRef();
  if (! GuiContext::getCurrent()->_mapOfSubjectDataType.count(typeName))
    {
      son = new SubjectDataType(type, this);
      GuiContext::getCurrent()->_mapOfSubjectDataType[typeName] = son;
      update(ADD, DATATYPE, son);
    }
  else
    GuiContext::getCurrent()->_lastErrorMessage = "Typecode " + typeName + " already had added in proc";
  return son;
}

void SubjectProc::removeSubjectDataType( YACS::ENGINE::TypeCode* theType )
{
  if ( !theType )
    return;

  YACS::HMI::GuiContext* aContext = GuiContext::getCurrent();
  if ( !aContext )
    return;

  YACS::ENGINE::Proc* aProc = aContext->getProc();
  if ( !aProc )
    return;

  string typeName = theType->name();
  if ( !aProc->typeMap.count( typeName ) )
    return;
  
  YACS::ENGINE::TypeCode* aTypeCode = aProc->typeMap[ typeName ];
  if ( !aTypeCode )
    return;

  if ( !aContext->_mapOfSubjectDataType.count( typeName ) )
    return;

  YACS::HMI::SubjectDataType* aSDataType = aContext->_mapOfSubjectDataType[ typeName ];
  if ( !aSDataType )
    return;

  unsigned int aRefCnt = aTypeCode->getRefCnt();
  if ( aRefCnt == 1 )
  {
    update( REMOVE, DATATYPE, aSDataType );
    aContext->_mapOfSubjectDataType.erase( typeName );
    aProc->typeMap.erase( typeName );
  }

  aTypeCode->decrRef();
}

// ----------------------------------------------------------------------------

SubjectElementaryNode::SubjectElementaryNode(YACS::ENGINE::ElementaryNode *elementaryNode,
                                             Subject *parent)
  : SubjectNode(elementaryNode, parent), _elementaryNode(elementaryNode)
{
}

/*!
 * all generic destruction is done in generic class SubjectNode
 */
SubjectElementaryNode::~SubjectElementaryNode()
{
  DEBTRACE("SubjectElementaryNode::~SubjectElementaryNode " << getName());
}

void SubjectElementaryNode::clean()
{
  localClean();
  SubjectNode::clean();
}

void SubjectElementaryNode::localClean()
{
  DEBTRACE("SubjectElementaryNode::localClean ");
}


SubjectDataPort* SubjectElementaryNode::addInputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectElementaryNode::addInputPort( " << catalog << ", " << type << ", " << name << " )");
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);
  else assert(0);
  CommandAddInputPortFromCatalog *command = new CommandAddInputPortFromCatalog(catalog,
                                                                               type,
                                                                               position,
                                                                               name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      InputPort * port = command->getInputPort();
      SubjectInputPort *son = addSubjectInputPort(port, name);
      return son;
    }
  else delete command;
  return 0;
}

SubjectDataPort* SubjectElementaryNode::addOutputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectElementaryNode::addOutputPort( " << catalog << ", " << type << ", " << name << " )");
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);
  else assert(0);
  CommandAddOutputPortFromCatalog *command = new CommandAddOutputPortFromCatalog(catalog,
                                                                                 type,
                                                                                 position,
                                                                                 name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      OutputPort * port = command->getOutputPort();
      SubjectOutputPort *son = addSubjectOutputPort(port, name);
      return son;
    }
  else delete command;
  return 0;
}

SubjectDataPort* SubjectElementaryNode::addIDSPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectElementaryNode::addIDSPort( " << catalog << ", " << type << ", " << name << " )");
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);
  else assert(0);
  CommandAddIDSPortFromCatalog *command = new CommandAddIDSPortFromCatalog(catalog,
                                                                           type,
                                                                           position,
                                                                           name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      InputDataStreamPort * port = command->getIDSPort();
      SubjectInputDataStreamPort *son = addSubjectIDSPort(port, name);
      return son;
    }
  else delete command;
  return 0;
}

SubjectDataPort* SubjectElementaryNode::addODSPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectElementaryNode::addODSPort( " << catalog << ", " << type << ", " << name << " )");
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);
  else assert(0);
  CommandAddODSPortFromCatalog *command = new CommandAddODSPortFromCatalog(catalog,
                                                                           type,
                                                                           position,
                                                                           name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      OutputDataStreamPort * port = command->getODSPort();
      SubjectOutputDataStreamPort *son = addSubjectODSPort(port, name);
      return son;
    }
  else delete command;
  return 0;
}

void SubjectElementaryNode::removePort(SubjectDataPort* port)
{
  DEBTRACE("SubjectElementaryNode::removePort " << port->getName());
  if (SubjectInputPort* inp = dynamic_cast<SubjectInputPort*>(port))
    {
      DEBTRACE("-");
      _listSubjectInputPort.remove(inp);
    }
  else if(SubjectOutputPort* outp = dynamic_cast<SubjectOutputPort*>(port))
    {
      DEBTRACE("--");
      _listSubjectOutputPort.remove(outp);
    }
  if (SubjectInputDataStreamPort* idsp = dynamic_cast<SubjectInputDataStreamPort*>(port))
    {
      DEBTRACE("---");
      _listSubjectIDSPort.remove(idsp);
    }
  else if(SubjectOutputDataStreamPort* odsp = dynamic_cast<SubjectOutputDataStreamPort*>(port))
    {
      DEBTRACE("----");
      _listSubjectODSPort.remove(odsp);
    }
}

void SubjectElementaryNode::loadChildren()
{
  list<InputPort*>  listInputPorts  = _elementaryNode->getLocalInputPorts();
  list<OutputPort*> listOutputPorts = _elementaryNode->getLocalOutputPorts();
  list<InputDataStreamPort*>  listIDSPorts = _elementaryNode->getSetOfInputDataStreamPort();
  list<OutputDataStreamPort*> listODSPorts = _elementaryNode->getSetOfOutputDataStreamPort();
  if (SplitterNode *splitterNode = dynamic_cast<SplitterNode*>(_elementaryNode))
    listInputPorts.push_back(splitterNode->getFather()->getInputPort("SmplsCollection"));
  list<InputPort*>::const_iterator iti;
  for (iti = listInputPorts.begin(); iti != listInputPorts.end(); ++iti)
    addSubjectInputPort(*iti);
  list<OutputPort*>::const_iterator ito;
  for (ito = listOutputPorts.begin(); ito != listOutputPorts.end(); ++ito)
    addSubjectOutputPort(*ito);
  list<InputDataStreamPort*>::const_iterator itids;
  for (itids = listIDSPorts.begin(); itids != listIDSPorts.end(); ++itids)
    addSubjectIDSPort(*itids);
  list<OutputDataStreamPort*>::const_iterator itods;
  for (itods = listODSPorts.begin(); itods != listODSPorts.end(); ++itods)
    addSubjectODSPort(*itods);
}

// ----------------------------------------------------------------------------


SubjectInlineNode::SubjectInlineNode(YACS::ENGINE::InlineNode *inlineNode, Subject *parent)
  : SubjectElementaryNode(inlineNode, parent), _inlineNode(inlineNode)
{
}

SubjectInlineNode::~SubjectInlineNode()
{
  DEBTRACE("SubjectInlineNode::~SubjectInlineNode " << getName());
}

bool SubjectInlineNode::setScript(std::string script)
{
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetInlineNodeScript *command =
    new CommandSetInlineNodeScript(proc->getChildName(_node), script);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

std::string SubjectInlineNode::getScript()
{
  return _inlineNode->getScript();
}

void SubjectInlineNode::clean()
{
  localClean();
  SubjectElementaryNode::clean();
}

void SubjectInlineNode::localClean()
{
  DEBTRACE("SubjectInlineNode::localClean ");
}


// ----------------------------------------------------------------------------

SubjectServiceNode::SubjectServiceNode(YACS::ENGINE::ServiceNode *serviceNode, Subject *parent)
  : SubjectElementaryNode(serviceNode, parent), _serviceNode(serviceNode)
{
  _subjectReference=0;
}

SubjectServiceNode::~SubjectServiceNode()
{
  DEBTRACE("SubjectServiceNode::~SubjectServiceNode " << getName());
}

void SubjectServiceNode::clean()
{
  localClean();
  SubjectElementaryNode::clean();
}

void SubjectServiceNode::localClean()
{
  DEBTRACE("SubjectServiceNode::localClean ");
  if (_subjectReference)
    {
      update( REMOVE, REFERENCE, _subjectReference );
      erase( _subjectReference );
      _subjectReference = 0;
    }
}


/*!
 *  When cloning a service node from a catalog, create the component associated to the node,
 *  if not already existing, and create the corresponding subject.
 */
void SubjectServiceNode::setComponentFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                 std::string compo,
                                                 std::string service)
{
  DEBTRACE("SubjectServiceNode::setComponent " << compo);
  if (catalog->_componentMap.count(compo))
    {
      YACS::ENGINE::ComponentDefinition* compodef = catalog->_componentMap[compo];
      if (compodef->_serviceMap.count(service))
        {
          Proc* proc = GuiContext::getCurrent()->getProc();
          ComponentInstance *instance = 0;
          instance = new SalomeComponent(compo);
          pair<string,int> key = pair<string,int>(compo, instance->getNumId());
          proc->componentInstanceMap[key] = instance;
          SubjectComponent* subCompo = GuiContext::getCurrent()->getSubjectProc()->addSubjectComponent(instance);
          assert(subCompo);
          addSubjectReference(subCompo);
          _serviceNode->setComponent(instance);
        }
    }
}

/*!
 *  When loading scheme from file, get the component associated to the node, if any,
 *  and create the corresponding subject.
 */
void SubjectServiceNode::setComponent()
{
  ComponentInstance *instance = _serviceNode->getComponent();
  if (instance)
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      string compo = instance->getCompoName();
      SubjectComponent* subCompo = 0;
      if (! GuiContext::getCurrent()->_mapOfSubjectComponent.count(instance))
        {
	  DEBTRACE("SubjectServiceNode::setComponent : create subject for compo = " << compo.c_str());
          pair<string,int> key = pair<string,int>(compo, instance->getNumId());
          proc->componentInstanceMap[key] = instance;
          subCompo =
            GuiContext::getCurrent()->getSubjectProc()->addSubjectComponent(instance);
        }
      else
        {
	  DEBTRACE("SubjectServiceNode::setComponent : get already created subject for compo = " <<compo.c_str());
          subCompo = GuiContext::getCurrent()->_mapOfSubjectComponent[instance];
        }       
      assert(subCompo);
      addSubjectReference(subCompo);
    }
}

void SubjectServiceNode::associateToComponent(SubjectComponent *subcomp)
{
  DEBTRACE("SubjectServiceNode::associateToComponent " << getName() << " " << subcomp->getName());
  SubjectReference* oldSReference = _subjectReference;
  string aName = GuiContext::getCurrent()->getProc()->getChildName(_serviceNode);
  CommandAssociateServiceToComponent *command =
    new CommandAssociateServiceToComponent(aName, subcomp->getKey());
  if (command->execute())
    {
      if (oldSReference) removeSubjectReference(oldSReference);
      GuiContext::getCurrent()->getInvoc()->add(command);
      addSubjectReference(subcomp);
    }
  else delete command;
}

void SubjectServiceNode::removeSubjectReference(Subject *ref)
{
  DEBTRACE("Subject::removeSubjectReference " << getName() << " " << ref->getName());
  update( REMOVE, REFERENCE, ref );
  erase( ref );
}

void SubjectServiceNode::addSubjectReference(Subject *ref)
{
  DEBTRACE("Subject::addSubjectReference " << getName() << " " << ref->getName());
  SubjectReference *son = new SubjectReference(ref, this);
  _subjectReference = son;
  update(ADDREF, 0, son);
}

SubjectReference* SubjectServiceNode::getSubjectReference()
{
  return _subjectReference;
}

// ----------------------------------------------------------------------------

SubjectPythonNode::SubjectPythonNode(YACS::ENGINE::PythonNode *pythonNode, Subject *parent)
  : SubjectInlineNode(pythonNode, parent), _pythonNode(pythonNode)
{
}

SubjectPythonNode::~SubjectPythonNode()
{
  DEBTRACE("SubjectPythonNode::~SubjectPythonNode " << getName());
}

void SubjectPythonNode::clean()
{
  localClean();
  SubjectInlineNode::clean();
}

void SubjectPythonNode::localClean()
{
  DEBTRACE("SubjectPythonNode::localClean ");
}


// ----------------------------------------------------------------------------

SubjectPyFuncNode::SubjectPyFuncNode(YACS::ENGINE::PyFuncNode *pyFuncNode, Subject *parent)
  : SubjectInlineNode(pyFuncNode, parent), _pyFuncNode(pyFuncNode)
{
}

SubjectPyFuncNode::~SubjectPyFuncNode()
{
  DEBTRACE("SubjectPyFuncNode::~SubjectPyFuncNode " << getName());
}

bool SubjectPyFuncNode::setFunctionName(std::string funcName)
{
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetFuncNodeFunctionName *command =
    new CommandSetFuncNodeFunctionName(proc->getChildName(_node), funcName);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

void SubjectPyFuncNode::clean()
{
  localClean();
  SubjectInlineNode::clean();
}

void SubjectPyFuncNode::localClean()
{
  DEBTRACE("SubjectPyFuncNode::localClean ");
}


// ----------------------------------------------------------------------------

SubjectCORBANode::SubjectCORBANode(YACS::ENGINE::CORBANode *corbaNode, Subject *parent)
  : SubjectServiceNode(corbaNode, parent), _corbaNode(corbaNode)
{
}

SubjectCORBANode::~SubjectCORBANode()
{
  DEBTRACE("SubjectCORBANode::~SubjectCORBANode " << getName());
}

void SubjectCORBANode::clean()
{
  localClean();
  SubjectServiceNode::clean();
}

void SubjectCORBANode::localClean()
{
  DEBTRACE("SubjectCORBANode::localClean ");
}


// ----------------------------------------------------------------------------

SubjectCppNode::SubjectCppNode(YACS::ENGINE::CppNode *cppNode, Subject *parent)
  : SubjectServiceNode(cppNode, parent), _cppNode(cppNode)
{
}

SubjectCppNode::~SubjectCppNode()
{
  DEBTRACE("SubjectCppNode::~SubjectCppNode " << getName());
}

void SubjectCppNode::clean()
{
  localClean();
  SubjectServiceNode::clean();
}

void SubjectCppNode::localClean()
{
  DEBTRACE("SubjectCppNode::localClean ");
}


// ----------------------------------------------------------------------------

SubjectSalomeNode::SubjectSalomeNode(YACS::ENGINE::SalomeNode *salomeNode, Subject *parent)
  : SubjectServiceNode(salomeNode, parent), _salomeNode(salomeNode)
{
}

SubjectSalomeNode::~SubjectSalomeNode()
{
  DEBTRACE("SubjectSalomeNode::~SubjectSalomeNode " << getName());
}

void SubjectSalomeNode::clean()
{
  localClean();
  SubjectServiceNode::clean();
}

void SubjectSalomeNode::localClean()
{
  DEBTRACE("SubjectSalomeNode::localClean ");
}


// ----------------------------------------------------------------------------

SubjectSalomePythonNode::SubjectSalomePythonNode(YACS::ENGINE::SalomePythonNode *salomePythonNode,
                                                 Subject *parent)
  : SubjectServiceNode(salomePythonNode, parent), _salomePythonNode(salomePythonNode)
{
}

SubjectSalomePythonNode::~SubjectSalomePythonNode()
{
  DEBTRACE("SubjectSalomePythonNode::~SubjectSalomePythonNode " << getName());
}

void SubjectSalomePythonNode::clean()
{
  localClean();
  SubjectServiceNode::clean();
}

void SubjectSalomePythonNode::localClean()
{
  DEBTRACE("SubjectSalomePythonNode::localClean ");
}


// ----------------------------------------------------------------------------

SubjectXmlNode::SubjectXmlNode(YACS::ENGINE::XmlNode *xmlNode, Subject *parent)
  : SubjectServiceNode(xmlNode, parent), _xmlNode(xmlNode)
{
}

SubjectXmlNode::~SubjectXmlNode()
{
  DEBTRACE("SubjectXmlNode::~SubjectXmlNode " << getName());
}

void SubjectXmlNode::clean()
{
  localClean();
  SubjectServiceNode::clean();
}

void SubjectXmlNode::localClean()
{
  DEBTRACE("SubjectXmlNode::localClean ");
}


// ----------------------------------------------------------------------------

SubjectSplitterNode::SubjectSplitterNode(YACS::ENGINE::SplitterNode *splitterNode, Subject *parent)
  : SubjectElementaryNode(splitterNode, parent), _splitterNode(splitterNode)
{
  _destructible = false;
}

SubjectSplitterNode::~SubjectSplitterNode()
{
  DEBTRACE("SubjectSplitterNode::~SubjectSplitterNode " << getName());
}

void SubjectSplitterNode::clean()
{
  localClean();
  SubjectElementaryNode::clean();
}

void SubjectSplitterNode::localClean()
{
  DEBTRACE("SubjectSplitterNode::localClean ");
}


std::string SubjectSplitterNode::getName()
{
  return "splitter";
}

// ----------------------------------------------------------------------------

SubjectDataNode::SubjectDataNode(YACS::ENGINE::DataNode *dataNode, Subject *parent)
  : SubjectElementaryNode(dataNode, parent), _dataNode(dataNode)
{
  _destructible = true;
}

SubjectDataNode::~SubjectDataNode()
{
  DEBTRACE("SubjectDataNode::~SubjectDataNode " << getName());
}

void SubjectDataNode::clean()
{
  localClean();
  SubjectElementaryNode::clean();
}

void SubjectDataNode::localClean()
{
  DEBTRACE("SubjectDataNode::localClean ");
}

// ----------------------------------------------------------------------------

SubjectPresetNode::SubjectPresetNode(YACS::ENGINE::PresetNode *presetNode, Subject *parent)
  : SubjectDataNode(presetNode, parent), _presetNode(presetNode)
{
  _destructible = true;
}

SubjectPresetNode::~SubjectPresetNode()
{
  DEBTRACE("SubjectPresetNode::~SubjectPresetNode " << getName());
}

void SubjectPresetNode::clean()
{
  localClean();
  SubjectDataNode::clean();
}

void SubjectPresetNode::localClean()
{
  DEBTRACE("SubjectPresetNode::localClean ");
}

// ----------------------------------------------------------------------------

SubjectOutNode::SubjectOutNode(YACS::ENGINE::OutNode *outNode, Subject *parent)
  : SubjectDataNode(outNode, parent), _outNode(outNode)
{
  _destructible = true;
}

SubjectOutNode::~SubjectOutNode()
{
  DEBTRACE("SubjectOutNode::~SubjectOutNode " << getName());
}

void SubjectOutNode::clean()
{
  localClean();
  SubjectDataNode::clean();
}

void SubjectOutNode::localClean()
{
  DEBTRACE("SubjectOutNode::localClean ");
}

// ----------------------------------------------------------------------------

SubjectStudyInNode::SubjectStudyInNode(YACS::ENGINE::StudyInNode *studyInNode, Subject *parent)
  : SubjectDataNode(studyInNode, parent), _studyInNode(studyInNode)
{
  _destructible = true;
}

SubjectStudyInNode::~SubjectStudyInNode()
{
  DEBTRACE("SubjectStudyInNode::~SubjectStudyInNode " << getName());
}

void SubjectStudyInNode::clean()
{
  localClean();
  SubjectDataNode::clean();
}

void SubjectStudyInNode::localClean()
{
  DEBTRACE("SubjectStudyInNode::localClean ");
}

// ----------------------------------------------------------------------------

SubjectStudyOutNode::SubjectStudyOutNode(YACS::ENGINE::StudyOutNode *studyOutNode, Subject *parent)
  : SubjectDataNode(studyOutNode, parent), _studyOutNode(studyOutNode)
{
  _destructible = true;
}

SubjectStudyOutNode::~SubjectStudyOutNode()
{
  DEBTRACE("SubjectStudyOutNode::~SubjectStudyOutNode " << getName());
}

void SubjectStudyOutNode::clean()
{
  localClean();
  SubjectDataNode::clean();
}

void SubjectStudyOutNode::localClean()
{
  DEBTRACE("SubjectStudyOutNode::localClean ");
}

// ----------------------------------------------------------------------------

SubjectForLoop::SubjectForLoop(YACS::ENGINE::ForLoop *forLoop, Subject *parent)
  : SubjectComposedNode(forLoop, parent), _forLoop(forLoop)
{
  _body = 0;
}

SubjectForLoop::~SubjectForLoop()
{
  DEBTRACE("SubjectForLoop::~SubjectForLoop " << getName());
}

void SubjectForLoop::clean()
{
  localClean();
  SubjectComposedNode::clean();
}

void SubjectForLoop::localClean()
{
  DEBTRACE("SubjectForLoop::localClean ");
  if (_body)
    erase(_body);
}


SubjectNode* SubjectForLoop::addNode(YACS::ENGINE::Catalog *catalog,
				     std::string compo,
				     std::string type,
				     std::string name)
{
  DEBTRACE("SubjectForLoop::addNode(catalog, compo, type, name)");
  SubjectNode* body = 0;
  if (_body) return body;
  body = createNode(catalog, compo, type, name);
  return body;
}

void SubjectForLoop::completeChildrenSubjectList(SubjectNode *son)
{
  _body = son;
}

// ----------------------------------------------------------------------------

SubjectWhileLoop::SubjectWhileLoop(YACS::ENGINE::WhileLoop *whileLoop, Subject *parent)
  : SubjectComposedNode(whileLoop, parent), _whileLoop(whileLoop)
{
  _body = 0;
}

SubjectWhileLoop::~SubjectWhileLoop()
{
  DEBTRACE("SubjectWhileLoop::~SubjectWhileLoop " << getName());
}

void SubjectWhileLoop::clean()
{
  localClean();
  SubjectComposedNode::clean();
}

void SubjectWhileLoop::localClean()
{
  DEBTRACE("SubjectWhileLoop::localClean ");
  if (_body)
    erase(_body);
}

SubjectNode* SubjectWhileLoop::addNode(YACS::ENGINE::Catalog *catalog,
				       std::string compo,
				       std::string type,
				       std::string name)
{
  DEBTRACE("SubjectWhileLoop::addNode(catalog, compo, type, name)");
  SubjectNode* body = 0;
  if (_body) return body;
  body = createNode(catalog, compo, type, name);
  return body;
}

void SubjectWhileLoop::completeChildrenSubjectList(SubjectNode *son)
{
  _body = son;
}

// ----------------------------------------------------------------------------

SubjectSwitch::SubjectSwitch(YACS::ENGINE::Switch *aSwitch, Subject *parent)
  : SubjectComposedNode(aSwitch, parent), _switch(aSwitch)
{
  _bodyMap.clear();
}

SubjectSwitch::~SubjectSwitch()
{
  DEBTRACE("SubjectSwitch::~SubjectSwitch " << getName());
}

void SubjectSwitch::clean()
{
  localClean();
  SubjectComposedNode::clean();
}

void SubjectSwitch::localClean()
{
  DEBTRACE("SubjectSwitch::localClean ");
  map<int, SubjectNode*>::iterator it;
  for (it = _bodyMap.begin(); it != _bodyMap.end(); ++it)
    erase((*it).second);
}

SubjectNode* SubjectSwitch::addNode(YACS::ENGINE::Catalog *catalog,
				    std::string compo,
				    std::string type,
				    std::string name,
				    int swCase,
				    bool replace)
{
  DEBTRACE("SubjectSwitch::addNode("<<catalog<<","<<compo<<","<<type<<","<<name<<","<<swCase<<","<<(int)replace<<")");
  SubjectNode* body = 0;
  if (!replace && _bodyMap.count(swCase)) return body;
  body = createNode(catalog, compo, type, name, swCase);
  return body;
}

void SubjectSwitch::removeNode(SubjectNode* son)
{
  DEBTRACE("SubjectSwitch::removeNode("<<son->getName()<<")");
  if (son)
  {
    int id;
    bool isFound = false;
    map<int, SubjectNode*>::const_iterator it;
    for (it = _bodyMap.begin(); it != _bodyMap.end(); ++it)
    {
      if ( (*it).second == son )
      {
	isFound = true;
	id = (*it).first;
	break;
      }
    }
    if (isFound)
    {
      DEBTRACE("id = "<<id);
      _bodyMap.erase(id);
    }
  }
}

std::map<int, SubjectNode*> SubjectSwitch::getBodyMap()
{
  return _bodyMap;
}

void SubjectSwitch::completeChildrenSubjectList(SubjectNode *son)
{
  _bodyMap[_switch->getRankOfNode(son->getNode())] = son;
}

SubjectNode* SubjectSwitch::getChild(YACS::ENGINE::Node* node) const
{
  SubjectNode* aChild = 0;

  if (node)
  {
    map<int, SubjectNode*>::const_iterator it;
    for (it = _bodyMap.begin(); it != _bodyMap.end(); ++it)
      if ( (*it).second->getNode() == node )
      {
	aChild = (*it).second;
	break;
      }
  }

  return aChild;
}

// ----------------------------------------------------------------------------

SubjectForEachLoop::SubjectForEachLoop(YACS::ENGINE::ForEachLoop *forEachLoop, Subject *parent)
  : SubjectComposedNode(forEachLoop, parent), _forEachLoop(forEachLoop)
{
  _body = 0;
  _splitter = 0;
}

SubjectForEachLoop::~SubjectForEachLoop()
{
  DEBTRACE("SubjectForEachLoop::~SubjectForEachLoop " << getName());
}

void SubjectForEachLoop::clean()
{
  Node* aSplitterEngine = 0;
  if (_splitter) aSplitterEngine = _splitter->getNode();

  localClean();
  SubjectComposedNode::clean();

  if (_forEachLoop && aSplitterEngine)
    {
      DEBTRACE("SubjectForEachLoop::clean: remove for each loop splitter");
      _forEachLoop->edRemoveChild(aSplitterEngine);
    }
}

void SubjectForEachLoop::localClean()
{
  DEBTRACE("SubjectForEachLoop::localClean ");
  if (_body)
    {
      DEBTRACE(_body->getName());
      erase(_body);
    }
  if (_splitter)
    {
      DEBTRACE(_splitter->getName());
      erase(_splitter);
    }
}


SubjectNode* SubjectForEachLoop::addNode(YACS::ENGINE::Catalog *catalog,
					 std::string compo,
					 std::string type,
					 std::string name)
{
  DEBTRACE("SubjectForEachLoop::addNode(catalog, compo, type, name)");
  SubjectNode* body = 0;
  if (_body) return body;
  body = createNode(catalog, compo, type, name);
  return body;
}

void SubjectForEachLoop::completeChildrenSubjectList(SubjectNode *son)
{
  if ( !son )
  {
    _body = son;
    return;
  }

  string name = son->getName();
  DEBTRACE("SubjectForEachLoop::completeChildrenSubjectList " << name);
  if (name == ForEachLoop::NAME_OF_SPLITTERNODE)
    _splitter = son;
  else
    _body = son;
}


// ----------------------------------------------------------------------------

SubjectOptimizerLoop::SubjectOptimizerLoop(YACS::ENGINE::OptimizerLoop *optimizerLoop,
                                           Subject *parent)
  : SubjectComposedNode(optimizerLoop, parent), _optimizerLoop(optimizerLoop)
{
  _body = 0;
}

SubjectOptimizerLoop::~SubjectOptimizerLoop()
{
  DEBTRACE("SubjectOptimizerLoop::~SubjectOptimizerLoop " << getName());
}

void SubjectOptimizerLoop::clean()
{
  localClean();
  SubjectComposedNode::clean();
}

void SubjectOptimizerLoop::localClean()
{
  DEBTRACE("SubjectOptimizerLoop::localClean ");
  if (_body)
    erase(_body);
}

SubjectNode* SubjectOptimizerLoop::addNode(YACS::ENGINE::Catalog *catalog,
					   std::string compo,
					   std::string type,
					   std::string name)
{
  DEBTRACE("SubjectOptimizerLoop::addNode(catalog, compo, type, name)");
  SubjectNode* body = 0;
  if (_body) return body;
  body = createNode(catalog, compo, type, name);
  return body;
}

void SubjectOptimizerLoop::completeChildrenSubjectList(SubjectNode *son)
{
  _body = son;
}

// ----------------------------------------------------------------------------

SubjectDataPort::SubjectDataPort(YACS::ENGINE::DataPort* port, Subject *parent)
  : Subject(parent), _dataPort(port)
{
  _listSubjectLink.clear();
}

SubjectDataPort::~SubjectDataPort()
{
  DEBTRACE("SubjectDataPort::~SubjectDataPort " << getName());
  if (isDestructible())
    {
      Node* node = _dataPort->getNode();
      assert(node);
      ElementaryNode * father = dynamic_cast<ElementaryNode*>(node);
      if (father)
        {
          DEBTRACE("father->edRemovePort(_dataPort)");
          try
            {
              father->edRemovePort(_dataPort);
            }
          catch (YACS::Exception &e)
            {
              DEBTRACE("------------------------------------------------------------------------------");
              DEBTRACE("SubjectDataPort::~SubjectDataPort: father->edRemovePort: YACS exception " << e.what());
              DEBTRACE("------------------------------------------------------------------------------");
            }
        }
    }
}

void SubjectDataPort::clean()
{
  localClean();
  Subject::clean();
}

void SubjectDataPort::localClean()
{
  DEBTRACE("SubjectDataPort::localClean ");
  list<SubjectLink*> lsl = getListOfSubjectLink();
  for (list<SubjectLink*>::iterator it = lsl.begin(); it != lsl.end(); ++it)
    erase(*it);
}

std::string SubjectDataPort::getName()
{
  return _dataPort->getName();
}

YACS::ENGINE::DataPort* SubjectDataPort::getPort()
{
  return _dataPort;
}

bool SubjectDataPort::tryCreateLink(SubjectDataPort *subOutport, SubjectDataPort *subInport,bool control)
{
  DEBTRACE("SubjectDataPort::tryCreateLink");
  Proc *proc = GuiContext::getCurrent()->getProc();

  string outNodePos = "";
  SubjectNode *sno = dynamic_cast<SubjectNode*>(subOutport->getParent());
  assert(sno);
  Node *outNode = sno->getNode();
  outNodePos = proc->getChildName(outNode);
  string outportName = subOutport->getName();

  string inNodePos = "";
  SubjectNode *sni = dynamic_cast<SubjectNode*>(subInport->getParent());
  assert(sni);
  Node *inNode = sni->getNode();
  inNodePos = proc->getChildName(inNode);
  string inportName = subInport->getName();
  
  CommandAddLink *command = new CommandAddLink(outNodePos, outportName,
                                               inNodePos, inportName,control);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      
      ComposedNode *cla = ComposedNode::getLowestCommonAncestor(outNode->getFather(),
                                                                inNode->getFather());
      SubjectComposedNode *scla = dynamic_cast<SubjectComposedNode*>(sno->getParent());
      ComposedNode *ancestor = outNode->getFather();
      while (ancestor && ancestor != cla)
        {
          ancestor = ancestor->getFather();
          scla = dynamic_cast<SubjectComposedNode*>(scla->getParent());
          assert(scla);
        }
      DEBTRACE(scla->getName());
      scla->addSubjectLink(sno, subOutport, sni, subInport);
      return true;
    }
  else
    {
      delete command;
      return false;
    }
}

// ----------------------------------------------------------------------------

SubjectInputPort::SubjectInputPort(YACS::ENGINE::InputPort *port, Subject *parent)
  : SubjectDataPort(port, parent), _inputPort(port)
{
  Node *node = _inputPort->getNode();
  if (ForLoop* forloop = dynamic_cast<ForLoop*>(node))
    {
      if (_inputPort->getName() == "nsteps") _destructible = false;
    }
  else if (WhileLoop* whileloop = dynamic_cast<WhileLoop*>(node))
    {
      if (_inputPort->getName() == "condition") _destructible = false;
    }
  else if (Switch* aSwitch = dynamic_cast<Switch*>(node))
    {
      if (_inputPort->getName() == "select") _destructible = false;
    }
  else if (ForEachLoop* foreach = dynamic_cast<ForEachLoop*>(node))
    {
      if (_inputPort->getName() == "nbBranches") _destructible = false;
    }
  else if (OptimizerLoop* optil = dynamic_cast<OptimizerLoop*>(node))
    {
      if (_inputPort->getName() == "nbBranches") _destructible = false;
    }
  else if (SplitterNode* split = dynamic_cast<SplitterNode*>(node))
    {
      if (_inputPort->getName() == "SmplsCollection") _destructible = false;
    }

}

SubjectInputPort::~SubjectInputPort()
{
  DEBTRACE("SubjectInputPort::~SubjectInputPort " << getName());
}

void SubjectInputPort::clean()
{
  localClean();
  SubjectDataPort::clean();
}

void SubjectInputPort::localClean()
{
  DEBTRACE("SubjectInputPort::localClean ");
  if (_parent)
    {
      SubjectElementaryNode* elem = dynamic_cast<SubjectElementaryNode*>(_parent);
      if (elem) elem->removePort(this);
    }
}

// ----------------------------------------------------------------------------

SubjectOutputPort::SubjectOutputPort(YACS::ENGINE::OutputPort *port, Subject *parent)
  : SubjectDataPort(port, parent), _outputPort(port)
{
  Node *node = _outputPort->getNode();
  if (ForEachLoop* foreach = dynamic_cast<ForEachLoop*>(node))
    {
      if (_outputPort->getName() == "SmplPrt") _destructible = false;
    }
  else if (OptimizerLoop* optil = dynamic_cast<OptimizerLoop*>(node))
    {
      if (_outputPort->getName() == "SmplPrt") _destructible = false;
    }
}

SubjectOutputPort::~SubjectOutputPort()
{
  DEBTRACE("SubjectOutputPort::~SubjectOutputPort " << getName());
}

void SubjectOutputPort::clean()
{
  localClean();
  SubjectDataPort::clean();
}

void SubjectOutputPort::localClean()
{
  DEBTRACE("SubjectOutputPort::localClean ");
  if (_parent)
    {
      SubjectElementaryNode* elem = dynamic_cast<SubjectElementaryNode*>(_parent);
      if (elem) elem->removePort(this);
    }
}

// ----------------------------------------------------------------------------

SubjectInputDataStreamPort::SubjectInputDataStreamPort(YACS::ENGINE::InputDataStreamPort *port,
                                                       Subject *parent)
  : SubjectDataPort(port, parent), _inputDataStreamPort(port)
{
}

SubjectInputDataStreamPort::~SubjectInputDataStreamPort()
{
  DEBTRACE("SubjectInputDataStreamPort::~SubjectInputDataStreamPort " << getName());
}

std::map<std::string, std::string> SubjectInputDataStreamPort::getProperties()
{
  return _inputDataStreamPort->getPropertyMap();
}

bool SubjectInputDataStreamPort::setProperties(std::map<std::string, std::string> properties)
{
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetDSPortProperties *command =
    new CommandSetDSPortProperties(proc->getChildName(getPort()->getNode()), getName(), true, properties);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

void SubjectInputDataStreamPort::clean()
{
  localClean();
  SubjectDataPort::clean();
}

void SubjectInputDataStreamPort::localClean()
{
  DEBTRACE("SubjectInputDataStreamPort::localClean ");
}

// ----------------------------------------------------------------------------

SubjectOutputDataStreamPort::SubjectOutputDataStreamPort(YACS::ENGINE::OutputDataStreamPort *port,
                                                         Subject *parent)
  : SubjectDataPort(port, parent), _outputDataStreamPort(port)
{
}

SubjectOutputDataStreamPort::~SubjectOutputDataStreamPort()
{
  DEBTRACE("SubjectOutputDataStreamPort::~SubjectOutputDataStreamPort " << getName());
}

std::map<std::string, std::string> SubjectOutputDataStreamPort::getProperties()
{
  return _outputDataStreamPort->getPropertyMap();
}

bool SubjectOutputDataStreamPort::setProperties(std::map<std::string, std::string> properties)
{
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetDSPortProperties *command =
    new CommandSetDSPortProperties(proc->getChildName(getPort()->getNode()), getName(), false, properties);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

void SubjectOutputDataStreamPort::clean()
{
  localClean();
  SubjectDataPort::clean();
}

void SubjectOutputDataStreamPort::localClean()
{
  DEBTRACE("SubjectOutputDataStreamPort::localClean ");
}

// ----------------------------------------------------------------------------

SubjectLink::SubjectLink(SubjectNode* subOutNode,
                         SubjectDataPort* outPort,
                         SubjectNode* subInNode,
                         SubjectDataPort* inPort,
                         Subject *parent)
  : Subject(parent),
    _subOutNode(subOutNode), _outPort(outPort), _subInNode(subInNode), _inPort(inPort)
{
  _name = "";
  ComposedNode *cla = ComposedNode::getLowestCommonAncestor(_subOutNode->getNode()->getFather(),
                                                            _subInNode->getNode()->getFather());
  DEBTRACE(_subOutNode->getName() << "." << _outPort->getName());
  DEBTRACE(_subInNode->getName() << "." << _inPort->getName());
  DEBTRACE(cla->getName());
  _name += cla->getChildName(_subOutNode->getNode());
  _name += "." + _outPort->getName();
  _name += "->";
  _name += cla->getChildName(_subInNode->getNode());
  _name += "." + _inPort->getName();
  DEBTRACE("SubjectLink::SubjectLink " << _name);
}

SubjectLink::~SubjectLink()
{
  DEBTRACE("SubjectLink::~SubjectLink " << getName());
}

void SubjectLink::clean()
{
  localClean();
  Subject::clean();
}

void SubjectLink::localClean()
{
  DEBTRACE("SubjectLink::localClean ");
  if (_parent)
    {
      DEBTRACE("clean link: " << _parent->getName() << " " << getName());
      SubjectComposedNode* father = dynamic_cast<SubjectComposedNode*>(_parent);
      assert(father);
      father->removeLink(this); // --- clean subjects first
      _cla = dynamic_cast<ComposedNode*>(father->getNode());
      assert(_cla);
      _outp = dynamic_cast<OutPort*>(_outPort->getPort());
      assert(_outp);
      _inp = dynamic_cast<InPort*>(_inPort->getPort());
      assert(_inp);
      if (isDestructible())
        _cla->edRemoveLink(_outp, _inp);
    }
}

std::string SubjectLink::getName()
{
  return _name;
}
// ----------------------------------------------------------------------------

SubjectControlLink::SubjectControlLink(SubjectNode* subOutNode,
                                       SubjectNode* subInNode,
                                       Subject *parent)
  : Subject(parent),
    _subOutNode(subOutNode), _subInNode(subInNode)
{
  _name = "";
  ComposedNode *cla = ComposedNode::getLowestCommonAncestor(_subOutNode->getNode()->getFather(),
                                                            _subInNode->getNode()->getFather());
  DEBTRACE(_subOutNode->getName());
  DEBTRACE(_subInNode->getName());
  DEBTRACE(cla->getName());
  _name += cla->getChildName(_subOutNode->getNode());
  _name += "-->>";
  _name += cla->getChildName(_subInNode->getNode());
  DEBTRACE("SubjectControlLink::SubjectControlLink " << _name);
}

SubjectControlLink::~SubjectControlLink()
{
  DEBTRACE("SubjectControlLink::~SubjectControlLink " << getName());
  if (isDestructible())
    {
      try
        {

          _cla->edRemoveCFLink(_subOutNode->getNode(), _subInNode->getNode());
        }
      catch (YACS::Exception &e)
        {
          DEBTRACE("------------------------------------------------------------------------------");
          DEBTRACE("SubjectControlLink::~SubjectControlLink: edRemoveLink YACS exception " << e.what());
          DEBTRACE("------------------------------------------------------------------------------");
        }
    }
}

void SubjectControlLink::clean()
{
  localClean();
  Subject::clean();
}

void SubjectControlLink::localClean()
{
  DEBTRACE("SubjectControlLink::localClean ");
  if (_parent)
    {
      DEBTRACE("clean control link: " << _parent->getName() << " " << getName());
      SubjectComposedNode* father = dynamic_cast<SubjectComposedNode*>(_parent);
      assert(father);
      father->removeControlLink(this); // --- clean subjects first
      _cla = dynamic_cast<ComposedNode*>(father->getNode());
      assert(_cla);
    }
}

std::string SubjectControlLink::getName()
{
  return _name;
}

// ----------------------------------------------------------------------------

SubjectComponent::SubjectComponent(YACS::ENGINE::ComponentInstance* component, Subject *parent)
  : Subject(parent), _compoInst(component)
{
  _compoInst->incrRef();
}

SubjectComponent::~SubjectComponent()
{
  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( aProc )
  {
    pair<string,int> key = pair<string,int>(_compoInst->getCompoName(),_compoInst->getNumId());
    aProc->componentInstanceMap.erase(key);
    
    std::map<std::string, ServiceNode*>::iterator it = aProc->serviceMap.begin();
    for ( ; it!=aProc->serviceMap.end(); it++ )
      if ( (*it).second->getComponent() == _compoInst )
	(*it).second->setComponent(0);
   
    GuiContext::getCurrent()->_mapOfSubjectComponent.erase(_compoInst);
    
    //if ( SalomeComponent* aSalomeCompo = static_cast<SalomeComponent*>(_compoInst) )
    //delete aSalomeCompo;
  }
  _compoInst->decrRef();
}

void SubjectComponent::clean()
{
  localClean();
  Subject::clean();
}

void SubjectComponent::localClean()
{
  DEBTRACE("SubjectComponent::localClean ");
}

std::string SubjectComponent::getName()
{
  DEBTRACE("SubjectComponent::getName()********************************************************************************");
  return _compoInst->getInstanceName();
}

std::pair<std::string, int> SubjectComponent::getKey()
{
  std::pair<std::string, int> key = std::pair<std::string, int>(_compoInst->getCompoName(), _compoInst->getNumId());
  return key;
}

YACS::ENGINE::ComponentInstance* SubjectComponent::getComponent() const
{
  return _compoInst;
}

/*!
 *  When loading scheme from file, get the container associated to the component, if any,
 *  and create the corresponding subject.
 */
void SubjectComponent::setContainer()
{
  DEBTRACE("SubjectComponent::setContainer " << getName());
  Container* container = _compoInst->getContainer();
  if (container)
    {
      SubjectContainer *subContainer;
      if (GuiContext::getCurrent()->_mapOfSubjectContainer.find(container)
	  ==
	  GuiContext::getCurrent()->_mapOfSubjectContainer.end())
	subContainer = 
	  GuiContext::getCurrent()->getSubjectProc()->addSubjectContainer(container, container->getName());
      else
	subContainer = GuiContext::getCurrent()->_mapOfSubjectContainer[container];
      addSubjectReference(subContainer);
    }
}

void SubjectComponent::associateToContainer(SubjectContainer* subcont)
{
  DEBTRACE("SubjectComponent::associateToContainer " << getName() << subcont->getName());
  CommandAssociateComponentToContainer *command =
    new CommandAssociateComponentToContainer(getKey(), subcont->getName());
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      addSubjectReference(subcont);
    }
  else delete command;
}

// ----------------------------------------------------------------------------

SubjectContainer::SubjectContainer(YACS::ENGINE::Container* container, Subject *parent)
  : Subject(parent), _container(container)
{
}

SubjectContainer::~SubjectContainer()
{
  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( aProc )
  {
    aProc->containerMap.erase(_container->getName());
    
    map<ComponentInstance*,SubjectComponent*>::iterator it = GuiContext::getCurrent()->_mapOfSubjectComponent.begin();
    for ( ; it!=GuiContext::getCurrent()->_mapOfSubjectComponent.end(); it++ )
      if ( (*it).first && (*it).first->getContainer() == _container )
      {
	(*it).first->setContainer(0);
	GuiContext::getCurrent()->getSubjectProc()->destroy((*it).second);
      }
   
    GuiContext::getCurrent()->_mapOfSubjectContainer.erase(_container);
  }
}

std::map<std::string, std::string> SubjectContainer::getProperties()
{
  return _container->getProperties();
}

bool SubjectContainer::setProperties(std::map<std::string, std::string> properties)
{
  CommandSetContainerProperties *command = new CommandSetContainerProperties(getName(), properties);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

void SubjectContainer::clean()
{
  localClean();
  Subject::clean();
}

void SubjectContainer::localClean()
{
  DEBTRACE("SubjectContainer::localClean ");
}

std::string SubjectContainer::getName()
{
  return _container->getName();
}

YACS::ENGINE::Container* SubjectContainer::getContainer() const
{
  return _container;
}

// ----------------------------------------------------------------------------

SubjectDataType::SubjectDataType(YACS::ENGINE::TypeCode *typeCode, Subject *parent)
  : Subject(parent), _typeCode(typeCode)
{
}

SubjectDataType::~SubjectDataType()
{
}

void SubjectDataType::clean()
{
  localClean();
  Subject::clean();
}

void SubjectDataType::localClean()
{
  DEBTRACE("SubjectDataType::localClean ");
}

std::string SubjectDataType::getName()
{
  return _typeCode->name();
}

YACS::ENGINE::TypeCode* SubjectDataType::getTypeCode()
{
  return _typeCode;
}

// ----------------------------------------------------------------------------
