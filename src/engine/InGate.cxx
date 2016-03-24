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

#include "InGate.hxx"
#include "Node.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <algorithm>

using namespace YACS::ENGINE;
using namespace std;

const char InGate::NAME[]="InGate";

InGate::InGate(Node *node):Port(node)
{
}

InGate::~InGate()
{
}

string InGate::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void InGate::edDisconnectAllLinksToMe()
{
  for(list< std::pair<OutGate *, bool> >::iterator iter=_backLinks.begin();iter!=_backLinks.end();iter++)
    ((*iter).first)->edRemoveInGate(this,false);
  _backLinks.clear();
}

class ItemCmp
{
private:
  OutGate *_itf;
public:
  ItemCmp(OutGate *itf):_itf(itf) { }
  bool operator()(const std::pair<OutGate *,bool>& elt) const { return elt.first==_itf; }
};

//! Notify this port that an upstream node connected by a control flow link is finished
/*!
 *  Calls the node's gate method : Node::exUpdateState
 *
 *  Called by OutGate::exNotifyDone
 */
void InGate::exNotifyFromPrecursor(OutGate *from)
{
  DEBTRACE("InGate::exNotifyFromPrecursor");
  list< pair<OutGate *, bool> >::iterator iter(std::find_if(_backLinks.begin(),_backLinks.end(),ItemCmp(from)));
  if(iter==_backLinks.end())
    throw YACS::Exception("InGate::exNotifyFromPrecursor : precursor not found !");
  (*iter).second=true;
  if(exIsReady())
    _node->exUpdateState();
}

//! Notify this port that an upstream node connected by a control flow link has failed
/*!
 *
 */
void InGate::exNotifyFailed()
{
  if(_node)
    _node->exFailedState();
}

//! Notify this port that an upstream node connected by a control flow link has been disabled
/*!
 *
 */
void InGate::exNotifyDisabled()
{
  if(_node)
    _node->exDisabledState();
}

void InGate::edAppendPrecursor(OutGate *from)
{
  list< pair<OutGate *, bool> >::iterator iter(std::find_if(_backLinks.begin(),_backLinks.end(),ItemCmp(from)));
  if(iter!=_backLinks.end())
    (*iter).second=false;
  else
    _backLinks.push_back(pair<OutGate *, bool>(from,false));
}

void InGate::edRemovePrecursor(OutGate *from)
{
  list< pair<OutGate *, bool> >::iterator iter(std::find_if(_backLinks.begin(),_backLinks.end(),ItemCmp(from)));
  if(iter!=_backLinks.end())
    _backLinks.erase(iter);
}

int InGate::getNumberOfBackLinks() const
{
  return _backLinks.size();
}

void InGate::exReset()
{
  for(list< std::pair<OutGate *, bool> >::iterator iter=_backLinks.begin();iter!=_backLinks.end();iter++)
    (*iter).second=false;
}

bool InGate::exIsReady() const
{
  bool isReady(true);
  for(list< std::pair<OutGate *, bool> >::const_iterator iter=_backLinks.begin();iter!=_backLinks.end() && isReady;iter++)
    isReady=(*iter).second;
  return isReady;
}

std::list<OutGate *> InGate::getBackLinks()
{
  list<OutGate *> listo;
  for(list< std::pair<OutGate *, bool> >::const_iterator iter=_backLinks.begin();iter!=_backLinks.end();iter++)
    listo.push_back(iter->first);
  return listo;
}

void InGate::setPrecursorDone(OutGate *from)
{
  list< std::pair<OutGate *, bool> >::iterator iter(std::find_if(_backLinks.begin(),_backLinks.end(),ItemCmp(from)));
  if(iter!=_backLinks.end())
    (*iter).second=true;
  else
    throw YACS::Exception("InGate::setPrecursorDone : precursor not found !");
}
