//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
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
#include "Container.hxx"
#include "ComponentInstance.hxx"
#include "ServiceNode.hxx"
#include "Visitor.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

const char DynParaLoop::NAME_OF_SPLITTED_SEQ_OUT[] = "evalSamples";
const char DynParaLoop::OLD_NAME_OF_SPLITTED_SEQ_OUT[] = "SmplPrt"; // For backward compatibility with 5.1.4

const char DynParaLoop::NAME_OF_NUMBER_OF_BRANCHES[]="nbBranches";

DynParaLoop::DynParaLoop(const std::string& name, TypeCode *typeOfDataSplitted)
  : ComposedNode(name),_node(0),_initNode(0),_finalizeNode(0),_nbOfEltConsumed(0),
    _nbOfBranches(NAME_OF_NUMBER_OF_BRANCHES,this,Runtime::_tc_int),
    _splittedPort(NAME_OF_SPLITTED_SEQ_OUT,this,typeOfDataSplitted)
{
}

DynParaLoop::~DynParaLoop()
{
  delete _node;
  delete _initNode;
  delete _finalizeNode;
}

DynParaLoop::DynParaLoop(const DynParaLoop& other, ComposedNode *father, bool editionOnly)
  : ComposedNode(other,father), _nbOfBranches(other._nbOfBranches,this),
    _splittedPort(other._splittedPort,this), _node(0), _initNode(0), _finalizeNode(0),
    _nbOfEltConsumed(0)
{
  if(other._node)
    _node=other._node->clone(this,editionOnly);
  if(other._initNode)
    _initNode=other._initNode->clone(this,editionOnly);
  if(other._finalizeNode)
    _finalizeNode = other._finalizeNode->clone(this,editionOnly);
  const AnyOutputPort& startOfLinksToReproduce=other._splittedPort;
  set<InPort *> endsOfLinksToReproduce=startOfLinksToReproduce.edSetInPort();
  for(set<InPort *>::iterator iter=endsOfLinksToReproduce.begin();iter!=endsOfLinksToReproduce.end();iter++)
    edAddLink(&_splittedPort,getInPort(other.getPortName(*iter)));
}

Node *DynParaLoop::edSetNode(Node *node)
{
  return checkConsistencyAndSetNode(_node, node);
}

//! This method is used to factorize methods edSetNode, edSetInitNode and edSetFinalizeNode
Node * DynParaLoop::checkConsistencyAndSetNode(Node* &nodeToReplace, Node* node)
{
  if (node == NULL or nodeToReplace == node)
    return 0;
  if (node->_father)
    throw Exception(string("Can't set node: node ") + node->getName() + " is not orphan.");
  if (_node and _node != nodeToReplace and _node->getName() == node->getName())
    throw Exception(string("Can't set node: node ") + node->getName() +
                    " has the same name than exec node already in " + _name + ".");
  if (_initNode and _initNode != nodeToReplace and _initNode->getName() == node->getName())
    throw Exception(string("Can't set node: node ") + node->getName() +
                    " has the same name than init node already in " + _name + ".");
  if (_finalizeNode and _finalizeNode != nodeToReplace and _finalizeNode->getName() == node->getName())
    throw Exception(string("Can't set node: node ") + node->getName() +
                    " has the same name than finalize node already in " + _name + ".");
  checkNoCrossHierachyWith(node);
  ComposedNode::edRemoveChild(nodeToReplace);
  Node * ret = nodeToReplace;
  nodeToReplace = node;
  nodeToReplace->_father = this;
  // set _modified flag so that edUpdateState can refresh state
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
  _node->init(start);
  if (_initNode) _initNode->init(start);
  if (_finalizeNode) _finalizeNode->init(start);
  _nbOfBranches.exInit(start);
  _splittedPort.exInit();
  _nbOfEltConsumed=0;
}

Node *DynParaLoop::edSetInitNode(Node *node)
{
  return checkConsistencyAndSetNode(_initNode, node);
}

