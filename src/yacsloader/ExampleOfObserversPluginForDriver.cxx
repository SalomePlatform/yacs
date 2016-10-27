// Copyright (C) 2016  CEA/DEN, EDF R&D
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
//
// Author : Anthony Geay (EDF R&D)


// This is an example of plugin implementation you can write to intercept events sent by YACS engine during execution
// 1 - Build your shared library with 2 symboles defined : DefineCustomObservers and CleanUpObservers (warning respect the API !).
// 2 - At run time set YACS_DRIVER_PLUGIN_PATH in your environement to the shared library

#include "Dispatcher.hxx"
#include "ForEachLoop.hxx"

#include <iostream>
#include <sstream>

class PluginObserver : public YACS::ENGINE::Observer
{
public:
  PluginObserver(YACS::ENGINE::ForEachLoop *fe):_fe(fe) { }
private:
  void notifyObserver(YACS::ENGINE::Node* object,const std::string& event);
  void notifyObserver2(YACS::ENGINE::Node* object,const std::string& event, void *something);
private:
  YACS::ENGINE::ForEachLoop *_fe;
};

void PluginObserver::notifyObserver(YACS::ENGINE::Node* object,const std::string& event)
{// Customize here !
  std::cerr << "------------" << event << std::endl;
}

void PluginObserver::notifyObserver2(YACS::ENGINE::Node* object,const std::string& event, void *something)
{// Customize here !
  std::ostringstream oss;
  if(event=="progress_ok")
    {
      int itemOk(*reinterpret_cast<int *>(something));
      oss << event << " " << itemOk;
      std::cerr << oss.str() << std::endl;
    }
}

class PluginObserverKeeper
{
public:
  ~PluginObserverKeeper() { clean(); }
  void clean() { for(std::vector<YACS::ENGINE::Observer *>::iterator it=_observers.begin();it!=_observers.end();it++) delete *it; _observers.clear(); }
  void registerObserver(YACS::ENGINE::Observer *newObs) { _observers.push_back(newObs); }
private:
  std::vector<YACS::ENGINE::Observer *> _observers;
};

PluginObserverKeeper pok;

#include "ForEachLoop.hxx"

extern "C"
{
  void DefineCustomObservers(YACS::ENGINE::Dispatcher *disp, YACS::ENGINE::ComposedNode *rootNode, YACS::ENGINE::Executor *executor)
  {// Customize here !
    YACS::ENGINE::Node *n(rootNode->getChildByName("ForEachLoop_pyobj1"));
    YACS::ENGINE::ForEachLoop *nc(dynamic_cast<YACS::ENGINE::ForEachLoop *>(n));
    if(!nc)
      throw YACS::Exception("Expect to have a ForEach node called ForEachLoop_pyobj1 !");
    PluginObserver *myCustomObsever(new PluginObserver(nc));
    pok.registerObserver(myCustomObsever);
    disp->addObserver(myCustomObsever,nc,"progress_ok");
  }
  
  void CleanUpObservers()
  {// Customize here !
    pok.clean();
  }
}

