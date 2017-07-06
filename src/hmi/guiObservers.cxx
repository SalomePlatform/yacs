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
#include "SalomeHPContainer.hxx"
#include "SalomeContainer.hxx"
#include "SalomeComponent.hxx"
#include "ComponentDefinition.hxx"
#include "TypeCode.hxx"
#include "RuntimeSALOME.hxx"

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

std::map<int, std::string> GuiObserver::_eventNameMap;

// ----------------------------------------------------------------------------

void Subject::erase(Subject* sub, Command *command, bool post)
{
  Subject* parent =sub->getParent();
  sub->clean(command);
  delete sub;
  if (!post) // --- avoid recursion in erase, see cleanPostErase
    GuiContext::getCurrent()->getSubjectProc()->cleanPostErase();
}

// ----------------------------------------------------------------------------

Subject::Subject(Subject *parent) : _parent(parent)
{
  _destructible = true;
  _askRegisterUndo = false;
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
void Subject::clean(Command *command)
{
  localclean(command);
}

/*!
 *  the local clean method of base class of subjects take care of Observers.
 *  Remaining Observers in the list are detached, if an observer has no more
 *  Subject to observe, it can be deleted.
 */
void Subject::localclean(Command *command)
{
  DEBTRACE("Subject::localClean ");
  set<GuiObserver*>::iterator it;
  while (int nbObs = _setObs.size())
    {
      DEBTRACE("--- " << this << " nbObs " << nbObs);
      set<GuiObserver*> copySet = _setObs;
      for (it = copySet.begin(); it != copySet.end(); ++it)
        {
          GuiObserver* anObs = (*it);
          detach(anObs);
          int nbsub = anObs->getNbSubjects();
          DEBTRACE("nbSubjects=" << nbsub << " obs=" << anObs);
          if (nbsub <= 0 && anObs->isDestructible())
            {
              delete anObs ;
              break; // --- each delete may induce remove of other observers
            }
        }
    }
  _setObs.clear();
}

void Subject::registerUndoDestroy()
{
  DEBTRACE("Subject::registerUndoDestroy");
}

void Subject::attach(GuiObserver *obs)
{
  DEBTRACE("Subject::attach " << obs);
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

std::map<std::string, std::string> Subject::getProperties()
{
  std::map<std::string, std::string> empty;
  return empty;
}

bool Subject::setProperties(std::map<std::string, std::string> properties)
{
  return true;
}

std::vector<std::string> Subject::knownProperties()
{
  std::vector<std::string> empty;
  return empty;
}

void Subject::select(bool isSelected)
{
  DEBTRACE("Subject::select " << isSelected << " " << this);
  set<GuiObserver*> copySet = _setObs;
  for (set<GuiObserver *>::iterator it = copySet.begin(); it != copySet.end(); ++it)
    {
      GuiObserver* currOb = *it;
      currOb->select(isSelected);
    }
}

void Subject::update(GuiEvent event,int type, Subject* son)
{
  //DEBTRACE("Subject::update " << type << "," << GuiObserver::eventName(event) << "," << son);
  set<GuiObserver*> copySet = _setObs;
  for (set<GuiObserver *>::iterator it = copySet.begin(); it != copySet.end(); ++it)
    {
      //DEBTRACE("Subject::update " << *it);
      try
        {
          (*it)->update(event, type, son);
        }
      catch(Exception& ex)
        {
          std::cerr << "Internal error in Subject::update: " << ex.what() << std::endl;
        }
      catch(...)
        {
          std::cerr << "Internal error in Subject::update: " << std::endl;
        }
    }
}

Subject* Subject::getParent()
{
  return _parent;
}

void Subject::setParent(Subject* son)
{
  _parent=son;
}

//! From user action in Ihm, destroy an object
/*!
 *  Called from GuiEditor::DeleteSubject(parent, toRemove).
 *  The subject (this) is the parent of the subject (son) to remove.
 *  Son must represent a Node, a DataPort, a Link or a ControlLink.
 *  All other objects can only be destroyed automatically, not via an user command.
 *  Only the destruction originated from user, using this method, are registered for undo-redo.
 */
bool Subject::destroy(Subject *son)
{
  string toDestroy = son->getName();
  DEBTRACE("Subject::destroy " << toDestroy);
  Proc *proc = GuiContext::getCurrent()->getProc();
  string startnode = "";
  string endnode = "";
  string startport = "";
  string endport = "";
  TypeOfElem startportType = UNKNOWN;
  TypeOfElem endportType = UNKNOWN;

  if (dynamic_cast<SubjectProc*>(son))
    startnode = proc->getName();
  else
    {
      if (SubjectNode *subNode = dynamic_cast<SubjectNode*>(son))
        {
          if (subNode->getNode()->getFather() )
            startnode = proc->getChildName(subNode->getNode());
        }
      else if (dynamic_cast<SubjectDataPort*>(son))
        {
          SubjectNode *subNodep = dynamic_cast<SubjectNode*>(son->getParent());
          startnode = proc->getChildName(subNodep->getNode());
          startport = son->getName();
          startportType = son->getType();
        }
      else if (SubjectLink* slink = dynamic_cast<SubjectLink*>(son))
        {
          startnode = proc->getChildName(slink->getSubjectOutNode()->getNode());
          endnode = proc->getChildName(slink->getSubjectInNode()->getNode());
          startport = slink->getSubjectOutPort()->getName();
          endport = slink->getSubjectInPort()->getName();
          startportType = slink->getSubjectOutPort()->getType();
          endportType = slink->getSubjectInPort()->getType();
        }
      else if (SubjectControlLink* sclink = dynamic_cast<SubjectControlLink*>(son))
        {
          startnode = proc->getChildName(sclink->getSubjectOutNode()->getNode());
          endnode = proc->getChildName(sclink->getSubjectInNode()->getNode());
        }
      else if (SubjectContainerBase* scont = dynamic_cast<SubjectContainerBase*>(son))
        {
          if(scont->getName() == "DefaultContainer")
            {
              GuiContext::getCurrent()->_lastErrorMessage = "You can't delete the default container";
              return false;
            }
          if(scont->isUsed())
            {
              GuiContext::getCurrent()->_lastErrorMessage = "You can't delete a container that contains components";
              return false;
            }
          startnode = scont->getName();
        }
      else
        {
          GuiContext::getCurrent()->_lastErrorMessage = "No command Destroy for that type";
          return false;
        }
    }
  if (son->isDestructible())
    {
      CommandDestroy* command = new CommandDestroy(son->getType(), startnode, startport, startportType, endnode, endport, endportType);
      if (command->execute())
        {
          DEBTRACE("Destruction done: " << toDestroy);
          GuiContext::getCurrent()->getInvoc()->add(command);
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

void Subject::setProgress( std::string newProgress )
{
  _progress = newProgress;
}

// ----------------------------------------------------------------------------

GuiObserver::GuiObserver()
  : _destructible(true)
{
  //DEBTRACE("GuiObserver::GuiObserver " << this);
  _subjectSet.clear();
}

GuiObserver::~GuiObserver()
{
  DEBTRACE("GuiObserver::~GuiObserver " << this);
  set<Subject*> subsetcpy = _subjectSet;
  set<Subject*>::iterator it= subsetcpy.begin();
  for (; it != subsetcpy.end(); ++it)
    (*it)->detach(this);
}

void GuiObserver::select(bool isSelected)
{
  DEBTRACE("GuiObserver::select() " << isSelected);
}

void GuiObserver::update(GuiEvent event, int type,  Subject* son)
{
  //DEBTRACE("GuiObserver::update, event not handled " << eventName(event) << " " << type );
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

void GuiObserver::setEventMap()
{
  _eventNameMap.clear();
  _eventNameMap[ADD]            = "ADD";
  _eventNameMap[REMOVE]         = "REMOVE";
  _eventNameMap[CUT]            = "CUT";
  _eventNameMap[PASTE]          = "PASTE";
  _eventNameMap[ORDER]          = "ORDER";
  _eventNameMap[EDIT]           = "EDIT";
  _eventNameMap[UPDATE]         = "UPDATE";
  _eventNameMap[UPDATEPROGRESS] = "UPDATEPROGRESS";
  _eventNameMap[SYNCHRO]        = "SYNCHRO";
  _eventNameMap[UP]             = "UP";
  _eventNameMap[DOWN]           = "DOWN";
  _eventNameMap[RENAME]         = "RENAME";
  _eventNameMap[NEWROOT]        = "NEWROOT";
  _eventNameMap[ENDLOAD]        = "ENDLOAD";
  _eventNameMap[ADDLINK]        = "ADDLINK";
  _eventNameMap[ADDCONTROLLINK] = "ADDCONTROLLINK";
  _eventNameMap[ADDREF]         = "ADDREF";
  _eventNameMap[ADDCHILDREF]    = "ADDCHILDREF";
  _eventNameMap[REMOVECHILDREF] = "REMOVECHILDREF";
  _eventNameMap[ASSOCIATE]      = "ASSOCIATE";
  _eventNameMap[SETVALUE]       = "SETVALUE";
  _eventNameMap[SETCASE]        = "SETCASE";
  _eventNameMap[SETSELECT]      = "SETSELECT";
  _eventNameMap[GEOMETRY]       = "GEOMETRY";
}

std::string GuiObserver::eventName(GuiEvent event)
{
  if (_eventNameMap.count(event))
    return _eventNameMap[event];
  else return "Unknown Event";
}

// ----------------------------------------------------------------------------

SubjectObserver::SubjectObserver(Subject* ref):_reference(ref)
{
}
SubjectObserver::~SubjectObserver()
{
}
void SubjectObserver::select(bool isSelected)
{
  DEBTRACE("SubjectObserver::select " << isSelected);
  //propagate nothing
}
void SubjectObserver::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SubjectObserver::update " << type << "," << eventName(event) << "," << son);
  //propagate only RENAME events
  if(event == RENAME)
    _reference->update(event,type,son);
}

SubjectReference::SubjectReference(Subject* ref, Subject *parent)
  : Subject(parent), _reference(ref)
{
  _sobs=new SubjectObserver(this);
  ref->attach(_sobs);
}

SubjectReference::~SubjectReference()
{
  delete _sobs;
}

void SubjectReference::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  Subject::clean(command);
}

void SubjectReference::localclean(Command *command)
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

void SubjectReference::reparent(Subject *parent)
{
  _parent = parent;
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
  _execState = YACS::UNDEFINED;
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
  GuiContext::getCurrent()->_mapOfSubjectNode.erase(_node);
  if (father)
    try
      {
        // Remove child except if it's the splitter node of a ForEachLoop
        if (dynamic_cast<ForEachLoop*>(father) == NULL ||
            getName() != ForEachLoop::NAME_OF_SPLITTERNODE)
          {
            father->edRemoveChild(_node);
          }
      }
    catch (YACS::Exception &e)
      {
        DEBTRACE("------------------------------------------------------------------------------");
        DEBTRACE("SubjectNode::localClean: father->edRemoveChild: YACS exception " << e.what());
        DEBTRACE("------------------------------------------------------------------------------");
      }
}

void SubjectNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  Subject::clean(command);
}

void SubjectNode::localclean(Command *command)
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
      else if( SubjectDynParaLoop* sdpl = dynamic_cast<SubjectDynParaLoop*>(_parent) )
        sdpl->removeNode(this);
      else if( SubjectSwitch* ss = dynamic_cast<SubjectSwitch*>(_parent) )
        ss->removeNode(this);
    }
}

void SubjectNode::registerUndoDestroy()
{
  DEBTRACE("SubjectNode::registerUndoDestroy " << getName());

  Proc *undoProc = GuiContext::getCurrent()->getInvoc()->getUndoProc();
  ComposedNode *oldFather = _node->getFather();

  // --- clone the node in undoProc

  ostringstream blocName;
  blocName << "undoBloc_" << Invocator::_ctr++;
  Bloc *undoBloc = new Bloc(blocName.str());
  undoProc->edAddChild(undoBloc);
  ComposedNode *newFather = undoBloc;
  Node *clone = _node->cloneWithoutCompAndContDeepCpy(0);
  newFather->edAddChild(clone);

  // --- register a CommandCopyNode from undoProc

  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = proc->getName();
  if (proc != dynamic_cast<Proc*>(_node->getFather())) position = proc->getChildName(_node->getFather());

  int swCase=0;
  if (Switch* aswitch = dynamic_cast<Switch*>(_node->getFather()))
    {
      //the node is in a switch, save the case
      swCase=aswitch->getRankOfNode(_node);
    }

  CommandCopyNode *command = new CommandCopyNode(undoProc,
                                                 undoProc->getChildName(clone),
                                                 position, swCase);
  GuiContext::getCurrent()->getInvoc()->add(command);

  // --- identify all the children service node

  list<ServiceNode*> serviceList;
  serviceList.clear();
  ServiceNode *service = 0;
  ComposedNode *cnode = 0;
  if (service = dynamic_cast<ServiceNode*>(_node))
    serviceList.push_back(service);
  else
    cnode = dynamic_cast<ComposedNode*>(_node);
  if (cnode)
    {
      list<Node*> children = cnode->getAllRecursiveNodes();
      list<Node*>::iterator it = children.begin();
      for (; it != children.end(); ++it)
        if (service = dynamic_cast<ServiceNode*>(*it))
          serviceList.push_back(service);
    }

  // --- for all the children service node, find the componentInstance,
  //     see if the componentInstance has other services that will stay alive after the destroy
  //     if not, register a CommandAddComponentInstance
  //

  list<ServiceNode*>::const_iterator ita = serviceList.begin();
  for (; ita != serviceList.end(); ++ita)
    {
      bool instanceUsedOutside = false;
      service = *ita;  
      ComponentInstance *compo = service->getComponent();
      if (!compo) break;
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectComponent.count(compo));
      SubjectComponent *scomp = GuiContext::getCurrent()->_mapOfSubjectComponent[compo];
      //list<ServiceNode*> compServiceSet; 
      set<SubjectServiceNode*>::const_iterator itset = scomp->_subServiceSet.begin();
      for (; itset != scomp->_subServiceSet.end(); ++itset)
        {
          ServiceNode *sn = dynamic_cast<ServiceNode*>((*itset)->getNode());
          if (sn && (sn != service))
            {
              //compServiceSet.push_back(sn);
              instanceUsedOutside = true;
              list<ServiceNode*>::const_iterator itb = serviceList.begin();
              for (; itb != serviceList.end(); ++itb)
                {
                  if ((*itb) == sn)
                    {
                      instanceUsedOutside = false;
                      break;
                    }
                }
              if (instanceUsedOutside)
                break;
            }
        }

      string instanceName = compo->getInstanceName();
      if (!instanceUsedOutside)
        {
          string compoName = compo->getCompoName();
          string containerName = compo->getContainer()->getName();
          CommandAddComponentInstance *comaci = new CommandAddComponentInstance(compoName, containerName, instanceName);
          GuiContext::getCurrent()->getInvoc()->add(comaci);
        }
       
      string servicePos = proc->getChildName(service);
      CommandAssociateServiceToComponent *comastc = new CommandAssociateServiceToComponent(servicePos,
                                                                                           instanceName);
      GuiContext::getCurrent()->getInvoc()->add(comastc);
    }

  // --- explore all the external links and register commands for recreation

  vector<pair<OutPort *, InPort *> > listLeaving  = getNode()->getSetOfLinksLeavingCurrentScope();
  vector<pair<InPort *, OutPort *> > listIncoming = getNode()->getSetOfLinksComingInCurrentScope();
  vector<pair<OutPort *, InPort *> > globalList = listLeaving;
  vector<pair<InPort *, OutPort *> >::iterator it1;
  for (it1 = listIncoming.begin(); it1 != listIncoming.end(); ++it1)
    {
      pair<OutPort *, InPort *> outin = pair<OutPort *, InPort *>((*it1).second, (*it1).first);
      globalList.push_back(outin);
    }
  vector<pair<OutPort *, InPort *> >::iterator it2;
  for (it2 = globalList.begin(); it2 != globalList.end(); ++it2)
    {
      SubjectLink* subject = 0;
      if (GuiContext::getCurrent()->_mapOfSubjectLink.count(*it2))
        {
          subject = GuiContext::getCurrent()->_mapOfSubjectLink[*it2];
          YASSERT(subject);
          DEBTRACE("register undo destroy link " << subject->getName());
          subject->registerUndoDestroy();
        }
    }

  // --- explore all the external control links and register commands for recreation

  list<SubjectControlLink*> cplcl = getSubjectControlLinks();
  list<SubjectControlLink*>::iterator its;
  Node* node = getNode();
  for (its = cplcl.begin(); its != cplcl.end(); ++its)
    {
      bool inside = true;
      Node *nout = (*its)->getSubjectOutNode()->getNode();
      Node *nin = (*its)->getSubjectInNode()->getNode();
      inside = inside && (node == nout);
      inside = inside && (node == nin);
      if (!inside)
        {
          (*its)->registerUndoDestroy();
        }
    }
}

