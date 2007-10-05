#include "OutPort.hxx"
#include "InPort.hxx"
#include "ComposedNode.hxx"
#include <algorithm>
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

OutPort::OutPort(const OutPort& other, Node *newHelder):DataPort(other,newHelder),Port(other,newHelder)
{
}

OutPort::OutPort(const std::string& name, Node *node, TypeCode* type):DataPort(name,node,type),Port(node)
{
}

OutPort::~OutPort()
{
}

void OutPort::getAllRepresented(std::set<OutPort *>& represented) const
{
  represented.insert((OutPort *)this);
}

int OutPort::edGetNumberOfOutLinks() const
{
  return edSetInPort().size();
}

std::vector<DataPort *> OutPort::calculateHistoryOfLinkWith(InPort *end)
{
  if(!isAlreadyLinkedWith(end))
    throw Exception("ComposedNode::edRemoveLink : unexisting link");
  vector<DataPort *> ret;
  ComposedNode* lwstCmnAnctr=ComposedNode::getLowestCommonAncestor(getNode(),end->getNode());
  set<ComposedNode *> allAscendanceOfNodeStart=getNode()->getAllAscendanceOf(lwstCmnAnctr);
  set<ComposedNode *> allAscendanceOfNodeEnd=end->getNode()->getAllAscendanceOf(lwstCmnAnctr);

  // --- Part of test if the link from 'start' to 'end' really exist particulary all eventually intermediate ports created

  ComposedNode *iterS=getNode()->getFather();
  pair<OutPort *,OutPort *> currentPortO(this,this);
  ret.push_back(currentPortO.first);
  while(iterS!=lwstCmnAnctr)
    {
      iterS->getDelegateOf(currentPortO, end, allAscendanceOfNodeEnd);
      if(currentPortO.first!=ret.back())
        ret.push_back(currentPortO.first);
      iterS=iterS->_father;
    }
  iterS=end->getNode()->getFather();
  InPort *currentPortI=end;
  int i=0;
  while(iterS!=lwstCmnAnctr)
    {
      vector<DataPort *>::iterator iter2;
      iterS->getDelegateOf(currentPortI, this, allAscendanceOfNodeStart);
      if(currentPortI!=ret.back())
        {
          i++;
          ret.push_back(currentPortI);
        }
      iterS=iterS->_father;
    }
  vector<DataPort *>::iterator iter=ret.end(); iter-=i;
  reverse(iter,ret.end());
  return ret;
}
