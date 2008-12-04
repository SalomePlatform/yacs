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
#include "ElementaryNode.hxx"
#include "Runtime.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ComposedNode.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "Visitor.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

ElementaryNode::ElementaryNode(const std::string& name):Node(name)
{
}

ElementaryNode::ElementaryNode(const ElementaryNode& other, ComposedNode *father):Node(other,father)
{
  for(list<InputPort *>::const_iterator iter1=other._setOfInputPort.begin();iter1!=other._setOfInputPort.end();iter1++)
    _setOfInputPort.push_back((InputPort *)(*iter1)->clone(this));
  for(list<OutputPort *>::const_iterator iter2=other._setOfOutputPort.begin();iter2!=other._setOfOutputPort.end();iter2++)
    _setOfOutputPort.push_back((OutputPort *)(*iter2)->clone(this));
  for(list<InputDataStreamPort *>::const_iterator iter3=other._setOfInputDataStreamPort.begin();iter3!=other._setOfInputDataStreamPort.end();iter3++)
    _setOfInputDataStreamPort.push_back((InputDataStreamPort *)(*iter3)->clone(this));
  for(list<OutputDataStreamPort *>::const_iterator iter4=other._setOfOutputDataStreamPort.begin();iter4!=other._setOfOutputDataStreamPort.end();iter4++)
    _setOfOutputDataStreamPort.push_back((OutputDataStreamPort *)(*iter4)->clone(this));
}

void ElementaryNode::performDuplicationOfPlacement(const Node& other)
{
}

ElementaryNode::~ElementaryNode()
{
  for(list<InputPort *>::iterator iter1=_setOfInputPort.begin();iter1!=_setOfInputPort.end();iter1++)
    delete *iter1;
  for(list<OutputPort *>::iterator iter2=_setOfOutputPort.begin();iter2!=_setOfOutputPort.end();iter2++)
    delete *iter2;
  for(list<InputDataStreamPort *>::iterator iter3=_setOfInputDataStreamPort.begin();iter3!=_setOfInputDataStreamPort.end();iter3++)
    delete *iter3;
  for(list<OutputDataStreamPort *>::iterator iter4=_setOfOutputDataStreamPort.begin();iter4!=_setOfOutputDataStreamPort.end();iter4++)
    delete *iter4;
}

void ElementaryNode::init(bool start)
{
  for(list<OutputPort *>::iterator iter=_setOfOutputPort.begin();iter!=_setOfOutputPort.end();iter++)
    (*iter)->exInit();
  for(list<InputPort *>::iterator iter2=_setOfInputPort.begin();iter2!=_setOfInputPort.end();iter2++)
    (*iter2)->exInit(start);
  _inGate.exReset();
  if(_state == YACS::DISABLED)
    {
      exDisabledState(); // to refresh propagation of DISABLED state
      return;
    }
  if(start) //complete initialization
    setState(YACS::READY);
  else //partial initialization (inside a loop)
    setState(YACS::LOADED);
}

bool ElementaryNode::isDeployable() const
{
  return false;
}

ComponentInstance *ElementaryNode::getComponent()
{
  return 0;
}

YACS::StatesForNode ElementaryNode::getState() const
{
  return Node::getState();
}

void ElementaryNode::exUpdateState()
{
  DEBTRACE("ElementaryNode::exUpdateState: " << getName() << " " << _state );
  if(_state==YACS::DISABLED)return;
  if(_inGate.exIsReady())
    if(areAllInputPortsValid())
      {
        if(_state == YACS::READY)
          ensureLoading();
        else if(_state == YACS::LOADED)
          setState(YACS::TOACTIVATE);
      }
    else
      {
        string what("ElementaryNode::exUpdateState : Invalid graph given : Node with name \"");
        what+=_name; what+="\" ready to run whereas some inputports are not set correctly\nCheck coherence DF/CF";
        setState(YACS::INTERNALERR);
        _errorDetails=what;
        throw Exception(what);
      }
}

