#include "ComposedNode.hxx"
#include "LinkInfo.hxx"
#include "Container.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ServiceNode.hxx"
#include "DataFlowPort.hxx"
#include "DataStreamPort.hxx"
#include "ElementaryNode.hxx"
#include "ComponentInstance.hxx"

#include <iostream>
#include <set>
#include <string>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char ComposedNode::SEP_CHAR_BTW_LEVEL[]=".";

ComposedNode::ComposedNode(const std::string& name):Node(name)
{
}

ComposedNode::ComposedNode(const ComposedNode& other, ComposedNode *father):Node(other,father)
{
}

ComposedNode::~ComposedNode()
{
}

void ComposedNode::performDuplicationOfPlacement(const Node& other)
{
  const ComposedNode &otherC=*(dynamic_cast<const ComposedNode *>(&other));
  DeploymentTree treeToDup=otherC.getDeploymentTree();
  set< ElementaryNode * > clones=otherC.getRecursiveConstituents();
  vector<Container *> conts=treeToDup.getAllContainers();
  for(vector<Container *>::iterator iterCt=conts.begin();iterCt!=conts.end();iterCt++)
    {
      vector<ComponentInstance *> comps=treeToDup.getComponentsLinkedToContainer(*iterCt);
      Container *contCloned=0;
      if((*iterCt))
        contCloned=(*iterCt)->clone();
      for(vector<ComponentInstance *>::iterator iterCp=comps.begin();iterCp!=comps.end();iterCp++)
        {
          vector<Task *> tasks=treeToDup.getTasksLinkedToComponent(*iterCp);
          ComponentInstance *curCloned=(*iterCp)->clone();
          curCloned->setContainer(contCloned);
          for(vector<Task *>::iterator iterT=tasks.begin();iterT!=tasks.end();iterT++)
            {
              //No risk for static cast : appendTask called by ComposedNode.
              set< ElementaryNode * >::iterator res=clones.find((ElementaryNode *)(*iterT));
              //No risk here to because called only on cloning process...
              ServiceNode *nodeC=(ServiceNode *)getChildByName(otherC.getChildName(*res));
              nodeC->setComponent(curCloned);
            }
          curCloned->decrRef();
        }
      if(contCloned)
        contCloned->decrRef();
    }
}

bool ComposedNode::isFinished()
{
  if(_state==YACS::DONE)return true;
  if(_state==YACS::ERROR)return true;
  if(_state==YACS::FAILED)return true;
  if(_state==YACS::DISABLED)return true;
  return false;
}

void ComposedNode::init(bool start)
{
  Node::init(start);
}

std::string ComposedNode::getName() const
{
  return Node::getName();
}

std::string ComposedNode::getTaskName(Task *task) const
{
  return getChildName(dynamic_cast<ElementaryNode *>(task));
}

//! Essentially for test. Use checkDeploymentTree instead to be sure that returned DeploymentTree is consistent.
DeploymentTree ComposedNode::getDeploymentTree() const
{
  DeploymentTree ret;
  set< ElementaryNode * > tasks=getRecursiveConstituents();
  for(set< ElementaryNode * >::iterator iter=tasks.begin();iter!=tasks.end();iter++)
    ret.appendTask(*iter,(*iter)->getDynClonerIfExists(this));
  return ret;
}

//! Perform check of deployment consistency of the current graph.
/*!
 * \param deep if \b true a deep check is perfomed. Typically has to be called by  an executor before any attempt to launch an execution.
 */
DeploymentTree ComposedNode::checkDeploymentTree(bool deep) const throw(Exception)
{
  DeploymentTree ret;
  set< ElementaryNode * > tasks=getRecursiveConstituents();
  for(set< ElementaryNode * >::iterator iter=tasks.begin();iter!=tasks.end();iter++)
    {
      switch(ret.appendTask(*iter,(*iter)->getDynClonerIfExists(this)))
        {
        case DeploymentTree::DUP_TASK_NOT_COMPATIBLE_WITH_EXISTING_TREE:
          {
            string what("ComposedNode::checkDeploymentTree : ServiceNode with name \""); what+=(*iter)->getName();
            what+="\" coexists in a component with an another Task which context is incorrect with it.";
            throw Exception(what);
          }
          case DeploymentTree::DEPLOYABLE_BUT_NOT_SPECIFIED :
            {
              if(deep)
                {
                  string what("ComposedNode::checkDeploymentTree : ServiceNode with name \""); what+=(*iter)->getName();
                  what+="\" is deployable but no component is specified on it.";
                  throw Exception(what);
                }
            }
        }
    }
  return ret;
}

std::vector<Task *> ComposedNode::getNextTasks(bool& isMore)
{
  vector<Task *> ret;
  isMore=false;
  getReadyTasks(ret);
  isMore=!ret.empty();
  return ret;
}

//! Notify the node a task has emitted an event
/*!
 * TO COMMENT MORE
 * \note Runtime called method. Overloads the Scheduler::notifyFrom abstract method.
 *       Typically Called in Executor (in a parallel thread or not) by the Task 'task'
 *       to inform the scheduler that an event coded 'event' (in Executor static const var) happened.
 *       Contrary to updateStateFrom several levels may exist between 'sender' and 'this'.
 *
 * \param sender : task emitting event
 * \param event  : event emitted
 *
 * Called by Executor::functionForTaskExecution on YACS::FINISH event
 *
 * Called by Executor::launchTask on YACS::START event
 *
 * Calls ComposedNode::updateStateFrom to update state from task to root node
 */
void ComposedNode::notifyFrom(const Task *sender, //* I : task emitting event
                              YACS::Event event   //* I : event emitted
                              )
{
  ElementaryNode *taskTyped=dynamic_cast<ElementaryNode *>((Task *)sender);
  YACS::Event curEvent=event;
  Node *lminus1LevelNode=taskTyped;
  ComposedNode *curLevelNode=taskTyped->_father;
  if(!curLevelNode)//Specific case of loop when 0 turn is specified without any enclosing bloc.
    return ;
  curEvent=curLevelNode->updateStateFrom(lminus1LevelNode,curEvent);
  while(curEvent!=YACS::NOEVENT && curLevelNode!=this)
    {
      lminus1LevelNode=curLevelNode;
      curLevelNode=curLevelNode->_father;
      curEvent=curLevelNode->updateStateFrom(lminus1LevelNode,curEvent);
    }
}

