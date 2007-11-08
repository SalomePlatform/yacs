
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

#include "guiContext.hxx"

#include <string>

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

// ----------------------------------------------------------------------------

Subject::Subject(Subject *parent) : _parent(parent)
{
}

Subject::~Subject()
{
  DEBTRACE("Subject::~Subject " << getName());
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
  DEBTRACE("Subject::destroy " << son->getName());
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (! dynamic_cast<SubjectProc*>(son))
    {
      SubjectNode *subNode = dynamic_cast<SubjectNode*>(son);
      if (subNode)
        position = proc->getChildName(subNode->getNode());
      else if (dynamic_cast<SubjectDataPort*>(son))
        {
          SubjectNode *subNodep = dynamic_cast<SubjectNode*>(son->getParent());
          position = proc->getChildName(subNodep->getNode());
        }
    }
  else
    position = proc->getName();
  CommandDestroy* command = new CommandDestroy(position, son);
  if (command->execute())
    {
      DEBTRACE("Destruction done: " << position);
      return true;
    }
  return false;
}

void Subject::loadChildren()
{
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

SubjectNode::SubjectNode(YACS::ENGINE::Node *node, Subject *parent)
  : Subject(parent), _node(node)
{
  _listSubjectInputPort.clear();
  _listSubjectOutputPort.clear();
}

SubjectNode::~SubjectNode()
{
  DEBTRACE("SubjectNode::~SubjectNode " << getName());
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

SubjectInputPort* SubjectNode::addSubjectInputPort(YACS::ENGINE::InputPort *port, std::string name)
{
  string theName = name;
  if (name.empty()) theName =port->getName();
  DEBTRACE("SubjectNode::addSubjectInputPort "<< theName);
  SubjectInputPort *son = new SubjectInputPort(port, this);
  GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(port)] = son;
  _listSubjectInputPort.push_back(son);
  if (!name.empty()) son->setName(name);
  update(ADD, INPUTPORT ,son);
  return son;
}


SubjectOutputPort* SubjectNode::addSubjectOutputPort(YACS::ENGINE::OutputPort *port, std::string name)
{
  string theName = name;
  if (name.empty()) theName =port->getName();
  DEBTRACE("SubjectNode::addSubjectOutputPort "<< theName);
  SubjectOutputPort *son = new SubjectOutputPort(port, this);
  GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(port)] = son;
  _listSubjectOutputPort.push_back(son);
  if (!name.empty()) son->setName(name);
  update(ADD, OUTPUTPORT ,son);
  return son;
}

// ----------------------------------------------------------------------------

SubjectComposedNode::SubjectComposedNode(YACS::ENGINE::ComposedNode *composedNode,
                                         Subject *parent)
  : SubjectNode(composedNode, parent), _composedNode(composedNode)
{
}

SubjectComposedNode::~SubjectComposedNode()
{
  DEBTRACE("SubjectComposedNode::~SubjectComposedNode " << getName());
}

bool SubjectComposedNode::addNode(YACS::ENGINE::Catalog *catalog,
                                  std::string type,
                                  std::string name)
{
  DEBTRACE("SubjectComposedNode::addNode(Catalog *catalog, string type, string name)");
  return false;
}

SubjectNode *SubjectComposedNode::createNode(YACS::ENGINE::Catalog *catalog,
                                             std::string type,
                                             std::string name,
                                             int swCase)
{
  Proc *proc = GuiContext::getCurrent()->getProc();
  string position = "";
  if (proc != dynamic_cast<Proc*>(_node)) position = proc->getChildName(_node);
  CommandAddNodeFromCatalog *command = new CommandAddNodeFromCatalog(catalog,
                                                                     type,
                                                                     position,
                                                                     name,
                                                                     swCase);
  if (command->execute())
    {
      GuiContext::getCurrent()->getInvoc()->add(command);
      Node * node = command->getNode();
      SubjectNode *son = addSubjectNode(node);
      return son;
    }
  return 0;
}

