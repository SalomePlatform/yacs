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
#include "StudyPorts.hxx"
#include "PresetPorts.hxx"
#include "ComponentDefinition.hxx"
#include "SalomeContainer.hxx"
#include "SalomeHPContainer.hxx"
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


static std::map<int, std::string> createErrorMsgMap()
{
  std::map<int, std::string> m;
  m[1] = "\nUse the [Ctrl] Drag_N_Drop method if you want to create a input/output link without the associated control link";
  return m;
}

std::map<int, std::string> ErrorMsg = createErrorMsgMap();

void setErrorMsg(YACS::Exception& ex)
{
  DEBTRACE("errorNumber= "<<ex.errNumber);
  if(ex.errNumber > 0 && ErrorMsg.count(ex.errNumber) != 0)
    {
      DEBTRACE(ErrorMsg[ex.errNumber]);
      GuiContext::getCurrent()->_lastErrorMessage = ex.what() + ErrorMsg[ex.errNumber];
    }
  else
    GuiContext::getCurrent()->_lastErrorMessage = ex.what();
}

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
                                                     bool newCompoInst,
                                                     int swCase)
  : Command(), _catalog(catalog), _compoName(compo), _typeName(type),
    _position(position), _name(name), _newCompoInst(newCompoInst), _swCase(swCase)
{
  DEBTRACE("CommandAddNodeFromCatalog " << compo << " " << type <<  " " << position <<  " " << name);
  _node=0;
  _snode=0;
}

std::string CommandAddNodeFromCatalog::dump()
{
  string ret ="CommandAddNodeFromCatalog " + _compoName + " " + _typeName + " " +  _position + " " + _name;
  return ret;
}

YACS::ENGINE::Node *CommandAddNodeFromCatalog::getNode()
{
  return _node;
}

YACS::HMI::SubjectNode *CommandAddNodeFromCatalog::getSubjectNode()
{
  return _snode;
}

bool CommandAddNodeFromCatalog::localExecute()
{
  DEBTRACE("CommandAddNodeFromCatalog::localExecute");

  Node *son = 0;
  Node *nodeToClone = 0;
  try
    {
      if (_compoName.empty())
        {
          if (_catalog->_nodeMap.count(_typeName))
            nodeToClone = _catalog->_nodeMap[_typeName];
          else if (_catalog->_composednodeMap.count(_typeName))
            nodeToClone = _catalog->_composednodeMap[_typeName];
        }
      else
        if (_catalog->_componentMap.count(_compoName))
          {
            YACS::ENGINE::ComponentDefinition* compodef = _catalog->_componentMap[_compoName];
            if (compodef->_serviceMap.count(_typeName))
              nodeToClone = compodef->_serviceMap[_typeName];
          }
      if (nodeToClone)
        _typeNode = ProcInvoc::getTypeOfNode(nodeToClone);

      GuiContext::getCurrent()->setCurrentCatalog(_catalog);
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc;
      ServiceNode *service = 0;

      if (!_position.empty()) node = proc->getChildByName(_position);
      ComposedNode* father =dynamic_cast<ComposedNode*> (node);
      if (father && nodeToClone)
        {
          son = nodeToClone->clone(0);
          son->setName(_name);
          service = dynamic_cast<ServiceNode*>(son);
        }

      // Node creation eventually reusing old component instance
      ComponentInstance *compo = 0;
      if (service)
        compo = service->getComponent();

      if(compo)
        {
          std::string compoName=compo->getCompoName();
          DEBTRACE(compoName);
          std::string compoInstName=compo->getInstanceName();
          DEBTRACE(compoInstName);
          if(!_newCompoInst)
            {
              ComponentInstance *lastcompo = GuiContext::getCurrent()->_mapOfLastComponentInstance[compoName];
              DEBTRACE(lastcompo);
              if(lastcompo)
                {
                  DEBTRACE(lastcompo->getInstanceName());
                  service->setComponent(lastcompo); // use the last component instance of the same type and not a new instance
                }
              else
                GuiContext::getCurrent()->_mapOfLastComponentInstance[compoName]=compo;
            }
          else
            GuiContext::getCurrent()->_mapOfLastComponentInstance[compoName]=compo;
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
      if (!_node)
        throw YACS::Exception("node from catalog is not correct: check if catalog is up to date!");

      SubjectComposedNode *sfather = dynamic_cast<SubjectComposedNode*>(GuiContext::getCurrent()->_mapOfSubjectNode[father]);
      _snode= sfather->addSubjectNode(_node, "", _catalog, _compoName, _typeName);
      _snode->loadChildren();
      _snode->loadLinks();
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddNode::localExecute() : " << ex.what());
      setErrorMsg(ex);
      if (son) delete son;
      _node = 0;
    }
  return (_node != 0);
}

bool CommandAddNodeFromCatalog::localReverse()
{
  DEBTRACE("CommandAddNodeFromCatalog::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      string nodeName;
      if (_position.empty())
        nodeName= _name;
      else
        nodeName = _position + "." + _name;
      DEBTRACE(nodeName);
      _node = proc->getChildByName(nodeName);
      YASSERT(_node);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(_node));
      _snode = GuiContext::getCurrent()->_mapOfSubjectNode[_node];

      Subject *father=_snode->getParent();
      DEBTRACE(father);
      DEBTRACE(father->getName());
      Subject::erase(_snode);
      _snode= 0;
      _node = 0;
      if (father)
        {
          DEBTRACE("REMOVE");
          father->select(true);
          father->update(REMOVE,0,0);
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddNodeFromCatalog::localReverse(): " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandReparentNode::CommandReparentNode(std::string position,
                                         std::string newParent)
  : Command(), _position(position), _newParent(newParent)
{
  DEBTRACE("CommandReparentNode::CommandReparentNode " << _position << " " << _newParent);
  _oldParent = "";
}

std::string CommandReparentNode::dump()
{
  string ret ="CommandReparentNode " + _position + " " + _newParent;
  return ret;
}

bool CommandReparentNode::localExecute()
{
  DEBTRACE("CommandReparentNode::localExecute");
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
      if (DynParaLoop * dpl = dynamic_cast<DynParaLoop*>(newFather))
        if (dpl->getExecNode() != NULL)
          throw YACS::Exception("Already an execution node in the new parent of type dynamic loop");
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
      //save existing links
      snode->saveLinks();
      snode->removeExternalLinks();
      snode->removeExternalControlLinks();
      sop->houseKeepingAfterCutPaste(true, snode);
      oldFather->edRemoveChild(node);
      newFather->edAddChild(node);
      snp->houseKeepingAfterCutPaste(false, snode);
      //restore links
      snode->restoreLinks();
      if (oldFather == proc) _oldParent = proc->getName();
      else _oldParent = proc->getChildName(oldFather);
      _newpos = proc->getChildName(node);
      sop->update(CUT, ProcInvoc::getTypeOfNode(node), snode);
      snp->update(PASTE, ProcInvoc::getTypeOfNode(node), snode);
      snode->recursiveUpdate(RENAME, 0, snode);
      snode->_parent = snp;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandReparentNode::localExecute() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }
  catch (...)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "Unknown exception";
      node = 0;
    }
  return (node != 0); 
}

bool CommandReparentNode::localReverse()
{
  DEBTRACE("CommandReparentNode::localReverse " << _newpos << " " << _oldParent);
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = 0;
  try
    {
      node = proc->getChildByName(_newpos);
      ComposedNode *father = node->getFather();
      ComposedNode *oldFather = proc;
      Node *oldF = 0;
      if (_oldParent != proc->getName())
        {
          oldF = proc->getChildByName(_oldParent);
          oldFather = dynamic_cast<ComposedNode*>(oldF);
        }
      SubjectNode * snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      Subject *subn = GuiContext::getCurrent()->_mapOfSubjectNode[father];
      Subject *subo = GuiContext::getCurrent()->_mapOfSubjectNode[oldFather];
      SubjectComposedNode* snp = dynamic_cast<SubjectComposedNode*>(subn);
      SubjectComposedNode* sop = dynamic_cast<SubjectComposedNode*>(subo);
      //save existing links
      snode->saveLinks();
      snode->removeExternalLinks();
      snode->removeExternalControlLinks();
      snp->houseKeepingAfterCutPaste(true, snode);
      father->edRemoveChild(node);
      oldFather->edAddChild(node);
      sop->houseKeepingAfterCutPaste(false, snode);
      //restore links
      snode->restoreLinks();
      snp->update(CUT, ProcInvoc::getTypeOfNode(node), snode);
      sop->update(PASTE, ProcInvoc::getTypeOfNode(node), snode);
      snode->recursiveUpdate(RENAME, 0, snode);
      snode->_parent = sop;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandReparentNode::localReverse() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }
  catch (...)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "Unknown exception";
      node = 0;
    }
  return (node != 0); 
}

// ----------------------------------------------------------------------------
CommandPutInComposedNode::CommandPutInComposedNode(std::string position,
                                                   std::string newParent,
						   std::string type,
						   bool toSaveRestoreLinks)
  : Command(), _position(position), _newParent(newParent), _type(type), _toSaveRestoreLinks(toSaveRestoreLinks)
{
  DEBTRACE("CommandPutInComposedNode::CommandPutInComposedNode " << _position << " " << _newParent);
  _newpos ="";
}

std::string CommandPutInComposedNode::dump()
{
  string save = _toSaveRestoreLinks ? "true" : "false";
  string ret ="CommandPutInComposedNode " + _position + " " + _newParent + " " + _type + " " + save;
  return ret;
}


bool CommandPutInComposedNode::localExecute()
{
  DEBTRACE("CommandPutInComposedNode::localExecute");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = 0;
  try
    {
      if (_position == proc->getName())
        throw YACS::Exception("You cannot put the proc (main bloc) in a " + _type);
      node = proc->getChildByName(_position);
      ComposedNode *oldFather = node->getFather();
      SubjectNode * snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      Subject *subo = GuiContext::getCurrent()->_mapOfSubjectNode[oldFather];
      SubjectComposedNode* sop = dynamic_cast<SubjectComposedNode*>(subo);
      if (_toSaveRestoreLinks)
	snode->saveLinks(); //save existing links
      //remove external links
      snode->removeExternalLinks();
      snode->removeExternalControlLinks();
      //remove subject node from subject old father
      sop->houseKeepingAfterCutPaste(true, snode);
      //remove node from old father
      oldFather->edRemoveChild(node);
      //refresh node views
      sop->update(CUT, ProcInvoc::getTypeOfNode(node), snode);

      // try to find a node with the given name:
      //   success: use it as target composed node
      //   fail:    create such a node and use it
      std::list<Node*> children = proc->getChildren();
      Node* composednode = 0;
      SubjectNode *scomposednode = 0;
      for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
	{
	  if ( _newParent == (*it)->getName() )
	    {
	      //get an existing ComposedNode with name _newParent
	      composednode = (*it);
	      break;
	    }
	}
      // target node was found
      if ( composednode )
	{
	  scomposednode = GuiContext::getCurrent()->_mapOfSubjectNode[composednode];
	}
      // creation of target node
      else 
	{
	  //create a ComposedNode (type _type) with name _newParent
	  YACS::ENGINE::Catalog *catalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();
	  Node* nodeToClone = catalog->_composednodeMap[_type];
	  composednode = nodeToClone->cloneWithoutCompAndContDeepCpy(0);
	  composednode->setName(_newParent);
	  //add the new composednode as child of oldfather
	  oldFather->edAddChild(composednode);
	  //create the subject composednode
	  scomposednode = sop->addSubjectNode(composednode,"",catalog,"",_type);
	}

      //add the old node as child of new composednode
      (dynamic_cast<YACS::ENGINE::ComposedNode*>(composednode))->edAddChild(node);
      _newpos = proc->getChildName(node);
      //add the subject node to subject composednode
      (dynamic_cast<SubjectComposedNode*>(scomposednode))->houseKeepingAfterCutPaste(false, snode);
      snode->setParent(scomposednode);
      if (_toSaveRestoreLinks)
	snode->restoreLinks(); //restore links
      //refresh all views
      scomposednode->update(PASTE, ProcInvoc::getTypeOfNode(node), snode);
      snode->recursiveUpdate(RENAME, 0, snode);
      snode->select(true);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandPutInComposedNode::localExecute() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }
  catch (...)
    {
      GuiContext::getCurrent()->_lastErrorMessage = "Unknown exception";
      node = 0;
    }
  return (node != 0);
}

