
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

ProcInvoc::ProcInvoc(YACS::ENGINE::Proc *proc):
  _proc(proc), Invocator()
{
}

TypeOfNode ProcInvoc::getTypeOfNode(YACS::ENGINE::Node* node)
{
  TypeOfNode nodeType = UNKNOWN;
  if      (dynamic_cast<YACS::ENGINE::Bloc*>(node))             nodeType = BLOC;
  else if (dynamic_cast<YACS::ENGINE::PythonNode*>(node))       nodeType = PYTHONNODE;
  else if (dynamic_cast<YACS::ENGINE::PyFuncNode*>(node))       nodeType = PYFUNCNODE;
  else if (dynamic_cast<YACS::ENGINE::CORBANode*>(node))        nodeType = CORBANODE;
  else if (dynamic_cast<YACS::ENGINE::CppNode*>(node))          nodeType = CPPNODE;
  else if (dynamic_cast<YACS::ENGINE::SalomeNode*>(node))       nodeType = SALOMENODE;
  else if (dynamic_cast<YACS::ENGINE::SalomePythonNode*>(node)) nodeType = SALOMEPYTHONNODE;
  else if (dynamic_cast<YACS::ENGINE::XmlNode*>(node))          nodeType = XMLNODE;
  else if (dynamic_cast<YACS::ENGINE::ForLoop*>(node))          nodeType = FORLOOP;
  else if (dynamic_cast<YACS::ENGINE::WhileLoop*>(node))        nodeType = WHILELOOP;
  else if (dynamic_cast<YACS::ENGINE::Switch*>(node))           nodeType = SWITCH;
  else if (dynamic_cast<YACS::ENGINE::ForEachLoop*>(node))      nodeType = FOREACHLOOP;
  else if (dynamic_cast<YACS::ENGINE::OptimizerLoop*>(node))    nodeType = OPTIMIZERLOOP;
  return nodeType;
}

// ----------------------------------------------------------------------------

CommandAddNodeFromCatalog::CommandAddNodeFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                     std::string type,
                                                     std::string position,
                                                     std::string name,
                                                     int swCase)
  : Command(), _catalog(catalog), _typeName(type),
    _position(position), _name(name), _swCase(swCase)
{
  if (_catalog->_nodeMap.count(_typeName))
    _typeNode = ProcInvoc::getTypeOfNode(_catalog->_nodeMap[_typeName]);
  else if (_catalog->_composednodeMap.count(_typeName))
    _typeNode = ProcInvoc::getTypeOfNode(_catalog->_composednodeMap[_typeName]);
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
          if (_catalog->_nodeMap.count(_typeName))
            son = _catalog->_nodeMap[_typeName]->clone(0);
          else if (_catalog->_composednodeMap.count(_typeName))
            son = _catalog->_composednodeMap[_typeName]->clone(0);
          else DEBTRACE(_typeName << " not found in catalog");
        }
      if (son)
        {
          TypeOfNode fatherType = ProcInvoc::getTypeOfNode(father);
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
