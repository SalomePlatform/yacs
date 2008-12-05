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
#include "ForLoop.hxx"
#include "Runtime.hxx"
#include "OutputPort.hxx"
#include "Visitor.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

const char ForLoop::NAME_OF_NSTEPS_NUMBER[]="nsteps";

ForLoop::ForLoop(const std::string& name):Loop(name),_nbOfTimesPort(NAME_OF_NSTEPS_NUMBER,this,Runtime::_tc_int)
{
}

ForLoop::ForLoop(const ForLoop& other, ComposedNode *father, bool editionOnly):Loop(other,father,editionOnly),
                                                                               _nbOfTimesPort(other._nbOfTimesPort,this)
{
}

Node *ForLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new ForLoop(*this,father,editionOnly);
}

InputPort* ForLoop::getInputPort(const std::string& name) const throw(Exception)
{
    if(name == NAME_OF_NSTEPS_NUMBER)return (InputPort*)&_nbOfTimesPort;
    return Loop::getInputPort(name);

}

//! Initialize the node
/*!
 * \param start: a boolean flag to indicate the initialization mode
 *
 * If start is true, it's a complete initialization (with port values initialization)
 * If start is false, there is no port values initialization
 *
 */
void ForLoop::init(bool start)
{
  Loop::init(start);
  _nbOfTimesPort.exInit(start);
}

//! Update the state of the for loop
/*!
 * If the inGate port is ready goes to YACS::TOACTIVATE state
 * If the steps number is 0, create an special internal node
 *
 */
void ForLoop::exUpdateState()
{
  if(_state == YACS::DISABLED)
    return;
  if(_inGate.exIsReady())
    {
      setState(YACS::TOACTIVATE);
      _node->exUpdateState();
      if(_nbOfTimesPort.isEmpty())
        {
          delete _nodeForNullTurnOfLoops;
          _nodeForNullTurnOfLoops=new FakeNodeForLoop(this,false,true);
        }
      else
        {
          if(_nbOfTimesPort.getIntValue()==0)
            {
              bool normalFinish=getAllOutPortsLeavingCurrentScope().empty();
              delete _nodeForNullTurnOfLoops;
              _nodeForNullTurnOfLoops=new FakeNodeForLoop(this,normalFinish);
            }
          else if(_nbOfTimesPort.getIntValue()<0)
            {
              delete _nodeForNullTurnOfLoops;
              _nodeForNullTurnOfLoops=new FakeNodeForLoop(this,false);
            }
          else
            {
              delete _nodeForNullTurnOfLoops;
              _nodeForNullTurnOfLoops=0;
            }
        }
    }
}

//! Method used to notify the node that a child node has ended
/*!
 * Update the loop state and return the loop change state 
 *
 *  \param node : the child node that has ended
 *  \return the loop state change
 */
YACS::Event ForLoop::updateStateOnFinishedEventFrom(Node *node)
{
  if((++_nbOfTurns)>=_nbOfTimesPort.getIntValue())
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

void ForLoop::accept(Visitor *visitor)
{
  visitor->visitForLoop(this);
}

std::list<InputPort *> ForLoop::getLocalInputPorts() const
{
  list<InputPort *> ret;
  ret.push_back((InputPort *)&_nbOfTimesPort);
  return ret;
}