bool CommandPutInComposedNode::localReverse()
{
  DEBTRACE("CommandPutInComposedNode::localReverse");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = 0;
  try
    {
      YASSERT(_newpos != proc->getName())
      node = proc->getChildByName(_newpos);
      ComposedNode *oldFather = node->getFather();
      SubjectNode * snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      Subject *subo = GuiContext::getCurrent()->_mapOfSubjectNode[oldFather];
      SubjectComposedNode* sop = dynamic_cast<SubjectComposedNode*>(subo);
      //save existing links
      snode->saveLinks();
      //remove external links
      snode->removeExternalLinks();
      snode->removeExternalControlLinks();
      //remove subject node from subject old father
      sop->houseKeepingAfterCutPaste(true, snode);
      //remove node from old father
      oldFather->edRemoveChild(node);
      //refresh node views, temporary paste in proc to keep widgets associated to node
      sop->update(CUT, ProcInvoc::getTypeOfNode(node), snode);
      GuiContext::getCurrent()->getSubjectProc()->update(PASTE, ProcInvoc::getTypeOfNode(node), snode);

      //remove composed node oldFather
      ComposedNode *oldGrandFather = oldFather->getFather();
      Subject *subog = GuiContext::getCurrent()->_mapOfSubjectNode[oldGrandFather];
      SubjectComposedNode* sogp = dynamic_cast<SubjectComposedNode*>(subog);
      Subject::erase(sop);

       //add the old node as child of new composednode
      oldGrandFather->edAddChild(node);
      _newpos = proc->getChildName(node);
      //add the subject node to subject composednode
      sogp->houseKeepingAfterCutPaste(false, snode);
      snode->setParent(sogp);
      //restore links
      snode->restoreLinks();
      //refresh all views
      GuiContext::getCurrent()->getSubjectProc()->update(CUT, ProcInvoc::getTypeOfNode(node), snode);
      sogp->update(PASTE, ProcInvoc::getTypeOfNode(node), snode);
      snode->recursiveUpdate(RENAME, 0, snode);
      snode->select(true);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandPutInComposedNode::localReverse() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }      
  return (node != 0);
}
// ----------------------------------------------------------------------------

CommandCopyNode::CommandCopyNode(YACS::ENGINE::Proc *fromproc,
                                 std::string position,
                                 std::string newParent,
                                 int swCase)
  : Command(), _fromproc(fromproc), _position(position), _newParent(newParent), _newName(""), _clone(0), _case(swCase)
{
  DEBTRACE("CommandCopyNode::CommandCopyNode " << _position << " " << _newParent);
}

std::string CommandCopyNode::dump()
{
  string ret ="CommandCopyNode " + _position + " " + _newParent;
  return ret;
}

YACS::ENGINE::Node *CommandCopyNode::getNode()
{
  return _clone;
}

bool CommandCopyNode::localExecute()
{
  DEBTRACE("CommandCopyNode::localExecute");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = 0;
  try
    {
      if (_position == _fromproc->getName())
        throw YACS::Exception("Copy the proc (main bloc) is impossible");
      node = _fromproc->getChildByName(_position);
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
      //_clone = node->cloneWithoutCompAndContDeepCpy(newFather);
      _clone = node->cloneWithoutCompAndContDeepCpy(0);
      if (!_clone)
        throw YACS::Exception("Node cannot be cloned");
      int nodeSuffix = -1;
      bool sameName = true;
      stringstream s;
      do
        {
          s.str("");
          s << node->getName();
          if (nodeSuffix >= 0) // --- first try <0 means without suffix
            s << nodeSuffix;
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
      _newName = _clone->getName();

      if (YACS::ENGINE::Switch* theswitch = dynamic_cast<YACS::ENGINE::Switch*>(newFather))
        {
          DEBTRACE("father is a switch " << newFather->getName());
          int theCase=_case;
          if(theswitch->edGetNode(_case))
            {
              //the case is already used. Try another one
              theCase=theswitch->getMaxCase()+1;
            }
          theswitch->edSetNode(theCase,_clone);
        }
      else
        newFather->edAddChild(_clone);

      _newName = _clone->getQualifiedName();
      DEBTRACE(_newName);

      SubjectNode *sub = GuiContext::getCurrent()->_mapOfSubjectNode[newFather];
      SubjectComposedNode *snp = dynamic_cast<SubjectComposedNode*>(sub);
      SubjectNode *son = snp->addSubjectNode(_clone);
      son->loadChildren();
      son->loadLinks();
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandCopyNode::localExecute() : " << ex.what());
      setErrorMsg(ex);
      _clone = 0;
    }
  return (_clone != 0); 
}

bool CommandCopyNode::localReverse()
{
  DEBTRACE("CommandCopyNode::localReverse " << _position << " " << _newParent);
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      string nodeName = _newParent + "." + _newName;
      if (_newParent == proc->getName())
        nodeName = _newName;
      DEBTRACE(nodeName);
      _clone = proc->getChildByName(nodeName);

      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(_clone));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[_clone];
      Subject *father = snode->getParent();
      Subject::erase(snode);
      _clone = 0;
      if (father)
        {
          DEBTRACE("REMOVE");
          father->select(true);
          father->update(REMOVE,0,0);
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandCopyNode::localReverse(): " << ex.what());
      setErrorMsg(ex);
      return false;
    }
  return true;
}

// ----------------------------------------------------------------------------

CommandRenameNode::CommandRenameNode(std::string position, std::string name)
  : Command(), _position(position), _name(name)
{
  DEBTRACE("CommandRenameNode::CommandRenameNode " << _position << " " << _name);
  _oldName ="";
  _newpos="";
}

std::string CommandRenameNode::dump()
{
  string ret ="CommandRenameNode " + _position + " " + _name;
  return ret;
}

bool CommandRenameNode::localExecute()
{
  DEBTRACE("CommandRenameNode::localExecute");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = proc;
  try
    {
      if (_position != proc->getName()) node = proc->getChildByName(_position);
      _oldName = node->getName();
      node->setName(_name);
      if (node == proc)
        _newpos = _name;
      else
        _newpos = proc->getChildName(node);
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      if (snode)
        snode->recursiveUpdate(RENAME, 0, snode);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameNode::localExecute() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }
  return (node != 0); 
}

bool CommandRenameNode::localReverse()
{
  DEBTRACE("CommandRenameNode::localReverse");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = proc;
  try
    {
      if (_newpos != proc->getName()) node = proc->getChildByName(_newpos);
      YASSERT(node->getName() == _name);
      node->setName(_oldName);
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      if (snode)
        snode->recursiveUpdate(RENAME, 0, snode);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameNode::localReverse() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }
  return (node != 0); 
}

// ----------------------------------------------------------------------------

CommandRenameContainer::CommandRenameContainer(std::string oldName, std::string newName)
  : Command(), _oldName(oldName), _newName(newName)
{
  DEBTRACE("CommandRenameContainer::CommandRenameContainer " << _oldName << " " << _newName);
}

std::string CommandRenameContainer::dump()
{
  string ret ="CommandRenameContainer " +_oldName + " " + _newName;
  return ret;
}

bool CommandRenameContainer::localExecute()
{
  DEBTRACE("CommandRenameContainer::localExecute");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Container *container = 0;
  try
    {
      YASSERT(proc->containerMap.count(_oldName));
      container = proc->containerMap[_oldName];
      if (proc->containerMap.count(_newName))
        {
          GuiContext::getCurrent()->_lastErrorMessage = "Container name already existing";
          return 0;
        }
      proc->containerMap.erase(_oldName);
      container->setName(_newName);
      proc->containerMap[_newName] = container;
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(container));
      SubjectContainerBase *scont(GuiContext::getCurrent()->_mapOfSubjectContainer[container]);
      scont-> update(RENAME, 0, scont);
      scont->notifyComponentsChange(ASSOCIATE, CONTAINER, scont);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameContainer::localExecute() : " << ex.what());
      setErrorMsg(ex);
      container = 0;
    }
  return (container != 0); 
}

bool CommandRenameContainer::localReverse()
{
  DEBTRACE("CommandRenameContainer::localReverse");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Container *container = 0;
  try
    {
      YASSERT(proc->containerMap.count(_newName));
      container = proc->containerMap[_newName];
      proc->containerMap.erase(_newName);
      container->setName(_oldName);
      proc->containerMap[_oldName] = container;
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(container));
      SubjectContainerBase *scont(GuiContext::getCurrent()->_mapOfSubjectContainer[container]);
      scont-> update(RENAME, 0, scont);
      scont->notifyComponentsChange(ASSOCIATE, CONTAINER, scont);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameContainer::localReverse() : " << ex.what());
      setErrorMsg(ex);
      container = 0;
    }
  return (container != 0); 
}

// ----------------------------------------------------------------------------

CommandRenameInDataPort::CommandRenameInDataPort(std::string position,
                                                 std::string oldName,
                                                 std::string newName, TypeOfElem portType)
  : Command(), _position(position), _oldName(oldName), _newName(newName), _portType(portType)
{
  DEBTRACE("CommandRenameInDataPort::CommandRenameInDataPort "
           << _position << " " << _oldName<< " " << _newName);
}

std::string CommandRenameInDataPort::dump()
{
  string ret ="CommandRenameInDataPort " + _position + " " + _oldName + " " + _newName;
  return ret;
}

bool CommandRenameInDataPort::localExecute()
{
  DEBTRACE("CommandRenameInDataPort::localExecute");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = proc;
  try
    {
      if (_position != proc->getName()) node = proc->getChildByName(_position);
      InPort * port = 0;

      try
        {
          if(_portType==INPUTPORT)
            port = node->getInputPort(_newName);
          else
            port = node->getInputDataStreamPort(_newName);
        }
      catch (Exception& e) {} // --- raised when no existing port with _newName
      if (port)
        throw Exception("there is already a port with the new name");

      if(_portType==INPUTPORT)
        port = node->getInputPort(_oldName);
      else
        port = node->getInputDataStreamPort(_oldName);
      port->setName(_newName);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(port));
      SubjectDataPort *sport = GuiContext::getCurrent()->_mapOfSubjectDataPort[port];
      sport->update(RENAME, 0, sport);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameInDataPort::localExecute() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }
  return (node != 0); 
}

bool CommandRenameInDataPort::localReverse()
{
  DEBTRACE("CommandRenameInDataPort::localReverse");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = proc;
  try
    {
      if (_position != proc->getName()) node = proc->getChildByName(_position);
      InPort * port = 0;

      try
        {
          if(_portType==INPUTPORT)
            port = node->getInputPort(_oldName);
          else
            port = node->getInputDataStreamPort(_oldName);
        }
      catch (Exception& e) {} // --- raised when no existing port with _newName
      if (port)
        throw Exception("there is already a port with the old name");

      if(_portType==INPUTPORT)
        port = node->getInputPort(_newName);
      else
        port = node->getInputDataStreamPort(_newName);
      port->setName(_oldName);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(port));
      SubjectDataPort *sport = GuiContext::getCurrent()->_mapOfSubjectDataPort[port];
      sport->update(RENAME, 0, sport);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameInDataPort::localReverse() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }
  return (node != 0); 
}

// ----------------------------------------------------------------------------

CommandRenameOutDataPort::CommandRenameOutDataPort(std::string position,
                                                   std::string oldName,
                                                   std::string newName, TypeOfElem portType)
  : Command(), _position(position), _oldName(oldName), _newName(newName), _portType(portType)
{
  DEBTRACE("CommandRenameOutDataPort::CommandRenameOutDataPort "
           << _position << " " << _oldName<< " " << _newName);
}

std::string CommandRenameOutDataPort::dump()
{
  string ret ="CommandRenameOutDataPort "  + _position + " " + _oldName + " " + _newName;
  return ret;
}

bool CommandRenameOutDataPort::localExecute()
{
  DEBTRACE("CommandRenameOutDataPort::localExecute");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = proc;
  try
    {
      if (_position != proc->getName()) node = proc->getChildByName(_position);
      OutPort * port = 0;
      try
        {
          if(_portType==OUTPUTPORT)
            port = node->getOutputPort(_newName);
          else
            port = node->getOutputDataStreamPort(_newName);
        }
      catch (Exception& e) {} // --- raised when no existing port with _newName
      if (port)
        throw Exception("there is already a port with the new name");

      if(_portType==OUTPUTPORT)
        port = node->getOutputPort(_oldName);
      else
        port = node->getOutputDataStreamPort(_oldName);
      port->setName(_newName);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(port));
      SubjectDataPort *sport = GuiContext::getCurrent()->_mapOfSubjectDataPort[port];
      sport->update(RENAME, 0, sport);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameOutDataPort::localExecute() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }
  return (node != 0); 
}

bool CommandRenameOutDataPort::localReverse()
{
  DEBTRACE("CommandRenameOutDataPort::localReverse");
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* node = proc;
  try
    {
      if (_position != proc->getName()) node = proc->getChildByName(_position);
      OutPort * port = 0;
      try
        {
          if(_portType==OUTPUTPORT)
            port = node->getOutputPort(_oldName);
          else
            port = node->getOutputDataStreamPort(_oldName);
        }
      catch (Exception& e) {} // --- raised when no existing port with _newName
      if (port)
        throw Exception("there is already a port with the old name");

      if(_portType==OUTPUTPORT)
        port = node->getOutputPort(_newName);
      else
        port = node->getOutputDataStreamPort(_newName);
      port->setName(_oldName);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(port));
      SubjectDataPort *sport = GuiContext::getCurrent()->_mapOfSubjectDataPort[port];
      sport->update(RENAME, 0, sport);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandRenameOutDataPort::localReverse() : " << ex.what());
      setErrorMsg(ex);
      node = 0;
    }
  return (node != 0); 
}

// ----------------------------------------------------------------------------

CommandAddDataTypeFromCatalog::CommandAddDataTypeFromCatalog(YACS::ENGINE::Catalog* catalog,
                                                             std::string typeName)
  : Command(), _catalog(catalog), _typeName(typeName)
{
  DEBTRACE("CommandAddDataTypeFromCatalog::CommandAddDataTypeFromCatalog: " << typeName);
}

