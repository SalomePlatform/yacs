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
#include "DynParaLoop.hxx"
#include "LinkInfo.hxx"
#include "OutPort.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

/*! \class YACS::ENGINE::DynParaLoop
 *  \brief Base class for dynamically (fully or semifully) built graphs.
 *  \ingroup Nodes
 */

const char DynParaLoop::NAME_OF_SPLITTED_SEQ_OUT[]="SmplPrt";

const char DynParaLoop::NAME_OF_NUMBER_OF_BRANCHES[]="nbBranches";

DynParaLoop::DynParaLoop(const std::string& name, TypeCode *typeOfDataSplitted):ComposedNode(name),_node(0),_initNode(0),_nbOfEltConsumed(0),
                                                                                _nbOfBranches(NAME_OF_NUMBER_OF_BRANCHES,this,Runtime::_tc_int),
                                                                                _splittedPort(NAME_OF_SPLITTED_SEQ_OUT,this,typeOfDataSplitted)
{
}

DynParaLoop::~DynParaLoop()
{
  delete _node;
  delete _initNode;
}

DynParaLoop::DynParaLoop(const DynParaLoop& other, ComposedNode *father, bool editionOnly):ComposedNode(other,father),
                                                                                           _nbOfBranches(other._nbOfBranches,this),
                                                                                           _splittedPort(other._splittedPort,this),
                                                                                           _node(0),_initNode(0),_nbOfEltConsumed(0)
{
  if(other._node)
    _node=other._node->clone(this,editionOnly);
  if(other._initNode)
    _initNode=other._initNode->clone(this,editionOnly);
  const AnyOutputPort& startOfLinksToReproduce=other._splittedPort;
  set<InPort *> endsOfLinksToReproduce=startOfLinksToReproduce.edSetInPort();
  for(set<InPort *>::iterator iter=endsOfLinksToReproduce.begin();iter!=endsOfLinksToReproduce.end();iter++)
    edAddLink(&_splittedPort,getInPort(other.getPortName(*iter)));
}

Node *DynParaLoop::edSetNode(Node *node)
{
  if(_node==node)
    return 0;
  if(node)
    {
      if(node->_father)
        {
          std::string what = "DynParaLoop::edSetNode: node "; what += node->getName(); what += " is not orphan ! "; 
          throw Exception(what);
        }
      if(_initNode)
        if(_initNode->getName()==node->getName())
          {
            std::string what = "DynParaLoop::edSetNode: node "; what += node->getName(); what += " has the same name than init node already in "; what+=_name; what+="! "; 
            throw Exception(what);
          }
    }
  checkNoCrossHierachyWith(node);
  ComposedNode::edRemoveChild(_node);
  Node *ret=_node;
  _node=node;
  _node->_father=this;
  //set _modified flag so that edUpdateState can refresh state
  modified();
  return ret;
}

void DynParaLoop::init(bool start)
{
  ComposedNode::init(start);
  if(!_node)
    {
      string what("DynParaLoop::init : no node specified for ForEachLoop with name "); what +=_name;
      throw Exception(what);
    }
  _nbOfBranches.exInit(start);
  _splittedPort.exInit();
  _nbOfEltConsumed=0;
}

Node *DynParaLoop::edSetInitNode(Node *node)
{
  if(_initNode==node)
    return 0;
  if(node)
    {
      if(node->_father)
        {
          std::string what = "DynParaLoop::edSetInitNode : node "; what += node->getName(); what += " is not orphan ! "; 
          throw Exception(what);
        }
      if(_node)
        if(_node->getName()==node->getName())
          {
            std::string what = "DynParaLoop::edSetInitNode : node "; what += node->getName(); what += " has the same name than node already in "; what+=_name; what+="! ";
            throw Exception(what);
          }
    }
  checkNoCrossHierachyWith(node);
  edRemoveChild(_initNode);
  Node *ret=_initNode;
  _initNode=node;
  _initNode->_father=this;
  modified();
  return ret;
}

