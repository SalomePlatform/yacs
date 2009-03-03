//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
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
#include "PresetPorts.hxx"
#include "ComponentDefinition.hxx"
#include "SalomeContainer.hxx"
#include "SalomeComponent.hxx"
#include "TypeCode.hxx"
#include "RuntimeSALOME.hxx"
#include "TypeConversions.hxx"

#include "guiContext.hxx"

#include <iostream>
#include <sstream>
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
  _typeNameMap[DATALINK]             = "DATALINK";
  _typeNameMap[CONTROLLINK]          = "CONTROLLINK";
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
      if (father && _nodeToClone)
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
              YASSERT(0);
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

CommandReparentNode::CommandReparentNode(std::string position,
                                         std::string newParent)
  : Command(), _position(position), _newParent(newParent)
{
  DEBTRACE("CommandReparentNode::CommandReparentNode " << _position << " " << _newParent);
}

bool CommandReparentNode::localExecute()
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = 0;
  try
    {
      if (_position == proc->getName())
        throw YACS::Exception("Reparent the proc (main bloc) is impossible");
      node = proc->getChildByName(_position);
      ComposedNode *oldFather = node->getFather();
      ComposedNode *newFather = proc;
      Node *newF = 0;
      if (_newParent != proc->getName())
        {
          newF = proc->getChildByName(_newParent);
          newFather = dynamic_cast<ComposedNode*>(newF);
        }
      if (!newFather)
        throw YACS::Exception("new parent must be a composed node");
      if (oldFather == newFather)
        throw YACS::Exception("no need to reparent to the same parent");
      if (ComposedNode *cnode = dynamic_cast<ComposedNode*>(node))
        if (cnode->isInMyDescendance(newFather))
          throw YACS::Exception("reparent a node to one of it's children is impossible");
      if (Loop *loop = dynamic_cast<Loop*>(newFather))
        if (!loop->edGetDirectDescendants().empty())
          throw YACS::Exception("Already a node in a new parent of Loop type");
      Node *nodeSameName = 0;
      try
        {
          nodeSameName = newFather->getChildByName(node->getName());
        }
      catch (Exception& e)
        {
        }
      if (nodeSameName)
        throw YACS::Exception("there is already a child of same name in the new parent");
      SubjectNode * snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      Subject *subo = GuiContext::getCurrent()->_mapOfSubjectNode[oldFather];
      Subject *subn = GuiContext::getCurrent()->_mapOfSubjectNode[newFather];
      SubjectComposedNode* sop = dynamic_cast<SubjectComposedNode*>(subo);
      SubjectComposedNode* snp = dynamic_cast<SubjectComposedNode*>(subn);
      snode->removeExternalLinks();
      snode->removeExternalControlLinks();
      sop->houseKeepingAfterCutPaste(true, snode);
      oldFather->edRemoveChild(node);
      newFather->edAddChild(node);
      snp->houseKeepingAfterCutPaste(false, snode);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameNode::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      node = 0;
    }
  return (node != 0); 
}

bool CommandReparentNode::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandCopyNode::CommandCopyNode(std::string position,
                                 std::string newParent)
  : Command(), _position(position), _newParent(newParent), _clone(0)
{
  DEBTRACE("CommandCopyNode::CommandCopyNode " << _position << " " << _newParent);
}

YACS::ENGINE::Node *CommandCopyNode::getNode()
{
  return _clone;
}

