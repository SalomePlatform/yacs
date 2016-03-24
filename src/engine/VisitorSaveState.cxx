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

#include "ElementaryNode.hxx"
#include "Bloc.hxx"
#include "Proc.hxx"
#include "ForEachLoop.hxx"
#include "OptimizerLoop.hxx"
#include "Loop.hxx"
#include "ForLoop.hxx"
#include "WhileLoop.hxx"
#include "Switch.hxx"
#include "InputPort.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "ServerNode.hxx"
#include "ServiceInlineNode.hxx"
#include "DataNode.hxx"

#include "VisitorSaveState.hxx"

#include <iostream>
#include <string>

using namespace YACS::ENGINE;
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

VisitorSaveState::VisitorSaveState(ComposedNode *root): Visitor(root)
{
  _nodeStateName[YACS::READY] ="READY";
  _nodeStateName[YACS::TOLOAD] ="TOLOAD";
  _nodeStateName[YACS::LOADED] ="LOADED";
  _nodeStateName[YACS::TOACTIVATE] ="TOACTIVATE";
  _nodeStateName[YACS::ACTIVATED] ="ACTIVATED";
  _nodeStateName[YACS::DESACTIVATED] ="DESACTIVATED";
  _nodeStateName[YACS::DONE] ="DONE";
  _nodeStateName[YACS::SUSPENDED] ="SUSPENDED";
  _nodeStateName[YACS::LOADFAILED] ="LOADFAILED";
  _nodeStateName[YACS::EXECFAILED] ="EXECFAILED";
  _nodeStateName[YACS::PAUSE] ="PAUSE";
  _nodeStateName[YACS::INTERNALERR] ="INTERNALERR";
  _nodeStateName[YACS::DISABLED] ="DISABLED";
  _nodeStateName[YACS::FAILED] ="FAILED";
  _nodeStateName[YACS::ERROR] ="ERROR";
}

VisitorSaveState::~VisitorSaveState()
{
  if (_out)
    {
      _out << "</graphState>" << endl;
      _out.close();
    }
}

void VisitorSaveState::openFileDump(const std::string& xmlDump) throw(YACS::Exception)
{
  _out.open(xmlDump.c_str(), ios::out);
  if (!_out)
    {
      string what = "Impossible to open file for writing: " + xmlDump;
      throw Exception(what);
    }
  _out << "<?xml version='1.0'?>" << endl;
  _out << "<graphState>" << endl;
}

void VisitorSaveState::closeFileDump()
{
  if (!_out) throw Exception("No file open for dump state");
  _out << "</graphState>" << endl;
  _out.close();
}

void VisitorSaveState::visitElementaryNode(ElementaryNode *node)
{
  if (!_out) throw Exception("No file open for dump state");
  string name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitElementaryNode --- " << name);
  _out << "  <node type='elementaryNode'>" << endl;
  _out << "    <name>" << name << "</name>" << endl;
  int nodeState = node->getState();
  _out << "    <state>" << _nodeStateName[nodeState] << "</state>" << endl;

  list<InputPort *> setOfInputPort = node->getSetOfInputPort();
  list<InputPort *>::iterator iter;
  for(iter = setOfInputPort.begin(); iter != setOfInputPort.end(); iter++)
    {
      _out << "    <inputPort>" << endl;
      _out << "      <name>" << (*iter)->getName() << "</name>" << endl;
      try
        {
          _out << "      ";
          _out << (*iter)->dump();
        }
      catch (YACS::Exception &e)
        {
          DEBTRACE("caught YACS:Exception: " << e.what());
          _out << "<value><error><![CDATA[" << e.what() << "]]></error></value>" << endl;
        }
      _out << "    </inputPort>" << endl;
    }

  list<OutputPort *> setOfOutputPort = node->getSetOfOutputPort();
  list<OutputPort *>::iterator oiter;
  for(oiter = setOfOutputPort.begin(); oiter != setOfOutputPort.end(); oiter++)
    {
      _out << "    <outputPort>" << endl;
      _out << "      <name>" << (*oiter)->getName() << "</name>" << endl;
      try
        {
          _out << "      ";
          _out << (*oiter)->dump();
        }
      catch (YACS::Exception &e)
        {
          DEBTRACE("caught YACS:Exception: " << e.what());
          _out << "<value><error><![CDATA[" << e.what() << "]]></error></value>" << endl;
        }
      _out << "    </outputPort>" << endl;
    }

  _out << "  </node>" << endl;
}

void VisitorSaveState::visitBloc(Bloc *node)
{
  node->ComposedNode::accept(this);
  if (!_out) throw Exception("No file open for dump state");
  string name = _root->getName();
  if (static_cast<ComposedNode*>(node) != _root) name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitBloc ------------- " << name);
  _out << "  <node type='bloc'>" << endl;
  _out << "    <name>" << name << "</name>" << endl;
  _out << "    <state>" << _nodeStateName[node->getState()] << "</state>" << endl;

  _out << "  </node>" << endl;
}

void VisitorSaveState::visitProc(Proc *node)
{
  node->ComposedNode::accept(this);
  if (!_out) throw Exception("No file open for dump state");
  string name = _root->getName();
  if (static_cast<ComposedNode*>(node) != _root) name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitProc ------------- " << name);
  _out << "  <node type='proc'>" << endl;
  _out << "    <name>" << name << "</name>" << endl;
  _out << "    <state>" << _nodeStateName[node->getState()] << "</state>" << endl;

  _out << "  </node>" << endl;
}

