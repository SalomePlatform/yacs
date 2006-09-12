#include "Port.hxx"

using namespace YACS::ENGINE;

const char Port::NAME[]="Port";

Port::Port(Node *node):_node(node)
{
}

Port::~Port()
{
}

std::string Port::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}
