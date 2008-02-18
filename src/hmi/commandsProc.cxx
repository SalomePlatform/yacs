
#include <Python.h>
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
#include "PresetNode.hxx"
#include "OutNode.hxx"
#include "StudyNodes.hxx"
#include "Exception.hxx"
#include "DataPort.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "ComponentDefinition.hxx"
#include "SalomeContainer.hxx"
#include "SalomeComponent.hxx"

#include "guiContext.hxx"

#include <iostream>
#include <string>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

std::map<int, std::string> ProcInvoc::_typeNameMap;

// ----------------------------------------------------------------------------

ProcInvoc::ProcInvoc()
  : Invocator()
{
  _typeNameMap.clear();
  _typeNameMap[SALOMEPROC]           = "SALOMEPROC";
  _typeNameMap[BLOC]                 = "BLOC";
  _typeNameMap[FOREACHLOOP]          = "FOREACHLOOP";
  _typeNameMap[OPTIMIZERLOOP]        = "OPTIMIZERLOOP";
  _typeNameMap[FORLOOP]              = "FORLOOP";
  _typeNameMap[WHILELOOP]            = "WHILELOOP";
  _typeNameMap[SWITCH]               = "SWITCH";
  _typeNameMap[PYTHONNODE]           = "PYTHONNODE";
  _typeNameMap[PYFUNCNODE]           = "PYFUNCNODE";
  _typeNameMap[CORBANODE]            = "CORBANODE";
  _typeNameMap[SALOMENODE]           = "SALOMENODE";
  _typeNameMap[CPPNODE]              = "CPPNODE";
  _typeNameMap[SALOMEPYTHONNODE]     = "SALOMEPYTHONNODE";
  _typeNameMap[XMLNODE]              = "XMLNODE";
  _typeNameMap[SPLITTERNODE]         = "SPLITTERNODE";
  _typeNameMap[DFTODSFORLOOPNODE]    = "DFTODSFORLOOPNODE";
  _typeNameMap[DSTODFFORLOOPNODE]    = "DSTODFFORLOOPNODE";
  _typeNameMap[PRESETNODE]           = "PRESETNODE";
  _typeNameMap[OUTNODE]              = "OUTNODE";
  _typeNameMap[STUDYINNODE]          = "STUDYINNODE";
  _typeNameMap[STUDYOUTNODE]         = "STUDYOUTNODE";
  _typeNameMap[INPUTPORT]            = "INPUTPORT";
  _typeNameMap[OUTPUTPORT]           = "OUTPUTPORT";
  _typeNameMap[INPUTDATASTREAMPORT]  = "INPUTDATASTREAMPORT";
  _typeNameMap[OUTPUTDATASTREAMPORT] = "OUTPUTDATASTREAMPORT";
  _typeNameMap[CONTAINER]            = "CONTAINER";
  _typeNameMap[COMPONENT]            = "COMPONENT";
  _typeNameMap[REFERENCE]            = "REFERENCE";
  _typeNameMap[DATATYPE]             = "DATATYPE";
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
  else if (dynamic_cast<YACS::ENGINE::PresetNode*>(node))       nodeType = PRESETNODE;
  else if (dynamic_cast<YACS::ENGINE::OutNode*>(node))          nodeType = OUTNODE;
  else if (dynamic_cast<YACS::ENGINE::StudyInNode*>(node))      nodeType = STUDYINNODE;
  else if (dynamic_cast<YACS::ENGINE::StudyOutNode*>(node))     nodeType = STUDYOUTNODE;
  return nodeType;
}

TypeOfElem ProcInvoc::getTypeOfPort(YACS::ENGINE::DataPort* port)
{
  TypeOfElem portType = UNKNOWN;
  if      (dynamic_cast<YACS::ENGINE::InputPort*>(port))            portType = INPUTPORT;
  else if (dynamic_cast<YACS::ENGINE::OutputPort*>(port))           portType = OUTPUTPORT;
  else if (dynamic_cast<YACS::ENGINE::InputDataStreamPort*>(port))  portType = INPUTDATASTREAMPORT;
  else if (dynamic_cast<YACS::ENGINE::OutputDataStreamPort*>(port)) portType = OUTPUTDATASTREAMPORT;
  return portType;
}

std::string ProcInvoc::getTypeName(TypeOfElem type)
{
  if (_typeNameMap.count(type))
    return _typeNameMap[type];
  else
    return "UNKNOWN";
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
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
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
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      node = 0;
    }
  return (node != 0); 
}

bool CommandRenameNode::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAddDataTypeFromCatalog::CommandAddDataTypeFromCatalog(YACS::ENGINE::Catalog* catalog,
                                                             std::string typeName)
  : Command(), _catalog(catalog), _typeName(typeName)
{
  DEBTRACE("CommandAddDataTypeFromCatalog::CommandAddDataTypeFromCatalog: " << typeName);
}

YACS::ENGINE::TypeCode *CommandAddDataTypeFromCatalog::getTypeCode()
{

  if (GuiContext::getCurrent()->getProc()->typeMap.count(_typeName))
    return GuiContext::getCurrent()->getProc()->typeMap[_typeName];
  else return 0;
}

