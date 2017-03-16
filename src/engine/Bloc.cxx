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

#include "Bloc.hxx"
#include "LinkInfo.hxx"
#include "InputPort.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "ElementaryNode.hxx"
#include "Visitor.hxx"
#include "SetOfPoints.hxx"

#include <queue>
#include <iostream>
#include <numeric>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

/*! \class YACS::ENGINE::Bloc
 *  \brief Composed node to group elementary and composed nodes
 *
 * \ingroup Nodes
 */

Bloc::Bloc(const Bloc& other, ComposedNode *father, bool editionOnly):StaticDefinedComposedNode(other,father),_fwLinks(0),_bwLinks(0)
{
  for(list<Node *>::const_iterator iter=other._setOfNode.begin();iter!=other._setOfNode.end();iter++)
    _setOfNode.push_back((*iter)->simpleClone(this,editionOnly));

  //CF Linking
  vector< pair<OutGate *, InGate *> > cfLinksToReproduce=other.getSetOfInternalCFLinks();
  vector< pair<OutGate *, InGate *> >::iterator iter1=cfLinksToReproduce.begin();
  for(;iter1!=cfLinksToReproduce.end();iter1++)
    edAddCFLink(getChildByName(other.getChildName((*iter1).first->getNode())),getChildByName(other.getChildName((*iter1).second->getNode())));

  //Data + DataStream linking
  vector< pair<OutPort *, InPort *> > linksToReproduce=other.getSetOfInternalLinks();
  vector< pair<OutPort *, InPort *> >::iterator iter2=linksToReproduce.begin();
  for(;iter2!=linksToReproduce.end();iter2++)
    {
      OutPort* pout = iter2->first;
      InPort* pin = iter2->second;
      if(&other == getLowestCommonAncestor(pout->getNode(), pin->getNode()))
      {
        edAddLink(getOutPort(other.getPortName(pout)),getInPort(other.getPortName(pin)));
      }
    }
}

//! Create a Bloc node with a given name
/*!
 *   \param name : the given name
 */
Bloc::Bloc(const std::string& name):StaticDefinedComposedNode(name),_fwLinks(0),_bwLinks(0)
{
}

