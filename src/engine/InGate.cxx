// Copyright (C) 2006-2015  CEA/DEN, EDF R&D
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
  for(map<OutGate *, bool >::iterator iter=_backLinks.begin();iter!=_backLinks.end();iter++)
    ((*iter).first)->edRemoveInGate(this,false);
  _backLinks.clear();
}

//! Notify this port that an upstream node connected by a control flow link is finished
/*!
 *  Calls the node's gate method : Node::exUpdateState
 *
 *  Called by OutGate::exNotifyDone
 */
void InGate::exNotifyFromPrecursor(OutGate *from)
{
  DEBTRACE("InGate::exNotifyFromPrecursor");
  map< OutGate *, bool >::iterator iter=_backLinks.find(from);
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
  if(_node) _node->exFailedState();
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
  _backLinks[from]=false;
}

void InGate::edRemovePrecursor(OutGate *from)
{
  _backLinks.erase(from);
}

int InGate::getNumberOfBackLinks() const
{
  return _backLinks.size();
}

void InGate::exReset()
{
  for(map<OutGate *, bool >::iterator iter=_backLinks.begin();iter!=_backLinks.end();iter++)
    (*iter).second=false;
}

bool InGate::exIsReady() const
{
  bool isReady=true;
  for(map<OutGate *, bool >::const_iterator iter=_backLinks.begin();iter!=_backLinks.end() && isReady;iter++)
    isReady=(*iter).second;
  return isReady;
}

std::list<OutGate *> InGate::getBackLinks()
{
  list<OutGate *> listo;
  for(map<OutGate *, bool >::iterator iter=_backLinks.begin();iter!=_backLinks.end();iter++)
    listo.push_back(iter->first);
  return listo;  
}

void InGate::setPrecursorDone(OutGate *from)
{
  map< OutGate *, bool >::iterator iter=_backLinks.find(from);
  (*iter).second=true;  
}
