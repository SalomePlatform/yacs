
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
#include "Exception.hxx"
#include "OutputDataStreamPort.hxx"
#include "ComponentDefinition.hxx"
#include "SalomeContainer.hxx"
#include "SalomeComponent.hxx"

#include "guiContext.hxx"

#include <iostream>
#include <string>

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

// ----------------------------------------------------------------------------

ProcInvoc::ProcInvoc()
  : Invocator()
{
}

TypeOfElem ProcInvoc::getTypeOfNode(YACS::ENGINE::Node* node)
{
  TypeOfElem nodeType = UNKNOWN;
  if      (dynamic_cast<YACS::ENGINE::Bloc*>(node))             nodeType = BLOC;
  else if (dynamic_cast<YACS::ENGINE::PythonNode*>(node))       nodeType = PYTHONNODE;
  else if (dynamic_cast<YACS::ENGINE::PyFuncNode*>(node))       nodeType = PYFUNCNODE;
  else if (dynamic_cast<YACS::ENGINE::CORBANode*>(node))        nodeType = CORBANODE;
  else if (dynamic_cast<YACS::ENGINE::CppNode*>(node))          nodeType = CPPNODE;
  else if (dynamic_cast<YACS::ENGINE::SalomeNode*>(node))       nodeType = SALOMENODE;
  else if (dynamic_cast<YACS::ENGINE::SalomePythonNode*>(node)) nodeType = SALOMEPYTHONNODE;
  else if (dynamic_cast<YACS::ENGINE::XmlNode*>(node))          nodeType = XMLNODE;
  else if (dynamic_cast<YACS::ENGINE::SplitterNode*>(node))     nodeType = SPLITTERNODE;
  else if (dynamic_cast<YACS::ENGINE::ForLoop*>(node))          nodeType = FORLOOP;
  else if (dynamic_cast<YACS::ENGINE::WhileLoop*>(node))        nodeType = WHILELOOP;
  else if (dynamic_cast<YACS::ENGINE::Switch*>(node))           nodeType = SWITCH;
  else if (dynamic_cast<YACS::ENGINE::ForEachLoop*>(node))      nodeType = FOREACHLOOP;
  else if (dynamic_cast<YACS::ENGINE::OptimizerLoop*>(node))    nodeType = OPTIMIZERLOOP;
  return nodeType;
}

// ----------------------------------------------------------------------------

CommandAddNodeFromCatalog::CommandAddNodeFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                     std::string compo,
                                                     std::string type,
                                                     std::string position,
                                                     std::string name,
                                                     int swCase)
  : Command(), _catalog(catalog), _compoName(compo), _typeName(type),
    _position(position), _name(name), _swCase(swCase)
{
  _nodeToClone = 0;
  if (_compoName.empty())
    {
      if (_catalog->_nodeMap.count(_typeName))
        _nodeToClone = _catalog->_nodeMap[_typeName];
      else if (_catalog->_composednodeMap.count(_typeName))
        _nodeToClone = _catalog->_composednodeMap[_typeName];
    }
  else
    if (_catalog->_componentMap.count(_compoName))
      {
        YACS::ENGINE::ComponentDefinition* compodef = _catalog->_componentMap[_compoName];
        if (compodef->_serviceMap.count(_typeName))
          _nodeToClone = compodef->_serviceMap[_typeName];
      }
  if (_nodeToClone)
    _typeNode = ProcInvoc::getTypeOfNode(_nodeToClone);
}


YACS::ENGINE::Node *CommandAddNodeFromCatalog::getNode()
{
  return _node;
}

bool CommandAddNodeFromCatalog::localExecute()
{
  Node *son = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc;
      if (!_position.empty()) node = proc->getChildByName(_position);
      ComposedNode* father =dynamic_cast<ComposedNode*> (node);
      if (father)
        {
          son = _nodeToClone->clone(0);
          son->setName(_name);
        }
      if (son)
        {
          TypeOfElem fatherType = ProcInvoc::getTypeOfNode(father);
          switch (fatherType)
            {
            case BLOC:
              (dynamic_cast<YACS::ENGINE::Bloc*>(father))->edAddChild(son);
              break;
            case FORLOOP:
              (dynamic_cast<YACS::ENGINE::ForLoop*>(father))->edSetNode(son);
              break;
            case WHILELOOP:
              (dynamic_cast<YACS::ENGINE::WhileLoop*>(father))->edSetNode(son);
              break;
            case SWITCH:
              (dynamic_cast<YACS::ENGINE::Switch*>(father))->edSetNode(_swCase,son);
              break;
            case FOREACHLOOP:
              (dynamic_cast<YACS::ENGINE::ForEachLoop*>(father))->edSetNode(son);
              break;
            case OPTIMIZERLOOP:
              (dynamic_cast<YACS::ENGINE::OptimizerLoop*>(father))->edSetNode(son);
              break;
            default:
              assert(0);
            }
        }
      _node = son;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddNode::localExecute() : " << ex.what());
      if (son) delete son;
      _node = 0;
    }
  return (_node != 0);
}

