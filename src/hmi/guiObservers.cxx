
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
#include "XMLNode.hxx"
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

#include "guiContext.hxx"

#include <string>
#include <sstream>
#include <vector>

#define _DEVDEBUG_
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

Subject::~Subject()
{
  DEBTRACE("Subject::~Subject " << getName());
}

void Subject::clean()
{
  localClean();
}

void Subject::localClean()
{
  DEBTRACE("Subject::localClean ");
  set<GuiObserver*>::iterator it;
  set<GuiObserver*> copySet = _setObs;
  for (it = copySet.begin(); it != copySet.end(); ++it)
    delete (*it) ;
}

void Subject::attach(GuiObserver *obs)
{
  _setObs.insert(obs);
}

void Subject::detach(GuiObserver *obs)
{
  DEBTRACE("Subject::detach");
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
  for (set<GuiObserver *>::iterator it = _setObs.begin(); it != _setObs.end(); ++it)
    {
      (*it)->select(isSelected);
    }
}

void Subject::update(GuiEvent event,int type, Subject* son)
{
  for (set<GuiObserver *>::iterator it = _setObs.begin(); it != _setObs.end(); ++it)
    {
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
    }
  return false;
}

void Subject::loadChildren()
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
}

GuiObserver::~GuiObserver()
{
  DEBTRACE("GuiObserver::~GuiObserver");
}

void GuiObserver::select(bool isSelected)
{
  DEBTRACE("GuiObserver::select() " << isSelected);
}

