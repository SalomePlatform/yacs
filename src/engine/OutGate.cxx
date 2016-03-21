// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "OutGate.hxx"
#include "InGate.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <algorithm>

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
  for(list< pair< InGate *, bool> >::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
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
  for(list< pair<InGate *, bool> >::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter).first->exNotifyFromPrecursor(this);
}

//! Notify this port that its node has failed
/*!
 *
 */
void OutGate::exNotifyFailed()
{
  for(list< pair<InGate *, bool> >::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter).first->exNotifyFailed();
}

//! Notify this port that its node has been disabled
/*!
 *
 */
void OutGate::exNotifyDisabled()
{
  for(list< pair<InGate *, bool> >::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter).first->exNotifyDisabled();
}

void OutGate::edDisconnectAllLinksFromMe()
{
  for(list< pair<InGate *, bool> >::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    (*iter).first->edRemovePrecursor(this);
  _setOfInGate.clear();
}

class ItemCmp
{
private:
  InGate *_itf;
public:
  ItemCmp(InGate *itf):_itf(itf) { }
  bool operator()(const std::pair<InGate *,bool>& elt) const { return elt.first==_itf; }
};

bool OutGate::edAddInGate(InGate *inGate)
{
  if(!isAlreadyInSet(inGate))
    {
      inGate->edAppendPrecursor(this);
      _setOfInGate.push_back(std::pair<InGate *,bool>(inGate,false));
      modified();
      inGate->modified();
      return true;
    }
  else
    return false;
}

std::list<InGate *> OutGate::edSetInGate() const
{
  list<InGate *> ret;
  for(list< pair<InGate *, bool> >::const_iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end();iter++)
    ret.push_back((*iter).first);
  return ret;
}

void OutGate::edRemoveInGate(InGate *inGate, bool coherenceWithInGate) throw(YACS::Exception)
{
  std::list< pair<InGate* , bool> >::iterator iter(std::find_if(_setOfInGate.begin(),_setOfInGate.end(),ItemCmp(inGate)));
  if(iter==_setOfInGate.end())
    throw Exception("InGate not already connected to OutGate");
  else
    {
      if(coherenceWithInGate)
        inGate->edRemovePrecursor(this);
      _setOfInGate.erase(iter);
      inGate->modified();
      modified();
    }
}

//Idem OutGate::edRemoveInGate except that no exception thrown if CF not exists
void OutGate::edRemoveInGateOneWay(InGate *inGate)
{
  bool found=false;
  for(list< pair<InGate *, bool> >::iterator iter=_setOfInGate.begin();iter!=_setOfInGate.end() && !found;iter++)
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
  return find_if(_setOfInGate.begin(),_setOfInGate.end(),ItemCmp(inGate))!=_setOfInGate.end();
}

int OutGate::getNbOfInGatesConnected() const
{
  return _setOfInGate.size();
}