int DynParaLoop::getNumberOfInputPorts() const
{
  return ComposedNode::getNumberOfInputPorts()+1;
}

int DynParaLoop::getNumberOfOutputPorts() const
{
  return ComposedNode::getNumberOfOutputPorts()+1;
}

std::list<OutputPort *> DynParaLoop::getSetOfOutputPort() const
{
  list<OutputPort *> ret=ComposedNode::getSetOfOutputPort();
  ret.push_back((OutputPort *)&_splittedPort);
  return ret;
}

OutPort *DynParaLoop::getOutPort(const std::string& name) const throw(Exception)
{
  if(name==NAME_OF_SPLITTED_SEQ_OUT)
    return (OutPort *)&_splittedPort;
  return ComposedNode::getOutPort(name);
}

InputPort *DynParaLoop::getInputPort(const std::string& name) const throw(Exception)
{
  if(name==NAME_OF_NUMBER_OF_BRANCHES)
    return (InputPort *)&_nbOfBranches;
  return ComposedNode::getInputPort(name);
}

OutputPort *DynParaLoop::getOutputPort(const std::string& name) const throw(Exception)
{
  if(name==NAME_OF_SPLITTED_SEQ_OUT)
    return (OutputPort *)&_splittedPort;
  return ComposedNode::getOutputPort(name);
}

bool DynParaLoop::isPlacementPredictableB4Run() const
{
  return false;
}

Node *DynParaLoop::edRemoveNode()
{
  if(!_node)
    return 0;
  ComposedNode::edRemoveChild(_node);
  Node *ret=_node;
  _node=0;
  modified();
  return ret;
}

Node *DynParaLoop::edRemoveInitNode()
{
  if(!_initNode)
    return 0;
  ComposedNode::edRemoveChild(_initNode);
  Node *ret=_initNode;
  _initNode=0;
  return ret;
}

void DynParaLoop::edRemoveChild(Node *node) throw(Exception)
{
  ComposedNode::edRemoveChild(node);
  if(node==_node)
    _node=0;
  if(node==_initNode)
    _initNode=0;
  modified();
}

std::list<Node *> DynParaLoop::edGetDirectDescendants() const
{
  list<Node *> ret;
  if(_node)
    ret.push_back(_node);
  if(_initNode)
    ret.push_back(_initNode);
  return ret;
}

std::list<InputPort *> DynParaLoop::getSetOfInputPort() const
{
  list<InputPort *> ret=ComposedNode::getSetOfInputPort();
  ret.push_back((InputPort *)&_nbOfBranches);
  return ret;
}

std::list<InputPort *> DynParaLoop::getLocalInputPorts() const
{
  list<InputPort *> ret=ComposedNode::getLocalInputPorts();
  ret.push_back((InputPort *)&_nbOfBranches);
  return ret;
}

unsigned DynParaLoop::getNumberOfBranchesCreatedDyn() const throw(Exception)
{
  if(_execNodes.empty())
    throw Exception("ForEachLoop::getNumberOfBranches : No branches created dynamically ! - ForEachLoop needs to run or to be runned to call getNumberOfBranches");
  else
    return _execNodes.size();
}

Node *DynParaLoop::getChildByShortName(const std::string& name) const throw(Exception)
{
  if(_node)
    if(name==_node->getName())
      return _node;
  if(_initNode)
    if(name==_initNode->getName())
      return  _initNode;
  std::string what("node "); what+= name ; what+=" is not a child of DynParaLoop node "; what += getName();
  throw Exception(what);
}

Node *DynParaLoop::getChildByNameExec(const std::string& name, unsigned id) const throw(Exception)
{
  if(id>=getNumberOfBranchesCreatedDyn())
    throw Exception("ForEachLoop::getChildByNameExec : invalid id - too large compared with dynamically created branches.");
  if(_node)
    if(name==_node->getName())
      return _execNodes[id];
  if(_initNode)
    if(name==_initNode->getName())
      return  _execInitNodes[id];
  std::string what("node "); what+= name ; what+=" is not a child of DynParaLoop node "; what += getName();
  throw Exception(what);
}