bool CommandAddNodeFromCatalog::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandRenameNode::CommandRenameNode(std::string position, std::string name)
  : Command(), _position(position), _name(name)
{
  DEBTRACE("CommandRenameNode::CommandRenameNode " << _position << " " << _name);
}

bool CommandRenameNode::localExecute()
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = proc;
  try
    {
      if (_position != proc->getName()) node = proc->getChildByName(_position);
      node->setName(_name);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameNode::localExecute() : " << ex.what());
      node = 0;
    }
  return (node != 0); 
}

bool CommandRenameNode::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAddInputPortFromCatalog::CommandAddInputPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                               std::string type,
                                                               std::string node,
                                                               std::string name)
  : Command(), _catalog(catalog), _typePort(type), _node(node), _name(name)
{
  _inputPort = 0;
}

YACS::ENGINE::InputPort *CommandAddInputPortFromCatalog::getInputPort()
{
  return _inputPort;
}

bool CommandAddInputPortFromCatalog::localExecute()
{
  InputPort *son = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      ElementaryNode* father =dynamic_cast<ElementaryNode*> (node);
      if (father)
        {
          if (_catalog->_typeMap.count(_typePort))
            son = father->edAddInputPort(_name, _catalog->_typeMap[_typePort]);
          else DEBTRACE(_typePort << " not found in catalog");
        }
      _inputPort = son;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddInputPortFromCatalog::localExecute() : " << ex.what());
      if (son) delete son;
      _inputPort = 0;
    }
  return (_inputPort != 0);
}

bool CommandAddInputPortFromCatalog::localReverse()
{
}
 
// ----------------------------------------------------------------------------

CommandAddOutputPortFromCatalog::CommandAddOutputPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                                 std::string type,
                                                                 std::string node,
                                                                 std::string name)
  : Command(), _catalog(catalog), _typePort(type), _node(node), _name(name)
{
  _outputPort = 0;
}

YACS::ENGINE::OutputPort *CommandAddOutputPortFromCatalog::getOutputPort()
{
  return _outputPort;
}

bool CommandAddOutputPortFromCatalog::localExecute()
{
  OutputPort *son = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      ElementaryNode* father =dynamic_cast<ElementaryNode*> (node);
      if (father)
        {
          if (_catalog->_typeMap.count(_typePort))
            son = father->edAddOutputPort(_name, _catalog->_typeMap[_typePort]);
          else DEBTRACE(_typePort << " not found in catalog");
        }
      _outputPort = son;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddOutputPortFromCatalog::localExecute() : " << ex.what());
      if (son) delete son;
      _outputPort = 0;
    }
  return (_outputPort != 0);
}

bool CommandAddOutputPortFromCatalog::localReverse()
{
}
 
// ----------------------------------------------------------------------------

CommandDestroy::CommandDestroy(std::string position,  Subject* subject)
  : Command(), _position(position), _subject(subject)
{
  //todo: store what is needed to recreate the subject
}

bool CommandDestroy::localExecute()
{
  delete _subject;
  _subject = 0;
  return true; 
}

bool CommandDestroy::localReverse()
{
}
 
// ----------------------------------------------------------------------------

CommandAddLink::CommandAddLink(std::string outNode, std::string outPort,
                               std::string inNode, std::string inPort)
  : Command(), _outNode(outNode), _outPort(outPort), _inNode(inNode), _inPort(inPort)
{
  DEBTRACE("CommandAddLink::CommandAddLink "<<outNode<<"."<<outPort<<"->"<<inNode<<"."<<inPort);
}

bool CommandAddLink::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* outn = proc->getChildByName(_outNode);
      Node* inn = proc->getChildByName(_inNode);
      OutPort* outp = outn->getOutPort(_outPort);
      InPort* inp = inn->getInPort(_inPort);
      ComposedNode *cla = ComposedNode::getLowestCommonAncestor(outn->getFather(),inn->getFather());
      DEBTRACE(cla->getName());
      if (dynamic_cast<OutputDataStreamPort*>(outp))
        cla->edAddLink(outp,inp);
      else
        cla->edAddDFLink(outp,inp);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddLink::localExecute() : " << ex.what());
      return false;
    }
}

