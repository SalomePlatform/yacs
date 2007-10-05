#include "ServiceInlineNode.hxx"
#include "Visitor.hxx"

using namespace YACS::ENGINE;

ServiceInlineNode::ServiceInlineNode(const std::string& name):ServiceNode(name)
{
}

ServiceInlineNode::ServiceInlineNode(const ServiceInlineNode& other, ComposedNode *father):ServiceNode(other,father),_script(other._script)
{
}

void ServiceInlineNode::setScript (const std::string &script)
{
  _script=script;
}

std::string  ServiceInlineNode::getScript() const
{
  return _script;
}

void ServiceInlineNode::accept(Visitor *visitor)
{
  visitor->visitServiceInlineNode(this);
}
