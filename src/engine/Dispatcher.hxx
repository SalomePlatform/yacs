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

#ifndef __DISPATCHER_HXX__
#define __DISPATCHER_HXX__

#include "YACSlibEngineExport.hxx"

#include <set>
#include <string>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class Executor;

/*! \brief Base class for observer in observer pattern
 *
 * When an event occurs, a registered observer is notified by
 * calling its notifyObserver method with 2 arguments : the object
 * which has emitted the event and the event type. There is no more
 * data. It's the responsability of the observer to request all needed
 * information from the emitting object.
 *
 */
    class YACSLIBENGINE_EXPORT Observer
    {
    public:
      virtual void notifyObserver(Node* object,const std::string& event);
      virtual void notifyObserver2(Node* object,const std::string& event, void *something);
      virtual ~Observer();
    };

/*! \brief Base class for dispatcher in observer pattern
 *
 * Dispatcher and Observer objects can be used to be notified about events
 * that occurs in editing or executing a calculation schema.
 *
 * When an object wants to notify an event, it calls the dispatch method
 * of the dispatcher with 2 arguments : the object reference and an event type.
 * The dispatcher which is a singleton is obtained by calling the class method
 * getDispatcher.
 * The dispatcher notifies all the registered observers by calling their notifyObserver
 * method.
 *
 * Observers can be registered by calling the addObserver method with two
 * arguments : an object reference and an event type. This observer will be
 * notify with events coming only from this object.
 *
 * Limitation : emitting objects can be only Node
 *
 */
    class YACSLIBENGINE_EXPORT Dispatcher
    {
    public:
      virtual void dispatch(Node* object,const std::string& event);
      virtual void dispatch2(Node* object,const std::string& event, void *something);
      virtual void addObserver(Observer* observer,Node* object,const std::string& event);
      virtual void removeObserver(Observer* observer,Node* object,const std::string& event);
      virtual void printObservers();
      static Dispatcher* getDispatcher();
      static void setDispatcher(Dispatcher* dispatcher);
      virtual ~Dispatcher();
    protected:
      std::map< std::pair<Node*,std::string> , std::set<Observer*> > _observers;
      static Dispatcher* _singleton;
    };

  }
}

#endif