bool SubjectNode::reparent(Subject* parent)
{
  DEBTRACE("SubjectNode::reparent");
  Subject *sub = getParent(); // --- old parent subject
  SubjectComposedNode *sop = dynamic_cast<SubjectComposedNode*>(sub);
  YASSERT(sop);
  SubjectComposedNode *snp = dynamic_cast<SubjectComposedNode*>(parent); // --- new parent subject
  if (!snp)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "new parent must be a composed node";
      DEBTRACE(GuiContext::getCurrent()->_lastErrorMessage);
      return false;
    }
  ComposedNode *cnp = dynamic_cast<ComposedNode*>(snp->getNode());
  YASSERT(cnp);
  Proc *proc = GuiContext::getCurrent()->getProc();
  Proc *fromproc=_node->getProc();
  if(proc != fromproc)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "cut is not yet possible across procs";
      DEBTRACE(GuiContext::getCurrent()->_lastErrorMessage);
      return false;
    }

  string position = "";
  if (proc != dynamic_cast<Proc*>(_node))
    position = proc->getChildName(_node);
  else
    position = _node->getName();

  string newParent = "";
  if (proc != dynamic_cast<Proc*>(cnp))
    newParent = proc->getChildName(cnp);
  else
    newParent = cnp->getName();

  CommandReparentNode *command = new CommandReparentNode(position, newParent);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  DEBTRACE(GuiContext::getCurrent()->_lastErrorMessage);
  return false;
}

void SubjectNode::recursiveUpdate(GuiEvent event, int type, Subject* son)
{
  update(event, type, son);
}

bool SubjectNode::copy(Subject* parent)
{
  DEBTRACE("SubjectNode::copy");
  Subject *sop = getParent(); // --- old parent subject
  SubjectComposedNode *snp = dynamic_cast<SubjectComposedNode*>(parent); // --- new parent subject
  if (!snp)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "new parent must be a composed node";
      DEBTRACE(GuiContext::getCurrent()->_lastErrorMessage);
      return false;
    }
  ComposedNode *cnp = dynamic_cast<ComposedNode*>(snp->getNode());
  YASSERT(cnp);
  Proc *proc = GuiContext::getCurrent()->getProc();
  Proc *fromproc=_node->getProc();
  if(proc != fromproc)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "It is not possible to paste an object from one schema to another.";
      DEBTRACE(GuiContext::getCurrent()->_lastErrorMessage);
      return false;
    }

  string position = "";
  if (fromproc != dynamic_cast<Proc*>(_node))
    position = fromproc->getChildName(_node);
  else
    position = _node->getName();

  string newParent = "";
  if (proc != dynamic_cast<Proc*>(cnp))
    newParent = proc->getChildName(cnp);
  else
    newParent = cnp->getName();

  CommandCopyNode *command = new CommandCopyNode(fromproc, position, newParent);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  DEBTRACE(GuiContext::getCurrent()->_lastErrorMessage);
  return false;

}

std::string SubjectNode::getName()
{
  return _node->getName();
}

YACS::ENGINE::Node* SubjectNode::getNode()
{
  return _node;
}

