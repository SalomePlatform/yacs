#include "Port.hxx"
#include "Node.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

const char Port::NAME[]="Port";

int Port::_total = 0;

Port::Port(Node *node):_node(node)
{
  _id = _total++;
}

Port::Port(const Port& other, Node *newHelder):_node(newHelder)
{
  _id = _total++;
}

Port::~Port()
{
}

string Port::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void Port::modified()
{
  DEBTRACE("Port::modified()");
  if(_node)
    _node->modified();
}