Node * DynParaLoop::edSetFinalizeNode(Node * node)
{
  return checkConsistencyAndSetNode(_finalizeNode, node);
}

//! Connect an OutPort to an InPort and add control link if necessary
/*!
 * Connect the ports with a data link (edAddLink)
 * In a Loop don't add control flow link : use this only to add data back links
 *
 * \param start : the OutPort to connect
 * \param end : the InPort to connect
 * \return  true if a new link has been created, false otherwise.
 */
bool DynParaLoop::edAddDFLink(OutPort *start, InPort *end) throw(YACS::Exception)
{
  return edAddLink(start,end);
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

std::list<OutputPort *> DynParaLoop::getLocalOutputPorts() const
{
  list<OutputPort *> ret=ComposedNode::getLocalOutputPorts();
  ret.push_back((OutputPort *)&_splittedPort);
  return ret;
}

OutPort *DynParaLoop::getOutPort(const std::string& name) const throw(YACS::Exception)
{
  if (name == NAME_OF_SPLITTED_SEQ_OUT or name == OLD_NAME_OF_SPLITTED_SEQ_OUT)
    return (OutPort *)&_splittedPort;
  return ComposedNode::getOutPort(name);
}


OutputPort *DynParaLoop::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  if (name == NAME_OF_SPLITTED_SEQ_OUT or name == OLD_NAME_OF_SPLITTED_SEQ_OUT)
    return (OutputPort *)&_splittedPort;
  return ComposedNode::getOutputPort(name);
}

bool DynParaLoop::isPlacementPredictableB4Run() const
{
  return false;
}

Node *DynParaLoop::edRemoveNode()
{
  return removeNode(_node);
}

//! This method is used to factorize methods edRemoveNode, edRemoveInitNode and edRemoveFinalizeNode
Node * DynParaLoop::removeNode(Node* &nodeToRemove)
{
  if (!nodeToRemove)
    return NULL;
  ComposedNode::edRemoveChild(nodeToRemove);
  Node * ret = nodeToRemove;
  nodeToRemove = NULL;
  modified();
  return ret;
}

Node *DynParaLoop::edRemoveInitNode()
{
  return removeNode(_initNode);
}

Node * DynParaLoop::edRemoveFinalizeNode()
{
  return removeNode(_finalizeNode);
}

void DynParaLoop::edRemoveChild(Node *node) throw(YACS::Exception)
{
  ComposedNode::edRemoveChild(node);
  if(node==_node)
    _node=0;
  if(node==_initNode)
    _initNode=0;
  if(node==_finalizeNode)
    _finalizeNode=0;
  modified();
}

bool DynParaLoop::edAddChild(Node *node) throw(YACS::Exception)
{
  return edSetNode(node);
}

std::list<Node *> DynParaLoop::edGetDirectDescendants() const
{
  list<Node *> ret;
  if(_node)
    ret.push_back(_node);
  if(_initNode)
    ret.push_back(_initNode);
  if(_finalizeNode)
    ret.push_back(_finalizeNode);
  return ret;
}

std::list<InputPort *> DynParaLoop::getSetOfInputPort() const
{
  list<InputPort *> ret=ComposedNode::getSetOfInputPort();
  ret.push_back((InputPort *)&_nbOfBranches);
  return ret;
}

InputPort *DynParaLoop::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_OF_NUMBER_OF_BRANCHES)
    return (InputPort *)&_nbOfBranches;
  return ComposedNode::getInputPort(name);
}

std::list<InputPort *> DynParaLoop::getLocalInputPorts() const
{
  list<InputPort *> ret=ComposedNode::getLocalInputPorts();
  ret.push_back((InputPort *)&_nbOfBranches);
  return ret;
}