void GuiObserver::update(GuiEvent event, int type,  Subject* son)
{
  DEBTRACE("GuiObserver::update, event not handled");
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

// ----------------------------------------------------------------------------

SubjectNode::SubjectNode(YACS::ENGINE::Node *node, Subject *parent)
  : Subject(parent), _node(node)
{
  _listSubjectInputPort.clear();
  _listSubjectOutputPort.clear();
  _listSubjectIDSPort.clear();
  _listSubjectODSPort.clear();
  _listSubjectLink.clear();
}

/*!
 * all destruction is done in generic class SubjectNode
 */
SubjectNode::~SubjectNode()
{
  DEBTRACE("SubjectNode::~SubjectNode " << getName());
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
                Switch *aSwitch = dynamic_cast<Switch*>(father);
                if (aSwitch) aSwitch->edRemoveChild(_node);
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
      SubjectBloc* sb = dynamic_cast<SubjectBloc*>(_parent);
      if (sb) sb->removeNode(this);
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
  GuiContext::getCurrent()->getSubjectProc()->addSubjectDataType(typcod);
  return son;
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

bool SubjectComposedNode::addNode(YACS::ENGINE::Catalog *catalog,
                                  std::string compo,
                                  std::string type,
                                  std::string name)
{
  DEBTRACE("SubjectComposedNode::addNode("<<catalog<<","<<compo<<","<<type<<","<<name<<")");
  return false;
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
      return son;
    }
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
      assert(0);
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
  set<Node *> setOfNode= _composedNode->edGetDirectDescendants();
  if (ForEachLoop *feloop = dynamic_cast<ForEachLoop*>(_composedNode))
    setOfNode.insert(feloop->getChildByName(ForEachLoop::NAME_OF_SPLITTERNODE));
  for(set<Node *>::iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      SubjectNode * son = addSubjectNode(*iter);
      son->loadChildren();
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
  SubjectLink *son = new SubjectLink(sno, spo, sni, spi, this);
  OutPort *outp = sno->getNode()->getOutPort(spo->getName());
  InPort *inp = sni->getNode()->getInPort(spi->getName());
  pair<OutPort*,InPort*> keyLink(outp,inp);
  GuiContext::getCurrent()->_mapOfSubjectLink[keyLink] = son;
  _listSubjectLink.push_back(son);
  spo->addSubjectLink(son);
  spi->addSubjectLink(son);
  update(ADDLINK, 0, son);
  DEBTRACE("addSubjectLink: " << getName() << " " << son->getName());
  return son;
}

void SubjectComposedNode::removeLink(SubjectLink* link)
{
  link->getSubjectOutPort()->removeSubjectLink(link);
  link->getSubjectInPort()->removeSubjectLink(link);
  _listSubjectLink.remove(link);
}

void SubjectComposedNode::loadLinks()
{
  set<Node *> setOfNode= _composedNode->edGetDirectDescendants();
  for(set<Node *>::iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
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

bool SubjectBloc::addNode(YACS::ENGINE::Catalog *catalog,
                          std::string compo,
                          std::string type,
                          std::string name)
{
  DEBTRACE("SubjectBloc::addNode( " << catalog << ", " << compo << ", " << type << ", " << name << " )");
  SubjectNode* child = createNode(catalog, compo, type, name);
  if (child) return true;
  return false;
}

void SubjectBloc::completeChildrenSubjectList(SubjectNode *son)
{
  _children.insert(son);
}

void SubjectBloc::removeNode(SubjectNode* child)
{
  _children.erase(child);
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
}

bool SubjectProc::addComponent(std::string name)
{
  DEBTRACE("SubjectProc::addComponent " << name);
  int id = GuiContext::getCurrent()->getNewId();
  pair<string,int> key = pair<string,int>(name,id);
  if (! GuiContext::getCurrent()->getProc()->componentInstanceMap.count(key))
    {
      CommandAddComponentInstance *command = new CommandAddComponentInstance(key);
      if (command->execute())
        {
          GuiContext::getCurrent()->getInvoc()->add(command);
          ComponentInstance *compo = command->getComponentInstance();
          SubjectComponent *son = addSubjectComponent(compo, key);
          GuiContext::getCurrent()->getProc()->componentInstanceMap[key] = compo;
          return son;
        }
    }
  return 0;
}

bool SubjectProc::addContainer(std::string name, std::string ref)
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
    }
  return 0;
}

bool SubjectProc::addDataType(YACS::ENGINE::Catalog* catalog, std::string typeName)
{
  DEBTRACE("SubjectProc::addDataType " << typeName);
  CommandAddDataTypeFromCatalog *command = new CommandAddDataTypeFromCatalog(catalog, typeName);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      SubjectDataType *son = addSubjectDataType(command->getTypeCode());
      return son;
    }
  return 0;
}

SubjectComponent* SubjectProc::addSubjectComponent(YACS::ENGINE::ComponentInstance* compo,
                                                   std::pair<std::string,int> key)
{
  DEBTRACE("SubjectProc::addSubjectComponent " << key.first << " " << key.second);
  SubjectComponent *son = new SubjectComponent(compo, key.second, this);
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
    {
      proc->typeMap[typeName] = type->clone();
      son = new SubjectDataType(typeName, this);
      update(ADD, DATATYPE, son);
    }
  return son;
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


bool SubjectElementaryNode::addInputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
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
  return 0;
}

bool SubjectElementaryNode::addOutputPort(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
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
          SubjectComponent* subCompo = 0;
          int id = GuiContext::getCurrent()->getNewId();
          pair<string,int> key = pair<string,int>(compo,id);
          if (! proc->componentInstanceMap.count(key))
            {
              instance = new SalomeComponent(compo);
              proc->componentInstanceMap[key] = instance;
              subCompo =
                GuiContext::getCurrent()->getSubjectProc()->addSubjectComponent(instance, key);
            }
          else
            {
              instance = proc->componentInstanceMap[key];
              subCompo = GuiContext::getCurrent()->_mapOfSubjectComponent[instance];
            }
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
      string compo = instance->getName();
      SubjectComponent* subCompo = 0;
      int id = GuiContext::getCurrent()->getNewId();
      pair<string,int> key = pair<string,int>(compo,id);
      if (! GuiContext::getCurrent()->_mapOfSubjectComponent.count(instance))
        {
          proc->componentInstanceMap[key] = instance;
          subCompo =
            GuiContext::getCurrent()->getSubjectProc()->addSubjectComponent(instance, key);
        }
      else
        {
          subCompo = GuiContext::getCurrent()->_mapOfSubjectComponent[instance];
        }       
      assert(subCompo);
      addSubjectReference(subCompo);
    }
}

void SubjectServiceNode::associateToComponent(SubjectComponent *subcomp)
{
  DEBTRACE("SubjectServiceNode::associateToComponent " << getName() << " " << subcomp->getName())
  CommandAssociateServiceToComponent *command =
    new CommandAssociateServiceToComponent(getName(), subcomp->getKey());
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      addSubjectReference(subcomp);
    }
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


