#include "InPort.hxx"
#include "OutPort.hxx"
#include "ComposedNode.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

InPort::InPort(const InPort& other, Node *newHelder):
  DataPort(other,newHelder),Port(other,newHelder)
{
}

InPort::InPort(const std::string& name, Node *node, TypeCode* type):
  DataPort(name,node,type),Port(node)
{
}

InPort::~InPort()
{
}

//! Returns number of \b physical backlinks \b NOT number of user backlinks.
int InPort::edGetNumberOfLinks() const
{
  return _backLinks.size();
}

void InPort::edRemoveAllLinksLinkedWithMe() throw(Exception)
{
  set<OutPort *> temp(_backLinks);//edRemoveLink called after causes invalidation of set iterator.
  for(set<OutPort *>::iterator iter=temp.begin();iter!=temp.end();iter++)
    {
      set<OutPort *> trueBackOutputs;
      (*iter)->getAllRepresented(trueBackOutputs);
      for(set<OutPort *>::iterator iter2=trueBackOutputs.begin();iter2!=trueBackOutputs.end();iter2++)
        _node->getRootNode()->edRemoveLink(*iter2,this);
    }
  _backLinks.clear();
  modified();
}

//! Returns \b physical backlinks \b NOT user backlinks.
std::set<OutPort *> InPort::edSetOutPort() const
{
  return _backLinks;
}

void InPort::edNotifyReferencedBy(OutPort *fromPort)
{
  _backLinks.insert(fromPort);
  modified();
}

void InPort::edNotifyDereferencedBy(OutPort *fromPort)
{
  _backLinks.erase(fromPort);
  modified();
}

void InPort::getAllRepresentants(std::set<InPort *>& repr) const
{
  repr.insert((InPort *)this);
}