//! Add a dataflow link between two data ports.
/*!
 *  Precondition : 'start' AND 'end' are in/outputPort contained in a node in descendance of 'this'.
 *  \exception Exception : if incompatibility between input and output (type), or 'start'/'end' 
 *                         is/are NOT in/outputPort contained in a node in descendance 
 *                         of 'this', or a multiple link to an input not supporting it.
 *  \return    true if a new link has been created, false otherwise.
 */

bool ComposedNode::edAddLink(OutPort *start, InPort *end) throw(Exception)
{
  set<OutPort *> represented;

  start->getAllRepresented(represented);
  if(represented.size()!=1)
    {
      bool ret=false;
      for(set<OutPort *>::iterator iter=represented.begin();iter!=represented.end();iter++)
        ret|=edAddLink(*iter,end);
      return ret;
    }
  if(start->isAlreadyLinkedWith(end))
    return false;
  ComposedNode* lwstCmnAnctr=getLowestCommonAncestor(start->getNode(),end->getNode());
  set<ComposedNode *> allAscendanceOfNodeStart=start->getNode()->getAllAscendanceOf(lwstCmnAnctr);
  set<ComposedNode *> allAscendanceOfNodeEnd=end->getNode()->getAllAscendanceOf(lwstCmnAnctr);
  checkInMyDescendance(lwstCmnAnctr);
  lwstCmnAnctr->checkLinkPossibility(start,allAscendanceOfNodeStart,end,allAscendanceOfNodeEnd);
  ComposedNode *iterS;
  if(dynamic_cast<ComposedNode *>(start->getNode()))
    iterS=(ComposedNode *)start->getNode();
  else
    iterS=start->getNode()->_father;
  pair<OutPort *, OutPort *> pO(start,start);
  while(iterS!=lwstCmnAnctr)
    {
      iterS->buildDelegateOf(pO, end, allAscendanceOfNodeEnd);
      iterS=iterS->_father;
    }
  if(dynamic_cast<ComposedNode *>(end->getNode()))
    iterS=(ComposedNode *)end->getNode();
  else
    iterS=end->getNode()->_father;

  InPort *currentPortI=end;
  while(iterS!=lwstCmnAnctr)
    {
      iterS->buildDelegateOf(currentPortI, start, allAscendanceOfNodeStart);
      iterS=iterS->_father;
    }
  bool ret=(pO.first)->addInPort(currentPortI);
  end->edNotifyReferencedBy(pO.second);
  return ret;
}

//! Connect an OutPort to an InPort and add the necessary control link
/*!
 * Connect the ports with a data link (edAddLink) and add
 * a control flow link between the children of the lowest common ancestor node.
 *
 * \param start : the OutPort to connect
 * \param end : the InPort to connect
 * \return  true if a new link has been created, false otherwise. 
 */
bool ComposedNode::edAddDFLink(OutPort *start, InPort *end) throw(Exception)
{
  Node* n1=start->getNode();
  Node* n2=end->getNode();
  DEBTRACE( n1->getName() << ":" << n2->getName())
  ComposedNode* father=getLowestCommonAncestor(n1,n2);
  DEBTRACE( "ComposedNode::edAddDFLink: this="<<this->getName() 
            << " father=" << father->getName() )
  DEBTRACE( "ComposedNode::edAddDFLink: OutPort=" << start->getName()
            << " InPort=" << end->getName() )
  if (father != this)
    {
      bool ret = father->edAddDFLink(start,end); // special treatement for loop
      return ret;
    }
  if(n2 == father)
    throw Exception("Back link authorized only in special context (loop for example)");

  bool ret= edAddLink(start,end);
  if(n1 != father)
    {
      //add a control link only if nodes are not in the same descendance
      //if n1 == father (n2 is after n1) : the control link is not needed 
      //if n2 == father (n1 is after n2) : it's a back link authorized only in loop context
      while(n1->getFather() != father)
        n1=n1->getFather();
      while(n2->getFather() != father)
        n2=n2->getFather();
      edAddCFLink(n1,n2);
    }
  return ret;
}

//! Add a controlflow link between two control ports.
/*!
 * \note Precondition : 'start' AND 'end' are in/outGate contained in a node 
 *       in DIRECT descendance of 'this'.
 * \exception Exception : If a cycle has been detected, or incompatibility 
 *             between input and output, or 'start'/'end' is/are NOT in/outputPort contained 
 *             in a node in descendance of 'this', or a mutilple link to an input not 
 *             supporting it.
 * \return  true if a new link has been created, false otherwise. 
 */
bool ComposedNode::edAddLink(OutGate *start, InGate *end) throw(Exception)
{
  Node* n1=start->getNode();
  Node* n2=end->getNode();
  if(n1==n2)
    throw Exception("ComposedNode::edAddLink: can not add a control link to a node with itself");
  ComposedNode* father=checkHavingCommonFather(start->getNode(),end->getNode());
  if(father==0)
    throw Exception("ComposedNode::edAddLink: Trying to add CF link on orphan nodes.");
  if(father!=this)
    {
      checkInMyDescendance(father);
      return father->edAddLink(start,end);
    }
  bool ret=start->edAddInGate(end);
  if(ret)
    checkNoCyclePassingThrough(end->getNode());
  return ret;
}

//! Add a controlflow link between two nodes.
/*!
 * Add a controlflow link between two nodes by calling edAddLink on their control ports
 */
bool ComposedNode::edAddCFLink(Node *nodeS, Node *nodeE) throw(Exception)
{
  return edAddLink(nodeS->getOutGate(),nodeE->getInGate());
}

//! Remove a controlflow link.
void ComposedNode::edRemoveCFLink(Node *nodeS, Node *nodeE) throw(Exception)
{
  edRemoveLink(nodeS->getOutGate(),nodeE->getInGate());
}