std::string CommandAddDataTypeFromCatalog::dump()
{
  string ret ="CommandAddDataTypeFromCatalog " + _typeName;
  return ret;
}

YACS::ENGINE::TypeCode *CommandAddDataTypeFromCatalog::getTypeCode()
{

  if (GuiContext::getCurrent()->getProc()->typeMap.count(_typeName))
    return GuiContext::getCurrent()->getProc()->typeMap[_typeName];
  else return 0;
}

bool CommandAddDataTypeFromCatalog::localExecute()
{
  DEBTRACE("CommandAddDataTypeFromCatalog::localExecute");
  Proc* proc = GuiContext::getCurrent()->getProc();
  if (proc->typeMap.count(_typeName))
    {
      DEBTRACE("typecode already existing in proc: " << _typeName);
      GuiContext::getCurrent()->_lastErrorMessage = "typeCode already existing in proc: " + _typeName;
      return false;
    }
  else
    if (_catalog->_typeMap.count(_typeName))
      {
        DEBTRACE("typecode found in catalog, cloned: " << _typeName);
        proc->typeMap[_typeName] = _catalog->_typeMap[_typeName]->clone();
        proc->typeMap[_typeName]->incrRef();
        SubjectProc *sproc = GuiContext::getCurrent()->getSubjectProc();
        SubjectDataType *son = sproc->addSubjectDataType(getTypeCode(), _typeName);
        return (son!=0);
      }
  GuiContext::getCurrent()->_lastErrorMessage = "typecode not found in catalog: " + _typeName;
  return false;
}

bool CommandAddDataTypeFromCatalog::localReverse()
{
  DEBTRACE("CommandAddDataTypeFromCatalog::localReverse");
  try
    {
      SubjectProc *sproc = GuiContext::getCurrent()->getSubjectProc();
      sproc->removeSubjectDataType(_typeName);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddDataTypeFromCatalog::localReverse(): " << ex.what());
      setErrorMsg(ex);
      return 0;
    }
}


// ----------------------------------------------------------------------------

CommandAddInputPortFromCatalog::CommandAddInputPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                               std::string type,
                                                               std::string node,
                                                               std::string name)
  : Command(), _catalog(catalog), _typePort(type), _node(node), _name(name)
{
  _inputPort = 0;
  _sip = 0;
}

std::string CommandAddInputPortFromCatalog::dump()
{
  string ret ="CommandAddInputPortFromCatalog " + _typePort + " " + _node + " " + _name;
  return ret;
}

YACS::ENGINE::InputPort *CommandAddInputPortFromCatalog::getInputPort()
{
  return _inputPort;
}
 
SubjectInputPort* CommandAddInputPortFromCatalog::getSubjectInputPort()
{
  return _sip;
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
          //try proc types and then catalog if not in proc
          if(proc->typeMap.count(_typePort))
            son = father->edAddInputPort(_name, proc->typeMap[_typePort]);
          else if (_catalog->_typeMap.count(_typePort))
            son = father->edAddInputPort(_name, _catalog->_typeMap[_typePort]);
          else
            {
              DEBTRACE(_typePort << " not found in catalog " << _catalog);
              GuiContext::getCurrent()->_lastErrorMessage = _typePort + " not found in catalog";
            }
        }
      _inputPort = son;
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      _sip = snode->addSubjectInputPort(son, _name);
      snode->update(SYNCHRO,0,0); // when output port tab in node edition is visible
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddInputPortFromCatalog::localExecute() : " << ex.what());
      setErrorMsg(ex);
      if (son) delete son;
      _inputPort = 0;
    }
  return (_inputPort != 0);
}

bool CommandAddInputPortFromCatalog::localReverse()
{
  DEBTRACE("CommandAddInputPortFromCatalog::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node *node = proc->getChildByName(_node);
      ElementaryNode* enode = dynamic_cast<ElementaryNode*>(node);
      YASSERT(enode);      
      _inputPort = enode->getInputPort(_name);
      YASSERT(_inputPort);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(_inputPort));
      _sip = dynamic_cast<SubjectInputPort*>(GuiContext::getCurrent()->_mapOfSubjectDataPort[_inputPort]);
      YASSERT(_sip);

      Subject *father=_sip->getParent();
      Subject::erase(_sip);
      _sip = 0;
      _inputPort = 0;
      if (father)
        {
          DEBTRACE("REMOVE");
          father->select(true);
          father->update(REMOVE,0,0);
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddInputPortFromCatalog::localReverse(): " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandAddOutputPortFromCatalog::CommandAddOutputPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                                 std::string type,
                                                                 std::string node,
                                                                 std::string name)
  : Command(), _catalog(catalog), _typePort(type), _node(node), _name(name)
{
  _outputPort = 0;
  _sop = 0;
}

std::string CommandAddOutputPortFromCatalog::dump()
{
  string ret ="CommandAddOutputPortFromCatalog " + _typePort + " " + _node + " " + _name;
  return ret;
}

YACS::ENGINE::OutputPort *CommandAddOutputPortFromCatalog::getOutputPort()
{
  return _outputPort;
}

SubjectOutputPort* CommandAddOutputPortFromCatalog::getSubjectOutputPort()
{
  return _sop;
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
          //try proc types and then catalog if not in proc
          if(proc->typeMap.count(_typePort))
            son = father->edAddOutputPort(_name, proc->typeMap[_typePort]);
          else if (_catalog->_typeMap.count(_typePort))
            son = father->edAddOutputPort(_name, _catalog->_typeMap[_typePort]);
          else
            {
              DEBTRACE(_typePort << " not found in catalog");
              GuiContext::getCurrent()->_lastErrorMessage = _typePort + " not found in catalog";
            }
        }
      _outputPort = son;
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      _sop = snode->addSubjectOutputPort(son, _name);
      snode->update(SYNCHRO,0,0); // when input port tab in node edition is visible
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddOutputPortFromCatalog::localExecute() : " << ex.what());
      setErrorMsg(ex);
      if (son) delete son;
      _outputPort = 0;
    }
  return (_outputPort != 0);
}

bool CommandAddOutputPortFromCatalog::localReverse()
{
  DEBTRACE("CommandAddOutputPortFromCatalog::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node *node = proc->getChildByName(_node);
      ElementaryNode* enode = dynamic_cast<ElementaryNode*>(node);
      YASSERT(enode);      
      _outputPort = enode->getOutputPort(_name);
      YASSERT(_outputPort);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(_outputPort));
      _sop = dynamic_cast<SubjectOutputPort*>(GuiContext::getCurrent()->_mapOfSubjectDataPort[_outputPort]);
      YASSERT(_sop);

      Subject *father=_sop->getParent();
      Subject::erase(_sop);
      _sop = 0;
      _outputPort = 0;
      if (father)
        {
          DEBTRACE("REMOVE");
          father->select(true);
          father->update(REMOVE,0,0);
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddOutputPortFromCatalog::localReverse(): " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandAddIDSPortFromCatalog::CommandAddIDSPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                           std::string type,
                                                           std::string node,
                                                           std::string name)
  : Command(), _catalog(catalog), _typePort(type), _node(node), _name(name)
{
  _IDSPort = 0;
  _sip = 0;
}

std::string CommandAddIDSPortFromCatalog::dump()
{
  string ret ="CommandAddIDSPortFromCatalog " + _typePort + " " + _node + " " + _name;
  return ret;
}

YACS::ENGINE::InputDataStreamPort *CommandAddIDSPortFromCatalog::getIDSPort()
{
  DEBTRACE("CommandAddIDSPortFromCatalog");
  return _IDSPort;
}

SubjectInputDataStreamPort* CommandAddIDSPortFromCatalog::getSubjectIDSPort()
{
  return _sip;
}

bool CommandAddIDSPortFromCatalog::localExecute()
{
  DEBTRACE("CommandAddIDSPortFromCatalog::localExecute");
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
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      _sip = snode->addSubjectIDSPort(son, _name);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddIDSPortFromCatalog::localExecute() : " << ex.what());
      setErrorMsg(ex);
      if (son) delete son;
      _IDSPort = 0;
    }
  return (_IDSPort != 0);
}