SubjectNode *SubjectComposedNode::addSubjectNode(YACS::ENGINE::Node * node, std::string name)
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
  update(ADD, ntyp ,son);
  return son;
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
  if (Switch *aSwitch = dynamic_cast<Switch*>(_composedNode))
    listInputPorts.push_back(aSwitch->edGetConditionPort());
  list<InputPort*>::const_iterator iti;
  for (iti = listInputPorts.begin(); iti != listInputPorts.end(); ++iti)
    addSubjectInputPort(*iti);
  list<OutputPort*>::const_iterator ito;
  for (ito = listOutputPorts.begin(); ito != listOutputPorts.end(); ++ito)
    addSubjectOutputPort(*ito);

  // --- TO BE COMPLETED : datastream ports

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
  update(ADDLINK, 0, son);
  return son;
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

SubjectBloc::~SubjectBloc()
{
  DEBTRACE("SubjectBloc::~SubjectBloc " << getName());
  set<SubjectNode*>::iterator it;
  set<SubjectNode*> copyChildren = _children;
  for (it = copyChildren.begin(); it !=copyChildren.end(); ++it)
    delete (*it) ;
  ComposedNode* father = _bloc->getFather();
  if (father)
    {
      Bloc *bloc = dynamic_cast<Bloc*>(father);
      if (bloc)
        {
          DEBTRACE("---");
          bloc->edRemoveChild(_bloc);
        }
      else
        {
          Loop *loop = dynamic_cast<Loop*>(father);
          if (loop) loop->edRemoveChild(_bloc);
          else
            {
              Switch *aSwitch = dynamic_cast<Switch*>(father);
              if (aSwitch) aSwitch->edRemoveChild(_bloc);
            }
        }
    }
  if (_parent)
    {
      SubjectBloc* sb = dynamic_cast<SubjectBloc*>(_parent);
      if (sb) sb->removeNode(this);
    }
}

bool SubjectBloc::addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectBloc::addNode( " << catalog << ", " << type << ", " << name << " )");
  SubjectNode* child = createNode(catalog, type, name);
  if (child)
    {
      _children.insert(child);
      return true;
    }
  return false;
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

void SubjectProc::loadProc()
{
  DEBTRACE("SubjectProc::loadProc "  << getName());
  loadChildren();
  loadLinks();
}

// ----------------------------------------------------------------------------

SubjectElementaryNode::SubjectElementaryNode(YACS::ENGINE::ElementaryNode *elementaryNode,
                                             Subject *parent)
  : SubjectNode(elementaryNode, parent), _elementaryNode(elementaryNode)
{
}