int ElementaryNode::getNumberOfInputPorts() const
{
  return _setOfInputPort.size();
}

int ElementaryNode::getNumberOfOutputPorts() const
{
  return _setOfOutputPort.size();
}

InputPort *ElementaryNode::getInputPort(const std::string& name) const throw(Exception)
{
  return getPort<InputPort>(name,_setOfInputPort);
}

OutputPort *ElementaryNode::getOutputPort(const std::string& name) const throw(Exception)
{
  return getPort<OutputPort>(name,_setOfOutputPort);
}

std::set<OutPort *> ElementaryNode::getAllOutPortsLeavingCurrentScope() const
{
  set<OutPort *> ret;
  list<OutPort *> temp=getSetOfOutPort();
  for(list<OutPort *>::iterator iter2=temp.begin();iter2!=temp.end();iter2++)
    {
      set<InPort *> temp2=(*iter2)->edSetInPort();
      if(temp2.size()!=0)
        ret.insert(*iter2);
    }
  return ret;
}

std::set<InPort *> ElementaryNode::getAllInPortsComingFromOutsideOfCurrentScope() const
{
  set<InPort *> ret;
  list<InPort *> temp=getSetOfInPort();
  for(list<InPort *>::iterator iter2=temp.begin();iter2!=temp.end();iter2++)
    {
      set<OutPort *> temp2=(*iter2)->edSetOutPort();
      if(temp2.size()!=0)
        ret.insert(*iter2);
    }
  return ret;
}