void VisitorSaveState::visitForEachLoop(ForEachLoop *node)
{
  node->ComposedNode::accept(this);
  if (!_out) throw Exception("No file open for dump state");
  string name = _root->getName();
  if (static_cast<ComposedNode*>(node) != _root) name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitForEachLoop ------ " << name);
  _out << "  <node type='forEachLoop'>" << endl;
  _out << "    <name>" << name << "</name>" << endl;
  _out << "    <state>" << _nodeStateName[node->getState()] << "</state>" << endl;

  _out << "  </node>" << endl;
}

void VisitorSaveState::visitOptimizerLoop(OptimizerLoop *node)
{
  node->ComposedNode::accept(this);
  if (!_out) throw Exception("No file open for dump state");
  string name = _root->getName();
  if (static_cast<ComposedNode*>(node) != _root) name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitOptimizerLoop ------ " << name);
  _out << "  <node type='optimizerLoop'>" << endl;
  _out << "    <name>" << name << "</name>" << endl;
  _out << "    <state>" << _nodeStateName[node->getState()] << "</state>" << endl;

  _out << "  </node>" << endl;
}

void VisitorSaveState::visitDynParaLoop(DynParaLoop *node)
{
  node->ComposedNode::accept(this);
}

void VisitorSaveState::visitLoop(Loop *node)
{
  node->ComposedNode::accept(this);
  if (!_out) throw Exception("No file open for dump state");
  string name = _root->getName();
  if (static_cast<ComposedNode*>(node) != _root) name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitLoop ------------- " << name);
  _out << "  <node type ='loop'>" << endl;
  _out << "    <name>" << name << "</name>" << endl;
  _out << "    <state>" << _nodeStateName[node->getState()] << "</state>" << endl;
  _out << "    <nbdone>" << node->getNbOfTurns() << "</nbdone>" << endl;

  _out << "  </node>" << endl;
}

void VisitorSaveState::visitForLoop(ForLoop *node)
{
  node->ComposedNode::accept(this);
  if (!_out) throw Exception("No file open for dump state");
  string name = _root->getName();
  if (static_cast<ComposedNode*>(node) != _root) name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitForLoop ---------- " << name);
  _out << "  <node type='forLoop'>" << endl;
  _out << "    <name>" << name << "</name>" << endl;
  _out << "    <state>" << _nodeStateName[node->getState()] << "</state>" << endl;
  _out << "    <nbdone>" << node->getNbOfTurns() << "</nbdone>" << endl;
  InputPort * ip = node->edGetNbOfTimesInputPort();
  if (ip->isEmpty())
    throw Exception("NbOfTimesInputPort in forLoop empty, case not handled yet...");
  Any *val = static_cast<Any*>(ip->get());
  int nsteps = val->getIntValue();
  _out << "    <nsteps>" << nsteps << "</nsteps>" << endl;

  _out << "  </node>" << endl;
}

void VisitorSaveState::visitWhileLoop(WhileLoop *node)
{
  node->ComposedNode::accept(this);
  if (!_out) throw Exception("No file open for dump state");
  string name = _root->getName();
  if (static_cast<ComposedNode*>(node) != _root) name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitWhileLoop -------- " << name);
  _out << "  <node type='whileLoop'>" << endl;
  _out << "    <name>" << name << "</name>" << endl;
  _out << "    <state>" << _nodeStateName[node->getState()] << "</state>" << endl;
  _out << "    <nbdone>" << node->getNbOfTurns() << "</nbdone>" << endl;
  InputPort * ip = node->edGetConditionPort();
  if (ip->isEmpty())
    throw Exception("condition in WhileLoop empty, case not handled yet...");
  if ( ConditionInputPort* cip = dynamic_cast<ConditionInputPort*>(ip) )
  {
    bool condition = cip->getValue();
    _out << "    <condition>" << condition << "</condition>" << endl;
  }

  _out << "  </node>" << endl;
}

void VisitorSaveState::visitSwitch(Switch *node)
{
  node->ComposedNode::accept(this);
  if (!_out) throw Exception("No file open for dump state");
  string name = _root->getName();
  if (static_cast<ComposedNode*>(node) != _root) name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitSwitch ----------- " << name);
  _out << "  <node type='switch'>" << endl;
  _out << "    <name>" << name << "</name>" << endl;
  _out << "    <state>" << _nodeStateName[node->getState()] << "</state>" << endl;
  InputPort * ip = node->edGetConditionPort();
  if (ip->isEmpty())
    throw Exception("condition in switch empty, case not handled yet...");
  Any *val = static_cast<Any*>(ip->get());
  int condition = val->getIntValue();
  _out << "    <condition>" << condition << "</condition>" << endl;
 
  _out << "  </node>" << endl;
}

void VisitorSaveState::visitInlineNode(InlineNode *node)
{
  visitElementaryNode(node);
}

void VisitorSaveState::visitInlineFuncNode(InlineFuncNode *node)
{
  visitElementaryNode(node);
}

void VisitorSaveState::visitServiceNode(ServiceNode *node)
{
  visitElementaryNode(node);
}

void VisitorSaveState::visitServerNode(ServerNode *node)
{
  visitElementaryNode(node);
}

void VisitorSaveState::visitServiceInlineNode(ServiceInlineNode *node)
{
  visitElementaryNode(node);
}

void VisitorSaveState::visitPresetNode(DataNode *node)
{
  visitElementaryNode(node);
}

void VisitorSaveState::visitOutNode(DataNode *node)
{
  visitElementaryNode(node);
}

void VisitorSaveState::visitStudyInNode(DataNode *node)
{
  visitElementaryNode(node);
}

void VisitorSaveState::visitStudyOutNode(DataNode *node)
{
  visitElementaryNode(node);
}
