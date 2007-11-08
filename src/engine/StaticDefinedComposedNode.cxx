#include "StaticDefinedComposedNode.hxx"
#include "OutPort.hxx"
#include "InPort.hxx"

using namespace std;
using namespace YACS::ENGINE;

StaticDefinedComposedNode::StaticDefinedComposedNode(const std::string& name):ComposedNode(name)
{
}

StaticDefinedComposedNode::StaticDefinedComposedNode(const StaticDefinedComposedNode& other, ComposedNode *father):ComposedNode(other,father)
{
}

bool StaticDefinedComposedNode::isPlacementPredictableB4Run() const
{
  return true;
}

bool StaticDefinedComposedNode::isMultiplicitySpecified(unsigned& value) const
{
  value=1;
  return true;
}

void StaticDefinedComposedNode::forceMultiplicity(unsigned value)
{
  //no sense for this class
}

void StaticDefinedComposedNode::checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                                       std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                                       std::vector<OutPort *>& fwCross,
                                                       std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                                       LinkInfo& info) const
{
  if(start->getNode()==end->getNode())
    bw[(ComposedNode *)this].push_back(start);
  else
    throw Exception("Internal error occured - dealing an unexpected link !");
}
