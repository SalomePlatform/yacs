#include "ComposedNode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ElementaryNode.hxx"

#include <set>

using namespace YACS::ENGINE;

ComposedNode::ComposedNode(const std::string& name):Node(name)
{
}

/**
 *
 * @ note : Runtime called method. Overloads the Scheduler::notifyFrom abstract method. Typically Called in Executor (in a parallel thread or not) by the Task 'task'
 *          to inform the scheduler that an event coded 'event' (in Executor static const var) happened. Contrary to updateStateFrom several level may exist between 'sender' and 'this'.
 *
 */
void ComposedNode::notifyFrom(const Task *sender, //* I : task emitting event
			      YACS::Event event   //* I : event emitted
			      )
{
  ElementaryNode *taskTyped=dynamic_cast<ElementaryNode *>((Task *)sender);
  //ASSERT(taskTyped != 0)
  YACS::Event curEvent=event;
  Node *lminus1LevelNode=taskTyped;
  ComposedNode *curLevelNode=taskTyped->_father;
  curEvent=curLevelNode->updateStateFrom(lminus1LevelNode,curEvent);
  while(curEvent!=YACS::NOEVENT && curLevelNode!=this)
    {
      lminus1LevelNode=curLevelNode;
      curLevelNode=curLevelNode->_father;
      curEvent=curLevelNode->updateStateFrom(lminus1LevelNode,curEvent);
    }
}

/**
 * @ note : Add a dataflow link.
 *          Precondition : 'start' AND 'end' are in/outputPort contained in a node in descendance of 'this'.
 * @ exception : incompatibility between input and output (type), or 'start'/'end' is/are NOT in/outputPort
 *               contained in a node in descendance of 'this', or a mutilple link to an input not supporting it.
 * @ return : true if a new link has been created, false otherwise.
 */
bool ComposedNode::edAddLink(OutputPort *start, InputPort *end) throw(Exception)
{
  ComposedNode* lwstCmnAnctr=getLowestCommonAncestor(start->getNode(),end->getNode());
  std::list<ComposedNode *> allAscendanceOfNodeStart=start->getNode()->getAllAscendanceOf(lwstCmnAnctr);
  std::list<ComposedNode *> allAscendanceOfNodeEnd=end->getNode()->getAllAscendanceOf(lwstCmnAnctr);
  checkInMyDescendance(lwstCmnAnctr);
  ComposedNode *iterS=start->getNode()->_father;
  OutPort *currentPortO=start;
  while(iterS!=lwstCmnAnctr)
    {
      currentPortO=iterS->buildDelegateOf(currentPortO, allAscendanceOfNodeEnd);
      iterS=iterS->_father;
    }
  iterS=end->getNode()->_father;
  InPort *currentPortI=end;
  while(iterS!=lwstCmnAnctr)
    {
      currentPortI=iterS->buildDelegateOf(currentPortI, allAscendanceOfNodeEnd);
      iterS=iterS->_father;
    }
  return currentPortO->addInPort(currentPortI);
}

/**
 * @ note : Add a controlflow link.
 *          Precondition : 'start' AND 'end' are in/outGate contained in a node in DIRECT descendance of 'this'.
 * @ exception : If a cycle has been detected, or incompatibility between input and output, or 'start'/'end' is/are NOT in/outputPort
 *               contained in a node in descendance of 'this', or a mutilple link to an input not supporting it.
 * @ return : true if a new link has been created, false otherwise. 
 */