bool CommandCopyNode::localExecute()
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = 0;
  try
    {
      if (_position == proc->getName())
        throw YACS::Exception("Copy the proc (main bloc) is impossible");
      node = proc->getChildByName(_position);
      ComposedNode *oldFather = node->getFather();
      ComposedNode *newFather = proc;
      Node *newF = 0;
      if (_newParent != proc->getName())
        {
          newF = proc->getChildByName(_newParent);
          newFather = dynamic_cast<ComposedNode*>(newF);
        }
      if (!newFather)
        throw YACS::Exception("new parent must be a composed node");
      if (Loop *loop = dynamic_cast<Loop*>(newFather))
        if (!loop->edGetDirectDescendants().empty())
          throw YACS::Exception("Already a node in a new parent of Loop type");
      //_clone = node->clone(newFather);
      _clone = node->clone(0);
      if (!_clone)
        throw YACS::Exception("Node cannot be cloned");
      int nodeSuffix = 0;
      bool sameName = true;
      stringstream s;
      do
        {
          s.str("");
          s << node->getName() << nodeSuffix;
          DEBTRACE(s.str());
          try
            {
              Node *nodeSameName = newFather->getChildByName(s.str());
            }
          catch (Exception& e)
            {
              sameName = false;
            }
          nodeSuffix++;
        }
      while(sameName);
      _clone->setName(s.str());
      newFather->edAddChild(_clone);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameNode::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      _clone = 0;
    }
  return (_clone != 0); 
}

bool CommandCopyNode::localReverse()
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

CommandRenameContainer::CommandRenameContainer(std::string oldName, std::string newName)
  : Command(), _oldName(oldName), _newName(newName)
{
  DEBTRACE("CommandRenameContainer::CommandRenameContainer " << _oldName << " " << _newName);
}

bool CommandRenameContainer::localExecute()
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  Container *container = 0;
  try
    {
      if (! proc->containerMap.count(_oldName)) return 0;
      container = proc->containerMap[_oldName];
      proc->containerMap.erase(_oldName);
      container->setName(_newName);
      proc->containerMap[_newName] = container;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameContainer::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      container = 0;
    }
  return (container != 0); 
}

bool CommandRenameContainer::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandRenameInDataPort::CommandRenameInDataPort(std::string position,
                                                 std::string oldName,
                                                 std::string newName)
  : Command(), _position(position), _oldName(oldName), _newName(newName)
{
  DEBTRACE("CommandRenameInDataPort::CommandRenameInDataPort "
           << _position << " " << _oldName<< " " << _newName);
}

bool CommandRenameInDataPort::localExecute()
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = proc;
  try
    {
      if (_position != proc->getName()) node = proc->getChildByName(_position);
      InPort * port = 0;

      try
        {
          port = node->getInPort(_newName);
        }
      catch (Exception& e) {} // --- raised when no existing port with _newName
      if (port)
        return false; // --- there is already a port with the new name

      port = node->getInPort(_oldName);
      port->setName(_newName);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameInDataPort::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      node = 0;
    }
  return (node != 0); 
}

bool CommandRenameInDataPort::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandRenameOutDataPort::CommandRenameOutDataPort(std::string position,
                                                   std::string oldName,
                                                   std::string newName)
  : Command(), _position(position), _oldName(oldName), _newName(newName)
{
  DEBTRACE("CommandRenameOutDataPort::CommandRenameOutDataPort "
           << _position << " " << _oldName<< " " << _newName);
}

bool CommandRenameOutDataPort::localExecute()
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = proc;
  try
    {
      if (_position != proc->getName()) node = proc->getChildByName(_position);
      OutPort * port = 0;
      try
        {
          port = node->getOutPort(_newName);
        }
      catch (Exception& e) {} // --- raised when no existing port with _newName
      if (port)
        return false; // --- there is already a port with the new name

      port = node->getOutPort(_oldName);
      port->setName(_newName);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameOutDataPort::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      node = 0;
    }
  return (node != 0); 
}

