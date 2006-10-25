#include "OutGate.hxx"
#include "InGate.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char OutGate::NAME[]="OutGate";

OutGate::OutGate(Node *node):Port(node)
{
}

string OutGate::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void OutGate::exNotifyDone()
{
  for(set<InGate *>::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter)->exNotifyFromPrecursor();
}

bool OutGate::edAddInGate(InGate *inGate)
{
  if(!isAlreadyInSet(inGate))
    {
      inGate->edAppendPrecursor();
      _setOfInGate.insert(inGate);
      return true;
    }
  else
    return false;
}

set<InGate *> OutGate::edSetInGate() const
{
  return _setOfInGate;
}

void OutGate::edRemoveInGate(InGate *inGate) throw(Exception)
{
  bool found=false;
  for(set<InGate *>::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end() && !found;iter++)
    if((*iter)==inGate)
      {
	_setOfInGate.erase(iter);
	inGate->edRemovePrecursor();
	found=true;
      }
  if(!found)
    throw Exception("InGate not already connected to OutGate");
}

//Idem OutGate::edRemoveInGateOneWay except that no exception thrown if CF not exists
void OutGate::edRemoveInGateOneWay(InGate *inGate)
{
  bool found=false;
  for(set<InGate *>::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end() && !found;iter++)
    if((*iter)==inGate)
      {
	_setOfInGate.erase(iter);
	inGate->edRemovePrecursor();
	found=true;
      }
}

bool OutGate::isAlreadyInSet(InGate *inGate) const
{
  bool ret=false;
  for(set<InGate *>::const_iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end() && !ret;iter++)
    if((*iter)==inGate)
      ret=true;
  return ret;
}

int OutGate::getNbOfInGatesConnected() const
{
  return _setOfInGate.size();
}