//! Remove a dataflow link.
/*!
 * Precondition : 'start' AND 'end' are in/outputPort contained in a node in descendance of 'this'.
 *
 * \exception Exception : If the specified link does not exist. The content of Exception 
 *                        is different in accordance with the link from 'start' to 'end' 
 *                        implies DF/DS gateway.
 */

void ComposedNode::edRemoveLink(OutPort *start, InPort *end) throw(Exception)
{
  if(!start->isAlreadyLinkedWith(end))
    throw Exception("ComposedNode::edRemoveLink : unexisting link");
  ComposedNode* lwstCmnAnctr=getLowestCommonAncestor(start->getNode(),end->getNode());
  checkInMyDescendance(lwstCmnAnctr);
  set<ComposedNode *> allAscendanceOfNodeStart=start->getNode()->getAllAscendanceOf(lwstCmnAnctr);
  set<ComposedNode *> allAscendanceOfNodeEnd=end->getNode()->getAllAscendanceOf(lwstCmnAnctr);

  // --- Part of test if the link from 'start' to 'end' really exist particulary all eventually intermediate ports created

  ComposedNode *iterS=start->getNode()->_father;
  pair<OutPort *,OutPort *> currentPortO(start,start);
  vector<pair< ComposedNode * , pair < OutPort* , OutPort *> > > needsToDestroyO;

  Node *nodeOTemp=start->getNode();
  if(*nodeOTemp<*lwstCmnAnctr)
    {
      iterS=nodeOTemp->_father;
      while(iterS!=lwstCmnAnctr)
	{
	  if (!iterS)
	    {
	      stringstream what;
	      what << "ComposedNode::edRemoveLink: "
		   << start->getNode()->getName() << "." <<start->getName() << "->"
		   << end->getNode()->getName() << "." << end->getName();
	      throw Exception(what.str());
	    }
	  OutPort *tmp=currentPortO.first;
	  iterS->getDelegateOf(currentPortO, end, allAscendanceOfNodeEnd);
	  needsToDestroyO.push_back(pair< ComposedNode * , pair < OutPort* , OutPort *> >(iterS,pair<OutPort* , OutPort *> (tmp,currentPortO.first)));
	  iterS=iterS->_father;
	}
    }
  Node *nodeTemp=end->getNode();
  InPort * currentPortI=end;
  if(*nodeTemp<*lwstCmnAnctr)
    {
      iterS=nodeTemp->_father;    
      while(iterS!=lwstCmnAnctr)
        {
          if (!iterS)
            {
              stringstream what;
              what << "ComposedNode::edRemoveLink: "
                   << start->getNode()->getName() << "." <<start->getName() << "->"
                   << end->getNode()->getName() << "." << end->getName();
              throw Exception(what.str());
            }
          iterS->getDelegateOf(currentPortI, start, allAscendanceOfNodeStart);
          iterS=iterS->_father;
        }
    }
  // --- End of test for evt intermediate ports created
  
  (currentPortO.first)->removeInPort(currentPortI,false);
  set<OutPort *> repr;
  (currentPortO.second)->getAllRepresented(repr);
  if(repr.size()==1)
    end->edNotifyDereferencedBy(currentPortO.second);

  // --- Performing deletion of intermediate ports
  
  iterS=start->getNode()->_father;
  vector<pair< ComposedNode * , pair < OutPort* , OutPort *> > >::reverse_iterator iter;
  for(iter=needsToDestroyO.rbegin();iter!=needsToDestroyO.rend();iter++)
    (*iter).first->releaseDelegateOf(((*iter).second).first, ((*iter).second).second, end,allAscendanceOfNodeEnd);
  nodeTemp=end->getNode();
  if(*nodeTemp<*lwstCmnAnctr)
    {
      iterS=end->getNode()->_father;
      currentPortI=end;
      while(iterS!=lwstCmnAnctr)
        {
          iterS->releaseDelegateOf(currentPortI, start, allAscendanceOfNodeStart);
          iterS=iterS->_father;
        }
    }
}

//! Remove a controlflow link.
void ComposedNode::edRemoveLink(OutGate *start, InGate *end) throw(Exception)
{
  ComposedNode* father=checkHavingCommonFather(start->getNode(),end->getNode());
  if(father!=this)
    throw Exception("edRemoveLink : nodes not in direct descendance of this");
  start->edRemoveInGate(end);
}

//! Remove a child node.
void ComposedNode::edRemoveChild(Node *node) throw(Exception)
{
  if(!node)
    return;
  if (node->_father!=this)
    {
      string what("node "); what+= node->getName() ; what+=" is not a child of node "; what += getName();
      throw Exception(what);
    }
  node->edDisconnectAllLinksWithMe();
  node->_father = 0;
}

//! Splits name globalName in 2 parts using separator.
/*!
 * \note   'priority' specifies if during search process of 'separator' the max part is 
 *         for 'firstPart' (priority=true) or 'lastPart' (priority=false).
 *
 * \throw Exception : 'lastPart' or 'firstPart' is empty.
 * \return  true if split process succeeds false otherwise (typically when character 
 *          separator was not found).
 */
bool ComposedNode::splitNamesBySep(const std::string& globalName, const char separator[],
                                   std::string& firstPart, std::string& lastPart, bool priority) throw(Exception)
{
  const string delims(separator);
  string portName, nodeName;
  string::size_type idx;
  if(priority)
    idx = globalName.find_last_of(delims);
  else
    idx = globalName.find_first_of(delims);
  if (idx == string::npos)
    {
      firstPart=globalName;
      lastPart="";
      return false;
    }
  firstPart = globalName.substr(0,idx);
  lastPart = globalName.substr(idx+1);
  if ((firstPart.empty()) or (lastPart.empty()))
    {
      string what("the name "); what+= globalName ; what+=" is not a valid port name";
      throw Exception(what);
    }
  return true;
}

