#include "DeploymentTree.hxx"
#include "ComponentInstance.hxx"
#include "Container.hxx"
#include "Scheduler.hxx"
#include "Task.hxx"

using namespace std;
using namespace YACS::ENGINE;

DeploymentTreeOnHeap::DeploymentTreeOnHeap():_cnt(1)
{
}

DeploymentTreeOnHeap::~DeploymentTreeOnHeap()
{
}

bool DeploymentTreeOnHeap::decrRef()
{
  bool ret=(--_cnt==0);
  if(ret)
    delete this;
  return ret;
}

void DeploymentTreeOnHeap::incrRef() const
{
  _cnt++;
}

unsigned char DeploymentTreeOnHeap::appendTask(Task *task, Scheduler *cloner)
{
  if(!task)
    return DeploymentTree::NULL_TASK;
  if(!task->isDeployable())//Task not needed to be placed.
    return DeploymentTree::NOT_DEPLOYABLE_TASK;
  ComponentInstance *ci=task->getComponent();
  if(!ci)//Task is not attached to a Component -> not needed to be placed.
    {
      _freePlacableTasks.push_back(pair<Task *,Scheduler *>(task,cloner));
      return DeploymentTree::DEPLOYABLE_BUT_NOT_SPECIFIED;
    }
  Container *cont=ci->getContainer();
  vector< vector< vector< pair<Task *, Scheduler *> > > >::iterator iter1;
  vector< vector< pair<Task *, Scheduler * > > >::iterator iter2;
  vector< pair<Task *, Scheduler *> >::iterator iter3;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    if(((*iter1)[0][0]).first->getComponent()->getContainer()==cont)
      break;
  if(iter1==_tree.end())
    {
      _tree.push_back(vector< vector< pair< Task *, Scheduler *> > >());
      iter1=_tree.end();
      iter1--;
    }
  for(iter2=(*iter1).begin();iter2!=(*iter1).end();iter2++)
    if(((*iter2)[0]).first->getComponent()==ci)
      break;
  if(iter2==(*iter1).end())
    {
      (*iter1).push_back(vector< pair< Task *, Scheduler *> >());
      iter2=(*iter1).end();
      iter2--;
    }
  for(iter3=(*iter2).begin();iter3!=(*iter2).end();iter3++)
    if((*iter3).first==task)
      return DeploymentTree::ALREADY_IN_TREE;
  if(!isConsistentTaskRegardingShCompInst(*iter2,cloner))
    return DeploymentTree::DUP_TASK_NOT_COMPATIBLE_WITH_EXISTING_TREE;
  (*iter2).push_back(pair<Task *,Scheduler *>(task,cloner));
  return DeploymentTree::APPEND_OK;
}

unsigned DeploymentTreeOnHeap::getNumberOfCTDefContainer() const
{
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  vector< vector< pair<Task *, Scheduler * > > >::const_iterator iter2;
  vector< pair<Task *, Scheduler *> >::const_iterator iter3;
  unsigned ret=0;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    {
      bool isCTDefSurely1=true;
      for(iter2=(*iter1).begin();iter2!=(*iter1).end() && isCTDefSurely1;iter2++)
        {
          bool isCTDefSurely2=true;
          for(iter3=(*iter2).begin();iter3!=(*iter2).end() && isCTDefSurely2;iter3++)
            if((*iter3).second!=0)
              isCTDefSurely2=false;
          if(isCTDefSurely2)
            isCTDefSurely1=true;
          else
            isCTDefSurely1=(((*iter2)[0].first)->getComponent()->isAttachedOnCloning());
        }
      Container *cont=((*iter1)[0][0].first)->getComponent()->getContainer();
      if(isCTDefSurely1)
        {
          if(cont)
            ret++;
        }
      else
        if(cont)
          if(cont->isAttachedOnCloning())
            ret++;
          else
            {
              unsigned val;
              if((*iter1)[0][0].second->isMultiplicitySpecified(val))
                ret+=val;
            }
      }
  return ret;
}