bool SubjectForLoop::addNode(YACS::ENGINE::Catalog *catalog,
                             std::string compo,
                             std::string type,
                             std::string name)
{
  DEBTRACE("SubjectForLoop::addNode(catalog, compo, type, name)");
  if (_body) return false;
  SubjectNode* body = createNode(catalog, compo, type, name);
  if (body) return true;
  return false;
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

bool SubjectWhileLoop::addNode(YACS::ENGINE::Catalog *catalog,
                               std::string compo,
                               std::string type,
                               std::string name)
{
  DEBTRACE("SubjectWhileLoop::addNode(catalog, compo, type, name)");
  if (_body) return false;
  SubjectNode* body = createNode(catalog, compo, type, name);
  if (body) return true;
  return false;
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

bool SubjectSwitch::addNode(YACS::ENGINE::Catalog *catalog,
                            std::string compo,
                            std::string type,
                            std::string name,
                            int swCase)
{
  DEBTRACE("SubjectSwitch::addNode("<<catalog<<","<<compo<<","<<type<<","<<name<<","<<swCase<<")");
  if (_bodyMap.count(swCase)) return false;
  SubjectNode* body = createNode(catalog, compo, type, name, swCase);
  if(body) return true;
  return false;
}

std::map<int, SubjectNode*> SubjectSwitch::getBodyMap()
{
  return _bodyMap;
}

void SubjectSwitch::completeChildrenSubjectList(SubjectNode *son)
{
  _bodyMap[_switch->getRankOfNode(son->getNode())] = son;
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
  localClean();
  SubjectComposedNode::clean();
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


bool SubjectForEachLoop::addNode(YACS::ENGINE::Catalog *catalog,
                                 std::string compo,
                                 std::string type,
                                 std::string name)
{
  DEBTRACE("SubjectForEachLoop::addNode(catalog, compo, type, name)");
  if (_body) return false;
  SubjectNode *body = createNode(catalog, compo, type, name);
  if (body) return true;
  return false;
}

void SubjectForEachLoop::completeChildrenSubjectList(SubjectNode *son)
{
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

bool SubjectOptimizerLoop::addNode(YACS::ENGINE::Catalog *catalog,
                                   std::string compo,
                                   std::string type,
                                   std::string name)
{
  DEBTRACE("SubjectOptimizerLoop::addNode(catalog, compo, type, name)");
  if (_body) return false;
  SubjectNode *body = createNode(catalog, compo, type, name);
  if (body) return true;
  return false;
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

void SubjectDataPort::tryCreateLink(SubjectDataPort *subOutport, SubjectDataPort *subInport)
{
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
                                               inNodePos, inportName);
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
  if (isDestructible())
    {
      try
        {
          _cla->edRemoveLink(_outp, _inp);
        }
      catch (YACS::Exception &e)
        {
          DEBTRACE("------------------------------------------------------------------------------");
          DEBTRACE("SubjectLink::~SubjectLink: edRemoveLink YACS exception " << e.what());
          DEBTRACE("------------------------------------------------------------------------------");
        }
    }
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
    }
}

std::string SubjectLink::getName()
{
  return _name;
}

// ----------------------------------------------------------------------------

SubjectComponent::SubjectComponent(YACS::ENGINE::ComponentInstance* component, int id, Subject *parent)
  : Subject(parent), _compoInst(component), _id(id)
{
}

SubjectComponent::~SubjectComponent()
{
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
  return _compoInst->getName();
}

std::pair<std::string, int> SubjectComponent::getKey()
{
  std::pair<std::string, int> key = std::pair<std::string, int>(_compoInst->getName(), _id);
  return key;
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
      SubjectContainer *subContainer =
        GuiContext::getCurrent()->getSubjectProc()->addSubjectContainer(container, container->getName());
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
}

// ----------------------------------------------------------------------------

SubjectContainer::SubjectContainer(YACS::ENGINE::Container* container, Subject *parent)
  : Subject(parent), _container(container)
{
}

SubjectContainer::~SubjectContainer()
{
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

// ----------------------------------------------------------------------------

SubjectDataType::SubjectDataType(std::string typeName, Subject *parent)
  : Subject(parent), _typeName(typeName)
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
  return _typeName;
}

// ----------------------------------------------------------------------------