std::vector< std::pair<OutPort *, InPort *> > ComposedNode::getSetOfInternalLinks() const
{
  vector< pair<OutPort *, InPort *> > ret;
  list<OutPort *> temp=getSetOfOutPort();
  for(list<OutPort *>::const_iterator iter2=temp.begin();iter2!=temp.end();iter2++)
    {
      set<InPort *> temp2=(*iter2)->edSetInPort();
      for(set<InPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        if(isInMyDescendance((*iter3)->getNode()))
          ret.push_back(pair<OutPort *, InPort *>((*iter2),(*iter3)));
    }
  return ret;
}

std::vector< std::pair<OutPort *, InPort *> > ComposedNode::getSetOfLinksLeavingCurrentScope() const
{
  vector< pair<OutPort *, InPort *> > ret;
  std::set<OutPort *> ports=getAllOutPortsLeavingCurrentScope();
  for(set<OutPort *>::iterator iter2=ports.begin();iter2!=ports.end();iter2++)
    {
      set<InPort *> temp2=(*iter2)->edSetInPort();
      for(set<InPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        if(!isInMyDescendance((*iter3)->getNode()))
          ret.push_back(pair<OutPort *, InPort *>(*iter2,*iter3));
    }
  return ret;
}

void ComposedNode::checkConsistency(LinkInfo& info) const throw(Exception)
{
  info.clearAll();
  info.setPointOfView((ComposedNode *)this);
  performCFComputations(info);
  list<InputPort *> setOfInToTest=getSetOfInputPort();
  for(list<InputPort *>::iterator iter1=setOfInToTest.begin();iter1!=setOfInToTest.end();iter1++)
    {
      vector<OutPort *> candidateForAdvCheck;
      set<OutPort *> outPorts=(*iter1)->edSetOutPort();
      //Filtering among outPorts, which of them, are candidates to fill *iter1 at the current scope.
      for(set<OutPort *>::iterator iter2=outPorts.begin();iter2!=outPorts.end();iter2++)
        {
          (*iter2)->checkConsistency(info);
          ComposedNode *manager=getLowestCommonAncestor((*iter2)->getNode(),(*iter1)->getNode());
          if(isInMyDescendance(manager))
            candidateForAdvCheck.push_back(*iter2);
        }
      if(!candidateForAdvCheck.empty())
        //End of filtering. Now regarding CF constraints for the current InPutPort.
        checkLinksCoherenceRegardingControl(candidateForAdvCheck,*iter1,info);
      else
        //No backlinks
        if(!(*iter1)->edIsManuallyInitialized())
          info.pushErrLink(0,*iter1,E_NEVER_SET_INPUTPORT);
    }
  destructCFComputations(info);
}

/*!
 * This method check that G1 <- G2 <- G3 <- G1 does not happened.
 * Typically called by methods that set a hierarchy (Bloc::edAddChild, Loop::edSetNode, ...).
 */
void ComposedNode::checkNoCrossHierachyWith(Node *node) const throw (Exception)
{
  ComposedNode *nodeC=dynamic_cast<ComposedNode *>(node);
  if(!nodeC)
    return ;
  set<ComposedNode *> ascendants=getAllAscendanceOf();
  if(ascendants.find(nodeC)!=ascendants.end())
    {
      const char what[]="ComposedNode::checkNoCrossHierachyWith : ComposedNode with name \"";
      string stream(what); stream+=node->getName(); stream+="\" is already in hierarchy ascendance of node with name \"";
      stream+=_name; stream+="\" ; So it can't be now in its descendance !";
      throw Exception(stream);
    }
}

//! perform \b recursively all CF computations.
void ComposedNode::performCFComputations(LinkInfo& info) const
{
  set<Node *> nodes=edGetDirectDescendants();
  for(set<Node *>::iterator iter=nodes.begin();iter!=nodes.end();iter++)
    if(dynamic_cast<ComposedNode *>(*iter))
      ((ComposedNode *)(*iter))->performCFComputations(info);
}

//! destroy \b recursively all results of initial computations.
void ComposedNode::destructCFComputations(LinkInfo& info) const
{
  set<Node *> nodes=edGetDirectDescendants();
  for(set<Node *>::iterator iter=nodes.begin();iter!=nodes.end();iter++)
    if(dynamic_cast<ComposedNode *>(*iter))
      ((ComposedNode *)(*iter))->destructCFComputations(info);
}

/*!
 * Returns the lowest Node (Elementary or Composed) (is sense of hierachy level ( operator< ) ) containing all 'ports'.
 * Typically use in consistency computation.
 * Precondition : 'ports' \b must contain at least one element. All elements of 'ports' should be in descendance of 'this'.
 */
Node *ComposedNode::getLowestNodeDealingAll(const std::list<OutPort *>& ports) const
{
  list< OutPort *>::const_iterator iter=ports.begin();
  Node *ret=(*iter)->getNode();
  iter++;
  for(;iter!=ports.end();iter++)
    {
      Node *tmp=(*iter)->getNode();
      if(*tmp>*ret)
        ret=tmp;
    }
  return ret;
}

/*!
 * call it only for 'starts' to 'end' links \b DEALED by 'this'.
 */
void ComposedNode::checkLinksCoherenceRegardingControl(const std::vector<OutPort *>& starts, InputPort *end, LinkInfo& info) const throw(Exception)
{
  map < ComposedNode *, list<OutPort *>, SortHierarc > outputs;//forward link classical
  vector<OutPort *> outputsCross;//forward link cross
  map < ComposedNode *, list<OutPort *>, SortHierarc > outputsBw;//backward
  vector<OutPort *>::const_iterator iter1;
  //vector<DataPort *> history=((*iter1).second)[0]->calculateHistoryOfLinkWith(end);
  //DataPort *cross=DataPort::isCrossingType(history);
  for(iter1=starts.begin();iter1!=starts.end();iter1++)
    {
      ComposedNode *manager=getLowestCommonAncestor((*iter1)->getNode(),end->getNode());
      manager->checkControlDependancy((*iter1), end, false, outputs, outputsCross, outputsBw, info);
    }
  //Ok now let's regarding outputs all combinations : (outputs.size())*(outputs.size()-1)/2
  unsigned char isAlreadyFed=FREE_ST;
  //Dealing excusively with DS. Level is useless here because simultaneity is required for DS.
  if(outputsCross.size()>0)
    {
      isAlreadyFed=FED_DS_ST;
      if(outputsCross.size()>1)
        for(vector< OutPort *>::const_iterator iter1=outputsCross.begin();iter1!=(outputsCross.end()-2);iter1++)
          info.pushErrLink(*iter1,end,E_COLLAPSE_DS);
    }
  map < ComposedNode *, list<OutPort *>, SortHierarc >::iterator iter3=outputs.begin();
  for(;iter3!=outputs.end();iter3++)
    ((*iter3).first)->checkCFLinks((*iter3).second,end,isAlreadyFed,true,info);
  if(isAlreadyFed==FREE_ST)
    if(!end->edIsManuallyInitialized())
      info.pushErrLink(0,end,E_ONLY_BACKWARD_DEFINED);
  isAlreadyFed=FREE_ST;
  //
  map < ComposedNode *, list<OutPort *>, SortHierarc >::reverse_iterator iter5=outputsBw.rbegin();
  for(;iter5!=outputsBw.rend();iter5++)
    ((*iter5).first)->checkCFLinks((*iter5).second,end,isAlreadyFed,false,info);
}

/*!
 * Internal method during CF links. This méthode is in charge to statuate on links consistency in the case that no control flow defined by user
 * is set.
 */
void ComposedNode::solveObviousOrDelegateCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const
{
  static const char what[]="ComposedNode::solveObviousOrDelegateCFLinks : Internal error occured - uncorrect hierarchy detected !";
  if(starts.size()==1)
    {
      if(alreadyFed==FREE_ST)
        {
          if(!direction)
            info.pushInfoLink(*(starts.begin()),end,I_BACK);
          alreadyFed=FED_ST;
        }
      else if(alreadyFed==FED_ST)
        info.pushInfoLink(*(starts.begin()),end,direction ?  I_USELESS : I_BACK_USELESS);
      else
        info.pushErrLink(*(starts.begin()),end,E_COLLAPSE_DFDS);
    }
  else
    {
      Node *levelOfDecision=getLowestNodeDealingAll(starts);
      if(levelOfDecision==this)
        throw Exception(what);
      if(dynamic_cast<ElementaryNode *>(levelOfDecision))
        {
          WarnReason reason;
          if(alreadyFed==FREE_ST || alreadyFed==FED_ST)
            {
              if(alreadyFed==FREE_ST)
                {
                  reason=direction ? W_COLLAPSE_EL : W_BACK_COLLAPSE_EL;
                  alreadyFed=FED_ST;
                }
              else
                reason=direction ? W_COLLAPSE_EL_AND_USELESS : W_BACK_COLLAPSE_EL_AND_USELESS;
              for(list< OutPort *>::const_iterator iter=starts.begin();iter!=starts.end();iter++)
                info.pushWarnLink(*iter,end,reason);
            }
        }
      else
        ((ComposedNode *)levelOfDecision)->checkCFLinks(starts,end,alreadyFed,direction,info);
    }
}

/*!
 * \param starts If different of 0, must aggregate at leat \b 1 element.
 * \param alreadyFed in/out parameter. Indicates if 'end' ports is already and surely set or fed by an another port.
 * \param direction If true : forward direction else backward direction.
 */
void ComposedNode::checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const
{
  static const char what[]="ComposedNode::checkCFLinks : Internal error occured - uncorrect hierarchy detected !";
  Node *nodeEnd=isInMyDescendance(end->getNode());
  if(!nodeEnd)
    return solveObviousOrDelegateCFLinks(starts,end,alreadyFed,direction,info);
  //This case is typically dedicated when direct son is ElementaryNode and self link is defined on this.
  if(!dynamic_cast<ElementaryNode *>(nodeEnd))
    throw Exception(what);
  list< OutPort *>::const_iterator iter=starts.begin();
  Node *nodeStart=(*iter)->getNode();
  iter++;
  if(nodeEnd!=nodeStart)
    throw Exception(what);
  for(;iter!=starts.end();iter++)
    if((*iter)->getNode()!=nodeStart)
      throw Exception(what);
  //Ok at this step we are sure that we have back links on the same elementary node.
  if(starts.size()>1)
    for(iter=starts.begin();iter!=starts.end();iter++)
      info.pushWarnLink(*iter,end,W_BACK_COLLAPSE_EL);
  else//here no need to look at 'alreadyFed' var because it is waranteed to be equal to FREE_ST by construction.
    info.pushInfoLink(*(starts.begin()),end,I_BACK);
  alreadyFed=FED_ST;
}

std::vector< std::pair<InPort *, OutPort *> > ComposedNode::getSetOfLinksComingInCurrentScope() const
{
  vector< pair<InPort *, OutPort *> > ret;
  set<InPort *> ports=getAllInPortsComingFromOutsideOfCurrentScope();
  for(set<InPort *>::iterator iter2=ports.begin();iter2!=ports.end();iter2++)
    {
      set<OutPort *> temp2=(*iter2)->edSetOutPort();
      for(set<OutPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        if(!isInMyDescendance((*iter3)->getNode()))
          ret.push_back(pair<InPort *, OutPort *>(*iter2,*iter3));
    }
  return ret;
}

//! List all output ports of children nodes that are linked to out of scope input ports
/*!
 * \note List all output ports of nodes sons of 'this' that are linked to input ports 
 *       of nodes not in descendance of 'this'.
 *       This method contrary to getAllInPortsComingFromOutsideOfCurrentScope is NOT virtual 
 *       because for the moment all daughter classes have no more hidden YACS::ENGINE::OutPort.
 */
std::set<OutPort *> ComposedNode::getAllOutPortsLeavingCurrentScope() const
{
  set<OutPort *> ret;
  list<OutPort *> temp=getSetOfOutPort();
  for(list<OutPort *>::iterator iter2=temp.begin();iter2!=temp.end();iter2++)
    {
      set<InPort *> temp2=(*iter2)->edSetInPort();
      for(set<InPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        if(!isInMyDescendance((*iter3)->getNode()))
          {
            ret.insert(*iter2);
            break;
          }
    }
  return ret;
}

//! List all input ports that are linked to out of scope ports
/*!
 *
 * List all input ports of 'this' so that, for each it exists at least 1 link coming 
 * from outside to it.
 *
 */
std::set<InPort *> ComposedNode::getAllInPortsComingFromOutsideOfCurrentScope() const
{
  set<InPort *> ret;
  list<InPort *> temp=getSetOfInPort();
  for(list<InPort *>::iterator iter2=temp.begin();iter2!=temp.end();iter2++)
    {
      set<OutPort *> temp2=(*iter2)->edSetOutPort();
      for(set<OutPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        if(*iter3)
          if(!isInMyDescendance((*iter3)->getNode()))
            {
              ret.insert(*iter2);
              break;
            }
    }
  return ret;
}

void ComposedNode::edDisconnectAllLinksWithMe()
{
  //CF
  DEBTRACE("-");
  Node::edDisconnectAllLinksWithMe();
  //Leaving part
  DEBTRACE("--");
  vector< pair<OutPort *, InPort *> > linksToDestroy=getSetOfLinksLeavingCurrentScope();
  vector< pair<OutPort *, InPort *> >::iterator iter;
  for(iter=linksToDestroy.begin();iter!=linksToDestroy.end();iter++)
    {
      DEBTRACE("---");
      (*iter).first->removeInPort((*iter).second,true);
    }
  //Arriving part
  vector< pair<InPort *, OutPort *> > linksToDestroy2=getSetOfLinksComingInCurrentScope();
  vector< pair<InPort *, OutPort *> >::iterator iter2;
  for(iter2=linksToDestroy2.begin();iter2!=linksToDestroy2.end();iter2++)
    {
      DEBTRACE("----");
      (*iter2).second->removeInPort((*iter2).first,true);
    }
}

ComposedNode *ComposedNode::getRootNode() throw(Exception)
{
  if(!_father)
    return this;
  return Node::getRootNode();
}

bool ComposedNode::isNodeAlreadyAggregated(Node *node) const
{
  set<ComposedNode *> nodeAncestors = node->getAllAscendanceOf();
  if ( nodeAncestors.find((ComposedNode*)this) == nodeAncestors.end() )
    return false;
  else
    return true;
}

//! Returns the parent of a node that is the direct child of this node 
/*!
 * \note if 'nodeToTest'=='this' this is returned. Else if 'nodeToTest' is in descendance of
 *       'this' the direct son is returned. 
 *       Else 0 is returned. 
 *
 * \param nodeToTest : the node to check
 */
Node *ComposedNode::isInMyDescendance(Node *nodeToTest) const
{
  if(nodeToTest==0)
    return 0;
  if((ComposedNode *)nodeToTest==this)
    return (Node *)this;
  Node *iterBack=nodeToTest;
  ComposedNode *iter=nodeToTest->_father;
  while(iter!=0 && iter!=this)
    {
      iterBack=iter;
      iter=iter->_father;
    }
  if(iter!=0)
    return iterBack;
  else
    return 0;
}

string ComposedNode::getChildName(Node* node) const throw(Exception)
{
  string nodeName = node->getQualifiedName();    
  if (!isNodeAlreadyAggregated(node))
    {
      if (node->getName() == "thisIsAFakeNode")
        {
          string child = node->getName()+".thisIsAFakeNode";
          return child;
        }
      else
        {
          string what("node "); what+= node->getName() ; what+=" is not a child of node "; what += getName();
          throw Exception(what);
        }
    }
  
  Node *father = node->_father;
  while (father != this)
    {
      nodeName = father->getQualifiedName() + SEP_CHAR_BTW_LEVEL + nodeName;
      father = father->_father;
    }
  return nodeName;
}

std::string ComposedNode::getMyQualifiedName(const Node *directSon) const
{
  return directSon->getName();
}

Node *ComposedNode::getChildByName(const std::string& name) const throw(Exception)
{
  string potentiallyDirectSonName, remainsPath;
  bool forwardNeeded=ComposedNode::splitNamesBySep(name, SEP_CHAR_BTW_LEVEL,
                                                   potentiallyDirectSonName,remainsPath,false);
  Node *child=getChildByShortName(potentiallyDirectSonName);
  if(!forwardNeeded)
    return child;
  else
    return child->getChildByName(remainsPath);
}

//! Check if a node is in the descendance of this node
/*!
 * \note Check that 'nodeToTest' is in descendance of 'this' OR equal to 'this'
 * \exception Exception : If 'nodeToTest' is NOT in descendance of 'this' AND not equal to 'this'
 * \param nodeToTest : the node to check
 */
void ComposedNode::checkInMyDescendance(Node *nodeToTest) const throw(Exception)
{
  const char whatC[]=" is not the descendance of node ";
  if(nodeToTest==0)
    {
      string what("node "); what+= nodeToTest->getName(); what+=" ";
      what+=whatC; what+=_name;
      throw Exception(what);
    }
  if((ComposedNode *)nodeToTest==this)
    return;
  ComposedNode *iter=nodeToTest->_father;
  while(iter!=0 && iter!=this)
    iter=iter->_father;
  if(iter==0)
    {
      string what("node "); what+= nodeToTest->getName(); what+=" ";
      what+=whatC; what+=_name;
      throw Exception(what);
    }
}

//! Retrieves the lowest common ancestor of 2 nodes
/*!
 * 
 * \note Retrieves the lowest common ancestor of 'node1' AND 'node2'. 
 *       If  'node1' or 'node2' are both or indiscriminately instances of ComposedNode and that
 *       'node1' is in descendance of 'node2' (resp. 'node2' in descendance of 'node1')
 *       'node2' is returned (resp. 'node1').
 * \exception Exception : if 'node1' and 'node2' do not share the same genealogy.
 * \return The lowest common ancestor if it exists.
 *
 */
ComposedNode *ComposedNode::getLowestCommonAncestor(Node *node1, Node *node2) throw(Exception)
{
  const char what[]="2 nodes does not share the same genealogy";
  if(node1==0 || node2==0)
    throw Exception(what);
  ComposedNode *temp;
  if(dynamic_cast<ComposedNode *>(node1))
    temp=(ComposedNode *)node1;//->_father;
  else
    temp=(ComposedNode *)node1->_father;
  set<ComposedNode *> s;
  while(temp)
    {
      s.insert(temp);
      temp=temp->_father;
    }
  //
  if(dynamic_cast<ComposedNode *>(node2))
    temp=(ComposedNode *)node2;//->_father;
  else
    temp=(ComposedNode *)node2->_father;
  set<ComposedNode *>::iterator iter=s.find(temp);
  while(temp && iter==s.end())
    {
      iter=s.find(temp);
      temp=temp->_father;
    }
  if(iter==s.end())
    throw Exception(what);
  return *iter;
}

set<ElementaryNode *> ComposedNode::getRecursiveConstituents() const
{
  set<ElementaryNode *> ret;
  set<Node *> setOfNode=edGetDirectDescendants();
  for(set<Node *>::const_iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      set<ElementaryNode *> myCurrentSet=(*iter)->getRecursiveConstituents();
      ret.insert(myCurrentSet.begin(),myCurrentSet.end());
    }
  return ret;
}

set<Node *> ComposedNode::getAllRecursiveConstituents()
{
  set<Node *> ret;
  set<Node *> setOfNode=edGetDirectDescendants();
  for(set<Node *>::const_iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      if ( dynamic_cast<ComposedNode*> (*iter) )
        {
          set<Node *> myCurrentSet=((ComposedNode*)(*iter))->getAllRecursiveConstituents();
          ret.insert(myCurrentSet.begin(),myCurrentSet.end());
          ret.insert(*iter);
        }
      else
        {
          set<ElementaryNode *> myCurrentSet=(*iter)->getRecursiveConstituents();
          ret.insert(myCurrentSet.begin(),myCurrentSet.end());
        }
    }
  return ret;
}

//! Get all children nodes  elementary and composed including this node
set<Node *> ComposedNode::getAllRecursiveNodes()
{
  set<Node *> ret;
  set<Node *> setOfNode=edGetDirectDescendants();
  for(set<Node *>::iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      if ( dynamic_cast<ElementaryNode*> (*iter) )
        {
          set<ElementaryNode *> myCurrentSet=(*iter)->getRecursiveConstituents();
          ret.insert(myCurrentSet.begin(),myCurrentSet.end());
          //ret.insert(*iter);
        }
      else
        {
          set<Node *> myCurrentSet=((ComposedNode*)(*iter))->getAllRecursiveNodes();
          ret.insert(myCurrentSet.begin(),myCurrentSet.end());
          //ret.insert(*iter);
        }
    }
  ret.insert(this);
  return ret;
}

//! Get the input port name 
/*!
 * get the input port name used by the current node, recursively built with children names.
 */

string ComposedNode::getInPortName(const InPort * inPort) const throw (Exception)
{
  return getPortName<InPort>(inPort);
}

string ComposedNode::getOutPortName(const OutPort *outPort) const throw (Exception)
{
  return getPortName<OutPort>(outPort);
}

int ComposedNode::getNumberOfInputPorts() const
{
  set<Node *> constituents=edGetDirectDescendants();
  int ret=0;
  for(set<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    ret+=(*iter)->getNumberOfInputPorts();
  return ret;
}

int ComposedNode::getNumberOfOutputPorts() const
{
  set<Node *> constituents=edGetDirectDescendants();
  int ret=0;
  for(set<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    ret+=(*iter)->getNumberOfOutputPorts();
  return ret;
}

list<InputPort *> ComposedNode::getSetOfInputPort() const
{
  set<Node *> constituents=edGetDirectDescendants();
  list<InputPort *> ret;
  for(set<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    {
      list<InputPort *> currentsPorts=(*iter)->getSetOfInputPort();
      ret.insert(ret.end(),currentsPorts.begin(),currentsPorts.end());
    }
  return ret;
}

list<OutputPort *> ComposedNode::getSetOfOutputPort() const
{
  set<Node *> constituents=edGetDirectDescendants();
  list<OutputPort *> ret;
  for(set<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    {
      list<OutputPort *> currentsPorts=(*iter)->getSetOfOutputPort();
      ret.insert(ret.end(),currentsPorts.begin(),currentsPorts.end());
    }
  return ret;
}

list<InputDataStreamPort *> ComposedNode::getSetOfInputDataStreamPort() const
{
  set<Node *> constituents=edGetDirectDescendants();
  list<InputDataStreamPort *> ret;
  for(set<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    {
      list<InputDataStreamPort *> currentsPorts=(*iter)->getSetOfInputDataStreamPort();
      ret.insert(ret.end(),currentsPorts.begin(),currentsPorts.end());
    }
  return ret;
}

list<OutputDataStreamPort *> ComposedNode::getSetOfOutputDataStreamPort() const
{
  set<Node *> constituents=edGetDirectDescendants();
  list<OutputDataStreamPort *> ret;
  for(set<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    {
      list<OutputDataStreamPort *> currentsPorts=(*iter)->getSetOfOutputDataStreamPort();
      ret.insert(ret.end(),currentsPorts.begin(),currentsPorts.end());
    }
  return ret;
}

OutPort *ComposedNode::getOutPort(const std::string& name) const throw(Exception)
{
  string portName, nodeName;
  if(splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,false))
    {
      Node *child = getChildByShortName(nodeName);
      return child->getOutPort(portName);
    }
  else
    {
      string what("ComposedNode::getOutPort : the port with name "); what+=name; what+=" does not exist on the current level";
      throw Exception(what);
    }
}

//! Get an input port given its name
/*!
 * Contrary to YACS::ENGINE::ComposedNode::getOutputPort, this method is \b NOT recursive 
 * and so the leaf of type ElementaryNode aggregating
 * this InputPort is directly invoked.
 */
InputPort * ComposedNode::getInputPort(const std::string& name) const throw(Exception)
{
  string portName, nodeName;
  if(splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,true))
    {
      Node *child = getChildByName(nodeName);
      return child->getInputPort(portName);
    }
  else
    {
      string what("ComposedNode::getInputPort : the port with name "); what+=name; what+=" does not exist on the current level";
      throw Exception(what);
    }
}

//! Get an output port given its name
/*!
 * Contrary to YACS::ENGINE::ComposedNode::getInputPort, this method is recursive and go 
 * down hierarchy step by step to complete its work.
 */
OutputPort * ComposedNode::getOutputPort(const std::string& name) const throw(Exception)
{
  string portName, nodeName;
  if(splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,false))
    {
      Node *child = getChildByShortName(nodeName);
      return child->getOutputPort(portName);
    }
  else
    {
      string what("ComposedNode::getOutputPort : the port with name "); what+=name; what+=" does not exist on the current level";
      throw Exception(what);
    }
}

InputDataStreamPort *ComposedNode::getInputDataStreamPort(const std::string& name) const throw(Exception)
{
  string portName, nodeName;
  if(splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,true))
    {
      Node *child = getChildByName(nodeName);
      return child->getInputDataStreamPort(portName);
    }
  else
    {
      string what("ComposedNode::getInputDataStreamPort : the port with name "); what+=name; what+=" does not exist on the current level";
      throw Exception(what);
    }
}

OutputDataStreamPort *ComposedNode::getOutputDataStreamPort(const std::string& name) const throw(Exception)
{
  string portName, nodeName;
  if(splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,true))
    {
      Node *child = getChildByName(nodeName);
      return child->getOutputDataStreamPort(portName);
    }
  else
    {
      string what("ComposedNode::getOutputDataStreamPort : the port with name "); what+=name; what+=" does not exist on the current level";
      throw Exception(what);
    }
}

//! Update node state on receiving event from a node
/*!
 *
 * \note Runtime called method. Perform, the state updating, from the son node 'node' 
 *       emitting the event 'event' (among Executor static const var).
 *         WARNING Precondition : this == node->_father
 * \return The event (among Executor static const var) destinated to this->_father node 
 *         to perform eventually up level update.
 *
 * Calls ComposedNode::updateStateOnStartEventFrom if event is YACS::START
 *
 * Calls ComposedNode::updateStateOnFinishedEventFrom if event is YACS::FINISH
 *
 * Called by ComposedNode::notifyFrom
 */
YACS::Event ComposedNode::updateStateFrom(Node *node,        //* I : node emitting event
                                          YACS::Event event  //* I : event emitted
                                          )
{
  DEBTRACE("updateStateFrom: " << node->getName() << " " << event);
  try
    {
      switch(event)
        {
        case YACS::START:
          return updateStateOnStartEventFrom(node);
          break;
        case YACS::FINISH:
          return updateStateOnFinishedEventFrom(node);
          break;
        case YACS::ABORT:
          return updateStateOnFailedEventFrom(node);
          break;
        default:
          return YACS::NOEVENT;//TODO unexpected type of event
          break;
        }
    }
  catch(YACS::Exception& ex)
    {
      //unexpected exception: probably a bug in engine
      //try to keep a consistent global state
      DEBTRACE( "updateStateFrom: " << ex.what() );
      setState(YACS::ERROR);
      exForwardFailed();
      return YACS::ABORT;
    }
  catch(...)
    {
      //unexpected exception: probably a bug in engine
      //try to keep a consistent global state
      setState(YACS::ERROR);
      exForwardFailed();
      return YACS::ABORT;
    }
}

//! Method used to notify the node that a child node has started
/*!
 * Update the ComposedNode state and return the ComposedNode change state 
 *
 *  \param node : the child node that has started
 *  \return the loop state change
 */
YACS::Event ComposedNode::updateStateOnStartEventFrom(Node *node)
{
  setState(YACS::ACTIVATED);
  return YACS::START;
}

//! Method used to notify the node that a child node has failed
YACS::Event ComposedNode::updateStateOnFailedEventFrom(Node *node)
{
   setState(YACS::FAILED);
   return YACS::ABORT;
}

void ComposedNode::checkLinkPossibility(OutPort *start, const std::set<ComposedNode *>& pointsOfViewStart,
                                        InPort *end, const std::set<ComposedNode *>& pointsOfViewEnd) throw(Exception)
{
  if((dynamic_cast<DataFlowPort *>(start) or dynamic_cast<DataFlowPort *>(end))
     and (dynamic_cast<DataStreamPort *>(start) or dynamic_cast<DataStreamPort *>(end)))
    {//cross protocol required : deeper check needed
      bool isOK=false;
      set<ComposedNode *>::iterator iter;
      for(iter=pointsOfViewStart.begin();iter!=pointsOfViewStart.end() and !isOK;iter++)
        isOK=(*iter)->isRepeatedUnpredictablySeveralTimes();
      for(iter=pointsOfViewEnd.begin();iter!=pointsOfViewEnd.end() and !isOK;iter++)
        isOK=(*iter)->isRepeatedUnpredictablySeveralTimes();
      if(!isOK)
        throw Exception("ComposedNode::checkLinkPossibility : Request for cross protocol link impossible.");
    }
}

void ComposedNode::buildDelegateOf(InPort * & port, OutPort *initialStart, const std::set<ComposedNode *>& pointsOfView)
{
}

void ComposedNode::buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView)
{
}

void ComposedNode::getDelegateOf(InPort * & port, OutPort *initialStart, const std::set<ComposedNode *>& pointsOfView) throw(Exception)
{
}

void ComposedNode::getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView) throw(Exception)
{
}

void ComposedNode::releaseDelegateOf(InPort * & port, OutPort *initialStart, const std::set<ComposedNode *>& pointsOfView) throw(Exception)
{
}

void ComposedNode::releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView) throw(Exception)
{
}

void ComposedNode::loaded()
{
}

void ComposedNode::accept(Visitor *visitor)
{
  set<Node *> constituents=edGetDirectDescendants();
  for(set<Node *>::iterator iter=constituents.begin(); iter!=constituents.end(); iter++)
    {
      (*iter)->accept(visitor);
    }
}

//! redefined on derived class of ComposedNode. by default a ComposedNode has no port by itself
std::list<InputPort *> ComposedNode::getLocalInputPorts() const
{
  std::list<InputPort *> lip; return lip; // empty list
}

//! redefined on derived class of ComposedNode. by default a ComposedNode has no port by itself
std::list<OutputPort *> ComposedNode::getLocalOutputPorts() const
{
  std::list<OutputPort *> lop; return lop; // empty list
}

bool ComposedNode::isNameAlreadyUsed(const std::string& name) const
{
  return false;
}