unsigned DeploymentTreeOnHeap::getNumberOfRTODefContainer() const
{
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  vector< vector< pair<Task *, Scheduler * > > >::const_iterator iter2;
  vector< pair<Task *, Scheduler *> >::const_iterator iter3;
  unsigned ret=0;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    {
      bool isRTODefSurely1=true;
      for(iter2=(*iter1).begin();iter2!=(*iter1).end() && isRTODefSurely1;iter2++)
        {
          bool isRTODefSurely2=true;
          for(iter3=(*iter2).begin();iter3!=(*iter2).end() && isRTODefSurely2;iter3++)
            if((*iter3).second==0)
              isRTODefSurely2=false;
          if(isRTODefSurely2)
            isRTODefSurely1=!(((*iter2)[0].first)->getComponent()->isAttachedOnCloning());
          else
            isRTODefSurely1=false;
        }
      if(isRTODefSurely1)
        if(((*iter1)[0][0].first)->getComponent()->getContainer())
          if(!((*iter1)[0][0].first)->getComponent()->getContainer()->isAttachedOnCloning())
            ret++;
    }
  return ret;
}

unsigned DeploymentTreeOnHeap::getNumberOfCTDefComponentInstances() const
{
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  vector< vector< pair<Task *, Scheduler * > > >::const_iterator iter2;
  vector< pair<Task *, Scheduler *> >::const_iterator iter3;
  unsigned ret=0;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    for(iter2=(*iter1).begin();iter2!=(*iter1).end();iter2++)
      {
        bool isCTDefSurely=true;
        for(iter3=(*iter2).begin();iter3!=(*iter2).end() && isCTDefSurely;iter3++)
          if((*iter3).second!=0)
            isCTDefSurely=false;
        if(isCTDefSurely)
          ret++;
        else
          if(((*iter2)[0].first)->getComponent()->isAttachedOnCloning())
            ret++;
      }
  return ret;
}

unsigned DeploymentTreeOnHeap::getNumberOfRTODefComponentInstances() const
{
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  vector< vector< pair<Task *, Scheduler * > > >::const_iterator iter2;
  vector< pair<Task *, Scheduler *> >::const_iterator iter3;
  unsigned ret=0;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    for(iter2=(*iter1).begin();iter2!=(*iter1).end();iter2++)
      {
        bool isRTODef=false;
        for(iter3=(*iter2).begin();iter3!=(*iter2).end() && !isRTODef;iter3++)
          if((*iter3).second!=0)
            isRTODef=true;
        if(isRTODef && !((*iter2)[0].first)->getComponent()->isAttachedOnCloning())
          ret++;
      }
  return ret;
}

std::vector<Container *> DeploymentTreeOnHeap::getAllContainers() const
{
  vector<Container *> ret;
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    ret.push_back(((*iter1)[0][0].first)->getComponent()->getContainer());
  return ret;
}

std::vector<Container *> DeploymentTreeOnHeap::getAllCTDefContainers() const
{
  vector<Container *> ret;
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  vector< vector< pair<Task *, Scheduler * > > >::const_iterator iter2;
  vector< pair<Task *, Scheduler *> >::const_iterator iter3;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    {
      bool isCTDefSurely1=true;
      for(iter2=(*iter1).begin();iter2!=(*iter1).end() && isCTDefSurely1;iter2++)
        {
          bool isCTDefSurely2=true;
          for(iter3=(*iter2).begin();iter3!=(*iter2).end() && isCTDefSurely2;iter3++)
            if((*iter3).second!=0)
              isCTDefSurely2=false;
          if(isCTDefSurely2)
            isCTDefSurely1=true;
          else
            isCTDefSurely1=(((*iter2)[0].first)->getComponent()->isAttachedOnCloning());
        }
      Container *cont=((*iter1)[0][0].first)->getComponent()->getContainer();
      if(isCTDefSurely1)
        {
          if(cont)
            ret.push_back(cont);
        }
      else
        if(cont)
          if(cont->isAttachedOnCloning())
            ret.push_back(cont);
      }
  return ret;
}