bool CommandAddIDSPortFromCatalog::localReverse()
{
  DEBTRACE("CommandAddIDSPortFromCatalog::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node *node = proc->getChildByName(_node);
      ElementaryNode* enode = dynamic_cast<ElementaryNode*>(node);
      YASSERT(enode);      
      _IDSPort = enode->getInputDataStreamPort(_name);
      YASSERT(_IDSPort);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(_IDSPort));
      _sip = dynamic_cast<SubjectInputDataStreamPort*>(GuiContext::getCurrent()->_mapOfSubjectDataPort[_IDSPort]);
      YASSERT(_sip);

      Subject *father=_sip->getParent();
      Subject::erase(_sip);
      _IDSPort =0;
      _sip = 0;
      if (father)
        {
          DEBTRACE("REMOVE");
          father->select(true);
          father->update(REMOVE,0,0);
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddIDSPortFromCatalog::localReverse(): " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}
 
// ----------------------------------------------------------------------------

CommandAddODSPortFromCatalog::CommandAddODSPortFromCatalog(YACS::ENGINE::Catalog *catalog,
                                                           std::string type,
                                                           std::string node,
                                                           std::string name)
  : Command(), _catalog(catalog), _typePort(type), _node(node), _name(name)
{
  DEBTRACE("CommandAddODSPortFromCatalog");
  _ODSPort = 0;
  _sop = 0;
}

std::string CommandAddODSPortFromCatalog::dump()
{
  string ret ="CommandAddODSPortFromCatalog " + _typePort + " " + _node + " " + _name;
  return ret;
}

YACS::ENGINE::OutputDataStreamPort *CommandAddODSPortFromCatalog::getODSPort()
{
  return _ODSPort;
}

SubjectOutputDataStreamPort* CommandAddODSPortFromCatalog::getSubjectODSPort()
{
  return _sop;
}

bool CommandAddODSPortFromCatalog::localExecute()
{
  DEBTRACE("CommandAddODSPortFromCatalog::localExecute");
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
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      _sop = snode->addSubjectODSPort(son, _name);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddODSPortFromCatalog::localExecute() : " << ex.what());
      setErrorMsg(ex);
      if (son) delete son;
      _ODSPort = 0;
    }
  return (_ODSPort != 0);
}

bool CommandAddODSPortFromCatalog::localReverse()
{
  DEBTRACE("CommandAddODSPortFromCatalog::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node *node = proc->getChildByName(_node);
      ElementaryNode* enode = dynamic_cast<ElementaryNode*>(node);
      YASSERT(enode);      
      _ODSPort = enode->getOutputDataStreamPort(_name);
      YASSERT(_ODSPort);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(_ODSPort));
      _sop = dynamic_cast<SubjectOutputDataStreamPort*>(GuiContext::getCurrent()->_mapOfSubjectDataPort[_ODSPort]);
      YASSERT(_sop);

      Subject *father=_sop->getParent();
      Subject::erase(_sop);
      _sop = 0;
      _ODSPort = 0;
      if (father)
        {
          DEBTRACE("REMOVE");
          father->select(true);
          father->update(REMOVE,0,0);
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddODSPortFromCatalog::localReverse(): " << ex.what());
      setErrorMsg(ex);
      return false;
    }
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
  DEBTRACE("CommandOrderInputPorts");
}

std::string CommandOrderInputPorts::dump()
{
  ostringstream s;
  s << _isUp;
  string ret ="CommandOrderInputPorts " + _node + " " + _port + " " + s.str();
  return ret;
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

      int isUp = _isUp;
      if (isUp)
        {
          if(pos == plist.begin())
            pos=plist.end(); // --- cycle
          else
            do { pos--; isUp--; } while (isUp);
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
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      SubjectDataPort *sport = GuiContext::getCurrent()->_mapOfSubjectDataPort[portToMove];
      snode->update(ORDER, true, sport);
      snode->update(SYNCHRO, true, sport); // --- synchronise edition and scene
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandOrderInputPorts::localExecute() : " << ex.what());
      setErrorMsg(ex);
      father = 0;
    }
  return (father != 0);
}

bool CommandOrderInputPorts::localReverse()
{
  DEBTRACE("CommandOrderInputPorts::localReverse " << _node << " " << _port  << " " << _isUp);
  ElementaryNode* father = 0;
  try
    {
      int isUpRev = -_isUp;
      if (isUpRev == 0) isUpRev =1;
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      father = dynamic_cast<ElementaryNode*> (node);
      if (!father) return false;
      InputPort *portToMove = father->getInputPort(_port);
      DEBTRACE(portToMove->getName());

      list<InputPort*> plist = father->getSetOfInputPort();
      list<InputPort*>::iterator pos = find(plist.begin(), plist.end(), portToMove);

      if (isUpRev>0)
        {
          if(pos == plist.begin())
            pos=plist.end(); // --- cycle
          else
            do { pos--; isUpRev--; } while (isUpRev);
        }
      else
        {
          pos++;
          if (pos == plist.end())
            pos = plist.begin(); // --- cycle
          else
            do { pos++; isUpRev++; } while (isUpRev<0);// --- insert before the 2nd next port
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
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      SubjectDataPort *sport = GuiContext::getCurrent()->_mapOfSubjectDataPort[portToMove];
      snode->update(ORDER, true, sport);
      snode->update(SYNCHRO, true, sport); // --- synchronise edition and scene
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandOrderInputPorts::localExecute() : " << ex.what());
      setErrorMsg(ex);
      father = 0;
    }
  return (father != 0);
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
  DEBTRACE("CommandOrderOutputPorts");
}

std::string CommandOrderOutputPorts::dump()
{
  ostringstream s;
  s << _isUp;
  string ret ="CommandOrderOutputPorts " + _node + " " + _port + " " + s.str();
  return ret;
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

      int isUp = _isUp;
      if (isUp)
        {
          if(pos == plist.begin())
            pos=plist.end(); // --- cycle
          else
            do { pos--; isUp--; } while (isUp);
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
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      SubjectDataPort *sport = GuiContext::getCurrent()->_mapOfSubjectDataPort[portToMove];
      snode->update(ORDER, false, sport);
      snode->update(SYNCHRO, false, sport); // --- synchronise edition and scene
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandOrderOutputPorts::localExecute() : " << ex.what());
      setErrorMsg(ex);
      father = 0;
    }
  return (father != 0);
}

bool CommandOrderOutputPorts::localReverse()
{
  DEBTRACE("CommandOrderOutputPorts::localReverse " << _node << " " << _port  << " " << _isUp);
  ElementaryNode* father = 0;
  try
    {
      int isUpRev = -_isUp;
      if (isUpRev == 0) isUpRev =1;
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      father = dynamic_cast<ElementaryNode*> (node);
      if (!father) return false;
      OutputPort *portToMove = father->getOutputPort(_port);
      DEBTRACE(portToMove->getName());

      list<OutputPort*> plist = father->getSetOfOutputPort();
      list<OutputPort*>::iterator pos = find(plist.begin(), plist.end(), portToMove);

      if (isUpRev>0)
        {
          if(pos == plist.begin())
            pos=plist.end(); // --- cycle
          else
            do { pos--; isUpRev--; } while (isUpRev);
        }
      else
        {
          pos++;
          if (pos == plist.end())
            pos = plist.begin(); // --- cycle
          else
            do { pos++; isUpRev++; } while (isUpRev<0);// --- insert before the 2nd next port
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
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      SubjectDataPort *sport = GuiContext::getCurrent()->_mapOfSubjectDataPort[portToMove];
      snode->update(ORDER, false, sport);
      snode->update(SYNCHRO, false, sport); // --- synchronise edition and scene
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandOrderOutputPorts::localExecute() : " << ex.what());
      setErrorMsg(ex);
      father = 0;
    }
  return (father != 0);
}

// ----------------------------------------------------------------------------

CommandDestroy::CommandDestroy(TypeOfElem elemType,
                               std::string startnode, std::string startport, TypeOfElem startportType,
                               std::string endnode, std::string endport, TypeOfElem endportType)
  : Command(), _elemType(elemType), _startnode(startnode), _startport(startport),
    _endnode(endnode), _endport(endport), _startportType(startportType), _endportType(endportType)
{
  DEBTRACE("CommandDestroy::CommandDestroy");
  _normalReverse = false;
}

std::string CommandDestroy::dump()
{
  string ret ="CommandDestroy " + ProcInvoc::getTypeName(_elemType);
  ret += " " + _startnode + " " + _startport;
  ret += " " + _endnode + " " + _endport;
  return ret;
}

bool CommandDestroy::localExecute()
{
  DEBTRACE("CommandDestroy::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Subject *subject = 0;
      Subject *father  = 0;
      switch (_elemType)
        {
        case SALOMEPROC:
        case BLOC:
        case FOREACHLOOP:
        case OPTIMIZERLOOP:
        case FORLOOP:
        case WHILELOOP:
        case SWITCH:
        case PYTHONNODE:
        case PYFUNCNODE:
        case CORBANODE:
        case SALOMENODE:
        case CPPNODE:
        case SALOMEPYTHONNODE:
        case XMLNODE:
        case SPLITTERNODE:
        case DFTODSFORLOOPNODE:
        case DSTODFFORLOOPNODE:
        case PRESETNODE:
        case OUTNODE:
        case STUDYINNODE:
        case STUDYOUTNODE:
          {
            Node* node = proc;
            if (!_startnode.empty()) node = proc->getChildByName(_startnode);
            YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
            subject = GuiContext::getCurrent()->_mapOfSubjectNode[node];
            father  = subject->getParent();
          }
          break;
        case INPUTPORT:
          {
            Node* node = proc->getChildByName(_startnode);
            InPort* inp = node->getInputPort(_startport);
            YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(inp));
            subject = GuiContext::getCurrent()->_mapOfSubjectDataPort[inp];
            father  = subject->getParent();
          }
          break;
        case INPUTDATASTREAMPORT:
          {
            Node* node = proc->getChildByName(_startnode);
            InPort* inp = node->getInputDataStreamPort(_startport);
            YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(inp));
            subject = GuiContext::getCurrent()->_mapOfSubjectDataPort[inp];
            father  = subject->getParent();
          }
          break;
        case OUTPUTPORT:
          {
            Node* node = proc->getChildByName(_startnode);
            OutPort* outp = node->getOutputPort(_startport);
            YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(outp));
            subject = GuiContext::getCurrent()->_mapOfSubjectDataPort[outp];
            father  = subject->getParent();
          }
          break;
        case OUTPUTDATASTREAMPORT:
          {
            Node* node = proc->getChildByName(_startnode);
            OutPort* outp = node->getOutputDataStreamPort(_startport);
            YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(outp));
            subject = GuiContext::getCurrent()->_mapOfSubjectDataPort[outp];
            father  = subject->getParent();
          }
          break;
        case DATALINK:
          {
            Node* outn = proc->getChildByName(_startnode);
            Node* inn = proc->getChildByName(_endnode);

            OutPort* outp;
            InPort* inp;

            if(_startportType == OUTPUTPORT)
              outp = outn->getOutputPort(_startport);
            else
              outp = outn->getOutputDataStreamPort(_startport);

            if(_endportType == INPUTPORT)
              inp = inn->getInputPort(_endport);
            else
              inp = inn->getInputDataStreamPort(_endport);

            pair<OutPort*,InPort*> keymap = pair<OutPort*,InPort*>(outp,inp);
            YASSERT(GuiContext::getCurrent()->_mapOfSubjectLink.count(keymap));
            subject = GuiContext::getCurrent()->_mapOfSubjectLink[keymap];
            father  = subject->getParent();
          }
          break;
        case CONTROLLINK:
          {
            Node* outn = proc->getChildByName(_startnode);
            Node* inn = proc->getChildByName(_endnode);
            pair<Node*,Node*> keymap = pair<Node*,Node*>(outn,inn);
            YASSERT(GuiContext::getCurrent()->_mapOfSubjectControlLink.count(keymap));
            subject = GuiContext::getCurrent()->_mapOfSubjectControlLink[keymap];
            father  = subject->getParent();
          }
          break;
        case CONTAINER:
          {
            Container *container = proc->containerMap[_startnode];
            subject = GuiContext::getCurrent()->_mapOfSubjectContainer[container];
            break;
          }
        case COMPONENT:
        case REFERENCE:
        case DATATYPE:
        case UNKNOWN:
          throw Exception("Command Destroy not implemented for that type");
          break;
        }
      YASSERT(subject);
      Subject::erase(subject);
      if (father) 
        {
          father->select(true);
          father->update(REMOVE, 0, 0);
        }
      subject = 0;
      return true; 
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandDestroy::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandDestroy::localReverse()
{
  DEBTRACE("CommandDestroy::localReverse");
  //! nothing to do here, all is done in subcommands
  return true;
}
 
// ----------------------------------------------------------------------------

CommandSetInPortValue::CommandSetInPortValue(std::string node,
                                             std::string port,
                                             std::string value)
  : Command(), _node(node), _port(port), _value(value)
{
  DEBTRACE("CommandSetInPortValue::CommandSetInPortValue " << node << " " << port << " " << value);
  _oldValue = "";
}

std::string CommandSetInPortValue::dump()
{
  string ret ="CommandSetInPortValue " + _node + " " + _port + " " + _value;
  return ret;
}
    
bool CommandSetInPortValue::localExecute()
{
  DEBTRACE("CommandSetInPortValue::localExecute");
  InputPort* inp ;
  InputPresetPort *inpp = 0;
  InputStudyPort *insp = 0;
  DataNode *dnode = 0;
  SubjectDataPort *sinp = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      inp = node->getInputPort(_port);
      inpp = dynamic_cast<InputPresetPort*>(inp);
      insp = dynamic_cast<InputStudyPort*>(inp);
      dnode = dynamic_cast<DataNode*>(node);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(inp));
      sinp = GuiContext::getCurrent()->_mapOfSubjectDataPort[inp];
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetInPortValue::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }

  if(insp && dnode)
    {
      //It's a study port
      _oldValue = insp->getAsString();
      DEBTRACE("old value="<< _oldValue);
      dnode->setData(insp, _value );
      sinp->update(SETVALUE, 0, sinp);
      return true;
    }

  PyObject *result;
  PyGILState_STATE gstate = PyGILState_Ensure();
  try
    {
      _oldValue = inp->getAsString();
      if (_oldValue == "None") _oldValue = "";
      DEBTRACE("old value="<< _oldValue);
      std::string strval;
      if (inp->edGetType()->kind() == YACS::ENGINE::String || inp->edGetType()->isA(Runtime::_tc_file))
        strval = "\"" + _value + "\"";
      else
        strval = _value;
      result = YACS::ENGINE::getSALOMERuntime()->convertStringToPyObject(strval.c_str());
      inp->edInit("Python", result);
      Py_DECREF(result);

      PyGILState_Release(gstate);
      sinp->update(SETVALUE, 0, sinp);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetInPortValue::localExecute() : " << ex.what());
      //Py_DECREF(result);
      PyGILState_Release(gstate);
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetInPortValue::localReverse()
{
  DEBTRACE("CommandSetInPortValue::localReverse");
  InputPort* inp ;
  InputPresetPort *inpp = 0;
  InputStudyPort *insp = 0;
  DataNode *dnode = 0;
  SubjectDataPort *sinp = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      inp = node->getInputPort(_port);
      inpp = dynamic_cast<InputPresetPort*>(inp);
      insp = dynamic_cast<InputStudyPort*>(inp);
      dnode = dynamic_cast<DataNode*>(node);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(inp));
      sinp = GuiContext::getCurrent()->_mapOfSubjectDataPort[inp];
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetInPortValue::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }

  if(insp && dnode)
    {
      //It's a study port
      _value = insp->getAsString();
      DEBTRACE("value="<< _value);
      DEBTRACE("old value="<< _oldValue);
      dnode->setData(insp, _oldValue );
      sinp->update(SETVALUE, 0, sinp);
      return true;
    }

  PyObject *result = Py_None;
  PyGILState_STATE gstate = PyGILState_Ensure();
  try
    {
      _value = inp->getAsString();
      DEBTRACE("value="<< _value);
      DEBTRACE("old value="<< _oldValue);
      if (!_oldValue.empty())
        {
          std::string strval;
          if (inp->edGetType()->kind() == YACS::ENGINE::String || inp->edGetType()->isA(Runtime::_tc_file))
            strval = "\"" + _oldValue + "\"";
          else
            strval = _oldValue;
          result = YACS::ENGINE::getSALOMERuntime()->convertStringToPyObject(strval.c_str());
        }
      inp->edInit("Python", result);
      Py_DECREF(result);
      PyGILState_Release(gstate);
      sinp->update(SETVALUE, 0, sinp);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetInPortValue::localExecute() : " << ex.what());
      //Py_DECREF(result);
      PyGILState_Release(gstate);
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetOutPortValue::CommandSetOutPortValue(std::string node,
                                               std::string port,
                                               std::string value)
  : Command(), _node(node), _port(port), _value(value)
{
  DEBTRACE("CommandSetOutPortValue::CommandSetOutPortValue " << node << " " << port << " " << value);
  _oldValue = "";
}
 
std::string CommandSetOutPortValue::dump()
{
  string ret ="CommandSetOutPortValue " + _node + " " + _port + " " + _value;
  return ret;
}
   
bool CommandSetOutPortValue::localExecute()
{
  DEBTRACE("CommandSetOutPortValue::localExecute");
  OutputPresetPort *outpp = 0;
  OutputStudyPort *outsp = 0;
  DataNode *dnode = 0;
  SubjectDataPort *soutp = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      OutputPort* outp = node->getOutputPort(_port);
      outpp = dynamic_cast<OutputPresetPort*>(outp);
      outsp = dynamic_cast<OutputStudyPort*>(outp);
      dnode = dynamic_cast<DataNode*>(node);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(outp));
      soutp = GuiContext::getCurrent()->_mapOfSubjectDataPort[outp];
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetOutPortValue::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }

  if (!outpp && !outsp)
    {
      DEBTRACE("Set value on output port only possible on a presetPort or a studyPort");
      GuiContext::getCurrent()->_lastErrorMessage = "Set value on output port only possible on a presetPort or a studyPort";
      return false;
    }

  if (!dnode)
    {
      DEBTRACE("Set value on output port only possible on a dataNode");
      GuiContext::getCurrent()->_lastErrorMessage = "Set value on output port only possible on a dataNode";
      return false;
    }

  if(outsp)
    {
      //It's a study port
      _oldValue = outsp->getAsString();
      DEBTRACE("old value="<< _oldValue);
      dnode->setData(outsp, _value );
      soutp->update(SETVALUE, 0, soutp);
     return true;
    }

  PyObject *result;
  try
    {
      _oldValue = outpp->getAsString();
      if (_oldValue == "None") _oldValue = "";
      DEBTRACE("old value="<< _oldValue);
      std::string strval;
      if (outpp->edGetType()->kind() == YACS::ENGINE::String || outpp->edGetType()->isA(Runtime::_tc_file))
        strval = "\"" + _value + "\"";
      else
        strval = _value;
      result = YACS::ENGINE::getSALOMERuntime()->convertStringToPyObject(strval.c_str());
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetOutPortValue::localExecute() : " << ex.what());
      setErrorMsg(ex);
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
      soutp->update(SETVALUE, 0, soutp);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetOutPortValue::localExecute() : " << ex.what());
      setErrorMsg(ex);
      Py_DECREF(result);
      PyGILState_Release(gstate);
      return false;
    }

  Py_DECREF(result);
  PyGILState_Release(gstate);
  return true;
}