std::vector< std::pair<OutPort *, InPort *> > ElementaryNode::getSetOfLinksLeavingCurrentScope() const
{
  vector< pair<OutPort *, InPort *> > ret;
  std::set<OutPort *> ports=getAllOutPortsLeavingCurrentScope();
  for(set<OutPort *>::iterator iter2=ports.begin();iter2!=ports.end();iter2++)
    {
      set<InPort *> temp2=(*iter2)->edSetInPort();
      for(set<InPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        ret.push_back(pair<OutPort *, InPort *>(*iter2,*iter3));
    }
  return ret;
}

std::vector< std::pair<InPort *, OutPort *> > ElementaryNode::getSetOfLinksComingInCurrentScope() const
{
  vector< pair<InPort *, OutPort *> > ret;
  set<InPort *> ports=getAllInPortsComingFromOutsideOfCurrentScope();
  for(set<InPort *>::iterator iter2=ports.begin();iter2!=ports.end();iter2++)
    {
      set<OutPort *> temp2=(*iter2)->edSetOutPort();
      for(set<OutPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        ret.push_back(pair<InPort *, OutPort *>(*iter2,*iter3));
    }
  return ret;
}

InputDataStreamPort *ElementaryNode::getInputDataStreamPort(const std::string& name) const throw(Exception)
{
  return getPort<InputDataStreamPort>(name,_setOfInputDataStreamPort);
}

OutputDataStreamPort *ElementaryNode::getOutputDataStreamPort(const std::string& name) const throw(Exception)
{
  return getPort<OutputDataStreamPort>(name,_setOfOutputDataStreamPort);
}

void ElementaryNode::edDisconnectAllLinksWithMe()
{
  //CF
  Node::edDisconnectAllLinksWithMe();
  //Leaving part
  // - DF
  for(list<InputPort *>::iterator iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    (*iter)->edRemoveAllLinksLinkedWithMe();
  // - DS
  for(list<InputDataStreamPort *>::iterator iter2=_setOfInputDataStreamPort.begin();iter2!=_setOfInputDataStreamPort.end();iter2++)
    (*iter2)->edRemoveAllLinksLinkedWithMe();
  //Arriving part
  // - DF
  for(list<OutputPort *>::iterator iter=_setOfOutputPort.begin();iter!=_setOfOutputPort.end();iter++)
    (*iter)->edRemoveAllLinksLinkedWithMe();
  // - DS
  for(list<OutputDataStreamPort *>::iterator iter2=_setOfOutputDataStreamPort.begin();iter2!=_setOfOutputDataStreamPort.end();iter2++)
    (*iter2)->edRemoveAllLinksLinkedWithMe();
}

/**
 * checks if all input ports contains a valid data. Used at execution to change the state of the node
 * for activation.
 */

bool ElementaryNode::areAllInputPortsValid() const
{
  bool ret=true;
  for(list<InputPort *>::const_iterator iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    {
      ret=!(*iter)->isEmpty();
      if (!ret) break;
    }
  return ret;
}

/**
 * add this node task to a given set of ready tasks, if this task is ready to activate
 */

void ElementaryNode::getReadyTasks(std::vector<Task *>& tasks)
{
  DEBTRACE("ElementaryNode::getReadyTasks: " << getName() << " " << _state);
  if(_state==YACS::TOACTIVATE or _state==YACS::TOLOAD)
    tasks.push_back(this);
}

/**
 * remove port from node at edition. Ports are typed.
 */

void ElementaryNode::edRemovePort(Port *port) throw(Exception)
{
  DEBTRACE("ElementaryNode::edRemovePort ");
  if(port->getNode()!=this)
    throw Exception("ElementaryNode::edRemovePort : Port is not held by this node");
  if(InputPort *p=dynamic_cast<InputPort *>(port))
    edRemovePortTypedFromSet<InputPort>(p,_setOfInputPort);
  else if(OutputPort *p=dynamic_cast<OutputPort *>(port))
    edRemovePortTypedFromSet<OutputPort>(p,_setOfOutputPort);
  else if(InputDataStreamPort *p=dynamic_cast<InputDataStreamPort *>(port))
    edRemovePortTypedFromSet<InputDataStreamPort>(p,_setOfInputDataStreamPort);
  else if(OutputDataStreamPort *p=dynamic_cast<OutputDataStreamPort *>(port))
    edRemovePortTypedFromSet<OutputDataStreamPort>(p,_setOfOutputDataStreamPort);
  else
    throw Exception("ElementaryNode::edRemovePort : unknown port type");
  delete port;
  modified();
}

/**
 * @return a set with only this node. (Same method in composed nodes)
 */

list<ElementaryNode *> ElementaryNode::getRecursiveConstituents() const
{
  list<ElementaryNode *> ret;
  ret.push_back((ElementaryNode *)this);
  return ret;
}

Node *ElementaryNode::getChildByName(const std::string& name) const throw(Exception)
{
  string what("ElementaryNode does not agregate any nodes particullary node with name "); what+=name;
  throw Exception(what);
}

void ElementaryNode::checkBasicConsistency() const throw(Exception)
{
  DEBTRACE("ElementaryNode::checkBasicConsistency");
  list<InputPort *>::const_iterator iter;
  for(iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    (*iter)->checkBasicConsistency();  
}

ComposedNode *ElementaryNode::getDynClonerIfExists(const ComposedNode *levelToStop) const
{
  for(ComposedNode *iter=_father;iter!=levelToStop && iter!=0; iter=iter->_father)
    if(!iter->isPlacementPredictableB4Run())
      return iter;
  return 0;
}

InputPort *ElementaryNode::createInputPort(const std::string& inputPortName, TypeCode* type)
{
  return getRuntime()->createInputPort(inputPortName, _implementation, this, type);
}

/**
 * the input port is also published recursively in ancestors because it may be visible from everywhere.
 * WARNING: CHECK CASE OF BLOC: ONLY INPUT PORTS NOT INTERNALLY CONNECTED MUST BE VISIBLE.
 */

InputPort *ElementaryNode::edAddInputPort(const std::string& inputPortName, TypeCode* type) throw(Exception)
{
  InputPort *ret = 0;
  if (edCheckAddPort<InputPort, TypeCode*>(inputPortName,_setOfInputPort,type))
    {
      ret = createInputPort(inputPortName, type);
      _setOfInputPort.push_back(ret);
      modified();
      /*
      ComposedNode *iter=_father;
      while(iter)
        iter=iter->_father;
        */
    }
  return ret;
}

void ElementaryNode::edOrderInputPorts(const std::list<InputPort*>& ports)
{
  std::set<InputPort *> s1;
  std::set<InputPort *> s2;
  for(list<InputPort *>::const_iterator it=_setOfInputPort.begin();it != _setOfInputPort.end();it++)
    s1.insert(*it);
  for(list<InputPort *>::const_iterator it=ports.begin();it != ports.end();it++)
    s2.insert(*it);

  if(s1 != s2)
    throw Exception("ElementaryNode::edOrderInputPorts : port list must contain same ports as existing ones");

  _setOfInputPort.clear();
  for(list<InputPort *>::const_iterator it=ports.begin();it != ports.end();it++)
    _setOfInputPort.push_back(*it);
}

void ElementaryNode::edOrderOutputPorts(const std::list<OutputPort*>& ports)
{
  std::set<OutputPort *> s1;
  std::set<OutputPort *> s2;
  for(list<OutputPort *>::const_iterator it=_setOfOutputPort.begin();it != _setOfOutputPort.end();it++)
    s1.insert(*it);
  for(list<OutputPort *>::const_iterator it=ports.begin();it != ports.end();it++)
    s2.insert(*it);

  if(s1 != s2)
    throw Exception("ElementaryNode::edOrderOutputPorts : port list must contain same ports as existing ones");

  _setOfOutputPort.clear();
  for(list<OutputPort *>::const_iterator it=ports.begin();it != ports.end();it++)
    _setOfOutputPort.push_back(*it);
}

OutputPort *ElementaryNode::createOutputPort(const std::string& outputPortName, TypeCode* type)
{
  return getRuntime()->createOutputPort(outputPortName, _implementation, this, type);
}

/**
 * TO SOLVE : The output port is not published in father. Father must create an output port.
 * for now, publication is done the same way as input ports
 */ 

OutputPort *ElementaryNode::edAddOutputPort(const std::string& outputPortName, TypeCode* type) throw(Exception)
{
  OutputPort *ret =0;
  if (edCheckAddPort<OutputPort, TypeCode*>(outputPortName,_setOfOutputPort,type))
    {
      ret = createOutputPort(outputPortName, type);
      _setOfOutputPort.push_back(ret);
      modified();
      /*
      ComposedNode *iter=_father;
      while(iter)
        iter=iter->_father;
        */
    }
  return ret;
}

InputDataStreamPort *ElementaryNode::createInputDataStreamPort(const std::string& inputPortDSName, TypeCode* type)
{
  return getRuntime()->createInputDataStreamPort(inputPortDSName, this, type);
}

InputDataStreamPort *ElementaryNode::edAddInputDataStreamPort(const std::string& inputPortDSName, TypeCode* type) throw(Exception)
{
  InputDataStreamPort *ret = 0;
  if (edCheckAddPort<InputDataStreamPort, TypeCode*>(inputPortDSName,_setOfInputDataStreamPort,type))
    {
      ret = createInputDataStreamPort(inputPortDSName, type);
      _setOfInputDataStreamPort.push_back(ret);
      modified();
    }
  return ret;
}

OutputDataStreamPort *ElementaryNode::createOutputDataStreamPort(const std::string& outputPortDSName, TypeCode* type)
{
  return getRuntime()->createOutputDataStreamPort(outputPortDSName, this, type);
}

OutputDataStreamPort *ElementaryNode::edAddOutputDataStreamPort(const std::string& outputPortDSName, TypeCode* type) throw(Exception)
{
  OutputDataStreamPort *ret = 0;
  if (edCheckAddPort<OutputDataStreamPort, TypeCode*>(outputPortDSName,_setOfOutputDataStreamPort,type))
    {
      ret = createOutputDataStreamPort(outputPortDSName, type);
      _setOfOutputDataStreamPort.push_back(ret);
      modified();
    }
  return ret;
}

/**
 * get the input port name used by the current node (see composed nodes)
 */

string ElementaryNode::getInPortName(const InPort * inPort) const throw (Exception)
{
  Node *node = inPort->getNode();
  if ( node != this ) 
    {
      string what("InputPort "); what += inPort->getName(); what += " does not belong to node "; what += node->getName();
      throw Exception(what);
    }
  return inPort->getName();
}

string ElementaryNode::getOutPortName(const OutPort *outPort) const throw (Exception)
{
  Node *node = outPort->getNode();
  if ( node != this ) 
    {
      string what("OutputPort "); what += outPort->getName(); what += " does not belong to node "; what += node->getName();
      throw Exception(what);
    }
  return outPort->getName();
}

void ElementaryNode::begin()
{
  setState(ACTIVATED);
}

bool ElementaryNode::isReady()
{
  return _state==TOACTIVATE;
}

void ElementaryNode::finished()
{
  setState(DONE);
}
void ElementaryNode::aborted()
{
  setState(ERROR);
}

//! Notify this node that it is loaded
/*!
 * When an elementary node has been loaded 
 * It is ready to be connected
 *
 */
void ElementaryNode::loaded()
{
}

//! Notify this node that it is connected
/*!
 * When an elementary node has been connected it goes to TOACTIVATE state
 * It is then ready to be executed
 *
 */
void ElementaryNode::connected()
{
  if(_inGate.exIsReady())
    if(areAllInputPortsValid())
      {
        setState(TOACTIVATE);
        return;
      }
  setState(LOADED);
}

void ElementaryNode::accept(Visitor *visitor)
{
  visitor->visitElementaryNode(this);
}

//! Give a description of error when node status is ERROR
/*!
 *
 */
std::string ElementaryNode::getErrorDetails()
{
  return _errorDetails;
}

void ElementaryNode::edUpdateState()
{
  DEBTRACE("ElementaryNode::edUpdateState: " << getName());
  YACS::StatesForNode state=YACS::READY;
  try
    {
      checkBasicConsistency();
      _errorDetails="";
    }
  catch(Exception& e)
    {
      state=YACS::INVALID;
      _errorDetails=e.what();
    }
  DEBTRACE("ElementaryNode::edUpdateState: " << _errorDetails);
  if(state != _state)
    setState(state);
  _modified=0;
}

//! Put this node into TOLOAD state when possible
/*!
 * Can be called by another ElementaryNode that is connected to this one by a datastream link
 * These 2 nodes must be loaded at the same time as they are coupled
 * It's the other node which requests this node loading
 */
void ElementaryNode::ensureLoading()
{
  DEBTRACE("ElementaryNode::ensureLoading: " << getName());
  if(_state != YACS::READY)
    return;
  setState(YACS::TOLOAD);

  // request loading for all nodes connected to this one by datastream link
  // Be careful that nodes can be connected in a loop. Put first this node in TOLOAD state to break the loop
  std::list<OutputDataStreamPort *>::iterator iterout;
  for(iterout = _setOfOutputDataStreamPort.begin(); iterout != _setOfOutputDataStreamPort.end(); iterout++)
    {
      OutputDataStreamPort *port=(OutputDataStreamPort *)*iterout;
      std::set<InPort *> ports=port->edSetInPort();
      std::set<InPort *>::iterator iter;
      for(iter=ports.begin();iter != ports.end(); iter++)
        {
          Node* node= (*iter)->getNode();
          node->ensureLoading();
        }
    }
  std::list<InputDataStreamPort *>::iterator iterin;
  for(iterin = _setOfInputDataStreamPort.begin(); iterin != _setOfInputDataStreamPort.end(); iterin++)
    {
      InputDataStreamPort *port=(InputDataStreamPort *)*iterin;
      std::set<OutPort *> ports=port->edSetOutPort();
      std::set<OutPort *>::iterator iter;
      for(iter=ports.begin();iter != ports.end(); iter++)
        {
          Node* node= (*iter)->getNode();
          node->ensureLoading();
        }
    }
}