std::vector<Container *> DeploymentTreeOnHeap::getAllRTODefContainers() const
{
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  vector< vector< pair<Task *, Scheduler * > > >::const_iterator iter2;
  vector< pair<Task *, Scheduler *> >::const_iterator iter3;
  vector<Container *> ret;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    {
      bool isRTODefSurely1=true;
      for(iter2=(*iter1).begin();iter2!=(*iter1).end() && isRTODefSurely1;iter2++)
        {
          bool isRTODefSurely2=true;
          for(iter3=(*iter2).begin();iter3!=(*iter2).end() && isRTODefSurely2;iter3++)
            if((*iter3).second==0)
              isRTODefSurely2=false;
          if(isRTODefSurely2)
            isRTODefSurely1=!(((*iter2)[0].first)->getComponent()->isAttachedOnCloning());
          else
            isRTODefSurely1=false;
        }
      if(isRTODefSurely1)
        if(((*iter1)[0][0].first)->getComponent()->getContainer())
          if(!((*iter1)[0][0].first)->getComponent()->getContainer()->isAttachedOnCloning())
            ret.push_back(((*iter1)[0][0].first)->getComponent()->getContainer());
    }
  return ret;
}

std::vector<Task *> DeploymentTreeOnHeap::getTasksLinkedToComponent(ComponentInstance *comp) const
{
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  vector< vector< pair<Task *, Scheduler * > > >::const_iterator iter2;
  vector< pair<Task *, Scheduler *> >::const_iterator iter3;
  
  std::vector<Task *> ret;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    for(iter2=(*iter1).begin();iter2!=(*iter1).end();iter2++)
      if(((*iter2)[0].first)->getComponent()==comp)
        for(iter3=(*iter2).begin();iter3!=(*iter2).end();iter3++)
          ret.push_back((*iter3).first);
  return ret;
}

std::vector<ComponentInstance *> DeploymentTreeOnHeap::getComponentsLinkedToContainer(Container *cont) const
{
  vector<ComponentInstance *> ret;
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  vector< vector< pair<Task *, Scheduler * > > >::const_iterator iter2;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    if(((*iter1)[0][0].first)->getComponent()->getContainer()==cont)
      for(iter2=(*iter1).begin();iter2!=(*iter1).end();iter2++)
        ret.push_back(((*iter2)[0].first)->getComponent());
  return ret;
}

bool DeploymentTreeOnHeap::presenceOfDefaultContainer() const
{
  vector< vector< vector< pair<Task *, Scheduler *> > > >::const_iterator iter1;
  for(iter1=_tree.begin();iter1!=_tree.end();iter1++)
    if(!((*iter1)[0][0].first)->getComponent()->getContainer())
      return true;
  return false;
}

std::vector<Task *> DeploymentTreeOnHeap::getFreeDeployableTasks() const
{
  vector<Task *> ret;
  for(vector< pair<Task *,Scheduler *> >::const_iterator iter=_freePlacableTasks.begin();iter!=_freePlacableTasks.end();iter++)
    ret.push_back((*iter).first);
  return ret;
}

bool DeploymentTreeOnHeap::isConsistentTaskRegardingShCompInst(std::vector< std::pair<Task *, Scheduler * > >& tasksSharingSameCompInst, Scheduler *cloner)
{
  vector< std::pair<Task *, Scheduler * > >::const_iterator iter;
  bool coexistenceOfDifferentSched=false;
  for(iter=tasksSharingSameCompInst.begin();iter!=tasksSharingSameCompInst.end() && !coexistenceOfDifferentSched;iter++)
    {
      coexistenceOfDifferentSched=(((*iter).second)!=cloner);
    }
  if(!coexistenceOfDifferentSched)
    return true;
  //In this case the component is duplicated on cloning raising on runtime on different policy (schedulers) than other tasks in tasksSharingSameCompInst
  return (tasksSharingSameCompInst[0].first)->getComponent()->isAttachedOnCloning();
}