void DynParaLoop::cleanDynGraph()
{
  for(vector<Node *>::iterator iter=_execNodes.begin();iter!=_execNodes.end();iter++)
    delete *iter;
  _execNodes.clear();
  for(vector<Node *>::iterator iter2=_execInitNodes.begin();iter2!=_execInitNodes.end();iter2++)
    delete *iter2;
  _execNodes.clear();
}

/*!
 * This method applies on newly cloned on exec nodes (_execNodes/_execInitNodes)
 * the setting of input ports coming from outside of 'this'
 */
void DynParaLoop::prepareInputsFromOutOfScope(int branchNb)
{
  set< InPort * > portsToSetVals=getAllInPortsComingFromOutsideOfCurrentScope();
  portsToSetVals.erase(&_nbOfBranches);//_nbOfBranches inport is not a candidate of dynamically duplicated inport.
  for(set< InPort * >::iterator iter=portsToSetVals.begin();iter!=portsToSetVals.end();iter++)
    {
      InputPort *curPortCasted=(InputPort *) *iter;//Cast granted by ForEachLoop::buildDelegateOf(InPort)
      void *val=curPortCasted->get();
      InputPort *portToSet=getDynInputPortByAbsName(branchNb,getInPortName(*iter),true);
      if(portToSet)//portToSet==0 in case of portToSet==_splitterNode._dataPortToDispatch of ForEach
        {
          portToSet->put((const void *)val);
          portToSet->edNotifyReferencedBy(0);//This is to indicate that somewhere somebody deals with this inputport
          //even if no direct physical link exists. This exclusively for _execNodes[branchNb]::init on the next turn of loop.
        }
    }
}

void DynParaLoop::putValueOnBranch(Any *val, unsigned branchId, bool first)
{
  bool isDispatched = false;
  set<InPort *> inPrtLkdWthSplttdPrt=_splittedPort.edSetInPort();
  for(set<InPort *>::iterator iter=inPrtLkdWthSplttdPrt.begin();iter!=inPrtLkdWthSplttdPrt.end();iter++)
    {
      std::string portNameOnCurPrt=getPortName(*iter);
      InputPort *portOnGivenBranch=getDynInputPortByAbsName(branchId,portNameOnCurPrt,first);//Cast granted because impossible to have cross protocol with _splittedPort
      //see OptimizerLoop::buildDelegateOf
      if(portOnGivenBranch)
        {
          if(first)
            portOnGivenBranch->edNotifyReferencedBy(0);
          InputPort *traducer=getRuntime()->adapt(portOnGivenBranch,
                                                  Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME,_splittedPort.edGetType());
          traducer->put((const void *)val);
	  isDispatched = true;
          if(traducer!=portOnGivenBranch)
            delete traducer;
        }
    }
  if ( isDispatched )
    {
      Any *tmp=val->clone();
      _splittedPort.setValue(tmp);
      tmp->decrRef();
    }
}

DynParaLoop::TypeOfNode DynParaLoop::getIdentityOfNotifyerNode(const Node *node, unsigned& id)
{
  id=0;
  for(vector<Node *>::iterator iter=_execNodes.begin();iter!=_execNodes.end();iter++,id++)
    if(*iter==node)
      return WORK_NODE;
  id=0;
  for(vector<Node *>::iterator iter2=_execInitNodes.begin();iter2!=_execInitNodes.end();iter2++,id++)
    if(*iter2==node)
      return INIT_NODE;
}

bool DynParaLoop::isMultiplicitySpecified(unsigned& value) const
{
  if(_nbOfBranches.edIsManuallyInitialized())
    if(_nbOfBranches.edGetNumberOfLinks()==0)
      {
        value=_nbOfBranches.getIntValue();
        return true;
      }
  return false;
}

