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

#ifndef __SALOMEDISPATCHER_HXX__
#define __SALOMEDISPATCHER_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "Dispatcher.hxx"
#include "yacsgui.hh"
#include "Thread.hxx"
#include "Semaphore.hxx"

#include <string>
#include <list>
#include <map>
#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class YACSRUNTIMESALOME_EXPORT SALOMEDispatcher: public Dispatcher
    {
    public:
      SALOMEDispatcher();
      void dispatch(Node* object, const std::string& event);
      void addObserver(YACS_ORB::Observer_ptr observer,int numid, std::string event);
      static void setSALOMEDispatcher();
      static SALOMEDispatcher* getSALOMEDispatcher();
      virtual ~SALOMEDispatcher();
    protected:
      static std::map< std::pair<int,std::string> , std::set<YACS_ORB::Observer_ptr> > _observers;
      static SALOMEDispatcher* _disp;
      static void *ThDisp( void *a);
      static YACS::BASES::Semaphore _s1;
      static std::list< std::pair<Node*,std::string> > _listOfEvents;
    };


  }
}

#endif
