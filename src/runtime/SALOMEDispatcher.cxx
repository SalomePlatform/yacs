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

#include "SALOMEDispatcher.hxx"
#include "Node.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::BASES;
using namespace YACS::ENGINE;

map< pair<int,string> , set<YACS_ORB::Observer_ptr> > SALOMEDispatcher::_observers;
SALOMEDispatcher* SALOMEDispatcher::_disp = 0;

SALOMEDispatcher::SALOMEDispatcher()
{
}

SALOMEDispatcher::~SALOMEDispatcher()
{
}

void SALOMEDispatcher::setSALOMEDispatcher()
{
  DEBTRACE("SALOMEDispatcher::setSALOMEDispatcher");
  _disp=new SALOMEDispatcher();
  YACS::ENGINE::Dispatcher::setDispatcher(_disp);
}

SALOMEDispatcher* SALOMEDispatcher::getSALOMEDispatcher()
{
  return _disp;
}

void SALOMEDispatcher::dispatch(Node* object, const std::string& event)
{
  DEBTRACE("SALOMEDispatcher::dispatch " << event << " " << object->getNumId());
  CORBA::Long numId = object->getNumId();
  typedef set<YACS_ORB::Observer_ptr>::iterator jt;
  std::pair<int,std::string> key(numId, event);
  for(jt iter = _observers[key].begin(); iter!=_observers[key].end(); iter++)
    {
      if (! CORBA::is_nil(*iter))
        {
          DEBTRACE("numId, event " << numId << " " << event );
          (*iter)->notifyObserver(numId, event.c_str());
        }
      else
        DEBTRACE("************************** dispatch on a CORBA::nil *******************************");
    }
}

void SALOMEDispatcher::addObserver(YACS_ORB::Observer_ptr observer,
                                   int numid,
                                   std::string event)
{
  _observers[std::pair<int,std::string>(numid,event)].insert(YACS_ORB::Observer::_duplicate(observer));
}
