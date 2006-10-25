#include "Port.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char Port::NAME[]="Port";

int Port::total_ = 0;

Port::Port(Node *node)
  : _node(node)
{
  id_ = total_++;
}

Port::~Port()
{
}

string Port::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

// TypeCode * Port::type()
// {
//   return _type;
// }