int SubjectNode::isValid()
{
  return _node->isValid();
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

void SubjectNode::saveLinks()
{
  DEBTRACE("SubjectNode::saveLinks");
  loutgate= _node->getInGate()->getBackLinks();
  singate= _node->getOutGate()->edSetInGate();
  std::list<OutGate *>::const_iterator ito;
  for(ito=loutgate.begin();ito != loutgate.end();ito++)
    {
      Node* n1=(*ito)->getNode();
      Node* n2=_node;
      DEBTRACE(n1->getName()<< " " << n2->getName());
    }
  std::list<InGate *>::const_iterator iti;
  for(iti=singate.begin();iti != singate.end();iti++)
    {
      Node* n1=_node;
      Node* n2=(*iti)->getNode();
      DEBTRACE(n1->getName()<< " " << n2->getName());
    }

  dataLinks.clear();
  dataflowLinks.clear();

  std::vector< std::pair<OutPort *, InPort *> > listLeaving  = getNode()->getSetOfLinksLeavingCurrentScope();
  std::vector< std::pair<OutPort *, InPort *> >::iterator it3;
  for (it3 = listLeaving.begin(); it3 != listLeaving.end(); ++it3)
    {
      OutPort* p1=(*it3).first;
      InPort* p2=(*it3).second;
      Node* n1=p1->getNode();
      Node* n2=p2->getNode();
      //are nodes in sequence (control link direct or indirect) ?
      ComposedNode* fath= ComposedNode::getLowestCommonAncestor(n1,n2);
      if(n1 == fath ||n2 == fath)
        {
          //consider it as a data only link
          DEBTRACE("It's a data link: " << n1->getName() << "." << p1->getName() << " -> "<< n2->getName() << "." << p2->getName());
          dataLinks.push_back(std::pair<OutPort *, InPort *>(p1,p2));
          continue;
        }
      while(n1->getFather() != fath) n1=n1->getFather();
      while(n2->getFather() != fath) n2=n2->getFather();
      OutGate* outg=n1->getOutGate();
      if(!outg->isAlreadyInSet(n2->getInGate()))
        {
          DEBTRACE("It's a data link: "<<p1->getNode()->getName()<<"."<<p1->getName()<<" -> "<< p2->getNode()->getName()<<"."<<p2->getName());
          dataLinks.push_back(std::pair<OutPort *, InPort *>(p1,p2));
        }
      else
        {
          DEBTRACE("It's a dataflow link: "<<p1->getNode()->getName()<<"."<<p1->getName()<<" -> "<< p2->getNode()->getName()<<"."<<p2->getName());
          dataflowLinks.push_back(std::pair<OutPort *, InPort *>(p1,p2));
        }
    }

  std::vector< std::pair<InPort *, OutPort *> > listIncoming  = getNode()->getSetOfLinksComingInCurrentScope();
  std::vector< std::pair<InPort *, OutPort *> >::iterator it4;
  for (it4 = listIncoming.begin(); it4 != listIncoming.end(); ++it4)
    {
      OutPort* p1=(*it4).second;
      InPort* p2=(*it4).first;
      Node* n1=p1->getNode();
      Node* n2=p2->getNode();
      //are nodes in sequence (control link direct or indirect) ?
      ComposedNode* fath= ComposedNode::getLowestCommonAncestor(n1,n2);
      if(n1 == fath ||n2 == fath)
        {
          //consider it as a data only link
          DEBTRACE("It's a data link: " << n1->getName() << "." << p1->getName() << " -> "<< n2->getName() << "." << p2->getName());
          dataLinks.push_back(std::pair<OutPort *, InPort *>(p1,p2));
          continue;
        }
      while(n1->getFather() != fath) n1=n1->getFather();
      while(n2->getFather() != fath) n2=n2->getFather();
      OutGate* outg=n1->getOutGate();
      if(!outg->isAlreadyInSet(n2->getInGate()))
        {
          DEBTRACE("It's a data link: "<<p1->getNode()->getName()<<"."<<p1->getName()<<" -> "<< p2->getNode()->getName()<<"."<<p2->getName());
          dataLinks.push_back(std::pair<OutPort *, InPort *>(p1,p2));
        }
      else
        {
          DEBTRACE("It's a dataflow link: "<<p1->getNode()->getName()<<"."<<p1->getName()<<" -> "<< p2->getNode()->getName()<<"."<<p2->getName());
          dataflowLinks.push_back(std::pair<OutPort *, InPort *>(p1,p2));
        }
    }
}

void SubjectNode::restoreLinks()
{
  DEBTRACE("SubjectNode::restoreLinks");
  //restore simple data links
  std::vector< std::pair<OutPort *, InPort *> >::iterator it3;
  for (it3 = dataLinks.begin(); it3 != dataLinks.end(); ++it3)
    {
      OutPort* p1=(*it3).first;
      InPort* p2=(*it3).second;
      Node* n1=p1->getNode();
      Node* n2=p2->getNode();
      ComposedNode* fath= ComposedNode::getLowestCommonAncestor(n1,n2);
      try
        {
          fath->edAddLink(p1,p2);
        }
      catch(Exception& ex)
        {
          // if a link can't be restored ignore it. It's possible when a node is reparented to a foreachloop 
          continue;
        }
      SubjectComposedNode *scla = dynamic_cast<SubjectComposedNode*>(GuiContext::getCurrent()->_mapOfSubjectNode[fath]);
      SubjectNode *sno = GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(n1)];
      SubjectNode *sni = GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(n2)];
      SubjectDataPort *spo = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(p1)];
      SubjectDataPort *spi = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(p2)];
      scla->addSubjectLink(sno,spo,sni,spi);
    }
  //restore dataflow links
  for (it3 = dataflowLinks.begin(); it3 != dataflowLinks.end(); ++it3)
    {
      OutPort* p1=(*it3).first;
      InPort* p2=(*it3).second;
      Node* n1=p1->getNode();
      Node* n2=p2->getNode();
      ComposedNode* fath= ComposedNode::getLowestCommonAncestor(n1,n2);
      try
        {
          fath->edAddDFLink(p1,p2);
        }
      catch(Exception& ex)
        {
          // if a link can't be restored ignore it. It's possible when a node is reparented to a foreachloop 
          continue;
        }
      SubjectComposedNode *scla = dynamic_cast<SubjectComposedNode*>(GuiContext::getCurrent()->_mapOfSubjectNode[fath]);
      SubjectNode *sno = GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(n1)];
      SubjectNode *sni = GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(n2)];
      SubjectDataPort *spo = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(p1)];
      SubjectDataPort *spi = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(p2)];
      scla->addSubjectLink(sno,spo,sni,spi);
      if(n1==fath || n2==fath) continue;
      while(n1->getFather() != fath) n1=n1->getFather();
      while(n2->getFather() != fath) n2=n2->getFather();
      OutGate *ogate = n1->getOutGate();
      InGate *igate = n2->getInGate();
      if (ogate->isAlreadyInSet(igate))
        {
          pair<Node*,Node*> keyLink(n1,n2);
          if (!GuiContext::getCurrent()->_mapOfSubjectControlLink.count(keyLink))
            {
              SubjectNode *sfno = GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(n1)];
              SubjectNode *sfni = GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(n2)];
              scla->addSubjectControlLink(sfno, sfni);
            }
        }
    }

  //reconnect control links
  // links from another node to this node
  std::list<OutGate *>::const_iterator it;
  for(it=loutgate.begin();it != loutgate.end();it++)
    {
      Node* n1=(*it)->getNode();
      Node* n2=_node;
      if(GuiContext::getCurrent()->_mapOfSubjectNode.count(n1)==0)
        {
          //It's an internal node or a destroyed one : don't treat it
          continue;
        }
      ComposedNode* fath= ComposedNode::getLowestCommonAncestor(n1,n2);
      if(n1 == fath)continue;
      if(n2 == fath)continue;
      //add a control link only if nodes are not in the same descendance
      while(n1->getFather() != fath) n1=n1->getFather();
      while(n2->getFather() != fath) n2=n2->getFather();
      OutGate *ogate = n1->getOutGate();
      InGate *igate = n2->getInGate();
      if (!ogate->isAlreadyInSet(igate))
        {
          fath->edAddCFLink(n1,n2);
          SubjectComposedNode *scla = dynamic_cast<SubjectComposedNode*>(GuiContext::getCurrent()->_mapOfSubjectNode[fath]);
          SubjectNode * subOutNode = GuiContext::getCurrent()->_mapOfSubjectNode[n1];
          SubjectNode * subInNode = GuiContext::getCurrent()->_mapOfSubjectNode[n2];
          scla->addSubjectControlLink(subOutNode,subInNode);
        }
    }

  std::list<InGate *>::const_iterator it2;
  for(it2=singate.begin();it2 != singate.end();it2++)
    {
      Node* n1=_node;
      Node* n2=(*it2)->getNode();
      if(GuiContext::getCurrent()->_mapOfSubjectNode.count(n2)==0)
        {
          //It's an internal node or a destroyed one : don't treat it
          continue;
        }
      ComposedNode* fath= ComposedNode::getLowestCommonAncestor(n1,n2);
      if(n1 == fath)continue;
      if(n2 == fath)continue;
      //add a control link only if nodes are not in the same descendance
      while(n1->getFather() != fath) n1=n1->getFather();
      while(n2->getFather() != fath) n2=n2->getFather();
      OutGate *ogate = n1->getOutGate();
      InGate *igate = n2->getInGate();
      if (!ogate->isAlreadyInSet(igate))
        {
          fath->edAddCFLink(n1,n2);
          SubjectComposedNode *scla = dynamic_cast<SubjectComposedNode*>(GuiContext::getCurrent()->_mapOfSubjectNode[fath]);
          SubjectNode * subOutNode = GuiContext::getCurrent()->_mapOfSubjectNode[n1];
          SubjectNode * subInNode = GuiContext::getCurrent()->_mapOfSubjectNode[n2];
          scla->addSubjectControlLink(subOutNode,subInNode);
        }
    }
}

bool SubjectNode::putInComposedNode(std::string name,std::string type, bool toSaveRestoreLinks)
{
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);

  CommandPutInComposedNode *command = new CommandPutInComposedNode(position, name, type, toSaveRestoreLinks);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  DEBTRACE(GuiContext::getCurrent()->_lastErrorMessage);
  return false;
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
  if (!GuiContext::getCurrent()->getSubjectProc()->addComSubjectDataType(typcod, typcod->name()))
    GuiContext::getCurrent()->getSubjectProc()->addSubjectDataType(typcod, typcod->name());  // --- new type of forEachLoop
  return son;
}

void SubjectNode::update( GuiEvent event, int type, Subject* son )
{
  Subject::update( event, type, son );
  
  // remove subject data type if necessary
  YACS::HMI::SubjectDataPort* aSPort = dynamic_cast< YACS::HMI::SubjectDataPort* >( son );
//   if ( aSPort && event == REMOVE )
//   {
//     YACS::ENGINE::DataPort* aEPort = aSPort->getPort();
//     if ( aEPort )
//     {
//       YACS::ENGINE::TypeCode* aTypeCode = aEPort->edGetType();
//       if ( aTypeCode )
//         GuiContext::getCurrent()->getSubjectProc()->removeSubjectDataType( aTypeCode );
//     }
//   }
}

void SubjectNode::setExecState(int execState)
{
  _execState = execState;
  update(YACS::HMI::UPDATEPROGRESS, _execState, this);
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
  GuiContext::getCurrent()->getSubjectProc()->addComSubjectDataType(typcod, typcod->name());
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
  GuiContext::getCurrent()->getSubjectProc()->addComSubjectDataType(typcod, typcod->name());
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
  GuiContext::getCurrent()->getSubjectProc()->addComSubjectDataType(typcod, typcod->name());
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
          YASSERT(subject);
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

void SubjectNode::removeExternalControlLinks()
{
  DEBTRACE("SubjectNode::removeExternalControlLinks " << getName());
  list<SubjectControlLink*> cplcl = getSubjectControlLinks();
  list<SubjectControlLink*>::iterator its;
  Node* node = getNode();
  for (its = cplcl.begin(); its != cplcl.end(); ++its)
    {
      bool inside = true;
      Node *nout = (*its)->getSubjectOutNode()->getNode();
      Node *nin = (*its)->getSubjectInNode()->getNode();
      inside = inside && (node == nout);
      inside = inside && (node == nin);
      if (!inside)
        Subject::erase(*its);
    }
}

std::map<std::string, std::string> SubjectNode::getProperties()
{
  return _node->getPropertyMap();
}

bool SubjectNode::setProperties(std::map<std::string, std::string> properties)
{
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);

  CommandSetNodeProperties *command = new CommandSetNodeProperties(position, properties);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
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

void SubjectComposedNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectNode::clean(command);
}

void SubjectComposedNode::localclean(Command *command)
{
  DEBTRACE("SubjectComposedNode::localClean ");
}

SubjectNode* SubjectComposedNode::addNode(YACS::ENGINE::Catalog *catalog,
                                          std::string compo,
                                          std::string type,
                                          std::string name,
                                          bool newCompoInst)
{
  DEBTRACE("SubjectComposedNode::addNode("<<catalog<<","<<compo<<","<<type<<","<<name<<")");
  SubjectNode* body = 0;
  GuiContext::getCurrent()->_lastErrorMessage = "YACS Internal Error: SubjectComposedNode::addNode"; 
  return body;
}

SubjectNode *SubjectComposedNode::createNode(YACS::ENGINE::Catalog *catalog,
                                             std::string compo,
                                             std::string type,
                                             std::string name,
                                             bool newCompoInst,
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
                                                                     newCompoInst,
                                                                     swCase);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      Node * node = command->getNode();
      return command->getSubjectNode();
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
      //YASSERT(0);
    }
  YASSERT(son);
  GuiContext::getCurrent()->_mapOfSubjectNode[static_cast<Node*>(node)] = son;
  GuiContext::getCurrent()->_mapOfExecSubjectNode[node->getNumId()] = son;
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
  OutPort *outp = dynamic_cast<OutPort*>(spo->getPort());
  InPort *inp = dynamic_cast<InPort*>(spi->getPort());
  pair<OutPort*,InPort*> keyLink(outp,inp);
  //Don't create a new subject if it already exists
  if(GuiContext::getCurrent()->_mapOfSubjectLink.count(keyLink)!=0)
    return GuiContext::getCurrent()->_mapOfSubjectLink[keyLink];

  SubjectLink *son = new SubjectLink(sno, spo, sni, spi, this);
  GuiContext::getCurrent()->_mapOfSubjectLink[keyLink] = son;
  _listSubjectLink.push_back(son);
  spo->addSubjectLink(son);
  spi->addSubjectLink(son);
  update(ADDLINK, DATALINK, son);
  spi->update(UPDATE, DATALINK, spo);
  spo->update(UPDATE, DATALINK, spi);
  DEBTRACE("addSubjectLink: " << getName() << " " << son->getName());
  return son;
}

void SubjectComposedNode::removeLink(SubjectLink* link)
{
  DEBTRACE("removeLink: " << link->getName());

  OutPort *outp = dynamic_cast<OutPort*>(link->getSubjectOutPort()->getPort());
  InPort  *inp  = dynamic_cast<InPort*>(link->getSubjectInPort()->getPort());
  pair<OutPort*,InPort*> keyLink(outp,inp);
  if (GuiContext::getCurrent()->_mapOfSubjectLink.count(keyLink))
    {
      DEBTRACE(outp->getName() << " " << inp->getName());
      GuiContext::getCurrent()->_mapOfSubjectLink.erase(keyLink);
    }

  link->getSubjectOutPort()->removeSubjectLink(link);
  link->getSubjectInPort()->removeSubjectLink(link);
  _listSubjectLink.remove(link);
}

SubjectControlLink* SubjectComposedNode::addSubjectControlLink(SubjectNode *sno,
                                                        SubjectNode *sni)
{
  Node *outn = sno->getNode();
  Node *inn = sni->getNode();
  pair<Node*,Node*> keyLink(outn,inn);
  //Don't create a new subject if it already exists
  if(GuiContext::getCurrent()->_mapOfSubjectControlLink.count(keyLink)!=0)
    return GuiContext::getCurrent()->_mapOfSubjectControlLink[keyLink];

  SubjectControlLink *son = new SubjectControlLink(sno, sni, this);
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

  Node *outn = link->getSubjectOutNode()->getNode();
  Node *inn = link->getSubjectInNode()->getNode();
  pair<Node*,Node*> keyLink(outn,inn);
  if (GuiContext::getCurrent()->_mapOfSubjectControlLink.count(keyLink))
    {
      DEBTRACE(outn->getName() << " " << inn->getName());
      GuiContext::getCurrent()->_mapOfSubjectControlLink.erase(keyLink);
    }

  link->getSubjectOutNode()->removeSubjectControlLink(link);
  link->getSubjectInNode()->removeSubjectControlLink(link);
  _listSubjectControlLink.remove(link);
}

