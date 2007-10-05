#include "StaticDefinedComposedNode.hxx"

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