bool ComposedNode::edAddLink(OutGate *start, InGate *end) throw(Exception)
{
  ComposedNode* father=checkHavingCommonFather(start->getNode(),end->getNode());
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

bool ComposedNode::edAddCFLink(Node *nodeS, Node *nodeE) throw(Exception)
{
  ComposedNode* father=checkHavingCommonFather(nodeS,nodeE);
  if(father!=this)
    {
      checkInMyDescendance(father);
      return father->edAddLink(nodeS->getOutGate(),nodeE->getInGate());
    }
  bool ret=nodeS->getOutGate()->edAddInGate(nodeE->getInGate());
  if(ret)
    checkNoCyclePassingThrough(nodeE);
  return ret;
}

/**
 * @ note : Remove a dataflow link.
 *          Precondition : 'start' AND 'end' are in/outputPort contained in a node in descendance of 'this'.
 * @ exception : The specified link does not exist. The content of Exception is different in accordance with the link from 'start' to 'end' implies DF/DS gateway.
 */
void ComposedNode::edRemoveLink(OutputPort *start, InputPort *end) throw(Exception)
{
  ComposedNode* lwstCmnAnctr=getLowestCommonAncestor(start->getNode(),end->getNode());
  checkInMyDescendance(lwstCmnAnctr);
  std::list<ComposedNode *> allAscendanceOfNodeStart=start->getNode()->getAllAscendanceOf(lwstCmnAnctr);
  std::list<ComposedNode *> allAscendanceOfNodeEnd=end->getNode()->getAllAscendanceOf(lwstCmnAnctr);
  //Part of test if the link from 'start' to 'end' really exist particulary all eventually intermediate ports created
  ComposedNode *iterS=start->getNode()->_father;
  OutPort *currentPortO=start;
  while(iterS!=lwstCmnAnctr)
    {
      currentPortO=iterS->getDelegateOf(currentPortO, allAscendanceOfNodeEnd);
      iterS=iterS->_father;
    }
  iterS=end->getNode()->_father;
  InPort *currentPortI=end;
  while(iterS!=lwstCmnAnctr)
    {
      currentPortI=iterS->getDelegateOf(currentPortI, allAscendanceOfNodeStart);
      iterS=iterS->_father;
    }
  //End of test for evt intermediate ports created
  currentPortO->removeInPort(currentPortI);
  //Performing deletion of intermediate ports
  iterS=start->getNode()->_father;
  currentPortO=start; currentPortI=end;
  while(iterS!=lwstCmnAnctr)
    {
      currentPortO=iterS->releaseDelegateOf(currentPortO, allAscendanceOfNodeEnd);
      iterS=iterS->_father;
    }
  iterS=end->getNode()->_father;
  while(iterS!=lwstCmnAnctr)
    {
      currentPortI=iterS->releaseDelegateOf(currentPortI, allAscendanceOfNodeStart);
      iterS=iterS->_father;
    }
}

void ComposedNode::edRemoveLink(OutGate *start, InGate *end) throw(Exception)
{
  ComposedNode* father=checkHavingCommonFather(start->getNode(),end->getNode());
  if(father!=this)
    throw Exception("edRemoveLink : nodes not in direct descendance of this");
  start->edRemoveInGate(end);
}

void ComposedNode::publishOutputPort(OutputPort *port) throw(Exception)
{
  checkInMyDescendance(port->getNode());
  _listOfOutputPort.push_back(port);
}

void ComposedNode::publishInputPort(InputPort *port)
{
  _listOfInputPort.push_back(port);
}

ComposedNode *ComposedNode::getRootNode() throw(Exception)
{
  if(!_father)
    return this;
  return Node::getRootNode();
}

/**
 * @ note : perform the disconnection of all links under the scope of 'this' connected to an input (dataflow or datastream) of node 'node'.
 *          This method is quite heavy because the links are stored in one direction.
 */
void ComposedNode::disconnectAllLinksConnectedTo(Node *node)
{
  std::list<ElementaryNode *> listOfAllNodes=getRecursiveConstituents();
  for(std::list<ElementaryNode *>::iterator iter=listOfAllNodes.begin();iter!=listOfAllNodes.end();iter++)
    (*iter)->disconnectAllLinksConnectedTo(node);
}

/**
 * @ note : Check that 'nodeToTest' is in descendance of 'this' OR equal to 'this'
 * @ exception : If 'nodeToTest' is NOT in descendance of 'this' AND not equal to 'this'
 */
void ComposedNode::checkInMyDescendance(Node *nodeToTest) const throw(Exception)
{
  const char what[]="check failed : node is not in the correct descendance";
  if(nodeToTest==0)
    throw Exception(what);
  if((ComposedNode *)nodeToTest==this)
    return;
  ComposedNode *iter=nodeToTest->_father;
  while(iter!=0 && iter!=this)
    iter=iter->_father;
  if(iter==0)
    throw Exception(what);
}

/**
 *
 * @ note : Retrieves the lowest common ancestor of 'node1' AND 'node2'. If 'node1' AND 'node2' are equals and are instance of ComposedNode 
 *          the father of 'node1' is returned.
 * @ exception : 'node1' and 'node2' does not share the same genealogy.
 * @ return : The lowest common ancestor if it exists.
 *
 */
ComposedNode *ComposedNode::getLowestCommonAncestor(Node *node1, Node *node2) throw(Exception)
{
  const char what[]="2 nodes does not share the same genealogy";
  if(node1==0 || node2==0)
    throw Exception(what);
  ComposedNode *temp=node1->_father;
  std::set<ComposedNode *> s;
  while(temp)
    {
      s.insert(temp);
      temp=temp->_father;
    }
  //
  temp=node2->_father;
  std::set<ComposedNode *>::iterator iter=s.find(temp);
  while(temp && iter==s.end())
    {
      iter=s.find(temp);
      temp=temp->_father;
    }
  if(iter==s.end())
    throw Exception(what);
  return *iter;
}

/**
 *
 * @ note : Runtime called method. Perform, the state updating, from the son node 'node' emitting the event 'event' (among Executor static const var).
 *          WARNING Precondition : this == node->_father
 * @ return : The event (among Executor static const var) destinated to this->_father node to perform eventually up level update.
 *
 */
YACS::Event ComposedNode::updateStateFrom(Node *node,        //* I : node emitting event
					  YACS::Event event  //* I : event emitted
					  )
{
  switch(event)
    {
    case YACS::START:
      return updateStateOnStartEventFrom(node);
      break;
    case YACS::FINISH:
      return updateStateOnFinishedEventFrom(node);
      break;
    default:
      return YACS::NOEVENT;//TODO unexpected type of event
      break;
    }
}

InPort *ComposedNode::buildDelegateOf(InPort *port, const std::list<ComposedNode *>& pointsOfView)
{
  return port;
}

OutPort *ComposedNode::buildDelegateOf(OutPort *port, const std::list<ComposedNode *>& pointsOfView)
{
  return port;
}

InPort *ComposedNode::getDelegateOf(InPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception)
{
  return port;
}

OutPort *ComposedNode::getDelegateOf(OutPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception)
{
  return port;
}

InPort *ComposedNode::releaseDelegateOf(InPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception)
{
  return port;
}

OutPort *ComposedNode::releaseDelegateOf(OutPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception)
{
  return port;
}
