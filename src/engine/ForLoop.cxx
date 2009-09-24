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
#include "LinkInfo.hxx"
#include "OutputPort.hxx"
#include "Visitor.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

/*! \class YACS::ENGINE::ForLoop
 *  \brief Class for for loop node
 *
 * \ingroup Nodes
 *
 *   This kind of loop makes a fixed number of steps and stops
 *
 */

const char ForLoop::NAME_OF_NSTEPS_NUMBER[]="nsteps";
const char ForLoop::NAME_OF_INDEX[]="index";

ForLoop::ForLoop(const std::string& name):Loop(name),_nbOfTimesPort(NAME_OF_NSTEPS_NUMBER,this,Runtime::_tc_int),
                                                     _indexPort(NAME_OF_INDEX,this,Runtime::_tc_int)
{
}

ForLoop::ForLoop(const ForLoop& other, ComposedNode *father, bool editionOnly):Loop(other,father,editionOnly),
                                                                               _nbOfTimesPort(other._nbOfTimesPort,this),
                                                                               _indexPort(other._indexPort,this)
{
}

Node *ForLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new ForLoop(*this,father,editionOnly);
}

InputPort* ForLoop::getInputPort(const std::string& name) const throw(YACS::Exception)
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
  _indexPort.exInit();
  Any* tmp=AtomAny::New(_nbOfTurns);
  _indexPort.put(tmp);
  tmp->decrRef();
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
      Any* tmp=AtomAny::New(_nbOfTurns);
      _indexPort.put(tmp);
      tmp->decrRef();
      setState(YACS::ACTIVATED);
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

OutPort *ForLoop::getOutPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_OF_INDEX)
    return (OutPort *)&_indexPort;
  return Loop::getOutPort(name);
}

OutputPort *ForLoop::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_OF_INDEX)
    return (OutputPort *)&_indexPort;
  return Loop::getOutputPort(name);
}

std::list<OutputPort *> ForLoop::getLocalOutputPorts() const
{
  list<OutputPort *> ret;
  ret.push_back(getOutputPort(NAME_OF_INDEX)); 
  return ret;
}

void ForLoop::checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                     std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                     std::vector<OutPort *>& fwCross,
                                     std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                     LinkInfo& info) const
{
  //First testing if start==indexPort. This is the only case possible in theory.
  if(start != &_indexPort)
    return StaticDefinedComposedNode::checkControlDependancy(start,end,cross,fw,fwCross,bw,info);
  if(cross)
    throw Exception("Internal error occured - cross type link detected on decision port of a loop. Forbidden !");
  fw[(ComposedNode *)this].push_back(start);
}

void ForLoop::checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, 
                           bool direction, LinkInfo& info) const
{
  const char what[]="ForLoop::checkCFLinks : internal error.";
  Node *nodeEnd=end->getNode();
  if(nodeEnd==this)
    {//In this case 'end' port is a special port of this (for exemple ForLoop::_nbOfTimesPort)
      solveObviousOrDelegateCFLinks(starts,end,alreadyFed,direction,info);
    }
  else if(isInMyDescendance(nodeEnd)==0)
    {
      solveObviousOrDelegateCFLinks(starts,end,alreadyFed,direction,info);
    }
  else
    {//no forwarding here.
      if(starts.size()!=1)
        throw Exception(what);

      Node *nodeStart=(*(starts.begin()))->getNode();
      if(nodeStart==this)
        {
          // Link between the loop and the internal node
          if(*(starts.begin())!=&_indexPort)
            throw Exception(what);
        }
      else
        {
          // Link from internal node to internal node
          if(nodeEnd!=nodeStart)
            throw Exception(what);
        }

      if(alreadyFed==FREE_ST)
        alreadyFed=FED_ST;
      else if(alreadyFed==FED_ST)
        {
          info.pushInfoLink(*(starts.begin()),end,I_USELESS);
        }
    }
}

std::list<OutputPort *> ForLoop::getSetOfOutputPort() const
{
  list<OutputPort *> ret=ComposedNode::getSetOfOutputPort();
  ret.push_back((OutputPort *)&_indexPort);
  return ret;
}