unsigned DynParaLoop::getNumberOfBranchesCreatedDyn() const throw(YACS::Exception)
{
  if(_execNodes.empty())
    throw Exception("ForEachLoop::getNumberOfBranches : No branches created dynamically ! - ForEachLoop needs to run or to be runned to call getNumberOfBranches");
  else
    return _execNodes.size();
}

Node *DynParaLoop::getChildByShortName(const std::string& name) const throw(YACS::Exception)
{
  if (_node and name == _node->getName())
    return _node;
  if (_initNode and name == _initNode->getName())
    return  _initNode;
  if (_finalizeNode and name == _finalizeNode->getName())
    return  _finalizeNode;
  std::string what("node "); what+= name ; what+=" is not a child of DynParaLoop node "; what += getName();
  throw Exception(what);
}

Node *DynParaLoop::getChildByNameExec(const std::string& name, unsigned id) const throw(YACS::Exception)
{
  if(id>=getNumberOfBranchesCreatedDyn())
    throw Exception("ForEachLoop::getChildByNameExec : invalid id - too large compared with dynamically created branches.");
  if (_node and name == _node->getName())
    return _execNodes[id];
  if (_initNode and name == _initNode->getName())
    return _execInitNodes[id];
  if (_finalizeNode and name == _finalizeNode->getName())
    return _execFinalizeNodes[id];
  std::string what("node "); what+= name ; what+=" is not a child of DynParaLoop node "; what += getName();
  throw Exception(what);
}

void DynParaLoop::cleanDynGraph()
{
  vector<Node *>::iterator iter;
  for(iter=_execNodes.begin() ; iter!=_execNodes.end() ; iter++)
    delete *iter;
  _execNodes.clear();
  for(iter=_execInitNodes.begin() ; iter!=_execInitNodes.end() ; iter++)
    delete *iter;
  _execInitNodes.clear();
  for(iter=_execFinalizeNodes.begin() ; iter!=_execFinalizeNodes.end() ; iter++)
    delete *iter;
  _execFinalizeNodes.clear();
}

/*!
 * This method applies on newly cloned on exec nodes (_execNodes/_execInitNodes)
 * the setting of input ports coming from outside of 'this'
 */
void DynParaLoop::prepareInputsFromOutOfScope(int branchNb)
{
  set< InPort * > portsToSetVals=getAllInPortsComingFromOutsideOfCurrentScope();
  // local input ports are not candidates for dynamically duplicated inport.
  list<InputPort *> localPorts = getLocalInputPorts();
  for(list<InputPort *>::iterator iter = localPorts.begin() ; iter != localPorts.end() ; iter++)
    portsToSetVals.erase(*iter);
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
  vector<Node *>::iterator iter;
  id=0;
  for (iter=_execNodes.begin() ; iter!=_execNodes.end() ; iter++,id++)
    if (*iter==node)
      return WORK_NODE;
  id=0;
  for (iter=_execInitNodes.begin() ; iter!=_execInitNodes.end() ; iter++,id++)
    if (*iter==node)
      return INIT_NODE;
  id=0;
  for (iter=_execFinalizeNodes.begin() ; iter!=_execFinalizeNodes.end() ; iter++,id++)
    if (*iter==node)
      return FINALIZE_NODE;
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

void DynParaLoop::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("DynParaLoop::checkBasicConsistency");
  ComposedNode::checkBasicConsistency();
  if(!_node)
    throw Exception("For a dynamic loop, internal node is mandatory");
}

