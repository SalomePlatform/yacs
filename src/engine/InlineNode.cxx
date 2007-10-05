#include "InlineNode.hxx"
#include "Visitor.hxx"

using namespace YACS::ENGINE;


InlineNode::~InlineNode() { }

void InlineNode::accept(Visitor *visitor)
{
  visitor->visitInlineNode(this);
}


InlineFuncNode::~InlineFuncNode() { }

void InlineFuncNode::accept(Visitor *visitor)
{
  visitor->visitInlineFuncNode(this);
}

