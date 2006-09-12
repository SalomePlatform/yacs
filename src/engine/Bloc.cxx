#include "Bloc.hxx"
#include "ElementaryNode.hxx"

using namespace YACS::ENGINE;

Bloc::Bloc(const std::string& name):ComposedNode(name)
{
}

Bloc::~Bloc()
{
  for(std::list<Node *>::iterator iter=_listOfNode.begin();iter!=_listOfNode.end();iter++)
    delete *iter;
}

void Bloc::init()
{
  for(std::list<Node *>::iterator iter=_listOfNode.begin();iter!=_listOfNode.end();iter++)
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
  for(std::list<Node *>::const_iterator iter=_listOfNode.begin();iter!=_listOfNode.end();iter++)
    {
      ret+=(*iter)->getOutGate()->getNbOfInGatesConnected();
    }
  return ret;
}

std::vector<Task *> Bloc::getNextTasks(bool& isMore)
{
  std::vector<Task *> ret;
  isMore=false;
  if(_state==YACS::DONE || _state==YACS::INITED)
    return ret;
  for(std::list<Node *>::iterator iter=_listOfNode.begin();iter!=_listOfNode.end();iter++)
    (*iter)->getReadyTasks(ret);
  isMore=!ret.empty();
  return ret;
}

void Bloc::getReadyTasks(std::vector<Task *>& tasks)
{
  if(_state==YACS::TOACTIVATE || _state==YACS::ACTIVATED)
    for(std::list<Node *>::iterator iter=_listOfNode.begin();iter!=_listOfNode.end();iter++)
      (*iter)->getReadyTasks(tasks);
}

std::list<ElementaryNode *> Bloc::getRecursiveConstituents()
{
  std::list<ElementaryNode *> ret;
  for(std::list<Node *>::iterator iter=_listOfNode.begin();iter!=_listOfNode.end();iter++)
    {
      std::list<ElementaryNode *> myCurrentList=(*iter)->getRecursiveConstituents();
      ret.insert(ret.begin(),myCurrentList.begin(),myCurrentList.end());
    }
  return ret;
}

/**
 * @ note : Update the '_state' attribute.
 *          Typically called by 'this->_inGate' when 'this->_inGate' is ready. Contrary to Node::exUpdateState no check done on inputs
 *          because internal linked DF inputports are not valid yet.
 */
void Bloc::exUpdateState()
{
  if(_inGate.exIsReady())
    _state=YACS::TOACTIVATE;
}

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
    throw Exception("Bloc::edAddChild : node is not orphan");
  if(isNameAlreadyUsed(node->getName()))
    {
      std::string what("Bloc::edAddChild : name "); what+=node->getName(); what+=" already exists in the scope of "; what+=_name;
      throw Exception(what);
    }
  node->_father=this;
  _listOfNode.push_back(node);
  return true;
}

/**
 * @ note : Remove 'node' from the list of direct children.
 *          WARNING 1 : node is destroyed after invocation of this method because Bloc class has ownership of its child nodes.
 *          WARNING 2 : all links to 'node' are automatically desactivated. As consequence this method is quite heavy for big graphs due to
 *                      unilateral storing policy of links. 
 * @ exception : If 'node' is NOT the direct son of 'this'.
 *
 */
void Bloc::edRemoveChild(Node *node) throw(Exception)
{
  if(node->_father!=this)
    throw Exception("Bloc::edRemoveChild : node is NOT managed by this");
  if(!isNodeAlreadyAggregated(node))
    throw Exception("Bloc::edRemoveChild : Internal error occured");
  ComposedNode *myRootNode=getRootNode();
  myRootNode->disconnectAllLinksConnectedTo(node);
  _listOfNode.remove(node);
  delete node;
}

void Bloc::selectRunnableTasks(std::vector<Task *>& tasks)
{
}

bool Bloc::areAllSubNodesFinished() const
{
  bool ret=true;
  for(std::list<Node *>::const_iterator iter=_listOfNode.begin();iter!=_listOfNode.end() && ret;iter++)
    if((*iter)->_state!=YACS::DONE)
      ret=false;
  return ret;
}

bool Bloc::isNodeAlreadyAggregated(Node *node) const
{
  for(std::list<Node *>::const_iterator iter=_listOfNode.begin();iter!=_listOfNode.end();iter++)
    {
      if((*iter)==node)
	return true;
    }
  return false;
}

bool Bloc::isNameAlreadyUsed(const std::string& name) const
{
  for(std::list<Node *>::const_iterator iter=_listOfNode.begin();iter!=_listOfNode.end();iter++)
      if((*iter)->getName()==name)
	return true;
  return false;
}

/**
 * @ note : Checks that in the forest from 'node' there are NO back-edges.
 *          WARNING : When using this method 'node' has to be checked in order to be part of direct children of 'this'. 
 *
 */
void Bloc::checkNoCyclePassingThrough(Node *node) throw(Exception)
{
  initChildrenForDFS();
  node->_colour=YACS::Grey;
  bool verdict=true;
  std::list<Node *> currentNodesToTest;
  currentNodesToTest.push_back(node);
  for(std::list<Node *>::iterator iter1=currentNodesToTest.begin();iter1!=currentNodesToTest.end() && verdict;)
    {
      std::list<Node *> outNodes=(*iter1)->getOutNodes();
      for(std::list<Node *>::iterator iter2=outNodes.begin();iter2!=outNodes.end() && verdict;iter2++)
	if((*iter2)->_colour==YACS::White)
	  {
	    currentNodesToTest.push_back(*iter2);
	    (*iter2)->_colour=YACS::Grey;
	  }
	else
	  verdict=false;
      iter1=currentNodesToTest.erase(iter1);
    }
  if(!verdict)
    throw Exception("Cycle has been detected");
}

void Bloc::initChildrenForDFS() const
{
  for(std::list<Node *>::const_iterator iter=_listOfNode.begin();iter!=_listOfNode.end();iter++)
    (*iter)->initForDFS();
}

/**
 *
 * @ note : Runtime called method. Indirectly called by ComposedNode::updateStateFrom which has dispatch to this method
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
 * @ note : Runtime called method. Indirectly called by ComposedNode::updateStateFrom which has dispatch to this method
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