Bloc::~Bloc()
{
  for(list<Node *>::iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    delete *iter;
  delete _fwLinks;
  delete _bwLinks;
}

//! Initialize the bloc
/*!
 * \param start : a boolean flag indicating the kind of initialization
 * If start is true, it's a complete initialization with reinitialization of port values
 * If start is false, there is no initialization of port values
 */
void Bloc::init(bool start)
{
  Node::init(start);
  for(list<Node *>::iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    (*iter)->init(start);
}

//! Indicate if the bloc execution is finished
/*!
 * The execution bloc is finished if all its child nodes
 * are finished with or without error or if it is disabled (not to execute)
 */
bool Bloc::isFinished()
{
    if(_state==YACS::DONE)return true;
    if(_state==YACS::ERROR)return true;
    if(_state==YACS::FAILED)return true;
    if(_state==YACS::DISABLED)return true;
    return false;
}

int Bloc::getNumberOfCFLinks() const
{
  int ret=0;
  for(list<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    ret+=(*iter)->getOutGate()->getNbOfInGatesConnected();
  return ret;
}

Node *Bloc::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new Bloc(*this,father,editionOnly);
}

//! Collect all nodes that are ready to execute
/*!
 * \param tasks : vector of tasks to collect ready nodes
 */
void Bloc::getReadyTasks(std::vector<Task *>& tasks)
{
  /*
   * ComposedNode state goes to ACTIVATED when one of its child has been ACTIVATED
   * To change this uncomment the following line
   * Then the father node will go to ACTIVATED state before its child node
   */
  if(_state==YACS::TOACTIVATE ) setState(YACS::ACTIVATED);
  if(_state==YACS::TOACTIVATE || _state==YACS::ACTIVATED)
    for(list<Node *>::iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
      (*iter)->getReadyTasks(tasks);
}

//! Update the bloc state
/*!
 * Update the '_state' attribute.
 * Typically called by 'this->_inGate' when 'this->_inGate' is ready. 
 * Contrary to Node::exUpdateState no check done on inputs
 * because internal linked DF inputports are not valid yet.
 */
void Bloc::exUpdateState()
{
  if(_state == YACS::DISABLED)return;
  if(_state == YACS::DONE)return;
  if(_inGate.exIsReady())
    {
      setState(YACS::ACTIVATED);
      for(list<Node *>::iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
        if((*iter)->exIsControlReady())
          (*iter)->exUpdateState();
    }
}

//! Add a child node to the bloc
/*!
 * \param node: the node to add to the bloc
 * \return a boolean flag indicating if the node has been added
 *
 * If node is already a direct child of current bloc, do nothing.
 * If node is a child of another bloc, throw exception.
 * If node name already used in bloc, throw exception.
 * Publish inputPorts in current bloc and ancestors.
 */
bool Bloc::edAddChild(Node *node) throw(YACS::Exception)
{
  if(isNodeAlreadyAggregated(node))
    {
      if(node->_father==this)
        return false;
      else
        {
          string what = "Bloc::edAddChild : node "; what += node->getName();
          what += " is already grand children of node";
          throw Exception(what);
        }
    }

  if(node->_father)
    {
      string what = "Bloc::edAddChild: node is not orphan: "; what += node->getName();
      throw Exception(what);
    }
  
  checkNoCrossHierachyWith(node);

  if(isNameAlreadyUsed(node->getName()))
    {
      string what("Bloc::edAddChild : name "); what+=node->getName(); 
      what+=" already exists in the scope of "; what+=_name;
      throw Exception(what);
    }
  
  node->_father=this;
  _setOfNode.push_back(node);
  //should we also set _modified flag for node ??
  ComposedNode *iter=node->_father;
  //set the _modified flag so that latter on edUpdateState (eventually called by isValid) refresh state
  //better call it at end
  modified();
  return true;
}

/**
 * Remove 'node' from the set of direct children.
 * @exception If 'node' is NOT the son of 'this'.
 */

void Bloc::edRemoveChild(Node *node) throw(YACS::Exception)
{
  StaticDefinedComposedNode::edRemoveChild(node);
  list<Node *>::iterator iter=find(_setOfNode.begin(),_setOfNode.end(),node);
  if(iter!=_setOfNode.end())
    {
      _setOfNode.erase(iter);
      modified();
    }
}

std::vector< std::list<Node *> > Bloc::splitIntoIndependantGraph() const
{
  std::size_t sz(_setOfNode.size());
  list<Node *>::const_iterator it=_setOfNode.begin();
  for(;it!=_setOfNode.end();it++)
    (*it)->_colour=White;
  it=_setOfNode.begin();
  std::vector< list<Node *> > ret;
  while(it!=_setOfNode.end())
    {
      Node *start(*it); start->_colour=Grey;
      ret.push_back(list<Node *>());
      list<Node *>& ll(ret.back());
      std::queue<Node *> fifo; fifo.push(start);
      while(!fifo.empty())
        {
          Node *cur(fifo.front()); fifo.pop();
          ll.push_back(cur);
          //
          OutGate *og(cur->getOutGate());
          list<InGate *> og2(og->edSetInGate());
          for(list<InGate *>::const_iterator it2=og2.begin();it2!=og2.end();it2++)
            {
              Node *cur2((*it2)->getNode());
              if(cur2->_colour==White)
                {
                  cur2->_colour=Grey;
                  fifo.push(cur2);
                }
            }
          //
          InGate *ig(cur->getInGate());
          list<OutGate *> bl(ig->getBackLinks());
          for(list<OutGate *>::const_iterator it3=bl.begin();it3!=bl.end();it3++)
            {
              Node *cur3((*it3)->getNode());
              if(cur3->_colour==White)
                {
                  cur3->_colour=Grey;
                  fifo.push(cur3);
                }
            }
        }
      for(it=_setOfNode.begin();it!=_setOfNode.end() && (*it)->_colour!=White;it++);
    }
  return ret;
}

Node *Bloc::getChildByShortName(const std::string& name) const throw(YACS::Exception)
{
  for (list<Node *>::const_iterator iter = _setOfNode.begin(); iter != _setOfNode.end(); iter++)
    if ((*iter)->getName() == name)
      return (*iter);
  string what("node "); what+= name ; what+=" is not a child of Bloc "; what += getName();
  throw Exception(what);
}

bool Bloc::areAllSubNodesDone() const
{
  for(list<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    {
      if((*iter)->_state == YACS::DONE)continue;
      if((*iter)->_state == YACS::DISABLED)continue;
      return false;
    }
  return true;
}

bool Bloc::areAllSubNodesFinished() const
{
  for(list<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    {
      if((*iter)->_state == YACS::DONE)continue;
      if((*iter)->_state == YACS::FAILED)continue;
      if((*iter)->_state == YACS::DISABLED)continue;
      if((*iter)->_state == YACS::ERROR)continue;
      if((*iter)->_state == YACS::INTERNALERR)continue;
      return false;
    }
  return true;
}

bool Bloc::isNameAlreadyUsed(const std::string& name) const
{
  for(list<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    if((*iter)->getName()==name)
      return true;
  return false;
}

bool insertNodeChildrenInSet(Node *node, std::set<Node *>& nodeSet)
{
  bool verdict=true;
  list<Node *> outNodes=node->getOutNodes();
  for (list<Node *>::iterator iter=outNodes.begin();iter!=outNodes.end(); iter++)
    {
      verdict=(nodeSet.insert(*iter)).second;
      if (verdict) verdict = insertNodeChildrenInSet((*iter),nodeSet);
    }
  return verdict;
}

/*!
 * \note  Checks that in the forest from 'node' there are NO back-edges.
 *        \b WARNING : When using this method 'node' has to be checked in order to be part of direct children of 'this'. 
 *
 */
void Bloc::checkNoCyclePassingThrough(Node *node) throw(YACS::Exception)
{
  set<Node *> currentNodesToTest;
  //don't insert node to test in set. 
  //If it is present after insertion of connected nodes we have a loop
  //collect all connected nodes
  insertNodeChildrenInSet(node,currentNodesToTest);
  //try to insert node
  if(!(currentNodesToTest.insert(node)).second)
    throw Exception("Cycle has been detected",1);
}

std::vector< std::pair<OutGate *, InGate *> > Bloc::getSetOfInternalCFLinks() const
{
  vector< pair<OutGate *, InGate *> > ret;
  for(list<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    {
      list<InGate *> outCFLinksOfCurNode=(*iter)->_outGate.edSetInGate();
      for(list<InGate *>::iterator iter2=outCFLinksOfCurNode.begin();iter2!=outCFLinksOfCurNode.end();iter2++)
        ret.push_back(pair<OutGate *, InGate *>(&(*iter)->_outGate,*iter2));
    }
  return ret;
}

/*!
 *
 * @note : Runtime called method. Indirectly called by StaticDefinedComposedNode::updateStateFrom which has dispatch to this method
 *          'when event == FINISH'.
 *          WARNING Precondition : '_state == Running' and 'node->_father==this'(garanteed by StaticDefinedComposedNode::notifyFrom)
 *
 * Calls the node's outgate OutGate::exNotifyDone if all nodes are not finished
 */
YACS::Event Bloc::updateStateOnFinishedEventFrom(Node *node)
{
  DEBTRACE("Bloc::updateStateOnFinishedEventFrom: " << node->getName());
  //ASSERT(node->_father==this)
  if(areAllSubNodesFinished())
    {
      setState(YACS::DONE);
      if(!areAllSubNodesDone())
        {
          setState(YACS::FAILED);
          return YACS::ABORT;
        }
      return YACS::FINISH;//notify to father node that 'this' has becomed finished.
    }
  //more job to do in 'this' bloc
  //Conversion exceptions can be thrown so catch them to control errors
  try
    {
      //notify the finished node to propagate to its following nodes
      node->exForwardFinished();
    }
  catch(YACS::Exception& ex)
    {
      //The node has failed to propagate. It must be put in error
      DEBTRACE("Bloc::updateStateOnFinishedEventFrom: " << ex.what());
      // notify the node it has failed
      node->exForwardFailed();
      setState(YACS::FAILED);
      return YACS::ABORT;
    }
  return YACS::NOEVENT;//no notification to father needed because from father point of view nothing happened.
}

//! Notify this bloc that a node has failed
/*!
 * \param node : node that has emitted the event
 * \return the event to notify to bloc's father
 */
YACS::Event Bloc::updateStateOnFailedEventFrom(Node *node, const Executor *execInst)
{
  node->exForwardFailed();
  if(areAllSubNodesFinished())
    {
      setState(YACS::DONE);
      if(!areAllSubNodesDone()){
          setState(YACS::FAILED);
          return YACS::ABORT;
      }
      return YACS::FINISH;//notify to father node that 'this' has becomed finished.
    }
  return YACS::NOEVENT;
}

void Bloc::writeDot(std::ostream &os) const
{
  os << "  subgraph cluster_" << getId() << "  {\n" ;
  list<Node *>nodes=getChildren();
  for(list<Node *>::const_iterator iter=nodes.begin();iter!=nodes.end();iter++)
    {
      (*iter)->writeDot(os);
      string p=(*iter)->getId();
      //not connected node
      if((*iter)->_inGate._backLinks.size() == 0) os << getId() << " -> " << p << ";\n";
      list<Node *>outnodes = (*iter)->getOutNodes();
      for(list<Node *>::const_iterator itout=outnodes.begin();itout!=outnodes.end();itout++)
        {
          os << p << " -> " << (*itout)->getId() << ";\n";
        }
    }
  os << "}\n" ;
  os << getId() << "[fillcolor=\"" ;
  YACS::StatesForNode state=getEffectiveState();
  os << getColorState(state);
  os << "\" label=\"" << "Bloc:" ;
  os << getQualifiedName() <<"\"];\n";
}

void Bloc::accept(Visitor* visitor)
{
  visitor->visitBloc(this);
}

/*!
 * Returns the max level of parallelism is this. The max of parallelism is equal to the sum of the max parallelism level
 * for all concurrent branches in \a this.
 */
int Bloc::getMaxLevelOfParallelism() const
{
  std::vector< std::list<Node *> > r(splitIntoIndependantGraph());
  int ret(0);
  for(std::vector< std::list<Node *> >::const_iterator it=r.begin();it!=r.end();it++)
    {
      SetOfPoints sop(*it);
      sop.simplify();
      ret+=sop.getMaxLevelOfParallelism();
    }
  return ret;
}

void Bloc::removeRecursivelyRedundantCL()
{
  StaticDefinedComposedNode::removeRecursivelyRedundantCL();
  LinkInfo info(I_CF_USELESS);
  initComputation();
  performCFComputationsOnlyOneLevel(info);
  std::set< std::pair<Node *, Node *> > linksToKill(info.getInfoUselessLinks());
  for(std::set< std::pair<Node *, Node *> >::const_iterator it=linksToKill.begin();it!=linksToKill.end();it++)
    edRemoveCFLink((*it).first,(*it).second);
  destructCFComputations(info);
}

void Bloc::performCFComputationsOnlyOneLevel(LinkInfo& info) const
{
  delete _fwLinks;//Normally useless
  delete _bwLinks;//Normally useless
  _fwLinks=new map<Node *,set<Node *> >;
  _bwLinks=new map<Node *,set<Node *> >;

  //a set to store all CF links : used to find fastly if two nodes are connected
  std::set< std::pair< Node*, Node* > > links;

  for(list<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    {
      Node* n1=*iter;
      std::list<InGate *> ingates=n1->getOutGate()->edSetInGate();
      for(std::list<InGate *>::const_iterator it2=ingates.begin();it2!=ingates.end();it2++)
        {
          //CF link : n1 -> (*it2)->getNode()
          Node* n2=(*it2)->getNode();
          links.insert(std::pair< Node*, Node* >(n1,n2));
          std::set<Node *> bwn1=(*_bwLinks)[n1];
          std::set<Node *> fwn1=(*_fwLinks)[n1];
          std::set<Node *> fwn2=(*_fwLinks)[n2];
          std::set<Node *> bwn2=(*_bwLinks)[n2];
          std::pair<std::set<Node*>::iterator,bool> ret;
          for(std::set<Node *>::const_iterator iter2=bwn1.begin();iter2!=bwn1.end();iter2++)
            {
              for(std::set<Node *>::const_iterator it3=fwn2.begin();it3!=fwn2.end();it3++)
                {
                  ret=(*_fwLinks)[*iter2].insert(*it3);
                  if(ret.second==false)
                    {
                      //dependency already exists (*iter2) -> (*it3) : if a direct link exists it's a useless one
                      if(links.find(std::pair< Node*, Node* >(*iter2,*it3)) != links.end())
                        info.pushUselessCFLink(*iter2,*it3);
                    }
                }
              ret=(*_fwLinks)[*iter2].insert(n2);
              if(ret.second==false)
                {
                  //dependency already exists (*iter2) -> n2 : if a direct link exists it's a useless one
                  if(links.find(std::pair< Node*, Node* >(*iter2,n2)) != links.end())
                    info.pushUselessCFLink(*iter2,n2);
                }
            }
          for(std::set<Node *>::const_iterator it3=fwn2.begin();it3!=fwn2.end();it3++)
            {
              ret=(*_fwLinks)[n1].insert(*it3);
              if(ret.second==false)
                {
                  //dependency already exists n1 -> *it3 : if a direct link exists it's a useless one
                  if(links.find(std::pair< Node*, Node* >(n1,*it3)) != links.end())
                    info.pushUselessCFLink(n1,*it3);
                }
            }
          ret=(*_fwLinks)[n1].insert(n2);
          if(ret.second==false)
            {
              //dependency already exists n1 -> n2 : it's a useless link
              info.pushUselessCFLink(n1,n2);
            }

          for(std::set<Node *>::const_iterator iter2=fwn2.begin();iter2!=fwn2.end();iter2++)
            {
              (*_bwLinks)[*iter2].insert(bwn1.begin(),bwn1.end());
              (*_bwLinks)[*iter2].insert(n1);
            }
          (*_bwLinks)[n2].insert(bwn1.begin(),bwn1.end());
          (*_bwLinks)[n2].insert(n1);
        }
    }
}

/*!
 * Updates mutable structures _fwLinks and _bwLinks with the result of computation (CPU consuming method).
 * _fwLinks is a map with a Node* as key and a set<Node*> as value. The set gives
 * all nodes that are forwardly connected to the key node 
 * _bwLinks is a map for backward dependencies
 * The method is : for all CF link (n1->n2) 
 * add n2 and _fwLinks[n2] in forward dependencies of n1 and _bwLinks[n1]
 * add n1 and _bwLinks[n1] in backward dependencies of n2 and _fwLinks[n2]
 * For useless links
 * If a node is already in a forward dependency when adding and the direct link
 * already exists so it's a useless link (see the code !)
 */
void Bloc::performCFComputations(LinkInfo& info) const
{
  StaticDefinedComposedNode::performCFComputations(info);
  performCFComputationsOnlyOneLevel(info);
}

void Bloc::destructCFComputations(LinkInfo& info) const
{
  StaticDefinedComposedNode::destructCFComputations(info);
  delete _fwLinks; _fwLinks=0;
  delete _bwLinks; _bwLinks=0;
}

/*!
 * \b WARNING \b Needs call of performCFComputations before beeing called.
 *  Perform updates of containers regarding attributes of link 'start' -> 'end' and check the correct linking.
 *  The output is in info struct.
 *
 * \param start : start port
 * \param end : end port
 * \param cross : 
 * \param fw out parameter being append if start -> end link is a forward link \b without cross type DF/DS.
 * \param fwCross out parameter being append if start -> end link is a forward link \b with cross type DF/DS.
 * \param bw out parameter being append if start -> end link is a backward link.
 * \param info out parameter being informed about eventual errors.
 */
void Bloc::checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const
{
  if(!cross)
    {
      Node *startN=isInMyDescendance(start->getNode());
      Node *endN=isInMyDescendance(end->getNode());
      if(startN==endN)
        bw[(ComposedNode *)this].push_back(start);
      else if(areLinked(startN,endN,true))
        fw[(ComposedNode *)this].push_back(start);
      else
        if(areLinked(startN,endN,false))
          bw[(ComposedNode *)this].push_back(start);
        else
          info.pushErrLink(start,end,E_UNPREDICTABLE_FED);
    }
  else//DFDS detected
    if(arePossiblyRunnableAtSameTime(isInMyDescendance(start->getNode()),isInMyDescendance(end->getNode())))
      fwCross.push_back(start);
    else
      info.pushErrLink(start,end,E_DS_LINK_UNESTABLISHABLE);
}

//! Check if two nodes are linked
/*!
 * 'start' and 'end' \b must be direct son of 'this'.
 * Typically used for data link.
 * \param start : start node
 * \param end : end node
 * \param fw indicates if it is a forward link searched (true : default value) or a backward link serach.
 * \return if true or false
 */
bool Bloc::areLinked(Node *start, Node *end, bool fw) const
{
  set<Node *>& nexts=fw ? (*_fwLinks)[start] : (*_bwLinks)[start];
  return nexts.find(end)!=nexts.end();
}

//! Check if two nodes can run in parallel
/*!
 * Typically used for stream link.
 * 'start' and 'end' \b must be direct son of 'this'.
 * \param start : start node
 * \param end : end node
 * \return true or false
 */
bool Bloc::arePossiblyRunnableAtSameTime(Node *start, Node *end) const
{
  set<Node *>& nexts=(*_fwLinks)[start];
  set<Node *>& preds=(*_bwLinks)[start];
  return nexts.find(end)==nexts.end() && preds.find(end)==preds.end();
}

//! Check control flow links
/*!
 * \param starts If different of 0, must aggregate at leat \b 1 element.
 * \param end : end port
 * \param alreadyFed in/out parameter. Indicates if 'end' ports is already and surely set or fed by an another port.
 * \param direction If true : forward direction else backward direction.
 * \param info : collected information
 */
void Bloc::checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const
{
  if(alreadyFed==FREE_ST || alreadyFed==FED_ST)
    {
      map<Node *,list <OutPort *> > classPerNodes;
      for(list< OutPort *>::const_iterator iter1=starts.begin();iter1!=starts.end();iter1++)
        classPerNodes[isInMyDescendance((*iter1)->getNode())].push_back(*iter1);
      set<Node *> allNodes;
      for(map<Node *,list <OutPort *> >::iterator iter2=classPerNodes.begin();iter2!=classPerNodes.end();iter2++)
        allNodes.insert((*iter2).first);
      vector<Node *> okAndUseless1,useless2;
      seekOkAndUseless1(okAndUseless1,allNodes);
      seekUseless2(useless2,allNodes);//after this point allNodes contains collapses
      verdictForOkAndUseless1(classPerNodes,end,okAndUseless1,alreadyFed,direction,info);
      verdictForCollapses(classPerNodes,end,allNodes,alreadyFed,direction,info);
      verdictForOkAndUseless1(classPerNodes,end,useless2,alreadyFed,direction,info);
    }
  else if(alreadyFed==FED_DS_ST)
    for(list< OutPort *>::const_iterator iter1=starts.begin();iter1!=starts.end();iter1++)
      info.pushErrLink(*iter1,end,E_COLLAPSE_DFDS);
}

void Bloc::initComputation() const
{
  for(list<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    {
      (*iter)->_colour=White;
      (*iter)->getInGate()->exReset();
      (*iter)->getOutGate()->exReset();
    }
}

/*!
 * Part of final step for CF graph anylizing. This is the part of non collapse nodes. 
 * \param pool :
 * \param end :
 * \param candidates :
 * \param alreadyFed in/out parameter. Indicates if 'end' ports is already and surely set or fed by an another port.
 * \param direction
 * \param info : collected information
 */
void Bloc::verdictForOkAndUseless1(const std::map<Node *,std::list <OutPort *> >& pool, InputPort *end, 
                                   const std::vector<Node *>& candidates, unsigned char& alreadyFed, 
                                   bool direction, LinkInfo& info)
{
  for(vector<Node *>::const_iterator iter=candidates.begin();iter!=candidates.end();iter++)
    {
      const list<OutPort *>& mySet=(*pool.find(*iter)).second;
      if(mySet.size()==1)
        {
          if(alreadyFed==FREE_ST)
            {
              alreadyFed=FED_ST;//This the final choice. General case !
              if(!direction)
                info.pushInfoLink(*(mySet.begin()),end,I_BACK);
            }
          else if(alreadyFed==FED_ST)
              info.pushInfoLink(*(mySet.begin()),end,direction ? I_USELESS : I_BACK_USELESS);//Second or more turn in case of alreadyFed==FREE_ST before call of this method
        }
      else
        {
          if(dynamic_cast<ElementaryNode *>(*iter))
            {
              WarnReason reason;
              if(alreadyFed==FREE_ST)
                reason=direction ? W_COLLAPSE_EL : W_BACK_COLLAPSE_EL;
              else if(alreadyFed==FED_ST)
                reason=direction ? W_COLLAPSE_EL_AND_USELESS : W_BACK_COLLAPSE_EL_AND_USELESS;
              for(list<OutPort *>::const_iterator iter2=mySet.begin();iter2!=mySet.end();iter2++)    
                info.pushWarnLink(*iter2,end,reason);
            }
          else
            ((ComposedNode *)(*iter))->checkCFLinks(mySet,end,alreadyFed,direction,info);//Thanks to recursive model!
        }
    }
}

/*!
 * Part of final step for CF graph anylizing. This is the part of collapses nodes. 
 * \param pool :
 * \param end :
 * \param candidates :
 * \param alreadyFed in/out parameter. Indicates if 'end' ports is already and surely set or fed by an another port.
 * \param direction
 * \param info : collected information
 */
void Bloc::verdictForCollapses(const std::map<Node *,std::list <OutPort *> >& pool, InputPort *end, 
                               const std::set<Node *>& candidates, unsigned char& alreadyFed, 
                               bool direction, LinkInfo& info)
{
  info.startCollapseTransac();
  for(set<Node *>::const_iterator iter=candidates.begin();iter!=candidates.end();iter++)
    {
      const list<OutPort *>& mySet=(*pool.find(*iter)).second;
      if(mySet.size()==1)
        {
          if(alreadyFed==FREE_ST)
            info.pushWarnLink(*(mySet.begin()),end,direction ? W_COLLAPSE : W_BACK_COLLAPSE);
          else if(alreadyFed==FED_ST)
            info.pushWarnLink(*(mySet.begin()),end,direction ? W_COLLAPSE_AND_USELESS : W_BACK_COLLAPSE_EL_AND_USELESS);
        }
      else
        {
          if(dynamic_cast<ElementaryNode *>(*iter))
            {
              WarnReason reason;
              if(alreadyFed==FREE_ST)
                reason=direction ? W_COLLAPSE_EL : W_BACK_COLLAPSE_EL;
              else if(alreadyFed==FED_ST)
                reason=direction ? W_COLLAPSE_EL_AND_USELESS : W_BACK_COLLAPSE_EL_AND_USELESS;
              for(list<OutPort *>::const_iterator iter2=mySet.begin();iter2!=mySet.end();iter2++)    
                info.pushWarnLink(*iter2,end,reason);
            }
          else
            {
              ((ComposedNode *)(*iter))->checkCFLinks(mySet,end,alreadyFed,direction,info);//Thanks to recursive model!
              WarnReason reason;
              if(alreadyFed==FREE_ST)
                reason=direction ? W_COLLAPSE : W_BACK_COLLAPSE;
              else if(alreadyFed==FED_ST)
                reason=direction ? W_COLLAPSE_AND_USELESS : W_BACK_COLLAPSE_AND_USELESS;
              for(list<OutPort *>::const_iterator iter2=mySet.begin();iter2!=mySet.end();iter2++)    
                info.pushWarnLink(*iter2,end,reason);
            }
        }
    }
  if(!candidates.empty())
    if(alreadyFed==FREE_ST)
      alreadyFed=FED_ST;
  info.endCollapseTransac();
}

/*!
 * \b WARNING use this method only after having called Bloc::performCFComputations method.
 * \param okAndUseless1 out param contains at the end, the nodes without any collapse.
 * \param allNodes in/out param. At the end, all the nodes in 'okAndUseless1' are deleted from 'allNodes'.
 */
void Bloc::seekOkAndUseless1(std::vector<Node *>& okAndUseless1, std::set<Node *>& allNodes) const
{
  set<Node *>::iterator iter=allNodes.begin();
  while(iter!=allNodes.end())
    {
      set<Node *>& whereToFind=(*_bwLinks)[*iter];
      std::set<Node *>::iterator iter2;
      for(iter2=allNodes.begin();iter2!=allNodes.end();iter2++)
        if((*iter)!=(*iter2))
          if(whereToFind.find(*iter2)==whereToFind.end())
            break;
      if(iter2!=allNodes.end())
        iter++;
      else
        {
          okAndUseless1.push_back((*iter));
          allNodes.erase(iter);
          iter=allNodes.begin();
        }
    }
}

/*!
 * \b WARNING use this method only after having called Bloc::performCFComputations method.
 * For params see Bloc::seekOkAndUseless1.
 */
void Bloc::seekUseless2(std::vector<Node *>& useless2, std::set<Node *>& allNodes) const
{
  set<Node *>::iterator iter=allNodes.begin();
  while(iter!=allNodes.end())
    {
      set<Node *>& whereToFind=(*_fwLinks)[*iter];
      std::set<Node *>::iterator iter2;
      for(iter2=allNodes.begin();iter2!=allNodes.end();iter2++)
        if((*iter)!=(*iter2))
          if(whereToFind.find(*iter2)==whereToFind.end())
            break;
      if(iter2!=allNodes.end())
        {
          iter++;
        }
      else
        {
          useless2.push_back((*iter));
          allNodes.erase(iter);
          iter=allNodes.begin();
        }
    }
}

/*! 
 * Internal method : Given a succeful path : updates 'fastFinder'
 */
void Bloc::updateWithNewFind(const std::vector<Node *>& path, std::map<Node *, std::set<Node *> >& fastFinder)
{
  if(path.size()>=3)
    {
      vector<Node *>::const_iterator iter=path.begin(); iter++;
      vector<Node *>::const_iterator iter2=path.end(); iter2-=1;
      for(;iter!=iter2;iter++)
        fastFinder[*iter].insert(*(iter+1));
    }
}

/*! 
 * Internal method : After all paths have been found, useless CF links are searched
 */
void Bloc::findUselessLinksIn(const std::list< std::vector<Node *> >& res , LinkInfo& info)
{
  unsigned maxSize=0;
  list< vector<Node *> >::const_iterator whereToPeerAt;
  for(list< vector<Node *> >::const_iterator iter=res.begin();iter!=res.end();iter++)
    if((*iter).size()>maxSize)
      {
        maxSize=(*iter).size();
        whereToPeerAt=iter;
      }
  //
  if(maxSize>1)
    {
      vector<Node *>::const_iterator iter2=(*whereToPeerAt).begin();
      map<Node *,bool>::iterator iter4;
      set<Node *> searcher(iter2+1,(*whereToPeerAt).end());//to boost research
      for(;iter2!=((*whereToPeerAt).end()-2);iter2++)
        {
          list< pair<InGate *,bool> >& nexts=(*iter2)->getOutGate()->edMapInGate();
          for(list< pair<InGate *,bool> >::iterator iter4=nexts.begin();iter4!=nexts.end();iter4++)
            if((*iter4).first->getNode()!=*(iter2+1))
              if(searcher.find((*iter4).first->getNode())!=searcher.end())
                info.pushUselessCFLink(*iter2,(*iter4).first->getNode());
          searcher.erase(*iter2);
        }
    }
}