void SubjectComposedNode::removeExternalControlLinks()
{
  DEBTRACE("SubjectComposedNode::removeExternalControlLinks " << getName());
  list<SubjectControlLink*> cplcl = getSubjectControlLinks();
  list<SubjectControlLink*>::iterator its;
  ComposedNode *cnode = dynamic_cast<ComposedNode*>(getNode());
  for (its = cplcl.begin(); its != cplcl.end(); ++its)
    {
      bool inside = true;
      Node *nout = (*its)->getSubjectOutNode()->getNode();
      Node *nin = (*its)->getSubjectInNode()->getNode();
      inside = inside && cnode->isInMyDescendance(nout); // --- 0 if nout is outside
      inside = inside && cnode->isInMyDescendance(nin);  // --- 0 if nin is outside
      if (!inside)
        Subject::erase(*its);
    }
}   

void SubjectComposedNode::houseKeepingAfterCutPaste(bool isCut, SubjectNode *son)
{
}

/*!
 * loadLinks is used when an existing scheme has been loaded in memory, to create gui representation.
 * Gui representation of links is done after node representation (loadChildren).
 * Proc is explored recursively to find the composedNodes and create the corresponding links
 * representation, from bottom to top.
 * For each composedNode, data links representation are created first and stored in a map to avoid
 * double representation. Then control links representation are created.
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
          YASSERT(subSon);
          SubjectComposedNode *subCnSon = dynamic_cast<SubjectComposedNode*>(subSon);
          YASSERT(subCnSon);
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
      std::list<InGate*> setIngate = outgate->edSetInGate();
      std::list<InGate*>::const_iterator itg;
      for(itg = setIngate.begin(); itg != setIngate.end(); ++itg)
        {
          Node* inNode = (*itg)->getNode();
          SubjectNode* sni = GuiContext::getCurrent()->_mapOfSubjectNode[inNode];
          if(sno && sni)
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

/*! used in derived classes using a counter, a selector, or a condition:
 *  ForLoop, ForEachLoop, Switch, WhileLoop.
 */
bool SubjectComposedNode::hasValue()
{
  return false;
}

/*! used in derived classes using a counter, a selector, or a condition:
 *  ForLoop, ForEachLoop, Switch, WhileLoop.
 */
std::string SubjectComposedNode::getValue()
{
  return "";
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

void SubjectBloc::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectComposedNode::clean(command);
}

void SubjectBloc::localclean(Command *command)
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
                                  std::string name,
                                  bool newCompoInst)
{
  DEBTRACE("SubjectBloc::addNode( " << catalog << ", " << compo << ", " << type << ", " << name << " )");
  SubjectNode* child = createNode(catalog, compo, type, name, newCompoInst);
  return child;
}

void SubjectBloc::houseKeepingAfterCutPaste(bool isCut, SubjectNode *son)
{
  if (isCut)
    removeNode(son);
  else
    completeChildrenSubjectList(son);
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
    set<SubjectNode*>::const_iterator it = _children.begin();
    for ( ; it != _children.end(); it++ )
      if ( (*it)->getNode() == node )
      {
        aChild = (*it);
        break;
      }
  }

  return aChild;
}

void SubjectBloc::recursiveUpdate(GuiEvent event, int type, Subject* son)
{
  update(event, type, son);
  set<SubjectNode*>::iterator it = _children.begin();
  for (; it != _children.end(); ++it)
    (*it)->recursiveUpdate(event, type, son);
}


// ----------------------------------------------------------------------------

SubjectProc::SubjectProc(YACS::ENGINE::Proc *proc, Subject *parent)
  : SubjectBloc(proc, parent), _proc(proc)
{
  _postEraseList.clear();
}

SubjectProc::~SubjectProc()
{
  DEBTRACE("SubjectProc::~SubjectProc " << getName());
}

void SubjectProc::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectBloc::clean(command);
}

void SubjectProc::localclean(Command *command)
{
  DEBTRACE("SubjectProc::localClean ");
}

void SubjectProc::cleanPostErase()
{
  DEBTRACE("SubjectProc::cleanPostErase");
  for (int i=0; i<_postEraseList.size(); i++)
    {
      DEBTRACE("cleanPostErase " << _postEraseList[i]->getName());
      erase(_postEraseList[i],0, true); // --- true: do not call recursively erase
    }
  _postEraseList.clear();
}

void SubjectProc::loadProc()
{
  DEBTRACE("SubjectProc::loadProc "  << getName());
  loadTypes();
  loadContainers();
  loadComponents();
  loadChildren();
  loadLinks();
}

//! Load types for a SubjectProc
/*!
 * This method loads (on import or on creation) all types of the builtin catalog and all types defined in the Proc
 */
void SubjectProc::loadTypes()
{
  Catalog* builtinCatalog = getSALOMERuntime()->getBuiltinCatalog();
  std::map<std::string, TypeCode *>::iterator pt;
  for(pt=builtinCatalog->_typeMap.begin();pt!=builtinCatalog->_typeMap.end();pt++)
    {
      addSubjectDataType((*pt).second , (*pt).first);
    }
  for(pt=_proc->typeMap.begin();pt!=_proc->typeMap.end();pt++)
    {
      addSubjectDataType((*pt).second , (*pt).first);
    }
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
  for (map<string, ComponentInstance*>::const_iterator itComp = aProc->componentInstanceMap.begin();
       itComp != aProc->componentInstanceMap.end(); ++itComp)
    {
      GuiContext::getCurrent()->_mapOfLastComponentInstance[itComp->second->getCompoName()]=itComp->second;

      if ( GuiContext::getCurrent()->_mapOfSubjectComponent.find((*itComp).second)
           ==
           GuiContext::getCurrent()->_mapOfSubjectComponent.end() )
      { // engine object for component already exists => add only a subject for it
        addSubjectComponent((*itComp).second);
      }
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
  for (map<string, Container*>::const_iterator itCont = aProc->containerMap.begin(); itCont != aProc->containerMap.end(); ++itCont)
    if ( GuiContext::getCurrent()->_mapOfSubjectContainer.find((*itCont).second) == GuiContext::getCurrent()->_mapOfSubjectContainer.end() )
      // engine object for container already exists => add only a subject for it
      addSubjectContainer((*itCont).second,(*itCont).second->getName());
}

SubjectComponent* SubjectProc::addComponent(std::string compoName, std::string containerName)
{
  DEBTRACE("SubjectProc::addComponent " << compoName << " " << containerName);
  CommandAddComponentInstance *command = new CommandAddComponentInstance(compoName, containerName);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return command->getSubjectComponent();
    }
  else delete command;
  return 0;
}

SubjectContainerBase *SubjectProc::addContainer(std::string name, std::string ref)
{
  DEBTRACE("SubjectProc::addContainer " << name << " " << ref);
  if (! GuiContext::getCurrent()->getProc()->containerMap.count(name))
    {
      CommandAddContainer *command(new CommandAddContainer(name,ref));
      if (command->execute())
        {
          GuiContext::getCurrent()->getInvoc()->add(command);
          return command->getSubjectContainer();
        }
      else
          delete command;
    }
  else GuiContext::getCurrent()->_lastErrorMessage = "There is already a container with that name";
  return 0;
}

SubjectContainerBase* SubjectProc::addHPContainer(std::string name, std::string ref)
{
  DEBTRACE("SubjectProc::addContainer " << name << " " << ref);
  if (! GuiContext::getCurrent()->getProc()->containerMap.count(name))
    {
      CommandAddHPContainer *command(new CommandAddHPContainer(name,ref));
      if (command->execute())
        {
          GuiContext::getCurrent()->getInvoc()->add(command);
          return command->getSubjectContainer();
        }
      else
        delete command;
    }
  else GuiContext::getCurrent()->_lastErrorMessage = "There is already a container with that name";
  return 0;
}

bool SubjectProc::addDataType(YACS::ENGINE::Catalog* catalog, std::string typeName)
{
  DEBTRACE("SubjectProc::addDataType " << typeName);
  CommandAddDataTypeFromCatalog *command = new CommandAddDataTypeFromCatalog(catalog, typeName);
  if (command->execute())
    {
      DEBTRACE("new datatype " << typeName);
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
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
  //son->setContainer();
  return son;
}

SubjectContainerBase *SubjectProc::addSubjectContainer(YACS::ENGINE::Container *cont, std::string name)
{
  DEBTRACE("SubjectProc::addSubjectContainer " << name);
  SubjectContainerBase *son(SubjectContainerBase::New(cont,this));
  // In edition mode do not clone containers
  // cont->attachOnCloning(); // agy : do not use _attachOnCloning attribute in edition mode. This attribute should be used now at runtime.
  GuiContext::getCurrent()->_mapOfSubjectContainer[cont] = son;
  update(ADD, CONTAINER, son);
  return son;
}

/*! register a sub command when importing a node from catalog with new data types
 *
 */
SubjectDataType* SubjectProc::addComSubjectDataType(YACS::ENGINE::TypeCode *type, std::string alias)
{
  string typeName = type->name();
  DEBTRACE("SubjectProc::addComSubjectDataType " << typeName);
  SubjectDataType* son = 0;
  if (GuiContext::getCurrent()->isLoading())
    son = addSubjectDataType(type,alias); // no sub command registration
  else
    {
      Catalog *catalog = GuiContext::getCurrent()->getCurrentCatalog();
      CommandAddDataTypeFromCatalog *command = new CommandAddDataTypeFromCatalog(catalog,
                                                                                 typeName);
      if (command->execute())
        GuiContext::getCurrent()->getInvoc()->add(command);
      else delete command;
    }
  return son;
}


SubjectDataType* SubjectProc::addSubjectDataType(YACS::ENGINE::TypeCode *type, std::string alias)
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
      son = new SubjectDataType(type, this, alias);
      GuiContext::getCurrent()->_mapOfSubjectDataType[typeName] = son;
      update(ADD, DATATYPE, son);
    }
  else
    GuiContext::getCurrent()->_lastErrorMessage = "Typecode " + typeName + " was already added in proc";
  return son;
}

void SubjectProc::removeSubjectDataType(std::string typeName)
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  YASSERT(proc->typeMap.count(typeName));
  YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataType.count(typeName));
  TypeCode* aTypeCode = proc->typeMap[typeName];
  SubjectDataType* aSDataType = GuiContext::getCurrent()->_mapOfSubjectDataType[typeName];
  unsigned int aRefCnt = aTypeCode->getRefCnt();
  DEBTRACE("SubjectProc::removeSubjectDataType: " << typeName << " " << aRefCnt);
  GuiContext::getCurrent()->_mapOfSubjectDataType.erase(typeName);
  proc->typeMap.erase(typeName);
  erase(aSDataType); // do all the necessary updates
  aTypeCode->decrRef();
}

void SubjectProc::removeSubjectContainer(SubjectContainerBase* scont)
{
  YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(scont->getContainer()));
  erase(scont); // do all the necessary updates
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

void SubjectElementaryNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectNode::clean(command);
}

void SubjectElementaryNode::localclean(Command *command)
{
  DEBTRACE("SubjectElementaryNode::localClean ");
}

void SubjectElementaryNode::recursiveUpdate(GuiEvent event, int type, Subject* son)
{
  update(event, type, son);
}