std::string DynParaLoop::getErrorReport()
{
  DEBTRACE("DynParaLoop::getErrorReport: " << getName() << " " << _state);
  YACS::StatesForNode effectiveState=getEffectiveState();

  if(effectiveState != YACS::INVALID &&  effectiveState != YACS::ERROR && effectiveState != YACS::FAILED)
    return "";

  std::string report="<error node= " + getName();
  switch(effectiveState)
    {
    case YACS::INVALID:
      report=report+" state= INVALID";
      break;
    case YACS::ERROR:
      report=report+" state= ERROR";
      break;
    case YACS::FAILED:
      report=report+" state= FAILED";
      break;
    default:
      break;
    }
  report=report + ">\n" ;
  if(_errorDetails != "")
    report=report+_errorDetails+"\n";

  if(_execNodes.empty())
    {
      // edition node
      list<Node *> constituents=edGetDirectDescendants();
      for(list<Node *>::iterator iter=constituents.begin(); iter!=constituents.end(); iter++)
        {
          std::string rep=(*iter)->getErrorReport();
          if(rep != "")
            {
              report=report+rep+"\n";
            }
        }
    }
  else
    {
      // execution nodes
      for(vector<Node *>::iterator iter=_execInitNodes.begin();iter!=_execInitNodes.end();iter++)
        {
          std::string rep=(*iter)->getErrorReport();
          if(rep != "")
            {
              report=report+rep+"\n";
            }
        }
      for(vector<Node *>::iterator iter=_execNodes.begin();iter!=_execNodes.end();iter++)
        {
          std::string rep=(*iter)->getErrorReport();
          if(rep != "")
            {
              report=report+rep+"\n";
            }
        }
      for(vector<Node *>::iterator iter=_execFinalizeNodes.begin();iter!=_execFinalizeNodes.end();iter++)
        {
          std::string rep=(*iter)->getErrorReport();
          if(rep != "")
            {
              report=report+rep+"\n";
            }
        }
    }

  report=report+"</error>";
  return report;
}

void DynParaLoop::forwardExecStateToOriginalBody(Node *execNode)
{
  unsigned int id;
  Node * origNode = NULL;
  switch (getIdentityOfNotifyerNode(execNode,id))
    {
    case INIT_NODE:
    {
      origNode = _initNode;
      break;
    }
    case WORK_NODE:
    {
      origNode = _node;
      break;
    }
    case FINALIZE_NODE:
    {
      origNode = _finalizeNode;
      break;
    }
    default:
      YASSERT(false)
    }

  YASSERT(origNode != NULL)
  origNode->setState(execNode->getState());
  origNode->setErrorDetails(execNode->getErrorDetails());

  ComposedNode* compNode = dynamic_cast<ComposedNode*>(origNode);
  ComposedNode* compNodeExe = dynamic_cast<ComposedNode*>(execNode);
  if (compNode && compNodeExe)
    {
      list<Node *> aChldn = compNodeExe->getAllRecursiveConstituents();
      list<Node *>::iterator iter=aChldn.begin();
      for(;iter!=aChldn.end();iter++)
        {
          Node* node=compNode->getChildByName(compNodeExe->getChildName(*iter));
          node->setState((*iter)->getState());
          node->setErrorDetails((*iter)->getErrorDetails());
        }
    }
}

//! Method used to notify the node that a child node has failed
/*!
 * Update the current state and return the change state
 *
 *  \param node : the child node that has failed
 *  \return the state change
 */
YACS::Event DynParaLoop::updateStateOnFailedEventFrom(Node *node)
{
  DEBTRACE("DynParaLoop::updateStateOnFailedEventFrom " << node->getName());
  setState(YACS::FAILED);
  forwardExecStateToOriginalBody(node);
  unsigned int id;
  switch (getIdentityOfNotifyerNode(node,id))
    {
    case INIT_NODE:
    {
      _node->setState(YACS::FAILED);
      if (_finalizeNode != NULL) _finalizeNode->setState(YACS::FAILED);
      break;
    }
    case WORK_NODE:
    {
      if (_finalizeNode != NULL) _finalizeNode->setState(YACS::FAILED);
      break;
    }
    }
  return YACS::ABORT;
}

//! Clone nodes and make their placement consistent with the placement of the original ones.
/*!
 *  For instance, if two original nodes are placed on a component comp1 in a container cont1
 *  and a third one is placed on a component comp2 in the container cont1, the clones of the two
 *  first nodes will be placed on a component comp3 in a container cont2 and the third clone
 *  will be placed on a component comp4 in the container cont2.
 */