bool CommandSetOutPortValue::localReverse()
{
  DEBTRACE("CommandSetOutPortValue::localReverse");
  OutputPresetPort *outpp = 0;
  OutputStudyPort *outsp = 0;
  DataNode *dnode = 0;
  SubjectDataPort *soutp = 0;
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_node);
      OutputPort* outp = node->getOutputPort(_port);
      outpp = dynamic_cast<OutputPresetPort*>(outp);
      outsp = dynamic_cast<OutputStudyPort*>(outp);
      dnode = dynamic_cast<DataNode*>(node);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(outp));
      soutp = GuiContext::getCurrent()->_mapOfSubjectDataPort[outp];
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetOutPortValue::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }

  if (!outpp && !outsp)
    {
      DEBTRACE("Set value on output port only possible on a presetPort or a studyPort");
      GuiContext::getCurrent()->_lastErrorMessage = "Set value on output port only possible on a presetPort or a studyPort";
      return false;
    }

  if (!dnode)
    {
      DEBTRACE("Set value on output port only possible on a dataNode");
      GuiContext::getCurrent()->_lastErrorMessage = "Set value on output port only possible on a dataNode";
      return false;
    }

  if(outsp)
    {
      //It's a study port
      _value = outsp->getAsString();
      DEBTRACE("value="<< _value);
      DEBTRACE("old value="<< _oldValue);
      dnode->setData(outsp, _oldValue );
      soutp->update(SETVALUE, 0, soutp);
      return true;
    }

  PyObject *result = Py_None;
  try
    {
      _value = outpp->getAsString();
      DEBTRACE("value="<< _value);
      DEBTRACE("old value="<< _oldValue);
      if (!_oldValue.empty())
        {
          std::string strval;
          if (outpp->edGetType()->kind() == YACS::ENGINE::String || outpp->edGetType()->isA(Runtime::_tc_file))
            strval = "\"" + _value + "\"";
          else
            strval = _value;
          result = YACS::ENGINE::getSALOMERuntime()->convertStringToPyObject(strval.c_str());
        }
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetOutPortValue::localExecute() : " << ex.what());
      setErrorMsg(ex);
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
      soutp->update(SETVALUE, 0, soutp);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetOutPortValue::localExecute() : " << ex.what());
      setErrorMsg(ex);
      Py_DECREF(result);
      PyGILState_Release(gstate);
      return false;
    }

  Py_DECREF(result);
  PyGILState_Release(gstate);
  return true;
}
 
// ----------------------------------------------------------------------------

CommandSetSwitchSelect::CommandSetSwitchSelect(std::string aSwitch,
                                               std::string value)
  : Command(), _switch(aSwitch), _value(value)
{
  DEBTRACE("CommandSetSwitchSelect::CommandSetSwitchSelect");
  _oldValue = "0";
}

std::string CommandSetSwitchSelect::dump()
{
  string ret ="CommandSetSwitchSelect " + _switch + " " + _value;
  return ret;
}
    