SubjectDataPort* SubjectElementaryNode::addInputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectElementaryNode::addInputPort( " << catalog << ", " << type << ", " << name << " )");
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);
  else YASSERT(0);
  CommandAddInputPortFromCatalog *command = new CommandAddInputPortFromCatalog(catalog,
                                                                               type,
                                                                               position,
                                                                               name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      SubjectInputPort *son = command->getSubjectInputPort();
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
  else YASSERT(0);
  CommandAddOutputPortFromCatalog *command = new CommandAddOutputPortFromCatalog(catalog,
                                                                                 type,
                                                                                 position,
                                                                                 name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      SubjectOutputPort *son = command->getSubjectOutputPort();
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
  else YASSERT(0);
  CommandAddIDSPortFromCatalog *command = new CommandAddIDSPortFromCatalog(catalog,
                                                                           type,
                                                                           position,
                                                                           name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      SubjectInputDataStreamPort *son = command->getSubjectIDSPort();
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
  else YASSERT(0);
  CommandAddODSPortFromCatalog *command = new CommandAddODSPortFromCatalog(catalog,
                                                                           type,
                                                                           position,
                                                                           name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      SubjectOutputDataStreamPort *son = command->getSubjectODSPort();
      return son;
    }
  else delete command;
  return 0;
}

bool SubjectElementaryNode::OrderDataPorts(SubjectDataPort* portToMove, int isUp)
{
  DEBTRACE("SubjectElementaryNode::OrderDataPorts");
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);
  else YASSERT(0);

  if (!portToMove) return false;
  string nameToMove = portToMove->getName();

  Command *command = 0;
  bool isInput = dynamic_cast<SubjectInputPort*>(portToMove);
  if (isInput)
    command = new CommandOrderInputPorts(position, nameToMove, isUp);
  else
    command = new CommandOrderOutputPorts(position, nameToMove, isUp);

  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  return false;
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

void SubjectElementaryNode::saveLinks()
{
  SubjectNode::saveLinks();
}

void SubjectElementaryNode::restoreLinks()
{
  SubjectNode::restoreLinks();
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

void SubjectInlineNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectElementaryNode::clean(command);
}

void SubjectInlineNode::localclean(Command *command)
{
  DEBTRACE("SubjectInlineNode::localClean ");
}

bool SubjectInlineNode::setExecutionMode(const std::string& mode)
{
  DEBTRACE("SubjectInlineNode::setExecutionMode ");
  Proc *proc = GuiContext::getCurrent()->getProc();

  CommandSetExecutionMode *command = new CommandSetExecutionMode(proc->getChildName(_node), mode);
  if (command->execute())
    {
      if (!GuiContext::getCurrent()->isLoading()) // do not register command when loading a schema
        GuiContext::getCurrent()->getInvoc()->add(command);
      else delete command;
      return true;
    }
  else delete command;
  return false;
}

bool SubjectInlineNode::setContainer(SubjectContainerBase *scont)
{
  DEBTRACE("SubjectInlineNode::setContainer ");
  Proc *proc = GuiContext::getCurrent()->getProc();

  CommandSetContainer *command = new CommandSetContainer(proc->getChildName(_node), scont->getName());
  if (command->execute())
    {
      if (!GuiContext::getCurrent()->isLoading()) // do not register command when loading a schema
        GuiContext::getCurrent()->getInvoc()->add(command);
      else delete command;
      return true;
    }
  else delete command;
  return false;
}


// ----------------------------------------------------------------------------

SubjectServiceNode::SubjectServiceNode(YACS::ENGINE::ServiceNode *serviceNode, Subject *parent)
  : SubjectElementaryNode(serviceNode, parent), _serviceNode(serviceNode)
{
  _subjectReference = 0;
  _subRefComponent = 0;
}

SubjectServiceNode::~SubjectServiceNode()
{
  DEBTRACE("SubjectServiceNode::~SubjectServiceNode " << getName());
  ComponentInstance *instance = _serviceNode->getComponent();
  if (!instance) return;
  if (GuiContext::getCurrent()->_mapOfSubjectComponent.count(instance))
    {
      SubjectComponent *scomp = GuiContext::getCurrent()->_mapOfSubjectComponent[instance];
      if (!scomp->hasServices())
        GuiContext::getCurrent()->getSubjectProc()->addPostErase(scomp);
    }
}

void SubjectServiceNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectElementaryNode::clean(command);
}

void SubjectServiceNode::localclean(Command *command)
{
  DEBTRACE("SubjectServiceNode::localClean ");
  if (_subRefComponent)
    {
      SubjectComponent *scomp = dynamic_cast<SubjectComponent*>(_subRefComponent->getParent());
      if (scomp)
        scomp->removeSubServiceFromSet(this);
    }
  if (_subjectReference)
    {
//       update( REMOVE, REFERENCE, _subjectReference );
      erase(_subjectReference);
      _subjectReference = 0;
    }
  if (_subRefComponent)
    {
      erase(_subRefComponent);
      _subRefComponent = 0;
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
  DEBTRACE("SubjectServiceNode::setComponentFromCatalog " << compo);

  Proc* proc = GuiContext::getCurrent()->getProc();
  string position = proc->getChildName(_node);
  CommandAddComponentFromCatalog* command =
    new CommandAddComponentFromCatalog(catalog,
                                       position,
                                       compo,
                                       service);
  if (command->execute())
    GuiContext::getCurrent()->getInvoc()->add(command);
  else delete command;
  return;
}

/*!
 *  When loading scheme from file, get the component associated to the node, if any,
 *  and create the corresponding subject.
 */
void SubjectServiceNode::setComponent()
{
  DEBTRACE("SubjectServiceNode::setComponent");
  ComponentInstance *instance = _serviceNode->getComponent();
  if (instance)
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      string compo = instance->getCompoName();
      SubjectComponent* subCompo = 0;
      if (! GuiContext::getCurrent()->_mapOfSubjectComponent.count(instance))
        {
          DEBTRACE("SubjectServiceNode::setComponent : create subject for compo = " << compo.c_str());
          if(proc->componentInstanceMap.count(instance->getInstanceName())==0)
            {
              std::cerr << "PROBLEM : ComponentInstance should be registered in proc, add it " << instance->getInstanceName() << std::endl;
              proc->componentInstanceMap[instance->getInstanceName()] = instance;
              Container* cont=instance->getContainer();
              //check if the associated container is consistent with containerMap
              if(cont)
                if (proc->containerMap.count(cont->getName()) == 0)
                  {
                    //container exists but is not in containerMap. Clone it, it's probably the result of copy paste from outside the proc
                    Container* newcont(cont->cloneAlways());
                    proc->containerMap[cont->getName()]=newcont;
                    instance->setContainer(newcont);
                    GuiContext::getCurrent()->getSubjectProc()->addSubjectContainer(newcont, newcont->getName());
                  }
                else
                  {
                    if(cont != proc->containerMap[cont->getName()])
                      {
                        //the associated container is not the same as the one in containerMap: use the containerMap one
                        instance->setContainer(proc->containerMap[cont->getName()]);
                      }
                  }
            }
          subCompo = GuiContext::getCurrent()->getSubjectProc()->addSubjectComponent(instance);
        }
      else
        {
          DEBTRACE("SubjectServiceNode::setComponent : get already created subject for compo = " <<compo.c_str());
          subCompo = GuiContext::getCurrent()->_mapOfSubjectComponent[instance];
        }       
      YASSERT(subCompo);
      addSubjectReference(subCompo);
      if (_subRefComponent)
        subCompo->moveService(_subRefComponent);
      else
        _subRefComponent = subCompo->attachService(this);
    }
}

bool SubjectServiceNode::associateToComponent(SubjectComponent *subcomp)
{
  DEBTRACE("SubjectServiceNode::associateToComponent " << getName() << " " << subcomp->getName());
  SubjectReference* oldSReference = _subjectReference;
  string aName = GuiContext::getCurrent()->getProc()->getChildName(_serviceNode);
  CommandAssociateServiceToComponent *command =
    new CommandAssociateServiceToComponent(aName, subcomp->getName());
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
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

void SubjectPythonNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectInlineNode::clean(command);
}

void SubjectPythonNode::localclean(Command *command)
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

void SubjectPyFuncNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectInlineNode::clean(command);
}

void SubjectPyFuncNode::localclean(Command *command)
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

void SubjectCORBANode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectServiceNode::clean(command);
}

void SubjectCORBANode::localclean(Command *command)
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

void SubjectCppNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectServiceNode::clean(command);
}

void SubjectCppNode::localclean(Command *command)
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

void SubjectSalomeNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectServiceNode::clean(command);
}

void SubjectSalomeNode::localclean(Command *command)
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

void SubjectSalomePythonNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectServiceNode::clean(command);
}

void SubjectSalomePythonNode::localclean(Command *command)
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

void SubjectXmlNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectServiceNode::clean(command);
}

void SubjectXmlNode::localclean(Command *command)
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

void SubjectSplitterNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectElementaryNode::clean(command);
}

void SubjectSplitterNode::localclean(Command *command)
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

void SubjectDataNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectElementaryNode::clean(command);
}

void SubjectDataNode::localclean(Command *command)
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

void SubjectPresetNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectDataNode::clean(command);
}

void SubjectPresetNode::localclean(Command *command)
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

void SubjectOutNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectDataNode::clean(command);
}

void SubjectOutNode::localclean(Command *command)
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

void SubjectStudyInNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectDataNode::clean(command);
}

void SubjectStudyInNode::localclean(Command *command)
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

void SubjectStudyOutNode::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectDataNode::clean(command);
}

void SubjectStudyOutNode::localclean(Command *command)
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

void SubjectForLoop::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectComposedNode::clean(command);
}

void SubjectForLoop::localclean(Command *command)
{
  DEBTRACE("SubjectForLoop::localClean ");
  if (_body)
    erase(_body);
}

void SubjectForLoop::recursiveUpdate(GuiEvent event, int type, Subject* son)
{
  update(event, type, son);
  if (_body)
    _body->recursiveUpdate(event, type, son);
}

SubjectNode* SubjectForLoop::addNode(YACS::ENGINE::Catalog *catalog,
                                     std::string compo,
                                     std::string type,
                                     std::string name,
                                     bool newCompoInst)
{
  DEBTRACE("SubjectForLoop::addNode("<<catalog<<","<<compo<<","<<type<<","<<name<<")");
  SubjectNode* body = 0;
  if (_body)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "If you need several nodes in a loop, put the nodes in a bloc"; 
      return body;
    }
  body = createNode(catalog, compo, type, name, newCompoInst);
  return body;
}

void SubjectForLoop::houseKeepingAfterCutPaste(bool isCut, SubjectNode *son)
{
  if (isCut)
    _body = 0;
  else
    _body = son;
}

void SubjectForLoop::completeChildrenSubjectList(SubjectNode *son)
{
  _body = son;
}

bool SubjectForLoop::setNbSteps(std::string nbSteps)
{
  DEBTRACE("SubjectForLoop::setNbSteps " << nbSteps);
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetForLoopSteps *command =
    new CommandSetForLoopSteps(proc->getChildName(getNode()), nbSteps);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}
bool SubjectForLoop::hasValue()
{
  return true;
}

std::string SubjectForLoop::getValue()
{
  return _forLoop->edGetNbOfTimesInputPort()->getAsString();
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

void SubjectWhileLoop::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectComposedNode::clean(command);
}

void SubjectWhileLoop::localclean(Command *command)
{
  DEBTRACE("SubjectWhileLoop::localClean ");
  if (_body)
    erase(_body);
}

void SubjectWhileLoop::recursiveUpdate(GuiEvent event, int type, Subject* son)
{
  update(event, type, son);
  if (_body)
    _body->recursiveUpdate(event, type, son);
}

SubjectNode* SubjectWhileLoop::addNode(YACS::ENGINE::Catalog *catalog,
                                       std::string compo,
                                       std::string type,
                                       std::string name,
                                       bool newCompoInst)
{
  DEBTRACE("SubjectWhileLoop::addNode(catalog, compo, type, name)");
  SubjectNode* body = 0;
  if (_body)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "If you need several nodes in a loop, put the nodes in a bloc"; 
      return body;
    }
  body = createNode(catalog, compo, type, name, newCompoInst);
  return body;
}

void SubjectWhileLoop::houseKeepingAfterCutPaste(bool isCut, SubjectNode *son)
{
  if (isCut)
    _body = 0;
  else
    _body = son;
}

void SubjectWhileLoop::completeChildrenSubjectList(SubjectNode *son)
{
  _body = son;
}