bool CommandAddDataTypeFromCatalog::localExecute()
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  if (proc->typeMap.count(_typeName))
    {
      DEBTRACE("typecode already existing in proc: " << _typeName);
      //GuiContext::getCurrent()->_lastErrorMessage = "typecode already existing in proc: " + _typeName;
      //return false;
      return true;
    }
  else
    if (_catalog->_typeMap.count(_typeName))
      {
        DEBTRACE("typecode found in catalog, cloned: " << _typeName);
        proc->typeMap[_typeName] = _catalog->_typeMap[_typeName]->clone();
        return true;
      }
  GuiContext::getCurrent()->_lastErrorMessage = "typecode not found in catalog: " + _typeName;
  return false;
}

bool CommandAddDataTypeFromCatalog::localReverse()
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
          else
            {
              DEBTRACE(_typePort << " not found in catalog");
              GuiContext::getCurrent()->_lastErrorMessage = _typePort + " not found in catalog";
            }
        }
      _inputPort = son;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddInputPortFromCatalog::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
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
          else
            {
              DEBTRACE(_typePort << " not found in catalog");
              GuiContext::getCurrent()->_lastErrorMessage = _typePort + " not found in catalog";
            }
        }
      _outputPort = son;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddOutputPortFromCatalog::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      if (son) delete son;
      _outputPort = 0;
    }
  return (_outputPort != 0);
}

bool CommandAddOutputPortFromCatalog::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAddIDSPortFromCatalog::CommandAddIDSPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                           std::string type,
                                                           std::string node,
                                                           std::string name)
  : Command(), _catalog(catalog), _typePort(type), _node(node), _name(name)
{
  _IDSPort = 0;
}

YACS::ENGINE::InputDataStreamPort *CommandAddIDSPortFromCatalog::getIDSPort()
{
  return _IDSPort;
}

bool CommandAddIDSPortFromCatalog::localExecute()
{
  InputDataStreamPort *son = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      ElementaryNode* father =dynamic_cast<ElementaryNode*> (node);
      if (father)
        {
          if (_catalog->_typeMap.count(_typePort))
            son = father->edAddInputDataStreamPort(_name, _catalog->_typeMap[_typePort]);
          else
            {
              DEBTRACE(_typePort << " not found in catalog");
              GuiContext::getCurrent()->_lastErrorMessage = _typePort + " not found in catalog";
            }
        }
      _IDSPort = son;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddIDSPortFromCatalog::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      if (son) delete son;
      _IDSPort = 0;
    }
  return (_IDSPort != 0);
}

bool CommandAddIDSPortFromCatalog::localReverse()
{
}
 
// ----------------------------------------------------------------------------

CommandAddODSPortFromCatalog::CommandAddODSPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                           std::string type,
                                                           std::string node,
                                                           std::string name)
  : Command(), _catalog(catalog), _typePort(type), _node(node), _name(name)
{
  _ODSPort = 0;
}

YACS::ENGINE::OutputDataStreamPort *CommandAddODSPortFromCatalog::getODSPort()
{
  return _ODSPort;
}

bool CommandAddODSPortFromCatalog::localExecute()
{
  OutputDataStreamPort *son = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      ElementaryNode* father =dynamic_cast<ElementaryNode*> (node);
      if (father)
        {
          if (_catalog->_typeMap.count(_typePort))
            son = father->edAddOutputDataStreamPort(_name, _catalog->_typeMap[_typePort]);
          else
            {
              DEBTRACE(_typePort << " not found in catalog");
              GuiContext::getCurrent()->_lastErrorMessage = _typePort + " not found in catalog";
            }
        }
      _ODSPort = son;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddODSPortFromCatalog::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      if (son) delete son;
      _ODSPort = 0;
    }
  return (_ODSPort != 0);
}

bool CommandAddODSPortFromCatalog::localReverse()
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
  Subject::erase(_subject);
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
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandAddLink::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAddControlLink::CommandAddControlLink(std::string outNode, std::string inNode)
  : Command(), _outNode(outNode), _inNode(inNode)
{
  DEBTRACE("CommandAddControlLink::CommandAddControlLink "<<outNode<<"-->>"<<inNode);
}

bool CommandAddControlLink::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* outn = proc;
      if (! _outNode.empty())
        outn = proc->getChildByName(_outNode);
      Node* inn = proc;
      if (! _inNode.empty())
        inn = proc->getChildByName(_inNode);
      ComposedNode *cla = ComposedNode::getLowestCommonAncestor(outn,inn);
      DEBTRACE(cla->getName());
      cla->edAddCFLink(outn,inn);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddControlLink::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandAddControlLink::localReverse()
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
      if (proc->containerMap.count(_name))
        {
          GuiContext::getCurrent()->_lastErrorMessage = "There is already a container with that name";
          return false;
        }
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
            {
              GuiContext::getCurrent()->_lastErrorMessage = "There is no reference container to clone properties";
              return false;
            }
        }
      _container = container;
      _container->setName(_name);
      proc->containerMap[_name] = _container;
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddContainer::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
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
      GuiContext::getCurrent()->_lastErrorMessage = "container not found: " + _container;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetContainerProperties::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandSetContainerProperties::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandSetDSPortProperties::CommandSetDSPortProperties(std::string node, std::string port, bool isInport,
                                                       std::map<std::string,std::string> properties)
  : Command(), _nodeName(node), _portName(port), _isInport(isInport), _properties(properties)
{
  DEBTRACE("CommandSetDSPortProperties::CommandSetDSPortProperties " << node << "." << port << " " << isInport);
}
      