bool CommandRenameOutDataPort::localReverse()
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
  DEBTRACE("CommandAddInputPortFromCatalog::localExecute");
  InputPort *son = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      ElementaryNode* father = dynamic_cast<ElementaryNode*>(node);
      if (father)
        {
          if (_catalog->_typeMap.count(_typePort))
            son = father->edAddInputPort(_name, _catalog->_typeMap[_typePort]);
          else
            {
              DEBTRACE(_typePort << " not found in catalog " << _catalog);
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
  DEBTRACE("CommandAddOutputPortFromCatalog::localExecute");
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

/*! move up or down a port in the list of ports of a node.
 *  if isUp = 0, move down one step, if isUp = n>0, move up n steps.
 */
CommandOrderInputPorts::CommandOrderInputPorts(std::string node,
                                               std::string port,
                                               int isUp)
  : Command(), _node(node), _port(port), _isUp(isUp), _rank(-1)
{
}

bool CommandOrderInputPorts::localExecute()
{
  DEBTRACE("CommandOrderInputPorts::localExecute " << _node << " " << _port  << " " << _isUp);
  ElementaryNode* father = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      father = dynamic_cast<ElementaryNode*> (node);
      if (!father) return false;
      InputPort *portToMove = father->getInputPort(_port);
      DEBTRACE(portToMove->getName());

      list<InputPort*> plist = father->getSetOfInputPort();
      list<InputPort*>::iterator pos = find(plist.begin(), plist.end(), portToMove);

      if (_isUp)
        {
          if(pos == plist.begin())
            pos=plist.end(); // --- cycle
          else
            do { pos--; _isUp--; } while (_isUp);
        }
      else
        {
          pos++;
          if (pos == plist.end())
            pos = plist.begin(); // --- cycle
          else
            pos++; // --- insert before the 2nd next port
        }

      InputPort *portBefore = 0;
      if (pos != plist.end())
        portBefore = (*pos);

      plist.remove(portToMove);
      if (portBefore)
        {
          DEBTRACE(portBefore->getName());
          pos = find(plist.begin(), plist.end(), portBefore);
          _rank = 0;
          for (list<InputPort*>::iterator it = plist.begin(); it != pos; ++it)
            _rank++;
          plist.insert(pos, portToMove);
        }
      else
        {
          _rank = plist.size();
          plist.push_back(portToMove);
        }
      father->edOrderInputPorts(plist);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandOrderInputPorts::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      father = 0;
    }
  return (father != 0);
}

bool CommandOrderInputPorts::localReverse()
{
}

// ----------------------------------------------------------------------------

/*! move up or down a port in the list of ports of a node.
 *  if isUp = 0, move down one step, if isUp = n>0, move up n steps.
 */
CommandOrderOutputPorts::CommandOrderOutputPorts(std::string node,
                                                 std::string port,
                                                 int isUp)
  : Command(), _node(node), _port(port), _isUp(isUp), _rank(-1)
{
}

bool CommandOrderOutputPorts::localExecute()
{
  DEBTRACE("CommandOrderOutputPorts::localExecute " << _node << " " << _port  << " " << _isUp);
  ElementaryNode* father = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      father = dynamic_cast<ElementaryNode*> (node);
      if (!father) return false;
      OutputPort *portToMove = father->getOutputPort(_port);
      DEBTRACE(portToMove->getName());

      list<OutputPort*> plist = father->getSetOfOutputPort();
      list<OutputPort*>::iterator pos = find(plist.begin(), plist.end(), portToMove);

      if (_isUp)
        {
          if(pos == plist.begin())
            pos=plist.end(); // --- cycle
          else
            do { pos--; _isUp--; } while (_isUp);
        }
      else
        {
          pos++;
          if (pos == plist.end())
            pos = plist.begin(); // --- cycle
          else
            pos++; // --- insert before the 2nd next port
        }

      OutputPort *portBefore = 0;
      if (pos != plist.end())
        portBefore = (*pos);

      plist.remove(portToMove);
      if (portBefore)
        {
          DEBTRACE(portBefore->getName());
          pos = find(plist.begin(), plist.end(), portBefore);
          _rank = 0;
          for (list<OutputPort*>::iterator it = plist.begin(); it != pos; ++it)
            _rank++;
          plist.insert(pos, portToMove);
        }
      else
        {
          _rank = plist.size();
          plist.push_back(portToMove);
        }
      father->edOrderOutputPorts(plist);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandOrderOutputPorts::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      father = 0;
    }
  return (father != 0);
}

bool CommandOrderOutputPorts::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandDestroy::CommandDestroy(std::string position,  Subject* subject)
  : Command(), _position(position), _subject(subject)
{
  DEBTRACE("CommandDestroy::CommandDestroy");
  //todo: store what is needed to recreate the subject
}

