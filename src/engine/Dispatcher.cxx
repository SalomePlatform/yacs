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

#include "Dispatcher.hxx"
#include "Node.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

Observer::~Observer()
{
}

void Observer::notifyObserver(Node* object, const std::string& event)
{
  DEBTRACE("notifyObserver " << event << object );
}

void Observer::notifyObserver2(Node* object,const std::string& event, void *something)
{
  DEBTRACE("notifyObserver2 " << event << object );
}

Dispatcher* Dispatcher::_singleton = 0;

Dispatcher::~Dispatcher()
{
  Dispatcher::_singleton =0;
}

Dispatcher* Dispatcher::getDispatcher()
{
  if ( !Dispatcher::_singleton )
    {
       Dispatcher::_singleton =new Dispatcher;
    }
  return Dispatcher::_singleton;
}

void Dispatcher::setDispatcher(Dispatcher* dispatcher)
{
  Dispatcher::_singleton=dispatcher;
}

void Dispatcher::printObservers()
{
  std::cerr << "Dispatcher::printObservers " << std::endl;
  typedef std::map< std::pair<Node*,std::string> , std::set<Observer*> >::iterator it;
  typedef std::set<Observer*>::iterator jt;

  for(it i=_observers.begin();i!=_observers.end();i++)
    {
      std::cerr << "Node*: " << (*i).first.first << " event: " << (*i).first.second << std::endl;
      for(jt j=(*i).second.begin();j!=(*i).second.end();j++)
        {
          std::cerr << "observer: " << *j << std::endl;
        }
    }
}

void Dispatcher::dispatch(Node* object, const std::string& event)
{
  DEBTRACE("Dispatcher::dispatch " << event );
  std::pair<Node*,std::string> key(object,event);
  std::map< std::pair<Node*,std::string> , std::set<Observer*> >::const_iterator it(_observers.find(key));
  if(it!=_observers.end())
    {
      for(std::set<Observer*>::const_iterator iter=(*it).second.begin();iter!=(*it).second.end();iter++)
        {
          (*iter)->notifyObserver(object,event);
        }
    }
}

void Dispatcher::dispatch2(Node* object,const std::string& event, void *something)
{
  std::pair<Node*,std::string> key(object,event);
  std::map< std::pair<Node*,std::string> , std::set<Observer*> >::const_iterator it(_observers.find(key));
  if(it!=_observers.end())
    {
      for(std::set<Observer*>::const_iterator iter=(*it).second.begin();iter!=(*it).second.end();iter++)
        {
          (*iter)->notifyObserver2(object,event,something);
        }
    }
}

void Dispatcher::addObserver(Observer* observer,Node* object, const std::string& event)
{
  _observers[std::pair<Node*,std::string>(object,event)].insert(observer);
//  printObservers();
}

void Dispatcher::removeObserver(Observer* observer,Node* object, const std::string& event)
{
  _observers[std::pair<Node*,std::string>(object,event)].erase(observer);
//  printObservers();
}