vector<Node *> DynParaLoop::cloneAndPlaceNodesCoherently(const vector<Node *> & origNodes)
{
  DEBTRACE("Begin cloneAndPlaceNodesCoherently")
  vector<Node *> clones;
  DeploymentTree treeToDup;
  vector<list<ElementaryNode *> > origElemNodeList;
  for (int i=0 ; i<origNodes.size() ; i++)
    {
      DEBTRACE("Cloning node " << i)
      if (origNodes[i] == NULL)
        {
          clones.push_back(NULL);
          origElemNodeList.push_back(list<ElementaryNode *>());
        }
      else
        {
          clones.push_back(origNodes[i]->simpleClone(this, false));
          list<ElementaryNode *> tasks = origNodes[i]->getRecursiveConstituents();
          origElemNodeList.push_back(tasks);
          for (list< ElementaryNode *>::iterator iter=tasks.begin() ; iter!=tasks.end() ; iter++)
            treeToDup.appendTask(*iter, (*iter)->getDynClonerIfExists(this));
        }
    }

  DEBTRACE("Placing nodes...")
  vector<Container *> conts=treeToDup.getAllContainers();
  for(vector<Container *>::iterator iterCt=conts.begin();iterCt!=conts.end();iterCt++)
    {
      DEBTRACE("Container " << ((*iterCt)?(*iterCt)->getName():"NULL"))
      vector<ComponentInstance *> comps=treeToDup.getComponentsLinkedToContainer(*iterCt);
      Container *contCloned=0;
      if((*iterCt))
        contCloned=(*iterCt)->clone();
      for(vector<ComponentInstance *>::iterator iterCp=comps.begin();iterCp!=comps.end();iterCp++)
        {
          DEBTRACE("Component " << (*iterCp)->getCompoName())
          vector<Task *> tasks=treeToDup.getTasksLinkedToComponent(*iterCp);
          ComponentInstance *curCloned=(*iterCp)->clone();
          curCloned->setContainer(contCloned);
          for(vector<Task *>::iterator iterT=tasks.begin();iterT!=tasks.end();iterT++)
            {
              DEBTRACE("Task " << ((ElementaryNode *)(*iterT))->getName())
              int i = 0;
              ElementaryNode * origElemNode = NULL;
              for (i=0 ; i<origNodes.size() ; i++)
                if (origNodes[i] != NULL)
                  {
                    DEBTRACE("Looking in original node " << i)
                    list<ElementaryNode *>::iterator res=find(origElemNodeList[i].begin(),
                                                              origElemNodeList[i].end(),
                                                              (ElementaryNode *)(*iterT));
                    if (res != origElemNodeList[i].end()) {
                      origElemNode = *res;
                      break;
                    }
                  }

              YASSERT(origElemNode != NULL)
              DEBTRACE("Found task in node " << i)
              ServiceNode * nodeC = NULL;
              if (origNodes[i] == origElemNode)
                nodeC = (ServiceNode *)clones[i];
              else
                {
                  string childName = ((ComposedNode *)origNodes[i])->getChildName(origElemNode);
                  nodeC = (ServiceNode *)clones[i]->getChildByName(childName);
                }
              nodeC->setComponent(curCloned);
            }
          curCloned->decrRef();
        }
      if(contCloned)
        contCloned->decrRef();
    }

  DEBTRACE("End cloneAndPlaceNodesCoherently")
  return clones;
}

void DynParaLoop::accept(Visitor *visitor)
{
  visitor->visitDynParaLoop(this);
}

Node * DynParaLoop::getInitNode()
{
  return _initNode;
}

Node * DynParaLoop::getExecNode()
{
  return _node;
}

Node * DynParaLoop::getFinalizeNode()
{
  return _finalizeNode;
}