SubjectElementaryNode::~SubjectElementaryNode()
{
  DEBTRACE("SubjectElementaryNode::~SubjectElementaryNode " << getName());
  list<SubjectInputPort*>::iterator iti;
  list<SubjectInputPort*> cpli = _listSubjectInputPort;
  for(iti = cpli.begin(); iti != cpli.end(); ++iti)
    delete (*iti);
  list<SubjectOutputPort*>::iterator ito;
  list<SubjectOutputPort*> cplo = _listSubjectOutputPort;
  for(ito = cplo.begin(); ito != cplo.end(); ++ito)
    delete (*ito);
  ComposedNode* father = _elementaryNode->getFather();
  if (father)
    {
      Bloc *bloc = dynamic_cast<Bloc*>(father);
      if (bloc) bloc->edRemoveChild(_elementaryNode);
      else
        {
          Loop *loop = dynamic_cast<Loop*>(father);
          if (loop) loop->edRemoveChild(_elementaryNode);
          else
            {
              Switch *aSwitch = dynamic_cast<Switch*>(father);
              if (aSwitch) aSwitch->edRemoveChild(_elementaryNode);
            }
        }
    }
  if (_parent)
    {
      SubjectBloc* sb = dynamic_cast<SubjectBloc*>(_parent);
      if (sb) sb->removeNode(this);
    }
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
  SubjectInputPort* inp = dynamic_cast<SubjectInputPort*>(port);
  if (inp)
    {
      DEBTRACE("---");
      _listSubjectInputPort.remove(inp);
    }
  else
    {
      SubjectOutputPort* outp = dynamic_cast<SubjectOutputPort*>(port);
      if (outp)
        {
          DEBTRACE("---");
          _listSubjectOutputPort.remove(outp);
        }
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
  // --- TO BE COMPLETED : datastream ports
}

// ----------------------------------------------------------------------------


SubjectInlineNode::SubjectInlineNode(YACS::ENGINE::InlineNode *inlineNode, Subject *parent)
  : SubjectElementaryNode(inlineNode, parent), _inlineNode(inlineNode)
{
}

SubjectInlineNode::~SubjectInlineNode()
{
}

// ----------------------------------------------------------------------------

SubjectServiceNode::SubjectServiceNode(YACS::ENGINE::ServiceNode *serviceNode, Subject *parent)
  : SubjectElementaryNode(serviceNode, parent), _serviceNode(serviceNode)
{
}

SubjectServiceNode::~SubjectServiceNode()
{
}

// ----------------------------------------------------------------------------

SubjectPythonNode::SubjectPythonNode(YACS::ENGINE::PythonNode *pythonNode, Subject *parent)
  : SubjectInlineNode(pythonNode, parent), _pythonNode(pythonNode)
{
}

SubjectPythonNode::~SubjectPythonNode()
{
}

// ----------------------------------------------------------------------------

SubjectPyFuncNode::SubjectPyFuncNode(YACS::ENGINE::PyFuncNode *pyFuncNode, Subject *parent)
  : SubjectInlineNode(pyFuncNode, parent), _pyFuncNode(pyFuncNode)
{
}

SubjectPyFuncNode::~SubjectPyFuncNode()
{
}

// ----------------------------------------------------------------------------

SubjectCORBANode::SubjectCORBANode(YACS::ENGINE::CORBANode *corbaNode, Subject *parent)
  : SubjectServiceNode(corbaNode, parent), _corbaNode(corbaNode)
{
}

SubjectCORBANode::~SubjectCORBANode()
{
}

// ----------------------------------------------------------------------------

SubjectCppNode::SubjectCppNode(YACS::ENGINE::CppNode *cppNode, Subject *parent)
  : SubjectServiceNode(cppNode, parent), _cppNode(cppNode)
{
}

SubjectCppNode::~SubjectCppNode()
{
}

// ----------------------------------------------------------------------------

SubjectSalomeNode::SubjectSalomeNode(YACS::ENGINE::SalomeNode *salomeNode, Subject *parent)
  : SubjectServiceNode(salomeNode, parent), _salomeNode(salomeNode)
{
}

SubjectSalomeNode::~SubjectSalomeNode()
{
}

// ----------------------------------------------------------------------------

SubjectSalomePythonNode::SubjectSalomePythonNode(YACS::ENGINE::SalomePythonNode *salomePythonNode,
                                                 Subject *parent)
  : SubjectServiceNode(salomePythonNode, parent), _salomePythonNode(salomePythonNode)
{
}

SubjectSalomePythonNode::~SubjectSalomePythonNode()
{
}

// ----------------------------------------------------------------------------

SubjectXmlNode::SubjectXmlNode(YACS::ENGINE::XmlNode *xmlNode, Subject *parent)
  : SubjectServiceNode(xmlNode, parent), _xmlNode(xmlNode)
{
}

SubjectXmlNode::~SubjectXmlNode()
{
}

// ----------------------------------------------------------------------------

SubjectSplitterNode::SubjectSplitterNode(YACS::ENGINE::SplitterNode *splitterNode, Subject *parent)
  : SubjectElementaryNode(splitterNode, parent), _splitterNode(splitterNode)
{
}

SubjectSplitterNode::~SubjectSplitterNode()
{
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
  if (_body) delete _body;
}

bool SubjectForLoop::addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectForLoop::addNode(catalog, type, name)");
  if (_body) return false;
  _body = createNode(catalog, type, name);
  if (_body) return true;
  return false;
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
  if (_body) delete _body;
}

bool SubjectWhileLoop::addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectWhileLoop::addNode(catalog, type, name)");
  if (_body) return false;
  _body = createNode(catalog, type, name);
  if (_body) return true;
  return false;
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
  map<int, SubjectNode*>::iterator it;
  for (it = _bodyMap.begin(); it != _bodyMap.end(); ++it)
    delete (*it).second;
}