DeploymentTree::DeploymentTree():_treeHandle(0)
{
}

DeploymentTree::~DeploymentTree()
{
  if(_treeHandle)
    _treeHandle->decrRef();
}

DeploymentTree::DeploymentTree(const DeploymentTree& other)
{
  _treeHandle=other._treeHandle;
  if(_treeHandle)
    _treeHandle->incrRef();
}

const DeploymentTree &DeploymentTree::operator=(const DeploymentTree& other)
{
  if(_treeHandle)
    _treeHandle->decrRef();
  _treeHandle=other._treeHandle;
  if(_treeHandle)
    _treeHandle->incrRef();
  return *this;
}

unsigned char DeploymentTree::appendTask(Task *task, Scheduler *cloner)
{
  if(_treeHandle)
    return _treeHandle->appendTask(task,cloner);
  if(!task)
    return DeploymentTree::NULL_TASK;
  if(!task->isDeployable())//Task not needed to be placed.
    return DeploymentTree::NOT_DEPLOYABLE_TASK;
  _treeHandle=new DeploymentTreeOnHeap;
  return _treeHandle->appendTask(task,cloner);
}

/*!
 * Returns number of containers predictably launchable \b without counting default container.
 */
unsigned DeploymentTree::getNumberOfCTDefContainer() const
{
  if(_treeHandle)
    return _treeHandle->getNumberOfCTDefContainer();
  return 0;
}

/*!
 * Returns number of containers unpredictably launchable \b without counting default container.
 */
unsigned DeploymentTree::getNumberOfRTODefContainer() const
{
  if(_treeHandle)
    return _treeHandle->getNumberOfRTODefContainer();
  return 0;
}

unsigned DeploymentTree::getNumberOfCTDefComponentInstances() const
{
  if(_treeHandle)
    return _treeHandle->getNumberOfCTDefComponentInstances();
  return 0;
}

unsigned DeploymentTree::getNumberOfRTODefComponentInstances() const
{
  if(_treeHandle)
    return _treeHandle->getNumberOfRTODefComponentInstances();
  return 0;
}

/*!
 * Returns all containers default included (0).
 */
std::vector<Container *> DeploymentTree::getAllContainers() const
{
  if(_treeHandle)
    return _treeHandle->getAllContainers();
  return vector<Container *>();
}

/*!
 * Returns containers predictably launchable \b without counting default container.
 */
std::vector<Container *> DeploymentTree::getAllCTDefContainers() const
{
  if(_treeHandle)
    return _treeHandle->getAllCTDefContainers();
  return vector<Container *>();
}

/*!
 * Returns containers unpredictably launchable \b without counting default container.
 */
std::vector<Container *> DeploymentTree::getAllRTODefContainers() const
{
  if(_treeHandle)
    return _treeHandle->getAllRTODefContainers();
  return vector<Container *>();
}

std::vector<Task *> DeploymentTree::getTasksLinkedToComponent(ComponentInstance *comp) const
{
  if(_treeHandle)
    return _treeHandle->getTasksLinkedToComponent(comp);
  return vector<Task *>();
}

std::vector<ComponentInstance *> DeploymentTree::getComponentsLinkedToContainer(Container *cont) const
{
  if(_treeHandle)
    return _treeHandle->getComponentsLinkedToContainer(cont);
  return vector<ComponentInstance *>();
}

bool DeploymentTree::presenceOfDefaultContainer() const
{
  if(_treeHandle)
    return _treeHandle->presenceOfDefaultContainer();
  return false;
}

bool DeploymentTree::isNull() const
{
  return _treeHandle==0;
}

std::vector<Task *> DeploymentTree::getFreeDeployableTasks() const
{
  if(_treeHandle)
    return _treeHandle->getFreeDeployableTasks();
  return vector<Task *>();
}