bool CommandSetDSPortProperties::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_nodeName);
      DataStreamPort* DSPort = 0;
      if (_isInport)
        DSPort = node->getInputDataStreamPort(_portName);
      else
        DSPort = node->getOutputDataStreamPort(_portName);
      DSPort->setProperties(_properties);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetDSPortProperties::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}
      
bool CommandSetDSPortProperties::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandSetFuncNodeFunctionName::CommandSetFuncNodeFunctionName(std::string node, std::string funcName)
  : Command(), _nodeName(node), _funcName(funcName)
{
  DEBTRACE("CommandSetFuncNodeFunctionName::CommandSetFuncNodeFunctionName " << node << " " <<funcName);
}

bool CommandSetFuncNodeFunctionName::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_nodeName);
      if (_funcName.empty())
        {
          GuiContext::getCurrent()->_lastErrorMessage = "InlineFuncNode function name empty: " + _nodeName;
          return false;
        }
      if (YACS::ENGINE::InlineFuncNode* funcNode = dynamic_cast<YACS::ENGINE::InlineFuncNode*>(node))
        {
          funcNode->setFname(_funcName);
          return true;
        }
      else
        {
          GuiContext::getCurrent()->_lastErrorMessage = "node is not an InlineFuncNode: " + _nodeName;
          return false;
        }
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetFuncNodeFunctionName::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }  
}

bool CommandSetFuncNodeFunctionName::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandSetInlineNodeScript::CommandSetInlineNodeScript(std::string node, std::string script)
  : Command(), _nodeName(node), _script(script)
{
  DEBTRACE("CommandSetInlineNodeScript::CommandSetInlineNodeScript " << node << " " <<script);
}

bool CommandSetInlineNodeScript::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_nodeName);
      if (_script.empty())
        {
          GuiContext::getCurrent()->_lastErrorMessage = "InlineNode script empty: " + _nodeName;
          return false;
        }
      if (YACS::ENGINE::InlineNode* inlineNode = dynamic_cast<YACS::ENGINE::InlineNode*>(node))
        {
          inlineNode->setScript(_script);
          return true;
        }
      else
        {
          GuiContext::getCurrent()->_lastErrorMessage = "node is not an InlineNode: " + _nodeName;
          return false;
        }
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetInlineNodeScript::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }  
}

bool CommandSetInlineNodeScript::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAddComponentInstance::CommandAddComponentInstance(std::string compoName)
  : Command(), _compoName(compoName), _compoInst(0)
{
  DEBTRACE("CommandAddComponentInstance::CommandAddComponentInstance " <<_compoName);
}

bool CommandAddComponentInstance::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      _compoInst = new SalomeComponent(_compoName);
      pair<string,int> key = pair<string,int>(_compoName, _compoInst->getNumId());
      proc->componentInstanceMap[key] = _compoInst;
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddComponentInstance::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
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

CommandAssociateComponentToContainer::CommandAssociateComponentToContainer(std::pair<std::string,int> key,
                                                                           std::string container)
  : Command(), _key(key), _container(container)
{
  DEBTRACE("CommandAssociateComponentToContainer::CommandAssociateComponentToContainer "
           << key.first << " " << key.second << " " << container);
}

bool CommandAssociateComponentToContainer::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_container))
        {
          Container *cont = proc->containerMap[_container];
          if (proc->componentInstanceMap.count(_key))
            {
              ComponentInstance *compo = proc->componentInstanceMap[_key];
              compo->setContainer(cont);
              return true;
            }
          else
            GuiContext::getCurrent()->_lastErrorMessage = "Component instance not found: " + _key.first;
        }
      else
        GuiContext::getCurrent()->_lastErrorMessage = "Container not found: " + _container;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAssociateComponentToContainer::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandAssociateComponentToContainer::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAssociateServiceToComponent::CommandAssociateServiceToComponent(std::string service,
                                                                       std::pair<std::string,int> key)
  : Command(), _service(service), _key(key)
{
  DEBTRACE("CommandAssociateServiceToComponent::CommandAssociateServiceToComponent "
           << service << " " << key.first << " " << key.second);
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
          if (proc->componentInstanceMap.count(_key))
            {
              ComponentInstance *compo = proc->componentInstanceMap[_key];
              service->setComponent(compo);
              return true;
            }
          else
            GuiContext::getCurrent()->_lastErrorMessage = "Component instance not found: " + _key.first;
        }
      else
        GuiContext::getCurrent()->_lastErrorMessage = "Node is note a service node: " + _service;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAssociateServiceToComponent::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandAssociateServiceToComponent::localReverse()
{
}

// ----------------------------------------------------------------------------