bool CommandAddLink::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAddContainer::CommandAddContainer(std::string name,
                                         std::string refContainer)
  : Command(), _name(name), _containerToClone(refContainer), _container(0)
{
  DEBTRACE("CommandAddContainer::CommandAddContainer " << name << " " << refContainer);
}

bool CommandAddContainer::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_name)) return false;
      Container *container = new SalomeContainer();
      if (! _containerToClone.empty())
        {
          if (proc->containerMap.count(_containerToClone))
            {
              Container *ref = proc->containerMap[_containerToClone];
              assert(ref);
              container->setProperties(ref->getProperties());
            }
          else
            return false;
        }
      _container = container;
      _container->setName(_name);
      proc->containerMap[_name] = _container;
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddContainer::localExecute() : " << ex.what());
      return false;
    }
}

bool CommandAddContainer::localReverse()
{
}

YACS::ENGINE::Container* CommandAddContainer::getContainer()
{
  return _container;
}

// ----------------------------------------------------------------------------

CommandSetContainerProperties::CommandSetContainerProperties(std::string container,
                                                             std::map<std::string,std::string> properties)
  : Command(), _container(container), _properties(properties)
{
  DEBTRACE("CommandSetContainerProperties::CommandSetContainerProperties " << container);
}

bool CommandSetContainerProperties::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_container))
        {
          Container *ref = proc->containerMap[_container];
          assert(ref);
          ref->setProperties(_properties);
          return true;
        }
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetContainerProperties::localExecute() : " << ex.what());
      return false;
    }
}

bool CommandSetContainerProperties::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAddComponentInstance::CommandAddComponentInstance(std::string name,
                                                         std::string refComponent)
  : Command(), _name(name), _componentToClone(refComponent), _compoInst(0)
{
  DEBTRACE("CommandAddComponentInstance::CommandAddComponentInstance " << name << " " << refComponent);
}

bool CommandAddComponentInstance::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      ComponentInstance *compo = 0;
      if (_componentToClone.empty())
        {
          compo = new SalomeComponent(_name);
        }
      else
        {
          DEBTRACE("CommandAddComponentInstance::localExecute with ref Component = semantic not clear ******");
          ComponentInstance *ref = 0;
          if (proc->componentInstanceMap.count(_componentToClone))
            {
              ComponentInstance *ref = proc->componentInstanceMap[_componentToClone]; // only the last one kept
              assert(ref);
              compo = ref->clone();
            }
          else
            return false;
        }
      _compoInst = compo;
      proc->componentInstanceMap[_name] = _compoInst; // --- potentially several instances under the same name
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddComponentInstance::localExecute() : " << ex.what());
      return false;
    }
}

bool CommandAddComponentInstance::localReverse()
{
}

YACS::ENGINE::ComponentInstance* CommandAddComponentInstance::getComponentInstance()
{
  return _compoInst;
}

  
// ----------------------------------------------------------------------------

CommandAssociateComponentToContainer::CommandAssociateComponentToContainer(std::string component,
                                                                           std::string container)
  : Command(), _component(component), _container(container)
{
  DEBTRACE("CommandAssociateComponentToContainer::CommandAssociateComponentToContainer "
           << component << " " << container);
}

bool CommandAssociateComponentToContainer::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_container))
        {
          Container *cont = proc->containerMap[_container];
          if (proc->componentInstanceMap.count(_component))
            {
              ComponentInstance *compo = proc->componentInstanceMap[_component];
              compo->setContainer(cont);
              return true;
            }
        }
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAssociateComponentToContainer::localExecute() : " << ex.what());
      return false;
    }
}

bool CommandAssociateComponentToContainer::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAssociateServiceToComponent::CommandAssociateServiceToComponent(std::string service,
                                                                       std::string component)
  : Command(), _service(service), _component(component)
{
  DEBTRACE("CommandAssociateServiceToComponent::CommandAssociateServiceToComponent "
           << service << " " << component);
}

bool CommandAssociateServiceToComponent::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (_service == proc->getName()) return false; // proc is not an elementary node
      Node* node = proc->getChildByName(_service);
      if (ServiceNode *service = dynamic_cast<ServiceNode*>(node))
        {
          if (proc->componentInstanceMap.count(_component))
            {
              ComponentInstance *compo = proc->componentInstanceMap[_component];
              service->setComponent(compo);
              return true;
            }
        }
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAssociateServiceToComponent::localExecute() : " << ex.what());
      return false;
    }
}

bool CommandAssociateServiceToComponent::localReverse()
{
}

// ----------------------------------------------------------------------------