bool CommandDestroy::localExecute()
{
  DEBTRACE("CommandDestroy::localExecute");
  try
    {
//       Subject* parent=_subject->getParent();
//       if(parent && _subject->getType() != UNKNOWN) parent->update(REMOVE,_subject->getType(),_subject);

      Subject::erase(_subject);

//       if(parent)
//         parent->update(REMOVE,0,0);

      _subject = 0;
      return true; 
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandDestroy::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandDestroy::localReverse()
{
}
 
// ----------------------------------------------------------------------------

CommandSetInPortValue::CommandSetInPortValue(std::string node,
                                             std::string port,
                                             std::string value)
  : Command(), _node(node), _port(port), _value(value)
{
  DEBTRACE("CommandSetInPortValue::CommandSetInPortValue " << node << " " << port << " " << value);
}
    
bool CommandSetInPortValue::localExecute()
{
  PyObject *result;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      InputPort* inp = node->getInputPort(_port);
      result = YACS::ENGINE::getSALOMERuntime()->convertStringToPyObject(_value.c_str());
      inp->edInit("Python", result);
      Py_DECREF(result);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetInPortValue::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      //Py_DECREF(result);
      return false;
    }
}

bool CommandSetInPortValue::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandSetOutPortValue::CommandSetOutPortValue(std::string node,
                                               std::string port,
                                               std::string value)
  : Command(), _node(node), _port(port), _value(value)
{
  DEBTRACE("CommandSetOutPortValue::CommandSetOutPortValue " << node << " " << port << " " << value);
}
    
bool CommandSetOutPortValue::localExecute()
{
  OutputPresetPort *outpp = 0;
  DataNode *dnode = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      OutputPort* outp = node->getOutputPort(_port);
      outpp = dynamic_cast<OutputPresetPort*>(outp);
      dnode = dynamic_cast<DataNode*>(node);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetOutPortValue::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }

  if (!outpp)
    {
      DEBTRACE("Set value on output port only possible on a presetPort");
      GuiContext::getCurrent()->_lastErrorMessage = "Set value on output port only possible on a presetPort";
      return false;
    }

  if (!dnode)
    {
      DEBTRACE("Set value on output port only possible on a dataNode");
      GuiContext::getCurrent()->_lastErrorMessage = "Set value on output port only possible on a dataNode";
      return false;
    }

  PyObject *result;
  try
    {
      result = YACS::ENGINE::getSALOMERuntime()->convertStringToPyObject(_value.c_str());
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetInPortValue::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      //Py_DECREF(result);
      return false;
    }

  string val;
  PyGILState_STATE gstate = PyGILState_Ensure();
  try
    {
      DEBTRACE(PyObject_Str(result));
      val = convertPyObjectXml(outpp->edGetType(), result);
      DEBTRACE(val);
      dnode->setData(outpp, val );
    }
  catch (Exception& ex)
    {
      PyGILState_Release(gstate);
      DEBTRACE("CommandSetOutPortValue::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      PyGILState_Release(gstate);
      Py_DECREF(result);
      return false;
    }

  Py_DECREF(result);
  PyGILState_Release(gstate);
  return true;
}

bool CommandSetOutPortValue::localReverse()
{
}
 
// ----------------------------------------------------------------------------

CommandSetSwitchSelect::CommandSetSwitchSelect(std::string aSwitch,
                                               std::string value)
  : Command(), _switch(aSwitch), _value(value)
{
  DEBTRACE("CommandSetSwitchSelect::CommandSetSwitchSelect");
}
    
bool CommandSetSwitchSelect::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Switch* aSwitch = dynamic_cast<Switch*>(proc->getChildByName(_switch));
      InputPort *condPort = aSwitch->edGetConditionPort();
      int val = atoi(_value.c_str());
      condPort->edInit(val);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetSwitchSelect::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandSetSwitchSelect::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandSetSwitchCase::CommandSetSwitchCase(std::string aSwitch,
                                           std::string node,
                                           std::string value)
  : Command(), _switch(aSwitch), _node(node), _value(value)
{
  DEBTRACE("CommandSetSwitchCase::CommandSetSwitchCase");
}

bool CommandSetSwitchCase::localExecute()
{
  try
    {
      DEBTRACE("CommandSetSwitchCase::localExecute");
      Proc* proc = GuiContext::getCurrent()->getProc();
      Switch* aSwitch = dynamic_cast<Switch*>(proc->getChildByName(_switch));
      Node* node = proc->getChildByName(_node);
      int val = atoi(_value.c_str());
      if (aSwitch->edGetNode(val))
        {
          throw YACS::Exception("Set Switch Case impossible: value already used");
        }
      int oldVal = aSwitch->getRankOfNode(node);
      Node *aNode = aSwitch->edReleaseCase(oldVal);
      aNode = aSwitch->edSetNode(val, aNode);
      DEBTRACE("CommandSetSwitchCase::localExecute OK " << val);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetSwitchCase::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandSetSwitchCase::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandSetForLoopSteps::CommandSetForLoopSteps(std::string forLoop,
                                               std::string value)
  : Command(), _forLoop(forLoop), _value(value)
{
  DEBTRACE("CommandSetForLoopSteps::CommandSetForLoopSteps");
}

    
bool CommandSetForLoopSteps::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      ForLoop* forLoop = dynamic_cast<ForLoop*>(proc->getChildByName(_forLoop));
      InputPort *nbSteps = forLoop->edGetNbOfTimesInputPort();
      int val = atoi(_value.c_str());
      nbSteps->edInit(val);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetSwitchSelect::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandSetForLoopSteps::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandSetWhileCondition::CommandSetWhileCondition(std::string whileLoop,
                                                   std::string value)
  : Command(), _whileLoop(whileLoop), _value(value)
{
  DEBTRACE("CommandSetWhileCondition::CommandSetWhileCondition");
}

bool CommandSetWhileCondition::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      WhileLoop* whileLoop = dynamic_cast<WhileLoop*>(proc->getChildByName(_whileLoop));
      InputPort *cond = whileLoop->edGetConditionPort();
      bool val = atoi(_value.c_str());
      cond->edInit(val);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetSwitchSelect::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandSetWhileCondition::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandSetForEachBranch::CommandSetForEachBranch(std::string forEach,
                                                 std::string value)
  : Command(), _forEach(forEach), _value(value)
{
  DEBTRACE("CommandSetForEachBranch::CommandSetForEachBranch");
}
    
bool CommandSetForEachBranch::localExecute()
{
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      ForEachLoop* forEach = dynamic_cast<ForEachLoop*>(proc->getChildByName(_forEach));
      InputPort *nbBranches = forEach->getInputPort("nbBranches");
      int val = atoi(_value.c_str());
      nbBranches->edInit(val);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetSwitchSelect::localExecute() : " << ex.what());
      GuiContext::getCurrent()->_lastErrorMessage = ex.what();
      return false;
    }
}

bool CommandSetForEachBranch::localReverse()
{
}

// ----------------------------------------------------------------------------

CommandAddLink::CommandAddLink(std::string outNode, std::string outPort,
                               std::string inNode, std::string inPort,bool control)
  : Command(), _outNode(outNode), _outPort(outPort), _inNode(inNode), _inPort(inPort),_control(control)
{
  DEBTRACE("CommandAddLink::CommandAddLink "<<outNode<<"."<<outPort<<"->"<<inNode<<"."<<inPort<<" "<<control);
}

bool CommandAddLink::localExecute()
{
  DEBTRACE(_outNode<<"."<<_outPort<<"->"<<_inNode<<"."<<_inPort<<" "<<_control);
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
      else if(_control)
        cla->edAddDFLink(outp,inp);
      else
        cla->edAddLink(outp,inp);
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
      return cla->edAddCFLink(outn,inn);
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
              YASSERT(ref);
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
          YASSERT(ref);
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