bool SubjectWhileLoop::setCondition(std::string condition)
{
  DEBTRACE("SubjectWhileLoop::setCondition " << condition);
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetWhileCondition *command =
    new CommandSetWhileCondition(proc->getChildName(getNode()), condition);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

bool SubjectWhileLoop::hasValue()
{
  return true;
}

std::string SubjectWhileLoop::getValue()
{
  return _whileLoop->edGetConditionPort()->getAsString();
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

void SubjectSwitch::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectComposedNode::clean(command);
}

void SubjectSwitch::localclean(Command *command)
{
  DEBTRACE("SubjectSwitch::localClean ");
  map<int, SubjectNode*>::iterator it;
  map<int, SubjectNode*> bodyMapCpy = _bodyMap;
  for (it = bodyMapCpy.begin(); it != bodyMapCpy.end(); ++it)
    erase((*it).second);
}

void SubjectSwitch::recursiveUpdate(GuiEvent event, int type, Subject* son)
{
  update(event, type, son);
  map<int, SubjectNode*>::iterator it = _bodyMap.begin();
  for (; it != _bodyMap.end(); ++it)
    (*it).second->recursiveUpdate(event, type, son);
}

SubjectNode* SubjectSwitch::addNode(YACS::ENGINE::Catalog *catalog,
                                    std::string compo,
                                    std::string type,
                                    std::string name,
                                    bool newCompoInst,
                                    int swCase,
                                    bool replace)
{
  DEBTRACE("SubjectSwitch::addNode("<<catalog<<","<<compo<<","<<type<<","<<name<<","<<swCase<<","<<(int)replace<<")");
  SubjectNode* body = 0;
  if (!replace && _bodyMap.count(swCase))
    {
      GuiContext::getCurrent()->_lastErrorMessage = "If you need several nodes in a switch case, put the nodes in a bloc"; 
      return body;
    }
  body = createNode(catalog, compo, type, name, newCompoInst, swCase);
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

void SubjectSwitch::houseKeepingAfterCutPaste(bool isCut, SubjectNode *son)
{
  DEBTRACE("SubjectSwitch::houseKeepingAfterCutPaste");
  if (isCut)
    _bodyMap.erase(_switch->getRankOfNode(son->getNode()));
  else
    _bodyMap[_switch->getRankOfNode(son->getNode())] = son;
}


void SubjectSwitch::completeChildrenSubjectList(SubjectNode *son)
{
  DEBTRACE("SubjectSwitch::completeChildrenSubjectList");
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

bool SubjectSwitch::setSelect(std::string select)
{
  DEBTRACE("SubjectSwitch::setSelect " << select);
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetSwitchSelect *command =
    new CommandSetSwitchSelect(proc->getChildName(getNode()), select);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

bool SubjectSwitch::setCase(std::string caseId, SubjectNode* snode)
{
  DEBTRACE("SubjectSwitch::setCase " << caseId);
  Proc *proc = GuiContext::getCurrent()->getProc();

  Switch* aSwitch = dynamic_cast<Switch*>(getNode());
  Node* node =snode->getNode();
  int previousRank = aSwitch->getRankOfNode(node);
  int newRank = atoi(caseId.c_str());
  if (previousRank == newRank) return true; // nothing to do.

  CommandSetSwitchCase *command =
    new CommandSetSwitchCase(proc->getChildName(getNode()),
                             proc->getChildName(snode->getNode()),
                             caseId);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

bool SubjectSwitch::hasValue()
{
  return true;
}

std::string SubjectSwitch::getValue()
{
  return _switch->edGetConditionPort()->getAsString();
}

// ----------------------------------------------------------------------------

SubjectDynParaLoop::SubjectDynParaLoop(YACS::ENGINE::DynParaLoop * dynParaLoop, Subject * parent)
  : SubjectComposedNode(dynParaLoop, parent), _dynParaLoop(dynParaLoop)
{
  _subjectExecNode = NULL;
  _subjectInitNode = NULL;
  _subjectFinalizeNode = NULL;
}

SubjectDynParaLoop::~SubjectDynParaLoop()
{
  DEBTRACE("SubjectDynParaLoop::~SubjectDynParaLoop " << getName());
}

void SubjectDynParaLoop::recursiveUpdate(GuiEvent event, int type, Subject * son)
{
  update(event, type, son);
  if (_subjectExecNode)
    _subjectExecNode->recursiveUpdate(event, type, son);
  if (_subjectInitNode)
    _subjectInitNode->recursiveUpdate(event, type, son);
  if (_subjectFinalizeNode)
    _subjectFinalizeNode->recursiveUpdate(event, type, son);
}

SubjectNode * SubjectDynParaLoop::addNode(YACS::ENGINE::Catalog * catalog,
                                          std::string compo,
                                          std::string type,
                                          std::string name,
                                          bool newCompoInst)
{
  DEBTRACE("SubjectDynParaLoop::addNode(catalog, compo, type, name)");
  if (_subjectExecNode)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "If you need several nodes in a loop, "
                                                    "put the nodes in a bloc";
      return NULL;
    }
  return createNode(catalog, compo, type, name, newCompoInst);
}

void SubjectDynParaLoop::houseKeepingAfterCutPaste(bool isCut, SubjectNode * son)
{
  if (isCut)
    removeNode(son);
  else
    _subjectExecNode = son;
}

void SubjectDynParaLoop::clean(Command * command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectComposedNode::clean(command);
}

void SubjectDynParaLoop::localclean(Command * command)
{
  DEBTRACE("SubjectDynParaLoop::localClean ");
  if (_subjectExecNode)
    {
      DEBTRACE(_subjectExecNode->getName());
      erase(_subjectExecNode);
    }
  if (_subjectInitNode)
    {
      DEBTRACE(_subjectInitNode->getName());
      erase(_subjectInitNode);
    }
  if (_subjectFinalizeNode)
    {
      DEBTRACE(_subjectFinalizeNode->getName());
      erase(_subjectFinalizeNode);
    }
}

void SubjectDynParaLoop::completeChildrenSubjectList(SubjectNode * son)
{
  YASSERT(son);
  if (son->getNode() == _dynParaLoop->getExecNode())
    _subjectExecNode = son;
  else if (son->getNode() == _dynParaLoop->getInitNode())
    _subjectInitNode = son;
  else if (son->getNode() == _dynParaLoop->getFinalizeNode())
    _subjectFinalizeNode = son;
  else
    YASSERT(false);
}

void SubjectDynParaLoop::removeNode(SubjectNode * child)
{
  YASSERT(child);
  if (child == _subjectExecNode)
    _subjectExecNode = NULL;
  else if (child == _subjectInitNode)
    _subjectInitNode = NULL;
  else if (child == _subjectFinalizeNode)
    _subjectFinalizeNode = NULL;
  else
    YASSERT(false);
}

SubjectNode * SubjectDynParaLoop::getChild(YACS::ENGINE::Node * node) const
{
  return _subjectExecNode;
}

bool SubjectDynParaLoop::setNbBranches(std::string nbBranches)
{
  DEBTRACE("SubjectDynParaLoop::setNbBranches " << nbBranches);
  Proc * proc = GuiContext::getCurrent()->getProc();
  CommandSetForEachBranch * command =
    new CommandSetForEachBranch(proc->getChildName(getNode()), nbBranches);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

bool SubjectDynParaLoop::hasValue()
{
  return true;
}

std::string SubjectDynParaLoop::getValue()
{
  return _dynParaLoop->edGetNbOfBranchesPort()->getAsString();
}

// ----------------------------------------------------------------------------

SubjectForEachLoop::SubjectForEachLoop(YACS::ENGINE::ForEachLoop *forEachLoop, Subject *parent)
  : SubjectDynParaLoop(forEachLoop, parent), _forEachLoop(forEachLoop)
{
  _splitter = 0;
}

SubjectForEachLoop::~SubjectForEachLoop()
{
  DEBTRACE("SubjectForEachLoop::~SubjectForEachLoop " << getName());
}

void SubjectForEachLoop::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  Node* aSplitterEngine = 0;
  if (_splitter) aSplitterEngine = _splitter->getNode();

  localclean(command);
  SubjectDynParaLoop::clean(command);

  if (_forEachLoop && aSplitterEngine)
    {
      DEBTRACE("SubjectForEachLoop::clean: remove for each loop splitter");
      _forEachLoop->edRemoveChild(aSplitterEngine);
    }
}

void SubjectForEachLoop::localclean(Command *command)
{
  DEBTRACE("SubjectForEachLoop::localClean ");
  if (_splitter)
    {
      DEBTRACE(_splitter->getName());
      erase(_splitter);
    }
}

void SubjectForEachLoop::completeChildrenSubjectList(SubjectNode *son)
{
  if (son && son->getName() == ForEachLoop::NAME_OF_SPLITTERNODE)
    _splitter = son;
  else
    SubjectDynParaLoop::completeChildrenSubjectList(son);
}

void SubjectForEachLoop::removeNode(SubjectNode * child)
{
  YASSERT(child);
  if (child->getName() == ForEachLoop::NAME_OF_SPLITTERNODE)
    _splitter = NULL;
  else {
    SubjectDynParaLoop::removeNode(child);
  }
}

// ----------------------------------------------------------------------------

SubjectOptimizerLoop::SubjectOptimizerLoop(YACS::ENGINE::OptimizerLoop *optimizerLoop,
                                           Subject *parent)
  : SubjectDynParaLoop(optimizerLoop, parent), _optimizerLoop(optimizerLoop)
{
}

SubjectOptimizerLoop::~SubjectOptimizerLoop()
{
  DEBTRACE("SubjectOptimizerLoop::~SubjectOptimizerLoop " << getName());
}

void SubjectOptimizerLoop::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectDynParaLoop::clean(command);
}

void SubjectOptimizerLoop::localclean(Command *command)
{
  DEBTRACE("SubjectOptimizerLoop::localClean ");
}

bool SubjectOptimizerLoop::setAlgorithm(const std::string& alglib,const std::string& symbol)
{
  DEBTRACE("SubjectOptimizerLoop::setAlgorithm " << alglib << " " << symbol);
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetAlgo *command = new CommandSetAlgo(proc->getChildName(getNode()), alglib, symbol);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

// ----------------------------------------------------------------------------

SubjectDataPort::SubjectDataPort(YACS::ENGINE::DataPort* port, Subject *parent)
  : Subject(parent), _dataPort(port)
{
  _listSubjectLink.clear();
  _execValue = "";
}

SubjectDataPort::~SubjectDataPort()
{
  DEBTRACE("SubjectDataPort::~SubjectDataPort " << getName());
  if (isDestructible())
    {
      Node* node = _dataPort->getNode();
      YASSERT(node);
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

void SubjectDataPort::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  Subject::clean(command);
}

void SubjectDataPort::localclean(Command *command)
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

bool SubjectDataPort::setName(std::string name)
{
  DEBTRACE("SubjectDataPort::setName " << name);
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  Node *node = getPort()->getNode();
  if (proc != dynamic_cast<Proc*>(node))
    position = proc->getChildName(node);
  else
    position = node->getName();

  Command *command = 0;
  bool isIn = dynamic_cast<InPort*>(_dataPort);
  if (isIn) 
    command = new CommandRenameInDataPort(position, _dataPort->getName(),name, getType());
  else 
    command = new CommandRenameOutDataPort(position, _dataPort->getName(),name, getType());

  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

YACS::ENGINE::DataPort* SubjectDataPort::getPort()
{
  return _dataPort;
}

bool SubjectDataPort::tryCreateLink(SubjectDataPort *subOutport, SubjectDataPort *subInport,bool control)
{
  DEBTRACE("SubjectDataPort::tryCreateLink");

  InPort *inp = dynamic_cast<InPort*>(subInport->getPort());
  OutPort *outp = dynamic_cast<OutPort*>(subOutport->getPort());
  if (outp && outp->isAlreadyLinkedWith(inp))
    {
      DEBTRACE("isAlreadyLinkedWith");
      GuiContext::getCurrent()->_lastErrorMessage = "Ports are already linked"; 
      return false;
    }

  Proc *proc = GuiContext::getCurrent()->getProc();

  string outNodePos = "";
  SubjectNode *sno = dynamic_cast<SubjectNode*>(subOutport->getParent());
  YASSERT(sno);
  Node *outNode = sno->getNode();
  outNodePos = proc->getChildName(outNode);
  string outportName = subOutport->getName();

  string inNodePos = "";
  SubjectNode *sni = dynamic_cast<SubjectNode*>(subInport->getParent());
  YASSERT(sni);
  Node *inNode = sni->getNode();
  inNodePos = proc->getChildName(inNode);
  string inportName = subInport->getName();

  CommandAddLink *command = new CommandAddLink(outNodePos, outportName, subOutport->getType(),
                                               inNodePos, inportName, subInport->getType(), control);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else
    {
      delete command;
      return false;
    }
}

/*! Generic method do nothing.
 *  Implementation in SubjectInputPort and SubjectOutputPort.
 */
bool SubjectDataPort::setValue(std::string value)
{
  return false;
}

void SubjectDataPort::setExecValue(std::string value)
{
  _execValue = value;
}

std::string SubjectDataPort::getExecValue()
{
  return _execValue;
}

void SubjectDataPort::registerUndoDestroy()
{
  DEBTRACE("SubjectDataPort::registerUndoDestroy");
  Proc *proc = GuiContext::getCurrent()->getProc();
  SubjectNode *sno = dynamic_cast<SubjectNode*>(getParent());
  YASSERT(sno);
  Node *node = sno->getNode();
  string nodeName = proc->getChildName(node);
  string portName = getName();
  ElementaryNode* father = dynamic_cast<ElementaryNode*>(node);
  YASSERT(father);

  DataPort *port = getPort();
  TypeCode *dtyp = port->edGetType();
  string typeName = dtyp->name();
  Catalog *undoCata = GuiContext::getCurrent()->getInvoc()->getUndoCata();
  undoCata->_typeMap[typeName] = dtyp;

  Command *command = 0;
  Command *comm2 =0;
  TypeOfElem elemType = getType();
  switch (elemType)
    {
    case INPUTPORT:
      command = new CommandAddInputPortFromCatalog(undoCata, typeName, nodeName, portName);
      {
        int rang = 1;
        int nbUp = 0;
        list<InputPort*> plist = father->getSetOfInputPort();
        list<InputPort*>::iterator pos = plist.begin();
        for (; (*pos) != port; pos++)
          rang++;
        nbUp = plist.size() -rang;
        if (nbUp)
          comm2 = new CommandOrderInputPorts(nodeName, portName, nbUp);
      }
      break;
    case OUTPUTPORT:
      command = new CommandAddOutputPortFromCatalog(undoCata, typeName, nodeName, portName);
      {
        int rang = 1;
        int nbUp = 0;
        list<OutputPort*> plist = father->getSetOfOutputPort();
        list<OutputPort*>::iterator pos = plist.begin();
        for (; (*pos) != port; pos++)
          rang++;
        nbUp = plist.size() -rang;
        if (nbUp)
          comm2 = new CommandOrderOutputPorts(nodeName, portName, nbUp);
      }
      break;
    case INPUTDATASTREAMPORT:
      command = new CommandAddIDSPortFromCatalog(undoCata, typeName, nodeName, portName);
      break;
    case OUTPUTDATASTREAMPORT:
      command = new CommandAddODSPortFromCatalog(undoCata, typeName, nodeName, portName);
      break;
    }
  if (command)
    GuiContext::getCurrent()->getInvoc()->add(command);
  if (comm2)
    GuiContext::getCurrent()->getInvoc()->add(comm2);
  //save links
  list<SubjectLink*> lsl = getListOfSubjectLink();
  for (list<SubjectLink*>::iterator it = lsl.begin(); it != lsl.end(); ++it)
    {
      (*it)->registerUndoDestroy();
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

void SubjectInputPort::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectDataPort::clean(command);
}

void SubjectInputPort::localclean(Command *command)
{
  DEBTRACE("SubjectInputPort::localClean ");
  if (_parent)
    {
      SubjectElementaryNode* elem = dynamic_cast<SubjectElementaryNode*>(_parent);
      if (elem) elem->removePort(this);
    }
}

bool SubjectInputPort::setValue(std::string value)
{
  DEBTRACE("SubjectInputPort::setValue " << value);
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetInPortValue *command =
    new CommandSetInPortValue(proc->getChildName(getPort()->getNode()), getName(), value);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

// ----------------------------------------------------------------------------

SubjectOutputPort::SubjectOutputPort(YACS::ENGINE::OutputPort *port, Subject *parent)
  : SubjectDataPort(port, parent), _outputPort(port)
{
  if (DynParaLoop* dynloop = dynamic_cast<DynParaLoop*>(_outputPort->getNode()))
    {
      if (_outputPort == dynloop->edGetSamplePort()) _destructible = false;
    }
}

SubjectOutputPort::~SubjectOutputPort()
{
  DEBTRACE("SubjectOutputPort::~SubjectOutputPort " << getName());
}

void SubjectOutputPort::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectDataPort::clean(command);
}

void SubjectOutputPort::localclean(Command *command)
{
  DEBTRACE("SubjectOutputPort::localClean ");
  if (_parent)
    {
      SubjectElementaryNode* elem = dynamic_cast<SubjectElementaryNode*>(_parent);
      if (elem) elem->removePort(this);
    }
}

bool SubjectOutputPort::setValue(std::string value)
{
  DEBTRACE("SubjectOutputPort::setValue " << value);
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetOutPortValue *command =
    new CommandSetOutPortValue(proc->getChildName(getPort()->getNode()), getName(), value);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
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
  return _inputDataStreamPort->getProperties();
}

std::vector<std::string> SubjectInputDataStreamPort::knownProperties()
{
  std::vector<std::string> props;
  props.push_back("StorageLevel");
  //props.push_back("DependencyType");
  props.push_back("DateCalSchem");
  props.push_back("Alpha");
  props.push_back("DeltaT");
  props.push_back("InterpolationSchem");
  props.push_back("ExtrapolationSchem");
  return props;
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

void SubjectInputDataStreamPort::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectDataPort::clean(command);
}

void SubjectInputDataStreamPort::localclean(Command *command)
{
  DEBTRACE("SubjectInputDataStreamPort::localClean ");
}

void SubjectInputDataStreamPort::registerUndoDestroy()
{
  DEBTRACE("SubjectInputDataStreamPort::registerUndoDestroy");
  SubjectDataPort::registerUndoDestroy();
  map<string,string> properties = getProperties();
  if (properties.empty())
    return;
  Proc *proc = GuiContext::getCurrent()->getProc();
  SubjectNode *sno = dynamic_cast<SubjectNode*>(getParent());
  YASSERT(sno);
  Node *node = sno->getNode();
  string nodeName = proc->getChildName(node);
  string portName = getName();
  Command *command = new CommandSetDSPortProperties(nodeName, portName, true, properties);
  GuiContext::getCurrent()->getInvoc()->add(command);
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
  return _outputDataStreamPort->getProperties();
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

void SubjectOutputDataStreamPort::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  SubjectDataPort::clean(command);
}

void SubjectOutputDataStreamPort::localclean(Command *command)
{
  DEBTRACE("SubjectOutputDataStreamPort::localClean ");
}

void SubjectOutputDataStreamPort::registerUndoDestroy()
{
  DEBTRACE("SubjectOutputDataStreamPort::registerUndoDestroy");
  SubjectDataPort::registerUndoDestroy();
  map<string,string> properties = getProperties();
  if (properties.empty())
    return;
  Proc *proc = GuiContext::getCurrent()->getProc();
  SubjectNode *sno = dynamic_cast<SubjectNode*>(getParent());
  YASSERT(sno);
  Node *node = sno->getNode();
  string nodeName = proc->getChildName(node);
  string portName = getName();
  Command *command = new CommandSetDSPortProperties(nodeName, portName, false, properties);
  GuiContext::getCurrent()->getInvoc()->add(command);
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
  _inPort->update(UPDATE,DATALINK,_outPort);
  _outPort->update(UPDATE,DATALINK,_inPort);
}

void SubjectLink::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  Subject::clean(command);
}

void SubjectLink::localclean(Command *command)
{
  DEBTRACE("SubjectLink::localClean ");
  if (_parent)
    {
      DEBTRACE("clean link: " << _parent->getName() << " " << getName());
      SubjectComposedNode* father = dynamic_cast<SubjectComposedNode*>(_parent);
      YASSERT(father);
      father->removeLink(this); // --- clean subjects first
      _cla = dynamic_cast<ComposedNode*>(father->getNode());
      YASSERT(_cla);
      _outp = dynamic_cast<OutPort*>(_outPort->getPort());
      YASSERT(_outp);
      _inp = dynamic_cast<InPort*>(_inPort->getPort());
      YASSERT(_inp);
      if (isDestructible())
        _cla->edRemoveLink(_outp, _inp);
    }
}

std::string SubjectLink::getName()
{
  return _name;
}

std::map<std::string, std::string> SubjectLink::getProperties()
{
  return getSubjectInPort()->getProperties();
}

std::vector<std::string> SubjectLink::knownProperties()
{
  return getSubjectInPort()->knownProperties();
}

bool SubjectLink::setProperties(std::map<std::string, std::string> properties)
{
  Proc *proc = GuiContext::getCurrent()->getProc();
  CommandSetLinkProperties *command =
    new CommandSetLinkProperties(proc->getChildName(getSubjectOutPort()->getPort()->getNode()), getSubjectOutPort()->getName(), 
                                 proc->getChildName(getSubjectInPort()->getPort()->getNode()), getSubjectInPort()->getName(), 
                                 properties);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

void SubjectLink::registerUndoDestroy()
{
  DEBTRACE("SubjectLink::registerUndoDestroy");
  Proc *proc = GuiContext::getCurrent()->getProc();
  Node *outn = getSubjectOutNode()->getNode();
  Node *inn = getSubjectInNode()->getNode();
  DataPort *outp = getSubjectOutPort()->getPort();
  DataPort *inp = getSubjectInPort()->getPort();
  CommandAddLink *command = new CommandAddLink(proc->getChildName(outn),
                                               outp->getName(),
                                               getSubjectOutPort()->getType(),
                                               proc->getChildName(inn),
                                               inp->getName(),
                                               getSubjectInPort()->getType(),
                                               false);
  GuiContext::getCurrent()->getInvoc()->add(command);

  map<string, string> properties = getProperties();
  if (properties.empty())
    return;

  CommandSetLinkProperties *command2 = new CommandSetLinkProperties(proc->getChildName(outn),
                                                                    outp->getName(),
                                                                    proc->getChildName(inn),
                                                                    inp->getName(),
                                                                    properties);
  GuiContext::getCurrent()->getInvoc()->add(command2); 
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

void SubjectControlLink::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  Subject::clean(command);
}

void SubjectControlLink::localclean(Command *command)
{
  DEBTRACE("SubjectControlLink::localClean ");
  if (_parent)
    {
      DEBTRACE("clean control link: " << _parent->getName() << " " << getName());
      SubjectComposedNode* father = dynamic_cast<SubjectComposedNode*>(_parent);
      YASSERT(father);
      father->removeControlLink(this); // --- clean subjects first
      _cla = dynamic_cast<ComposedNode*>(father->getNode());
      YASSERT(_cla);
    }
}

std::string SubjectControlLink::getName()
{
  return _name;
}

void SubjectControlLink::registerUndoDestroy()
{
  DEBTRACE("SubjectControlLink::registerUndoDestroy");
  Proc *proc = GuiContext::getCurrent()->getProc();
  Node *outn = getSubjectOutNode()->getNode();
  Node *inn = getSubjectInNode()->getNode();
  CommandAddControlLink *command = new CommandAddControlLink(proc->getChildName(outn),
                                                             proc->getChildName(inn));
  GuiContext::getCurrent()->getInvoc()->add(command);
}

// ----------------------------------------------------------------------------

SubjectComponent::SubjectComponent(YACS::ENGINE::ComponentInstance* component, Subject *parent)
  : Subject(parent), _compoInst(component)
{
  _compoInst->incrRef();
  _subRefContainer = 0;
  _subServiceSet.clear();
  _subReferenceMap.clear();
}

SubjectComponent::~SubjectComponent()
{
  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( aProc )
  {
    aProc->componentInstanceMap.erase(_compoInst->getInstanceName());
    GuiContext::getCurrent()->_mapOfSubjectComponent.erase(_compoInst);
  }
  _compoInst->decrRef();
}

void SubjectComponent::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  Subject::clean(command);
}

void SubjectComponent::localclean(Command *command)
{
  DEBTRACE("SubjectComponent::localClean " << this);
  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( aProc )
  {
    std::map<Node*, SubjectNode*>::iterator it = GuiContext::getCurrent()->_mapOfSubjectNode.begin();
    std::list<SubjectNode*> services;
    for ( ; it!=GuiContext::getCurrent()->_mapOfSubjectNode.end(); it++ )
      {
        if(ServiceNode* service=dynamic_cast<ServiceNode*>((*it).first))
          {
            if ( service->getComponent() == _compoInst )
              {
                services.push_back((*it).second);
              }
          }
      }
    while(!services.empty())
      {
        SubjectNode* son=services.front();
        services.pop_front();
        Subject* parent=son->getParent();
//         parent->update(REMOVE,son->getType(),son);
        parent->erase(son);
        parent->update(REMOVE,0,0);
      }
      
    Container* container = _compoInst->getContainer();
    if (!container) return;
    SubjectContainerBase *subContainer(0);
    YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(container));
    subContainer = GuiContext::getCurrent()->_mapOfSubjectContainer[container];
    subContainer->removeSubComponentFromSet(this);
  }
}

std::string SubjectComponent::getName()
{
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
      SubjectContainerBase *subContainer;
      if (GuiContext::getCurrent()->_mapOfSubjectContainer.count(container))
        subContainer = GuiContext::getCurrent()->_mapOfSubjectContainer[container];
      else
        subContainer = 
          GuiContext::getCurrent()->getSubjectProc()->addSubjectContainer(container, container->getName());
      addSubjectReference(subContainer);
      if (_subRefContainer)
        subContainer->moveComponent(_subRefContainer);
      else
        _subRefContainer = subContainer->attachComponent(this);
      notifyServicesChange(ASSOCIATE, CONTAINER, subContainer);
    }
}

bool SubjectComponent::associateToContainer(SubjectContainerBase *subcont)
{
  DEBTRACE("SubjectComponent::associateToContainer " << getName() << " " << subcont->getName());
  CommandAssociateComponentToContainer *command =
    new CommandAssociateComponentToContainer(_compoInst->getInstanceName(), subcont->getName());
  if (command->execute())
    {
      if (!GuiContext::getCurrent()->isLoading()) // do not register command when loading a schema
        GuiContext::getCurrent()->getInvoc()->add(command);
      else delete command;
      return true;
    }
  else delete command;
  return false;
}

SubjectReference* SubjectComponent::attachService(SubjectServiceNode* service)
{
  DEBTRACE("SubjectComponent::attachService");
  SubjectReference *son = new SubjectReference(service, this);
  _subServiceSet.insert(service);
  _subReferenceMap[service] = son;
  update(ADDCHILDREF, SALOMENODE, son);
  return son;
}

void SubjectComponent::detachService(SubjectServiceNode* service)
{
  DEBTRACE("SubjectComponent::detachService");
  YASSERT(_subReferenceMap.count(service));
  SubjectReference *reference = _subReferenceMap[service];
  update(REMOVECHILDREF, SALOMENODE, reference);
  _subServiceSet.erase(service);
  _subReferenceMap.erase(service);
  service->removeSubRefComponent();
  erase(reference);
}

void SubjectComponent::moveService(SubjectReference* reference)
{
  DEBTRACE("SubjectComponent::moveService");
  SubjectComponent* oldcomp = dynamic_cast<SubjectComponent*>(reference->getParent());
  YASSERT(oldcomp);

  SubjectServiceNode* service = dynamic_cast<SubjectServiceNode*>(reference->getReference());
  oldcomp->removeSubServiceFromSet(service);
  _subServiceSet.insert(service);
  _subReferenceMap[service] = reference;
  oldcomp->update(CUT, SALOMENODE, reference);
  reference->reparent(this);
  update(PASTE, SALOMENODE, reference);
  if (!oldcomp->hasServices())
    Subject::erase(oldcomp);
}

void SubjectComponent::removeSubServiceFromSet(SubjectServiceNode *service)
{
  DEBTRACE("SubjectComponent::removeSubServiceFromSet " << getName() << " " << service->getName());
  _subServiceSet.erase(service);
  _subReferenceMap.erase(service);
}

void SubjectComponent::notifyServicesChange(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SubjectComponent::notifyServicesChange");
  set<SubjectServiceNode*>::iterator it = _subServiceSet.begin();
  for(; it != _subServiceSet.end(); ++it)
    {
      (*it)->update(event, type, son);
    }
}

bool SubjectComponent::setProperties(std::map<std::string, std::string> properties)
{
  CommandSetComponentInstanceProperties *command = new CommandSetComponentInstanceProperties(getName(), properties);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

std::map<std::string, std::string> SubjectComponent::getProperties()
{
  return _compoInst->getProperties();
}

// ----------------------------------------------------------------------------

SubjectContainerBase *SubjectContainerBase::New(YACS::ENGINE::Container* container, Subject *parent)
{
  if(!container)
    return 0;
  if(!dynamic_cast<YACS::ENGINE::HomogeneousPoolContainer *>(container))
    return new SubjectContainer(container,parent);
  else
    return new SubjectHPContainer(dynamic_cast<YACS::ENGINE::HomogeneousPoolContainer *>(container),parent);
}

SubjectContainerBase::SubjectContainerBase(YACS::ENGINE::Container* container, Subject *parent):Subject(parent), _container(container)
{
  _subComponentSet.clear();
  _subReferenceMap.clear();
}

SubjectContainerBase::~SubjectContainerBase()
{
  DEBTRACE("SubjectContainerBase::~SubjectContainerBase");
  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( aProc )
  {
    map<ComponentInstance*,SubjectComponent*> mapOfSubjectComponentCpy
      = GuiContext::getCurrent()->_mapOfSubjectComponent;
    map<ComponentInstance*,SubjectComponent*>::iterator it = mapOfSubjectComponentCpy.begin();
    for ( ; it!=mapOfSubjectComponentCpy.end(); it++ )
      if ( (*it).first && (*it).first->getContainer() == _container )
      {
        (*it).first->setContainer(0);
        GuiContext::getCurrent()->getSubjectProc()->destroy((*it).second);
      }

    GuiContext::getCurrent()->_mapOfSubjectContainer.erase(_container);
    aProc->removeContainer(_container);
  }
}

std::map<std::string, std::string> SubjectContainerBase::getProperties()
{
  return _container->getProperties();
}

bool SubjectContainerBase::setProperties(std::map<std::string, std::string> properties)
{
  CommandSetContainerProperties *command(new CommandSetContainerProperties(getName(), properties));
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else
    delete command;
  return false;
}

bool SubjectContainerBase::setName(std::string name)
{
  DEBTRACE("SubjectContainer::setName " << name);
  if (name == getName())
    return true;
  CommandRenameContainer* command = new CommandRenameContainer(getName(), name);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      return true;
    }
  else delete command;
  return false;
}

SubjectReference* SubjectContainerBase::attachComponent(SubjectComponent* component)
{
  DEBTRACE("SubjectContainer::attachComponent");
  SubjectReference *son = new SubjectReference(component, this);
  _subComponentSet.insert(component);
  _subReferenceMap[component] = son;
  update(ADDCHILDREF, COMPONENT, son);
  return son;
}

void SubjectContainerBase::detachComponent(SubjectComponent* component)
{
  DEBTRACE("SubjectContainer::detachComponent");
  YASSERT(_subReferenceMap.count(component));
  SubjectReference *reference = _subReferenceMap[component];
  update(REMOVECHILDREF, PYTHONNODE, reference);
  _subComponentSet.erase(component);
  _subReferenceMap.erase(component);
  erase(reference);
}

void SubjectContainerBase::moveComponent(SubjectReference* reference)
{
  DEBTRACE("SubjectContainer::moveComponent");
  SubjectContainer* oldcont = dynamic_cast<SubjectContainer*>(reference->getParent());
  YASSERT(oldcont);
  SubjectComponent* component = dynamic_cast<SubjectComponent*>(reference->getReference());
  _subComponentSet.insert(component);
  _subReferenceMap[component] = reference;
  oldcont->removeSubComponentFromSet(component);
  oldcont->update(CUT, COMPONENT, reference);
  reference->reparent(this);
  update(PASTE, COMPONENT, reference);
}

void SubjectContainerBase::removeSubComponentFromSet(SubjectComponent *component)
{
  DEBTRACE("SubjectContainer::removeSubComponentFromSet");
  _subComponentSet.erase(component);
  _subReferenceMap.erase(component);
}

void SubjectContainerBase::notifyComponentsChange(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SubjectContainer::notifyComponentsChange");
  set<SubjectComponent*>::iterator it = _subComponentSet.begin();
  for(; it != _subComponentSet.end(); ++it)
    {
      (*it)->update(event, type, son);
      (*it)->notifyServicesChange(event, type, son);
    }
}

void SubjectContainerBase::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  Subject::clean(command);
}

void SubjectContainerBase::localclean(Command *command)
{
  DEBTRACE("SubjectContainerBase::localClean ");
  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( aProc )
  {
    SubjectComponent* compo;
    map<ComponentInstance*,SubjectComponent*>::iterator it = GuiContext::getCurrent()->_mapOfSubjectComponent.begin();
    std::list<SubjectComponent*> compos;
    for ( ; it!=GuiContext::getCurrent()->_mapOfSubjectComponent.end(); it++ )
      if ( (*it).first && (*it).first->getContainer() == _container )
      {
        compo=(*it).second;
        (*it).first->setContainer(0);
        compos.push_back((*it).second);
      }
    while(!compos.empty())
      {
        compo=compos.front();
        compos.pop_front();
        GuiContext::getCurrent()->getSubjectProc()->update(REMOVE,compo->getType(),compo);
        GuiContext::getCurrent()->getSubjectProc()->erase(compo);
      }
  }
}

std::string SubjectContainerBase::getName()
{
  return _container->getName();
}

// ----------------------------------------------------------------------------

SubjectContainer::SubjectContainer(YACS::ENGINE::Container *container, Subject *parent):SubjectContainerBase(container,parent)
{
}

void SubjectContainer::registerUndoDestroy()
{
  DEBTRACE("SubjectContainer::registerUndoDestroy");
  Command *command = new CommandAddContainer(getName(),"");
  GuiContext::getCurrent()->getInvoc()->add(command);
}

// ----------------------------------------------------------------------------

SubjectHPContainer::SubjectHPContainer(YACS::ENGINE::HomogeneousPoolContainer *container, Subject *parent):SubjectContainerBase(container,parent)
{
}

YACS::ENGINE::Container *SubjectHPContainer::getContainer() const
{
  if(!_container)
    return 0;
  else
    {
      YACS::ENGINE::HomogeneousPoolContainer *ret(dynamic_cast<YACS::ENGINE::HomogeneousPoolContainer *>(_container));
      if(!ret)
        throw Exception("Invalid container type in SubjectHPContainer !");
      return ret;
    }
}

void SubjectHPContainer::registerUndoDestroy()
{
  DEBTRACE("SubjectHPContainer::registerUndoDestroy");
  Command *command = new CommandAddHPContainer(getName(),"");
  GuiContext::getCurrent()->getInvoc()->add(command);
}

// ----------------------------------------------------------------------------

SubjectDataType::SubjectDataType(YACS::ENGINE::TypeCode *typeCode, Subject *parent, std::string alias)
  : Subject(parent), _typeCode(typeCode), _alias(alias)
{
}

SubjectDataType::~SubjectDataType()
{
}

void SubjectDataType::clean(Command *command)
{
  if (_askRegisterUndo)
    {
      _askRegisterUndo = false;
      registerUndoDestroy();
    }
  localclean(command);
  Subject::clean(command);
}

void SubjectDataType::localclean(Command *command)
{
  DEBTRACE("SubjectDataType::localClean ");
}

std::string SubjectDataType::getName()
{
  return _typeCode->name();
}

std::string SubjectDataType::getAlias()
{
  return _alias;
}

YACS::ENGINE::TypeCode* SubjectDataType::getTypeCode()
{
  return _typeCode;
}

// ----------------------------------------------------------------------------
