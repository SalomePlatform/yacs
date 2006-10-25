#include "Bloc.hxx"
#include "ElementaryNode.hxx"

using namespace YACS::ENGINE;
using namespace std;

Bloc::Bloc(const string& name):ComposedNode(name)
{
}

Bloc::~Bloc()
{
  for(set<Node *>::iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    delete *iter;
}

void Bloc::init()
{
  _inGate.exReset();
  for(set<Node *>::iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    (*iter)->init();
  if(_inGate.exIsReady())
    _state=YACS::TOACTIVATE;
  else
    _state=YACS::INITED;
}

bool Bloc::isFinished()
{
  return _state==YACS::DONE;
}

int Bloc::getNumberOfCFLinks() const
{
  int ret=0;
  for(set<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    {
      ret+=(*iter)->getOutGate()->getNbOfInGatesConnected();
    }
  return ret;
}

vector<Task *> Bloc::getNextTasks(bool& isMore)
{
  vector<Task *> ret;
  isMore=false;
  if(_state==YACS::DONE || _state==YACS::INITED)
    return ret;
  for(set<Node *>::iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    (*iter)->getReadyTasks(ret);
  isMore=!ret.empty();
  return ret;
}

void Bloc::getReadyTasks(vector<Task *>& tasks)
{
  if(_state==YACS::TOACTIVATE || _state==YACS::ACTIVATED)
    for(set<Node *>::iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
      (*iter)->getReadyTasks(tasks);
}

set<ElementaryNode *> Bloc::getRecursiveConstituents()
{
  set<ElementaryNode *> ret;
  for(set<Node *>::iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    {
      set<ElementaryNode *> myCurrentSet=(*iter)->getRecursiveConstituents();
      ret.insert(myCurrentSet.begin(),myCurrentSet.end());
    }
  return ret;
}

/**
 * Update the '_state' attribute.
 * Typically called by 'this->_inGate' when 'this->_inGate' is ready. Contrary to Node::exUpdateState no check done on inputs
 * because internal linked DF inputports are not valid yet.
 */

void Bloc::exUpdateState()
{
  if(_inGate.exIsReady())
    _state=YACS::TOACTIVATE;
}

/**
* If node is already a direct child of current bloc, do nothing.
* If node is a child of another bloc, throw exception.
* If node name already used in bloc, throw exception.
* Publish inputPorts in current bloc and ancestors.
*/

bool Bloc::edAddChild(Node *node) throw(Exception)
{
  if(isNodeAlreadyAggregated(node))
    {
      if(node->_father==this)
	return false;
      else
	throw Exception("Bloc::edAddChild : Internal error occured");
    }

  if(node->_father)
    {
      string what = "Bloc::edAddChild: node is not orphan: "; what += node->getName();
      throw Exception(what);
    }

  if(isNameAlreadyUsed(node->getName()))
    {
      string what("Bloc::edAddChild : name "); what+=node->getName(); what+=" already exists in the scope of "; what+=_name;
      throw Exception(what);
    }

  node->_father=this;
  _setOfNode.insert(node);

  ComposedNode *iter=node->_father;
  while(iter)
    {
      for(set<InputPort *>::iterator itn = node->_setOfInputPort.begin(); itn != node->_setOfInputPort.end(); itn++)
	iter->publishInputPort(*itn);
      iter=iter->_father;
    }

  return true;
}

/**
 * Remove 'node' from the set of direct children.
 * WARNING 1 : node is destroyed after invocation of this method because Bloc class has ownership of its child nodes.
 * WARNING 2 : all links to 'node' are automatically desactivated. As consequence this method is quite heavy for big graphs due to
 *             unilateral storing policy of links. 
 * @exception If 'node' is NOT the direct son of 'this'.
 */

void Bloc::edRemoveChild(Node *node) throw(Exception)
{
  if(node->_father!=this)
    throw Exception("Bloc::edRemoveChild : node is NOT managed by this");
  if(!isNodeAlreadyAggregated(node))
    throw Exception("Bloc::edRemoveChild : Internal error occured");
  ComposedNode *myRootNode=getRootNode();
  myRootNode->disconnectAllLinksConnectedTo(node);
  _setOfNode.erase(node);
  delete node;
}

void Bloc::selectRunnableTasks(vector<Task *>& tasks)
{
}

bool Bloc::areAllSubNodesFinished() const
{
  bool ret=true;
  for(set<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end() && ret;iter++)
    if((*iter)->_state!=YACS::DONE)
      ret=false;
  return ret;
}

bool Bloc::isNodeAlreadyAggregated(Node *node) const
{
  for(set<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    {
      if((*iter)==node)
	return true;
    }
  return false;
}

bool Bloc::isNameAlreadyUsed(const string& name) const
{
  for(set<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
      if((*iter)->getName()==name)
	return true;
  return false;
}

bool insertNodeChildrenInSet(Node *node, set<Node *> nodeSet)
{
  bool verdict=true;
  set<Node *> outNodes=node->getOutNodes();
  for (set<Node *>::iterator iter=outNodes.begin();iter!=outNodes.end(); iter++)
    {
      verdict=(nodeSet.insert(*iter)).second;
      if (verdict) verdict = insertNodeChildrenInSet((*iter),nodeSet);
      if (!verdict) break;
    }
  return verdict;
}

/**
 * @note : Checks that in the forest from 'node' there are NO back-edges.
 *          WARNING : When using this method 'node' has to be checked in order to be part of direct children of 'this'. 
 *
 */
void Bloc::checkNoCyclePassingThrough(Node *node) throw(Exception)
{
  set<Node *> currentNodesToTest;
  currentNodesToTest.insert(node);
  if (!insertNodeChildrenInSet(node,currentNodesToTest))
    throw Exception("Cycle has been detected");
}

void Bloc::initChildrenForDFS() const
{
  for(set<Node *>::const_iterator iter=_setOfNode.begin();iter!=_setOfNode.end();iter++)
    (*iter)->initForDFS();
}

/**
 *
 * @note : Runtime called method. Indirectly called by ComposedNode::updateStateFrom which has dispatch to this method
 *          'when event == START'.
 *          WARNING Precondition : '_state == Running' and 'node->_father==this'(garanteed by ComposedNode::notifyFrom)
 *
 */
YACS::Event Bloc::updateStateOnStartEventFrom(Node *node)
{
  _state=YACS::ACTIVATED;
  return YACS::START;
}

/**
 *
 * @note : Runtime called method. Indirectly called by ComposedNode::updateStateFrom which has dispatch to this method
 *          'when event == FINISH'.
 *          WARNING Precondition : '_state == Running' and 'node->_father==this'(garanteed by ComposedNode::notifyFrom)
 *
 */
YACS::Event Bloc::updateStateOnFinishedEventFrom(Node *node)
{
  //ASSERT(node->_father==this)
  if(areAllSubNodesFinished())
    {
      _state=YACS::DONE;
      return YACS::FINISH;//notify to father node that 'this' has becomed finished.
    }
  //more job to do in 'this'
  node->_outGate.exNotifyDone();
  return YACS::NOEVENT;//no notification to father needed because from father point of view nothing happened.
}