bool CommandSetSwitchSelect::localExecute()
{
  DEBTRACE("CommandSetSwitchSelect::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Switch* aSwitch = dynamic_cast<Switch*>(proc->getChildByName(_switch));
      InputPort *condPort = aSwitch->edGetConditionPort();
      _oldValue = condPort->getAsString();
      if (_oldValue == "None") _oldValue = "0";
      int val = atoi(_value.c_str());
      condPort->edInit(val);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(aSwitch));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[aSwitch];
      snode->update(SETSELECT, 0, snode);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetSwitchSelect::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetSwitchSelect::localReverse()
{
  DEBTRACE("CommandSetSwitchSelect::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Switch* aSwitch = dynamic_cast<Switch*>(proc->getChildByName(_switch));
      InputPort *condPort = aSwitch->edGetConditionPort();
      int val = atoi(_oldValue.c_str());
      condPort->edInit(val);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(aSwitch));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[aSwitch];
      snode->update(SETSELECT, 0, snode);
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetSwitchSelect::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetSwitchCase::CommandSetSwitchCase(std::string aSwitch,
                                           std::string node,
                                           std::string value)
  : Command(), _switch(aSwitch), _node(node), _value(value)
{
  DEBTRACE("CommandSetSwitchCase::CommandSetSwitchCase");
  _oldValue = 0;
  _oldNode = "";
}

std::string CommandSetSwitchCase::dump()
{
  string ret ="CommandSetSwitchCase " + _switch + " " + _node + " " + _value;
  return ret;
}

bool CommandSetSwitchCase::localExecute()
{
  DEBTRACE("CommandSetSwitchCase::localExecute");
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
      aSwitch->edChangeCase(oldVal,val);
      _oldValue = oldVal;
      _oldNode = proc->getChildName(node);
      DEBTRACE("CommandSetSwitchCase::localExecute OK " << val);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(aSwitch));
      SubjectNode *ssw = GuiContext::getCurrent()->_mapOfSubjectNode[aSwitch];
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      ssw->update(SETCASE, val, snode);
      snode->recursiveUpdate(RENAME, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetSwitchCase::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetSwitchCase::localReverse()
{
  DEBTRACE("CommandSetSwitchCase::localReverse");
  try
    {
      DEBTRACE("CommandSetSwitchCase::localReverse");
      Proc* proc = GuiContext::getCurrent()->getProc();
      Switch* aSwitch = dynamic_cast<Switch*>(proc->getChildByName(_switch));
      Node* node = proc->getChildByName(_oldNode);
      int val = _oldValue;
      if (aSwitch->edGetNode(val))
        {
          throw YACS::Exception("Set Switch Case impossible: value already used");
        }
      int oldVal = aSwitch->getRankOfNode(node);
      aSwitch->edChangeCase(oldVal,val);
      DEBTRACE("CommandSetSwitchCase::localReverse OK " << val);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(aSwitch));
      SubjectNode *ssw = GuiContext::getCurrent()->_mapOfSubjectNode[aSwitch];
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      ssw->update(SETCASE, val, snode);
      snode->recursiveUpdate(RENAME, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetSwitchCase::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetForLoopSteps::CommandSetForLoopSteps(std::string forLoop,
                                               std::string value)
  : Command(), _forLoop(forLoop), _value(value)
{
  DEBTRACE("CommandSetForLoopSteps::CommandSetForLoopSteps");
  _oldValue = 0;
}


std::string CommandSetForLoopSteps::dump()
{
  string ret ="CommandSetForLoopSteps " + _forLoop + " " + _value;
  return ret;
}
    
bool CommandSetForLoopSteps::localExecute()
{
  DEBTRACE("CommandSetForLoopSteps::localExecute");
  try
    {
      DEBTRACE("CommandSetForLoopSteps::localExecute");
      Proc* proc = GuiContext::getCurrent()->getProc();
      ForLoop* forLoop = dynamic_cast<ForLoop*>(proc->getChildByName(_forLoop));
      InputPort *nbSteps = forLoop->edGetNbOfTimesInputPort();
      _oldValue = atoi(forLoop->edGetNbOfTimesInputPort()->getAsString().c_str());
      int val = atoi(_value.c_str());
      nbSteps->edInit(val);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(forLoop));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[forLoop];
      snode->update(SETVALUE, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetForLoopSteps::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetForLoopSteps::localReverse()
{
  DEBTRACE("CommandSetForLoopSteps::localReverse");
  try
    {
      DEBTRACE("CommandSetForLoopSteps::localReverse");
      Proc* proc = GuiContext::getCurrent()->getProc();
      ForLoop* forLoop = dynamic_cast<ForLoop*>(proc->getChildByName(_forLoop));
      InputPort *nbSteps = forLoop->edGetNbOfTimesInputPort();
      nbSteps->edInit(_oldValue);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(forLoop));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[forLoop];
      snode->update(SETVALUE, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetForLoopSteps::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetWhileCondition::CommandSetWhileCondition(std::string whileLoop,
                                                   std::string value)
  : Command(), _whileLoop(whileLoop), _value(value)
{
  DEBTRACE("CommandSetWhileCondition::CommandSetWhileCondition");
  _oldValue = 0;
}

std::string CommandSetWhileCondition::dump()
{
  string ret ="CommandSetWhileCondition " + _whileLoop + " " + _value;
  return ret;
}

bool CommandSetWhileCondition::localExecute()
{
  DEBTRACE("CommandSetWhileCondition::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      WhileLoop* whileLoop = dynamic_cast<WhileLoop*>(proc->getChildByName(_whileLoop));
      InputPort *cond = whileLoop->edGetConditionPort();
      _oldValue = atoi(whileLoop->edGetConditionPort()->getAsString().c_str());
      bool val = atoi(_value.c_str());
      cond->edInit(val);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(whileLoop));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[whileLoop];
      snode->update(SETVALUE, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetWhileCondition::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetWhileCondition::localReverse()
{
  DEBTRACE("CommandSetWhileCondition::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      WhileLoop* whileLoop = dynamic_cast<WhileLoop*>(proc->getChildByName(_whileLoop));
      InputPort *cond = whileLoop->edGetConditionPort();
      cond->edInit(_oldValue);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(whileLoop));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[whileLoop];
      snode->update(SETVALUE, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetWhileCondition::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetForEachBranch::CommandSetForEachBranch(std::string forEach,
                                                 std::string value)
  : Command(), _forEach(forEach), _value(value)
{
  DEBTRACE("CommandSetForEachBranch::CommandSetForEachBranch");
  _oldValue = 0;
}
 
std::string CommandSetForEachBranch::dump()
{
  string ret ="CommandSetForEachBranch " + _forEach + " " + _value;
  return ret;
}
   
bool CommandSetForEachBranch::localExecute()
{
  DEBTRACE("CommandSetForEachBranch::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node=proc->getChildByName(_forEach);
      InputPort *nbBranches = node->getInputPort("nbBranches");
      _oldValue = atoi(nbBranches->getAsString().c_str());
      int val = atoi(_value.c_str());
      nbBranches->edInit(val);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(nbBranches));
      SubjectDataPort *spo = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(nbBranches)];
      spo->update(SETVALUE, 0, spo);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      snode->update(SETVALUE, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetForEachBranch::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetForEachBranch::localReverse()
{
  DEBTRACE("CommandSetForEachBranch::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node=proc->getChildByName(_forEach);
      InputPort *nbBranches = node->getInputPort("nbBranches");
      nbBranches->edInit(_oldValue);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(nbBranches));
      SubjectDataPort *spo = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(nbBranches)];
      spo->update(SETVALUE, 0, spo);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      snode->update(SETVALUE, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetForEachBranch::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetAlgo::CommandSetAlgo(std::string optimizer, std::string alglib, std::string symbol)
  : Command(), _optimizer(optimizer), _alglib(alglib), _symbol(symbol)
{
  DEBTRACE("CommandSetAlgo::CommandSetAlgo" << _optimizer << " " << _alglib << " " << _symbol);
  _oldAlglib = "";
  _oldSymbol = "";
}

std::string CommandSetAlgo::dump()
{
  string ret ="CommandSetAlgo " + _optimizer + " " + _alglib + " " + _symbol;
  return ret;
}

bool CommandSetAlgo::localExecute()
{
  DEBTRACE("CommandSetAlgo::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      OptimizerLoop* loop = dynamic_cast<OptimizerLoop*>(proc->getChildByName(_optimizer));
      loop->setAlgorithm(_alglib,_symbol);
      _oldAlglib = _alglib;
      _oldSymbol = _symbol;
      InputPort *port = loop->edGetPortForOutPool();
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(static_cast<DataPort*>(port)));
      SubjectDataPort *spo = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(port)];
      spo->update(UPDATE, 0, spo);
      OutputPort *oport = loop->edGetSamplePort();
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(static_cast<DataPort*>(oport)));
      spo = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(oport)];
      spo->update(UPDATE, 0, spo);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(loop));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[loop];
      snode->update(SETVALUE, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetAlgo::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetAlgo::localReverse()
{
  DEBTRACE("CommandSetAlgo::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      OptimizerLoop* loop = dynamic_cast<OptimizerLoop*>(proc->getChildByName(_optimizer));
      loop->setAlgorithm(_oldAlglib,_oldSymbol);
      InputPort *port = loop->edGetPortForOutPool();
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(static_cast<DataPort*>(port)));
      SubjectDataPort *spo = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(port)];
      spo->update(UPDATE, 0, spo);
      OutputPort *oport = loop->edGetSamplePort();
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectDataPort.count(static_cast<DataPort*>(oport)));
      spo = GuiContext::getCurrent()->_mapOfSubjectDataPort[static_cast<DataPort*>(oport)];
      spo->update(UPDATE, 0, spo);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(loop));
      SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[loop];
      snode->update(SETVALUE, 0, snode);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetAlgo::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
  return true;
}

// ----------------------------------------------------------------------------

CommandAddLink::CommandAddLink(std::string outNode, std::string outPort, TypeOfElem outPortType,
                               std::string inNode, std::string inPort, TypeOfElem inPortType, bool control)
  : Command(), _outNode(outNode), _outPort(outPort), _outPortType(outPortType),
               _inNode(inNode), _inPort(inPort), _inPortType(inPortType), _control(control)
{
  DEBTRACE("CommandAddLink::CommandAddLink "<<outNode<<"."<<outPort<<"->"<<inNode<<"."<<inPort<<" "<<control);
  _controlCreatedWithDF = false;
}

std::string CommandAddLink::dump()
{
  string s = "false";
  if (_control) s = "true";
  string ret ="CommandAddLink " + _outNode + " " + _outPort + " " + _inNode + " " + _inPort + " " + s;
  return ret;
}

bool CommandAddLink::localExecute()
{
  DEBTRACE("CommandAddLink::localExecute");
  DEBTRACE(_outNode<<"."<<_outPort<<"->"<<_inNode<<"."<<_inPort<<" "<<_control);
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* outn = proc->getChildByName(_outNode);
      Node* inn = proc->getChildByName(_inNode);
      OutPort* outp;
      InPort* inp;

      // --- is a control link already existing ?
      bool preexistingControl = false;
      {
        Node* outn2=outn;
        Node* inn2=inn;
        ComposedNode* father = ComposedNode::getLowestCommonAncestor(outn2,inn2);
        if(outn2==father || inn2==father)
          preexistingControl = true;
        else
          {
            while(outn2->getFather() != father)
              outn2 = outn2->getFather();
            while(inn2->getFather() != father)
              inn2 = inn2->getFather();
            OutGate *ogate = outn2->getOutGate();
            InGate *igate = inn2->getInGate();
            if (ogate->isAlreadyInSet(igate))
              preexistingControl = true;
          }
      }

      if(_outPortType == OUTPUTPORT)
        outp = outn->getOutputPort(_outPort);
      else
        outp = outn->getOutputDataStreamPort(_outPort);

      if(_inPortType == INPUTPORT)
        inp = inn->getInputPort(_inPort);
      else
        inp = inn->getInputDataStreamPort(_inPort);

      ComposedNode *cla = ComposedNode::getLowestCommonAncestor(outn->getFather(),inn->getFather());
      DEBTRACE(cla->getName());
      if (dynamic_cast<OutputDataStreamPort*>(outp))
        cla->edAddLink(outp,inp);
      else if(_control)
        cla->edAddDFLink(outp,inp);
      else
        cla->edAddLink(outp,inp);

      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(cla));
      SubjectNode *sub = GuiContext::getCurrent()->_mapOfSubjectNode[cla];
      SubjectComposedNode *scla = dynamic_cast<SubjectComposedNode*>(sub);
      DEBTRACE(scla->getName());
      SubjectNode *sno = GuiContext::getCurrent()->_mapOfSubjectNode[outn];
      SubjectNode *sni = GuiContext::getCurrent()->_mapOfSubjectNode[inn];
      SubjectDataPort *subOutport = GuiContext::getCurrent()->_mapOfSubjectDataPort[outp];
      SubjectDataPort *subInport = GuiContext::getCurrent()->_mapOfSubjectDataPort[inp];
      SubjectLink *slink = scla->addSubjectLink(sno, subOutport, sni, subInport);

      // --- if control link, identify the nodes linked and draw the control link if not already existing

      if (_control)
        {
          ComposedNode* father = ComposedNode::getLowestCommonAncestor(outn,inn);
          if(outn==father || inn==father) return true;
          while(outn->getFather() != father)
            outn = outn->getFather();
          while(inn->getFather() != father)
            inn = inn->getFather();
          OutGate *ogate = outn->getOutGate();
          InGate *igate = inn->getInGate();
          if (ogate->isAlreadyInSet(igate))
            {
              if (!preexistingControl)
                _controlCreatedWithDF = true;
              pair<Node*,Node*> keyLink(outn,inn);
              if (!GuiContext::getCurrent()->_mapOfSubjectControlLink.count(keyLink))
                {
                  SubjectNode *sfno = GuiContext::getCurrent()->_mapOfSubjectNode[outn];
                  SubjectNode *sfni = GuiContext::getCurrent()->_mapOfSubjectNode[inn];
                  if (!sfno || !sfni) return true;
                  SubjectControlLink *sclink = scla->addSubjectControlLink(sfno, sfni);
                }
            }
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddLink::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandAddLink::localReverse()
{
  DEBTRACE("CommandAddLink::localReverse");
  try
    {
      SubjectLink *slink =0;
      SubjectControlLink *sclink = 0;
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* outn = proc->getChildByName(_outNode);
      Node* inn = proc->getChildByName(_inNode);
      OutPort* outp;
      InPort* inp;
      if(_outPortType == OUTPUTPORT)
        outp = outn->getOutputPort(_outPort);
      else
        outp = outn->getOutputDataStreamPort(_outPort);
      if(_inPortType == INPUTPORT)
        inp = inn->getInputPort(_inPort);
      else
        inp = inn->getInputDataStreamPort(_inPort);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectLink.count(pair<OutPort*,InPort*>(outp,inp)));
      slink = GuiContext::getCurrent()->_mapOfSubjectLink[pair<OutPort*,InPort*>(outp,inp)];
      if (_controlCreatedWithDF)
        {
          YASSERT(GuiContext::getCurrent()->_mapOfSubjectControlLink.count(pair<Node*,Node*>(outn,inn)));
          sclink = GuiContext::getCurrent()->_mapOfSubjectControlLink[pair<Node*,Node*>(outn,inn)];         
        }

      Subject *father = slink->getParent();
      Subject::erase(slink);
      slink = 0;
      if (father)
        {
          DEBTRACE("REMOVE");
          father->select(true);
          father->update(REMOVE,0,0);
        }
      if (!sclink)
        return true;
      father=sclink->getParent();
      Subject::erase(sclink);
      sclink = 0;
      if (father)
        {
          DEBTRACE("REMOVE");
          father->select(true);
          father->update(REMOVE,0,0);
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddLink::localReverse(): " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandAddControlLink::CommandAddControlLink(std::string outNode, std::string inNode)
  : Command(), _outNode(outNode), _inNode(inNode)
{
  DEBTRACE("CommandAddControlLink::CommandAddControlLink "<<outNode<<"-->>"<<inNode);
}

std::string CommandAddControlLink::dump()
{
  string ret ="CommandAddControlLink " + _outNode + " " + _inNode;
  return ret;
}

bool CommandAddControlLink::localExecute()
{
  DEBTRACE("CommandAddControlLink::localExecute");
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
      bool ret= cla->edAddCFLink(outn,inn);
      if(ret==false)
        GuiContext::getCurrent()->_lastErrorMessage = "Link already exists";

      cla = ComposedNode::getLowestCommonAncestor(outn->getFather(),
                                                  inn->getFather());
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(cla));
      SubjectNode *sub = GuiContext::getCurrent()->_mapOfSubjectNode[cla];
      SubjectComposedNode *scla = dynamic_cast<SubjectComposedNode*>(sub);
      DEBTRACE(scla->getName());
      SubjectNode *sno = GuiContext::getCurrent()->_mapOfSubjectNode[outn];
      SubjectNode *sni = GuiContext::getCurrent()->_mapOfSubjectNode[inn];
      SubjectControlLink *sclink = scla->addSubjectControlLink(sno,sni);
      return ret;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddControlLink::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandAddControlLink::localReverse()
{
  DEBTRACE("CommandAddControlLink::localReverse");
  try
    {
      SubjectControlLink *sclink = 0;
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* outn = proc->getChildByName(_outNode);
      Node* inn = proc->getChildByName(_inNode);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectControlLink.count(pair<Node*,Node*>(outn,inn)));
      sclink = GuiContext::getCurrent()->_mapOfSubjectControlLink[pair<Node*,Node*>(outn,inn)];         

      Subject *father=sclink->getParent();
      Subject::erase(sclink);
      sclink = 0;
      if (father)
        {
          DEBTRACE("REMOVE");
          father->select(true);
          father->update(REMOVE,0,0);
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddControlLink::localReverse(): " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandAddContainerBase::CommandAddContainerBase(std::string name, std::string refContainer)
  : Command(), _name(name), _containerToClone(refContainer), _subcont(0)
{
  DEBTRACE("CommandAddContainerBase::CommandAddContainerBase " << name << " " << refContainer);
}

CommandAddContainerBase::~CommandAddContainerBase()
{
}

bool CommandAddContainerBase::localExecute()
{
  DEBTRACE("CommandAddContainerBase::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_name))
        {
          GuiContext::getCurrent()->_lastErrorMessage = "There is already a container with that name";
          return false;
        }
      Container *container(createNewInstance());
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
      container->setName(_name);
      container->setProc(proc);
      proc->containerMap[_name] = container;

      SubjectProc* sproc = GuiContext::getCurrent()->getSubjectProc();
      _subcont = sproc->addSubjectContainer(container, _name);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddContainerBase::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandAddContainerBase::localReverse()
{
  DEBTRACE("CommandAddContainerBase::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      YASSERT(proc->containerMap.count(_name));
      Container *container = proc->containerMap[_name];
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(container));
      _subcont = GuiContext::getCurrent()->_mapOfSubjectContainer[container];
      YASSERT(!_subcont->isUsed());
      SubjectProc* sproc = GuiContext::getCurrent()->getSubjectProc();
      sproc->removeSubjectContainer(_subcont);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddContainer::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandAddContainer::CommandAddContainer(std::string name, std::string refContainer):CommandAddContainerBase(name,refContainer)
{
}

std::string CommandAddContainer::dump()
{
  string ret ="CommandAddContainer " + _name + " " + _containerToClone;
  return ret;
}

Container *CommandAddContainer::createNewInstance() const
{
  return new SalomeContainer;
}

// ----------------------------------------------------------------------------

CommandAddHPContainer::CommandAddHPContainer(std::string name, std::string refContainer):CommandAddContainerBase(name,refContainer)
{
}

std::string CommandAddHPContainer::dump()
{
  string ret ="CommandAddHPContainer " + _name + " " + _containerToClone;
  return ret;
}

Container *CommandAddHPContainer::createNewInstance() const
{
  return new SalomeHPContainer;
}

// ----------------------------------------------------------------------------

CommandSetNodeProperties::CommandSetNodeProperties(std::string position, std::map<std::string,std::string> properties)
  : Command(), _position(position), _properties(properties)
{
  DEBTRACE("CommandSetNodeProperties::CommandSetNodeProperties " << position);
  _oldProp.clear();
}

std::string CommandSetNodeProperties::dump()
{
  string ret ="CommandSetNodeProperties " + _position;
  return ret;
}

bool CommandSetNodeProperties::localExecute()
{
  DEBTRACE("CommandSetNodeProperties::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc;

      if (!_position.empty()) node = proc->getChildByName(_position);

      if (node)
        {
          _oldProp = node->getPropertyMap();
          node->setProperties(_properties);
          SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
          snode->update(SETVALUE, 0, snode);
          return true;
        }
      GuiContext::getCurrent()->_lastErrorMessage = "node not found: " + _position;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetNodeProperties::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetNodeProperties::localReverse()
{
  DEBTRACE("CommandSetNodeProperties::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc;

      if (!_position.empty()) node = proc->getChildByName(_position);

      if (node)
        {
          node->setProperties(_oldProp);
          SubjectNode *snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
          snode->update(SETVALUE, 0, snode);
          return true;
        }
      GuiContext::getCurrent()->_lastErrorMessage = "node not found: " + _position;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetNodeProperties::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetComponentInstanceProperties::CommandSetComponentInstanceProperties(std::string compoinstance,
                                                                             std::map<std::string,std::string> properties)
  : Command(), _compoinstance(compoinstance), _properties(properties)
{
  DEBTRACE("CommandSetComponentInstanceProperties::CommandSetComponentInstanceProperties " << compoinstance);
  _oldProp.clear();
}

std::string CommandSetComponentInstanceProperties::dump()
{
  string ret ="CommandSetComponentInstanceProperties " + _compoinstance;
  return ret;
}

bool CommandSetComponentInstanceProperties::localExecute()
{
  DEBTRACE("CommandSetComponentInstanceProperties::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->componentInstanceMap.count(_compoinstance))
        {
          ComponentInstance *ref = proc->componentInstanceMap[_compoinstance];
          YASSERT(ref);
          _oldProp = ref->getProperties();
          _oldAnon = ref->isAnonymous();
          ref->setProperties(_properties);
          ref->setAnonymous(false);
          SubjectComponent* subcompo = GuiContext::getCurrent()->_mapOfSubjectComponent[ref];
          subcompo->update(SETVALUE, 0, subcompo);
          return true;
        }
      GuiContext::getCurrent()->_lastErrorMessage = "compoinstance not found: " + _compoinstance;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetComponentInstanceProperties::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetComponentInstanceProperties::localReverse()
{
  DEBTRACE("CommandSetComponentInstanceProperties::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->componentInstanceMap.count(_compoinstance))
        {
          ComponentInstance *ref = proc->componentInstanceMap[_compoinstance];
          YASSERT(ref);
          ref->setProperties(_oldProp);
          ref->setAnonymous(_oldAnon);
          SubjectComponent* subcompo = GuiContext::getCurrent()->_mapOfSubjectComponent[ref];
          subcompo->update(SETVALUE, 0, subcompo);
          return true;
        }
      GuiContext::getCurrent()->_lastErrorMessage = "compoinstance not found: " + _compoinstance;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetComponentInstanceProperties::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetContainerProperties::CommandSetContainerProperties(std::string container,
                                                             std::map<std::string,std::string> properties)
  : Command(), _container(container), _properties(properties)
{
  DEBTRACE("CommandSetContainerProperties::CommandSetContainerProperties " << container);
  _oldProp.clear();
}

std::string CommandSetContainerProperties::dump()
{
  string ret ="CommandSetContainerProperties " + _container;
  return ret;
}

bool CommandSetContainerProperties::localExecute()
{
  DEBTRACE("CommandSetContainerProperties::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_container))
        {
          Container *ref(proc->containerMap[_container]);
          YASSERT(ref);
          _oldProp = ref->getProperties();
          ref->setProperties(_properties);
          SubjectContainerBase *scont(GuiContext::getCurrent()->_mapOfSubjectContainer[ref]);
          scont->update(UPDATE, 0, scont);
          scont->notifyComponentsChange(ASSOCIATE, CONTAINER, scont);
          return true;
        }
      GuiContext::getCurrent()->_lastErrorMessage = "container not found: " + _container;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetContainerProperties::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetContainerProperties::localReverse()
{
  DEBTRACE("CommandSetContainerProperties::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_container))
        {
          Container *ref = proc->containerMap[_container];
          YASSERT(ref);
          ref->setProperties(_oldProp);
          return true;
        }
      GuiContext::getCurrent()->_lastErrorMessage = "container not found: " + _container;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetContainerProperties::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetDSPortProperties::CommandSetDSPortProperties(std::string node, std::string port, bool isInport,
                                                       std::map<std::string,std::string> properties)
  : Command(), _nodeName(node), _portName(port), _isInport(isInport), _properties(properties)
{
  DEBTRACE("CommandSetDSPortProperties::CommandSetDSPortProperties " << node << "." << port << " " << isInport);
  _oldProp.clear();
}
      
std::string CommandSetDSPortProperties::dump()
{
  string s = "false";
  if (_isInport) s = "true";
  string ret ="CommandSetDSPortProperties " + _nodeName + " " + _portName + " " + s;
  return ret;
}

bool CommandSetDSPortProperties::localExecute()
{
  DEBTRACE("CommandSetDSPortProperties::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_nodeName);
      DataStreamPort* DSPort = 0;
      if (_isInport)
        DSPort = node->getInputDataStreamPort(_portName);
      else
        DSPort = node->getOutputDataStreamPort(_portName);
      _oldProp = DSPort->getProperties();
      DSPort->setProperties(_properties);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetDSPortProperties::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}
      
bool CommandSetDSPortProperties::localReverse()
{
  DEBTRACE("CommandSetDSPortProperties::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_nodeName);
      DataStreamPort* DSPort = 0;
      if (_isInport)
        DSPort = node->getInputDataStreamPort(_portName);
      else
        DSPort = node->getOutputDataStreamPort(_portName);
      DSPort->setProperties(_oldProp);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetDSPortProperties::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetLinkProperties::CommandSetLinkProperties(std::string startnode, std::string startport, 
                                                   std::string endnode, std::string endport,
                                                   std::map<std::string,std::string> properties)
  : Command(), _startNodeName(startnode), _startPortName(startport), 
    _endNodeName(endnode), _endPortName(endport),
    _properties(properties)
{
  DEBTRACE("CommandSetLinkProperties::CommandSetLinkProperties " );
  _oldProp.clear();
}

std::string CommandSetLinkProperties::dump()
{
  string ret ="CommandSetLinkProperties " + _startNodeName + " " + _startPortName + " " + _endNodeName + " " + _endPortName;
  return ret;
}

bool CommandSetLinkProperties::localExecute()
{
  DEBTRACE("CommandSetLinkProperties::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node;
      InputDataStreamPort* inDSPort = 0;
      OutputDataStreamPort* outDSPort = 0;

      node   = proc->getChildByName(_startNodeName);
      outDSPort = node->getOutputDataStreamPort(_startPortName);
      outDSPort->setProperties(_properties);

      node   = proc->getChildByName(_endNodeName);
      inDSPort = node->getInputDataStreamPort(_endPortName);
      _oldProp = inDSPort->getProperties();
      inDSPort->setProperties(_properties);

      std::pair<OutPort*,InPort*> keymap = std::pair<OutPort*,InPort*>(outDSPort,inDSPort);
      SubjectLink* subject = GuiContext::getCurrent()->_mapOfSubjectLink[keymap];
      subject->update(SETVALUE, 0, subject);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetDSPortProperties::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetLinkProperties::localReverse()
{
  DEBTRACE("CommandSetLinkProperties::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node;
      InputDataStreamPort* inDSPort = 0;
      OutputDataStreamPort* outDSPort = 0;

      node   = proc->getChildByName(_startNodeName);
      outDSPort = node->getOutputDataStreamPort(_startPortName);
      outDSPort->setProperties(_properties);

      node   = proc->getChildByName(_endNodeName);
      inDSPort = node->getInputDataStreamPort(_endPortName);
      inDSPort->setProperties(_oldProp);

      std::pair<OutPort*,InPort*> keymap = std::pair<OutPort*,InPort*>(outDSPort,inDSPort);
      SubjectLink* subject = GuiContext::getCurrent()->_mapOfSubjectLink[keymap];
      subject->update(SETVALUE, 0, subject);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetDSPortProperties::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandSetFuncNodeFunctionName::CommandSetFuncNodeFunctionName(std::string node, std::string funcName)
  : Command(), _nodeName(node), _funcName(funcName)
{
  DEBTRACE("CommandSetFuncNodeFunctionName::CommandSetFuncNodeFunctionName " << node << " " <<funcName);
  _oldName.clear();
}

std::string CommandSetFuncNodeFunctionName::dump()
{
  string ret ="CommandSetFuncNodeFunctionName " + _nodeName + " " + _funcName;
  return ret;
}

bool CommandSetFuncNodeFunctionName::localExecute()
{
  DEBTRACE("CommandSetFuncNodeFunctionName::localExecute");
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
          _oldName = funcNode->getFname();
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
      setErrorMsg(ex);
      return false;
    }  
}

bool CommandSetFuncNodeFunctionName::localReverse()
{
  DEBTRACE("CommandSetFuncNodeFunctionName::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_nodeName);
      if (YACS::ENGINE::InlineFuncNode* funcNode = dynamic_cast<YACS::ENGINE::InlineFuncNode*>(node))
        {
          funcNode->setFname(_oldName);
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
      DEBTRACE("CommandSetFuncNodeFunctionName::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }  
}

// ----------------------------------------------------------------------------

CommandSetInlineNodeScript::CommandSetInlineNodeScript(std::string node, std::string script)
  : Command(), _nodeName(node), _script(script)
{
  DEBTRACE("CommandSetInlineNodeScript::CommandSetInlineNodeScript " << node << " " <<script);
  _oldScript.clear();
}

std::string CommandSetInlineNodeScript::dump()
{
  string ret ="CommandSetInlineNodeScript " + _nodeName;
  return ret;
}

bool CommandSetInlineNodeScript::localExecute()
{
  DEBTRACE("CommandSetInlineNodeScript::localExecute");
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
          _oldScript = inlineNode->getScript();
          inlineNode->setScript(_script);
          YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
          SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
          snode->update(SYNCHRO,0,snode);
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
      setErrorMsg(ex);
      return false;
    }  
}

bool CommandSetInlineNodeScript::localReverse()
{
  DEBTRACE("CommandSetInlineNodeScript::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_nodeName);
      if (YACS::ENGINE::InlineNode* inlineNode = dynamic_cast<YACS::ENGINE::InlineNode*>(node))
        {
          inlineNode->setScript(_oldScript);
          YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
          SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
          snode->update(SYNCHRO,0,snode);
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
      setErrorMsg(ex);
      return false;
    }  
}

// ----------------------------------------------------------------------------

CommandAddComponentInstance::CommandAddComponentInstance(std::string compoName,
                                                         std::string container,
                                                         std::string name)
  : Command(), _compoName(compoName), _container(container), _name(name), _subcompo(0)
{
  DEBTRACE("CommandAddComponentInstance::CommandAddComponentInstance " <<_compoName);
}

std::string CommandAddComponentInstance::dump()
{
  string ret ="CommandAddComponentInstance " + _compoName + " " + _container + " " + _name;
  return ret;
}

bool CommandAddComponentInstance::localExecute()
{
  DEBTRACE("CommandAddComponentInstance::localExecute "<< _compoName << " " << _container << " " << _name);
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      YASSERT(proc->containerMap.count(_container));
      Container *cont = proc->containerMap[_container];
      ComponentInstance* compoInst = new SalomeComponent(_compoName);
      compoInst->setContainer(cont);
      proc->addComponentInstance(compoInst, _name);
      
      SubjectProc *sproc = GuiContext::getCurrent()->getSubjectProc();
      _subcompo = sproc->addSubjectComponent(compoInst);
      _name = compoInst->getInstanceName();
      DEBTRACE(_name);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddComponentInstance::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandAddComponentInstance::localReverse()
{
  DEBTRACE("CommandAddComponentInstance::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      YASSERT(proc->componentInstanceMap.count(_name));
      ComponentInstance *compo = proc->componentInstanceMap[_name];
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectComponent.count(compo));
      _subcompo = GuiContext::getCurrent()->_mapOfSubjectComponent[compo];
      YASSERT(!_subcompo->hasServices());
      Container *cont = compo->getContainer();
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(cont));
      SubjectContainerBase *subcont = GuiContext::getCurrent()->_mapOfSubjectContainer[cont];
      subcont->detachComponent(_subcompo);
      GuiContext::getCurrent()->_mapOfSubjectComponent.erase(compo);
      proc->removeComponentInstance(compo);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddComponentInstance::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------
CommandSetExecutionMode::CommandSetExecutionMode(std::string nodeName, std::string mode)
  : Command(), _mode(mode),_nodeName(nodeName)
{
  DEBTRACE("CommandSetExecutionMode::CommandSetExecutionMode " << nodeName << " " << mode);
  _oldmode = "local";
}

std::string CommandSetExecutionMode::dump()
{
  string ret ="CommandSetExecutionMode " + _mode + " " + _nodeName;
  return ret;
}

bool CommandSetExecutionMode::localExecute()
{
  DEBTRACE("CommandSetExecutionMode::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_nodeName);
      if (YACS::ENGINE::InlineNode* pyNode = dynamic_cast<YACS::ENGINE::InlineNode*>(node))
        {
          _oldmode = pyNode->getExecutionMode();
          pyNode->setExecutionMode(_mode);
          SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[pyNode];
          snode->update(UPDATE, 0, 0);
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
      DEBTRACE("CommandSetExecutionMode::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetExecutionMode::localReverse()
{
  DEBTRACE("CommandSetExecutionMode::localReverse");
  try
    {
      if (_oldmode == _mode) return true;
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc->getChildByName(_nodeName);
      if (YACS::ENGINE::InlineNode* pyNode = dynamic_cast<YACS::ENGINE::InlineNode*>(node))
        {
          pyNode->setExecutionMode(_oldmode);
          SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[pyNode];
          snode->update(UPDATE, 0, 0);
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
      DEBTRACE("CommandSetExecutionMode::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
  return true;
}


// ----------------------------------------------------------------------------

CommandSetContainer::CommandSetContainer(std::string nodeName, std::string container)
  : Command(), _container(container),_nodeName(nodeName)
{
  DEBTRACE("CommandSetContainer::CommandSetContainer " << nodeName << " " << container);
  _oldcont = "DefaultContainer";
}

std::string CommandSetContainer::dump()
{
  string ret ="CommandSetContainer " + _container + " " + _nodeName;
  return ret;
}

bool CommandSetContainer::localExecute()
{
  DEBTRACE("CommandSetContainer::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_container))
        {
          Container *cont = proc->containerMap[_container];
          Node* node = proc->getChildByName(_nodeName);
          if (YACS::ENGINE::InlineNode* pyNode = dynamic_cast<YACS::ENGINE::InlineNode*>(node))
            {
              Container* oldcont = pyNode->getContainer();
              if(oldcont)
                _oldcont = pyNode->getContainer()->getName();
              pyNode->setContainer(cont);
              SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[pyNode];
              SubjectContainerBase *subcont = GuiContext::getCurrent()->_mapOfSubjectContainer[cont];
              snode->update(ASSOCIATE, 0, subcont);
              return true;
            }
          else
            {
              GuiContext::getCurrent()->_lastErrorMessage = "node is not an InlineNode: " + _nodeName;
              return false;
            }
        }
      else
        GuiContext::getCurrent()->_lastErrorMessage = "Container not found: " + _container;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetContainer::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandSetContainer::localReverse()
{
  DEBTRACE("CommandSetContainer::localReverse");
  try
    {
      if (_oldcont == _container) return true;
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_oldcont))
        {
          Container *cont = proc->containerMap[_oldcont];
          Node* node = proc->getChildByName(_nodeName);
          if (YACS::ENGINE::InlineNode* pyNode = dynamic_cast<YACS::ENGINE::InlineNode*>(node))
            {
              pyNode->setContainer(cont);
              SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[pyNode];
              SubjectContainerBase *subcont = GuiContext::getCurrent()->_mapOfSubjectContainer[cont];
              snode->update(ASSOCIATE, 0, subcont);
              return true;
            }
          else
            {
              GuiContext::getCurrent()->_lastErrorMessage = "node is not an InlineNode: " + _nodeName;
              return false;
            }
        }
      else
        GuiContext::getCurrent()->_lastErrorMessage = "Container not found: " + _oldcont;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandSetContainer::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
  return true;
}


  
// ----------------------------------------------------------------------------

CommandAssociateComponentToContainer::CommandAssociateComponentToContainer(std::string instanceName,
                                                                           std::string container)
  : Command(), _container(container),_instanceName(instanceName)
{
  DEBTRACE("CommandAssociateComponentToContainer::CommandAssociateComponentToContainer " << instanceName << " " << container);
  _oldcont = "DefaultContainer";
}

std::string CommandAssociateComponentToContainer::dump()
{
  string ret ="CommandAssociateComponentToContainer " + _container + " " + _instanceName;
  return ret;
}

bool CommandAssociateComponentToContainer::localExecute()
{
  DEBTRACE("CommandAssociateComponentToContainer::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_container))
        {
          Container *cont = proc->containerMap[_container];
          if (proc->componentInstanceMap.count(_instanceName))
            {
              DEBTRACE(_instanceName);
              ComponentInstance *compo = proc->componentInstanceMap[_instanceName];
              if (compo->getContainer())
                _oldcont = compo->getContainer()->getName();
              compo->setContainer(cont);

              YASSERT(GuiContext::getCurrent()->_mapOfSubjectComponent.count(compo));
              SubjectComponent *scomp =  GuiContext::getCurrent()->_mapOfSubjectComponent[compo];
              YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(cont));
              SubjectContainerBase *subcont =  GuiContext::getCurrent()->_mapOfSubjectContainer[cont];
              scomp->addSubjectReference(subcont);
              if (scomp->_subRefContainer)
                subcont->moveComponent(scomp->_subRefContainer);
              else
                scomp->_subRefContainer = subcont->attachComponent(scomp);
              scomp->notifyServicesChange(ASSOCIATE, CONTAINER, subcont);
              return true;
            }
          else
            GuiContext::getCurrent()->_lastErrorMessage = "Component instance not found: " + _instanceName;
        }
      else
        GuiContext::getCurrent()->_lastErrorMessage = "Container not found: " + _container;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAssociateComponentToContainer::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandAssociateComponentToContainer::localReverse()
{
  DEBTRACE("CommandAssociateComponentToContainer::localReverse");
  try
    {
      if (_oldcont == _container) return true;
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (proc->containerMap.count(_oldcont))
        {
          Container *cont = proc->containerMap[_oldcont];
          if (proc->componentInstanceMap.count(_instanceName))
            {
              DEBTRACE(_instanceName);
              ComponentInstance *compo = proc->componentInstanceMap[_instanceName];
              compo->setContainer(cont);

              YASSERT(GuiContext::getCurrent()->_mapOfSubjectComponent.count(compo));
              SubjectComponent *scomp =  GuiContext::getCurrent()->_mapOfSubjectComponent[compo];
              YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(cont));
              SubjectContainerBase *subcont =  GuiContext::getCurrent()->_mapOfSubjectContainer[cont];
              scomp->addSubjectReference(subcont);
              if (scomp->_subRefContainer)
                subcont->moveComponent(scomp->_subRefContainer);
              else
                scomp->_subRefContainer = subcont->attachComponent(scomp);
              scomp->notifyServicesChange(ASSOCIATE, CONTAINER, subcont);
              return true;
            }
          else
            GuiContext::getCurrent()->_lastErrorMessage = "Component instance not found: " + _instanceName;
        }
      else
        GuiContext::getCurrent()->_lastErrorMessage = "Container not found: " + _oldcont;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAssociateComponentToContainer::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
  return true;
}

// ----------------------------------------------------------------------------

CommandAssociateServiceToComponent::CommandAssociateServiceToComponent(std::string service,
                                                                       std::string instanceName)
  : Command(), _service(service), _instanceName(instanceName)
{
  DEBTRACE("CommandAssociateServiceToComponent::CommandAssociateServiceToComponent "<< service << " " <<instanceName);
  _oldInstance="";
}

std::string CommandAssociateServiceToComponent::dump()
{
  string ret ="CommandAssociateServiceToComponent " + _service + " " + _instanceName;
  return ret;
}

bool CommandAssociateServiceToComponent::localExecute()
{
  DEBTRACE("CommandAssociateServiceToComponent::localExecute");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (_service == proc->getName()) return false; // proc is not an elementary node
      Node* node = proc->getChildByName(_service);
      if (ServiceNode *service = dynamic_cast<ServiceNode*>(node))
        {
          if (proc->componentInstanceMap.count(_instanceName))
            {
              DEBTRACE(_instanceName);
              ComponentInstance *compo = proc->componentInstanceMap[_instanceName];
              service->setComponent(compo);

              YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(service));
              SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[service];
              SubjectServiceNode *ssnode = dynamic_cast<SubjectServiceNode*>(snode);
              YASSERT(ssnode);
              YASSERT(GuiContext::getCurrent()->_mapOfSubjectComponent.count(compo));
              SubjectComponent *subCompo = GuiContext::getCurrent()->_mapOfSubjectComponent[compo];
              snode->addSubjectReference(subCompo);
              if (ssnode->_subRefComponent)
                {
                  SubjectComponent* oldcomp = dynamic_cast<SubjectComponent*>(ssnode->_subRefComponent->getParent());
                  YASSERT(oldcomp);
                  _oldInstance = oldcomp->getName();
                  _oldcont = oldcomp->getComponent()->getContainer()->getName();
                  subCompo->moveService(ssnode->_subRefComponent);
                }
              else
                ssnode->_subRefComponent = subCompo->attachService(ssnode);

              return true;
            }
          else
            GuiContext::getCurrent()->_lastErrorMessage = "Component instance not found: " + _instanceName;
        }
      else
        GuiContext::getCurrent()->_lastErrorMessage = "Node is not a service node: " + _service;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAssociateServiceToComponent::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

bool CommandAssociateServiceToComponent::localReverse()
{
  DEBTRACE("CommandAssociateServiceToComponent::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      if (_service == proc->getName()) return false; // proc is not an elementary node
      Node* node = proc->getChildByName(_service);
      if (ServiceNode *service = dynamic_cast<ServiceNode*>(node))
        {
          ComponentInstance *compo;
          if (!proc->componentInstanceMap.count(_oldInstance))
            {
              //component instance does not exist anymore recreate it
              ComponentInstance *oldcompo = service->getComponent();
              compo = oldcompo->cloneAlways();
              compo->setName(_oldInstance);
              proc->addComponentInstance(compo, _oldInstance);
              Container *cont = proc->containerMap[_oldcont];
              compo->setContainer(cont);
              SubjectProc *sproc = GuiContext::getCurrent()->getSubjectProc();
              sproc->addSubjectComponent(compo);
            }
          else
            {
              compo = proc->componentInstanceMap[_oldInstance];
            }

          DEBTRACE(_oldInstance);
          service->setComponent(compo);

          YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(service));
          SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[service];
          SubjectServiceNode *ssnode = dynamic_cast<SubjectServiceNode*>(snode);
          YASSERT(ssnode);
          YASSERT(GuiContext::getCurrent()->_mapOfSubjectComponent.count(compo));
          SubjectComponent *subCompo = GuiContext::getCurrent()->_mapOfSubjectComponent[compo];
          snode->addSubjectReference(subCompo);
          if (ssnode->_subRefComponent)
            subCompo->moveService(ssnode->_subRefComponent);
          else
            ssnode->_subRefComponent = subCompo->attachService(ssnode);

          return true;
        }
      else
        GuiContext::getCurrent()->_lastErrorMessage = "Node is not a service node: " + _service;
      return false;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAssociateServiceToComponent::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }
}

// ----------------------------------------------------------------------------

CommandAddComponentFromCatalog::CommandAddComponentFromCatalog(YACS::ENGINE::Catalog* catalog,
                                                               std::string position,
                                                               std::string compo,
                                                               std::string service)
  : Command(), _catalog(catalog), _position(position), _compo(compo), _service(service)
{
  DEBTRACE("CommandAddComponentFromCatalog::CommandAddComponentFromCatalog " << position << " " << compo << " " << service);
  _nameInProc="";
  _createdInstance = false;
}

std::string CommandAddComponentFromCatalog::dump()
{
  string ret = "CommandAddComponentFromCatalog " + _position + " " + _compo + " " + _service;
  return ret;
}
    
bool CommandAddComponentFromCatalog::localExecute()
{
  DEBTRACE("CommandAddComponentFromCatalog::localExecute");
  try
    {
      DEBTRACE("_nameInProc: " << _nameInProc);
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc;
      if (!_position.empty()) node = proc->getChildByName(_position);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      SubjectServiceNode *ssnode = dynamic_cast<SubjectServiceNode*>(snode);
      YASSERT(ssnode);
      if (_catalog->_componentMap.count(_compo))
        {
          YACS::ENGINE::ComponentDefinition* compodef = _catalog->_componentMap[_compo];
          if (compodef->_serviceMap.count(_service))
            {
              Proc* proc = GuiContext::getCurrent()->getProc();
              ComponentInstance *instance =ssnode->_serviceNode->getComponent();
              YASSERT(instance);
              SubjectComponent* subCompo = 0;
              _createdInstance = false;
              if(!GuiContext::getCurrent()->_mapOfSubjectComponent.count(instance))
                {
                  _createdInstance = true;
                  //automatic rename of the component instance by the proc on first execute
                  DEBTRACE("name given to proc:" << _nameInProc);
                  proc->addComponentInstance(instance,_nameInProc, true);
                  _nameInProc= instance->getInstanceName();
                  DEBTRACE("name given by proc:" << _nameInProc);
                  subCompo = GuiContext::getCurrent()->getSubjectProc()->addSubjectComponent(instance);
                }
              else 
                subCompo = GuiContext::getCurrent()->_mapOfSubjectComponent[instance];
              YASSERT(subCompo);
              ssnode->addSubjectReference(subCompo);
              YASSERT(! ssnode->_subRefComponent);
              ssnode->_subRefComponent = subCompo->attachService(ssnode);
            }
        }
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddComponentFromCatalog::localExecute() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }      
}

bool CommandAddComponentFromCatalog::localReverse()
{
  DEBTRACE("CommandAddComponentFromCatalog::localReverse");
  try
    {
      Proc* proc = GuiContext::getCurrent()->getProc();
      Node* node = proc;
      if (!_position.empty()) node = proc->getChildByName(_position);
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectNode.count(node));
      SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[node];
      SubjectServiceNode *ssnode = dynamic_cast<SubjectServiceNode*>(snode);
      YASSERT(ssnode);

      DEBTRACE("_nameInProc: " << _nameInProc);
      YASSERT(proc->componentInstanceMap.count(_nameInProc));
      ComponentInstance *compo = proc->componentInstanceMap[_nameInProc];
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectComponent.count(compo));
      SubjectComponent *subCompo = GuiContext::getCurrent()->_mapOfSubjectComponent[compo];

      subCompo->detachService(ssnode);
      if (subCompo->hasServices())
        throw YACS::Exception("Component instance with services attached, not removed");
      Container *cont = compo->getContainer();
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(cont));
      SubjectContainerBase *subcont = GuiContext::getCurrent()->_mapOfSubjectContainer[cont];
      subcont->detachComponent(subCompo);
      //remove componentInstance from proc, from context
      if (_createdInstance)
        {
          GuiContext::getCurrent()->_mapOfSubjectComponent.erase(compo);
          proc->removeComponentInstance(compo);
        }
      DEBTRACE("_nameInProc: " << _nameInProc);
      return true;
    }
  catch (Exception& ex)
    {
      DEBTRACE("CommandAddComponentFromCatalog::localReverse() : " << ex.what());
      setErrorMsg(ex);
      return false;
    }      
}