void DynParaLoop::forceMultiplicity(unsigned value)
{
  _nbOfBranches.edRemoveAllLinksLinkedWithMe();
  _nbOfBranches.edInit((int)value);
}

void DynParaLoop::buildDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView)
{
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME)
    throw Exception("DynParaLoop::buildDelegateOf : A link with datastream end inside DynParaLoop this is not possible");
}

void DynParaLoop::buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView)
{
  if(_initNode)
    if(isInMyDescendance(port.first->getNode())==_initNode)
      throw Exception("DynParaLoop::buildDelegateOf : uncorrect ForEach link : a link starting from init node can't leave the scope of ForEachLoop node it belongs to.");
  if(port.first==&_splittedPort)
    throw Exception("DynParaLoop::buildDelegateOf : uncorrect ForEach link : splitted port must be link within the scope of ForEachLoop node it belongs to.");
}

void DynParaLoop::checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const
{
  const char what[]="DynParaLoop::checkCFLinks : internal error.";
  //First dealing checkCFLinks forwarding...
  if(isInMyDescendance(end->getNode())==0)//no chance that _splittedPort is in starts due to buildDelegate.
    solveObviousOrDelegateCFLinks(starts,end,alreadyFed,direction,info);
  else
    {//no forwarding here.
      if(starts.size()!=1)
        throw Exception(what);
      //ASSERT(direction) : see DynParaLoop::checkControlDependancy only 'fw' filled.
      if(*(starts.begin())!=&_splittedPort)
        throw Exception(what);
      if(alreadyFed==FREE_ST)
        alreadyFed=FED_ST;
      else if(alreadyFed==FED_ST)
        {//Shame ! splittedPort fills a port already fed...
          info.pushInfoLink(*(starts.begin()),end,I_USELESS);
        }
    }
}

/*!
 * \param start : start port
 * \param end : end port
 * \param cross indicates if start -> end link is a DS link behind.
 * \param fw out parameter.
 * \param fwCross out parameter storing links where a cross has been detected.
 * \param bw out parameter where backward links are stored.
 * \param info : collected information
 */
void DynParaLoop::checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                         std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                         std::vector<OutPort *>& fwCross,
                                         std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                         LinkInfo& info) const
{
  if(start==&_splittedPort)
    fw[(ComposedNode *)this].push_back(start);
  else
    throw Exception("DynParaLoop::checkControlDependancy : Internal error occured - should never been called !");
}

/*!
 * \note : For a given name 'name' of port in absolute form from this, returns the corresponding InputPort 
 *         instance of the port for the branch # 'branchNb'. 
 *         The port can be part of _node or _initNode if it exists (if 'initNodeAdmitted' is true).
 *         \b WARNING : no check performed on  'branchNb' value to see if it is compatible with size of '_execNodes'.
 *         This method is called to dispatch value on each InputPort linked to this->._splitterNode._splittedPort
 */
InputPort *DynParaLoop::getDynInputPortByAbsName(int branchNb, const std::string& name, bool initNodeAdmitted)
{
  string portName, nodeName;
  splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,true);
  Node *staticChild = getChildByName(nodeName);
  Node *desc=isInMyDescendance(staticChild);
  if(desc==_node)
    {
      splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,false);
      return _execNodes[branchNb]->getInputPort(portName);
    }
  else if(desc==_initNode)
    if(initNodeAdmitted)
      {
        splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,false);
        return _execInitNodes[branchNb]->getInputPort(portName);
      }
  return 0;
}

void DynParaLoop::checkBasicConsistency() const throw(Exception)
{
  DEBTRACE("DynParaLoop::checkBasicConsistency");
  ComposedNode::checkBasicConsistency();
  if(!_node)
    throw Exception("For a dynamic loop, internal node is mandatory");
}