bool SubjectSwitch::addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name,
                            int swCase)
{
  DEBTRACE("SubjectSwitch::addNode( " << catalog << ", " << type << ", " << name <<", " << swCase << " )");
  if (_bodyMap.count(swCase)) return false;
  SubjectNode* body = createNode(catalog, type, name, swCase);
  if(body)
    {
      _bodyMap[swCase] = body;
      return true;
    }
  return false;
}

std::map<int, SubjectNode*> SubjectSwitch::getBodyMap()
{
  return _bodyMap;
}

// ----------------------------------------------------------------------------

SubjectForEachLoop::SubjectForEachLoop(YACS::ENGINE::ForEachLoop *forEachLoop, Subject *parent)
  : SubjectComposedNode(forEachLoop, parent), _forEachLoop(forEachLoop)
{
  _body = 0;
}

SubjectForEachLoop::~SubjectForEachLoop()
{
  DEBTRACE("SubjectForEachLoop::~SubjectForEachLoop " << getName());
  if (_body) delete _body;
}

bool SubjectForEachLoop::addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectForEachLoop::addNode(catalog, type, name)");
  if (_body) return false;
  _body = createNode(catalog, type, name);
  if (_body) return true;
  return false;
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
  if (_body) delete _body;
}

bool SubjectOptimizerLoop::addNode(YACS::ENGINE::Catalog *catalog, std::string type, std::string name)
{
  DEBTRACE("SubjectOptimizerLoop::addNode(catalog, type, name)");
  if (_body) return false;
  _body = createNode(catalog, type, name);
  if (_body) return true;
  return false;
}

// ----------------------------------------------------------------------------

SubjectDataPort::SubjectDataPort(YACS::ENGINE::DataPort* port, Subject *parent)
: Subject(parent), _dataPort(port)
{
}

SubjectDataPort::~SubjectDataPort()
{
  DEBTRACE("SubjectDataPort::~SubjectDataPort " << getName());
  Node* node = _dataPort->getNode();
  assert(node);
  ElementaryNode * father = dynamic_cast<ElementaryNode*>(node);
  if (father)
    {
      DEBTRACE("father->edRemovePort(_dataPort)");
      father->edRemovePort(_dataPort);
    }
}

std::string SubjectDataPort::getName()
{
  return _dataPort->getName();
}

void SubjectDataPort::tryCreateLink(SubjectDataPort *subInport)
{
}

// ----------------------------------------------------------------------------

SubjectInputPort::SubjectInputPort(YACS::ENGINE::InputPort *port, Subject *parent)
  : SubjectDataPort(port, parent), _inputPort(port)
{
}

SubjectInputPort::~SubjectInputPort()
{
  DEBTRACE("SubjectInputPort::~SubjectInputPort " << getName());
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
}

SubjectOutputPort::~SubjectOutputPort()
{
  DEBTRACE("SubjectOutputPort::~SubjectOutputPort " << getName());
  if (_parent)
    {
      SubjectElementaryNode* elem = dynamic_cast<SubjectElementaryNode*>(_parent);
      if (elem) elem->removePort(this);
    }
}

void SubjectOutputPort::tryCreateLink(SubjectDataPort *subInport)
{
  SubjectInputPort *sip = dynamic_cast<SubjectInputPort*>(subInport);
  if (sip)
    {
      Proc *proc = GuiContext::getCurrent()->getProc();
      string outNodePos = "";
      SubjectNode *sno = dynamic_cast<SubjectNode*>(_parent);
      assert(sno);
      Node *outNode = sno->getNode();
      outNodePos = proc->getChildName(outNode);
      string outportName = getName();
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
          scla->addSubjectLink(sno, this, sni, subInport);
        }
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

void SubjectOutputDataStreamPort::tryCreateLink(SubjectDataPort *subInport)
{
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

std::string SubjectLink::getName()
{
  return _name;
}

// ----------------------------------------------------------------------------
