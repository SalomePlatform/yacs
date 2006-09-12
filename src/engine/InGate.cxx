#include "InGate.hxx"
#include "Node.hxx"

using namespace YACS::ENGINE;

const char InGate::NAME[]="InGate";

InGate::InGate(Node *node):Port(node),_nbPrecursor(0),_nbPrecursorDone(0),_colour(YACS::White)
{
}

std::string InGate::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void InGate::exNotifyFromPrecursor()
{
  _nbPrecursorDone++;
  if(exIsReady() && _node)
    _node->exUpdateState();
}

void InGate::edAppendPrecursor()
{
  _nbPrecursor++;
}

void InGate::edRemovePrecursor()
{
  _nbPrecursor--;
}

void InGate::edSet(int nbOfPrecursors)
{
  _nbPrecursor=nbOfPrecursors;
}

void InGate::exReset()
{
  _nbPrecursorDone=0;
}

bool InGate::exIsReady() const
{
  return _nbPrecursor==_nbPrecursorDone;
}
