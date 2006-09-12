#include "OutGate.hxx"
#include "InGate.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char OutGate::NAME[]="OutGate";

OutGate::OutGate(Node *node):Port(node)
{
}

std::string OutGate::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void OutGate::exNotifyDone()
{
  for(list<InGate *>::iterator iter=_listOfInGate.begin();iter!=_listOfInGate.end();iter++)
    (*iter)->exNotifyFromPrecursor();
}

bool OutGate::edAddInGate(InGate *inGate)
{
  if(!isAlreadyInList(inGate))
    {
      inGate->edAppendPrecursor();
      _listOfInGate.push_back(inGate);
      return true;
    }
  else
    return false;
}

std::list<InGate *> OutGate::edListInGate() const
{
  return _listOfInGate;
}

void OutGate::edRemoveInGate(InGate *inGate) throw(Exception)
{
  bool found=false;
  for(list<InGate *>::iterator iter=_listOfInGate.begin();iter!=_listOfInGate.end() && !found;iter++)
    if((*iter)==inGate)
      {
	_listOfInGate.erase(iter);
	inGate->edRemovePrecursor();
	found=true;
      }
  if(!found)
    throw Exception("InGate not already connected to OutGate");
}

bool OutGate::isAlreadyInList(InGate *inGate) const
{
  bool ret=false;
  for(list<InGate *>::const_iterator iter=_listOfInGate.begin();iter!=_listOfInGate.end() && !ret;iter++)
    if((*iter)==inGate)
      ret=true;
  return ret;
}

int OutGate::getNbOfInGatesConnected() const
{
  return _listOfInGate.size();
}
