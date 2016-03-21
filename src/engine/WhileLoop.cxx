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

#include "WhileLoop.hxx"
#include "Runtime.hxx"
#include "OutputPort.hxx"
#include "Visitor.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char WhileLoop::NAME_OF_INPUT_CONDITION[]="condition";

WhileLoop::WhileLoop(const std::string& name):Loop(name),_conditionPort(NAME_OF_INPUT_CONDITION,this)
{
}

WhileLoop::WhileLoop(const WhileLoop& other, ComposedNode *father, bool editionOnly):Loop(other,father,editionOnly),
                                                                                     _conditionPort(other._conditionPort,this)
{
  //Copy Data linking
  std::vector< std::pair<OutPort *, InPort *> > linksToReproduce=other.getSetOfInternalLinks();
  std::vector< std::pair<OutPort *, InPort *> >::iterator iter=linksToReproduce.begin();
  for(;iter!=linksToReproduce.end();++iter)
    {
      OutPort* pout = iter->first;
      InPort* pin = iter->second;
      edAddLink(getOutPort(other.getPortName(pout)),getInPort(other.getPortName(pin)));
    }
}

void WhileLoop::init(bool start)
{
  Loop::init(start);
  _conditionPort.exInit(start);
}

void WhileLoop::exUpdateState()
{
  if(_state == YACS::DISABLED)
    return;
  if(_inGate.exIsReady())
    {
      setState(YACS::ACTIVATED);
      _node->exUpdateState();
      if(_conditionPort.isLinkedOutOfScope())
        if(_conditionPort.isEmpty())
          {
            delete _nodeForNullTurnOfLoops;
            _nodeForNullTurnOfLoops=new FakeNodeForLoop(this,false,true);
          }
        else
          if(!_conditionPort.getValue())
            {
              bool normalFinish=getAllOutPortsLeavingCurrentScope().empty();
              delete _nodeForNullTurnOfLoops;
              _nodeForNullTurnOfLoops=new FakeNodeForLoop(this,normalFinish);
            }
          else
            {
              delete _nodeForNullTurnOfLoops;
              _nodeForNullTurnOfLoops=0;
            }
    }
}

Node *WhileLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new WhileLoop(*this,father,editionOnly);
}

InputPort *WhileLoop::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_OF_INPUT_CONDITION)
    return (InputPort*)&_conditionPort;
  return Loop::getInputPort(name);
}

//! Method used to notify the node that a child node has ended
/*!
 * Update the loop state and return the loop change state
 *
 *  \param node : the child node that has ended
 *  \return the loop state change
 */
YACS::Event WhileLoop::updateStateOnFinishedEventFrom(Node *node)
{
  _nbOfTurns++;
  if(!_conditionPort.getValue())
    {
      setState(YACS::DONE);
      return YACS::FINISH;
    }
  else    
    {
      node->init(false);
      node->exUpdateState();
    }
  return YACS::NOEVENT;
}

void WhileLoop::checkLinkPossibility(OutPort *start, 
                                     const std::list<ComposedNode *>& pointsOfViewStart,
                                     InPort *end, 
                                     const std::list<ComposedNode *>& pointsOfViewEnd) throw(YACS::Exception)
{
  DEBTRACE("WhileLoop::checkLinkPossibility");
}

void WhileLoop::accept(Visitor *visitor)
{
  visitor->visitWhileLoop(this);
}

std::list<InputPort *> WhileLoop::getLocalInputPorts() const
{
  list<InputPort *> ret;
  ret.push_back((InputPort *)&_conditionPort);
  return ret;
}
