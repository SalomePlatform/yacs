#include "OutGate.hxx"
#include "InGate.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

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

void OutGate::exReset()
{
  for(map<InGate *, bool>::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter).second=false;
}

//! Notify this port that its node is finished
/*!
 *  Calls (notify) all the connected ingates : InGate::exNotifyFromPrecursor
 *
 *  Called by Bloc::updateStateOnFinishedEventFrom
 */

void OutGate::exNotifyDone()
{
  DEBTRACE("OutGate::exNotifyDone");
  for(map<InGate *, bool>::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter).first->exNotifyFromPrecursor(this);
}

//! Notify this port that its node has failed
/*!
 *
 */
void OutGate::exNotifyFailed()
{
  for(map<InGate *, bool>::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter).first->exNotifyFailed();
}

//! Notify this port that its node has been disabled
/*!
 *
 */
void OutGate::exNotifyDisabled()
{
  for(map<InGate *, bool>::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter).first->exNotifyDisabled();
}

void OutGate::edDisconnectAllLinksFromMe()
{
  for(map<InGate *, bool>::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter).first->edRemovePrecursor(this);
  _setOfInGate.clear();
}

bool OutGate::edAddInGate(InGate *inGate)
{
  if(!isAlreadyInSet(inGate))
    {
      inGate->edAppendPrecursor(this);
      _setOfInGate[inGate]=false;
      modified();
      inGate->modified();
      return true;
    }
  else
    return false;
}

std::set<InGate *> OutGate::edSetInGate() const
{
  set<InGate *> ret;
  for(map<InGate *, bool>::const_iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    ret.insert((*iter).first);
  return ret;
}

void OutGate::edRemoveInGate(InGate *inGate, bool coherenceWithInGate) throw(Exception)
{
  map<InGate *, bool>::iterator iter;
  for(iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    if((*iter).first==inGate)
      {
        _setOfInGate.erase(iter);
        if(coherenceWithInGate)
          inGate->edRemovePrecursor(this);
        modified();
        inGate->modified();
        break;
      }
  if(iter==_setOfInGate.end())
    throw Exception("InGate not already connected to OutGate");
}

//Idem OutGate::edRemoveInGateOneWay except that no exception thrown if CF not exists
void OutGate::edRemoveInGateOneWay(InGate *inGate)
{
  bool found=false;
  for(map<InGate *, bool>::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end() && !found;iter++)
    if((*iter).first==inGate)
      {
        _setOfInGate.erase(iter);
        inGate->edRemovePrecursor(this);
        found=true;
        modified();
        inGate->modified();
      }
}

bool OutGate::isAlreadyInSet(InGate *inGate) const
{
  return _setOfInGate.find(inGate)!=_setOfInGate.end();
}

int OutGate::getNbOfInGatesConnected() const
{
  return _setOfInGate.size();
}
