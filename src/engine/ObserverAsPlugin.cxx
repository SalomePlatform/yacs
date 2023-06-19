// Copyright (C) 2018-2023  CEA, EDF
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

#include "ObserverAsPlugin.hxx"
#include "Dispatcher.hxx"
#include "Exception.hxx"

#include <sstream>

#ifndef WIN32
#include <dlfcn.h>
#include <stdlib.h>
#endif

void *HandleOnLoadedPlugin=nullptr;
void (*DefineCustomObservers)(YACS::ENGINE::Dispatcher *, YACS::ENGINE::ComposedNode *, YACS::ENGINE::Executor *)=nullptr;
void (*CleanUpObservers) ()=nullptr;

void YACS::ENGINE::LoadObserversPluginIfAny(ComposedNode *rootNode, Executor *executor)
{
  static const char SYMBOLE_NAME_1[]="DefineCustomObservers";
  static const char SYMBOLE_NAME_2[]="CleanUpObservers";
#ifndef WIN32
  Dispatcher *disp(Dispatcher::getDispatcher());
  if(!disp)
    throw YACS::Exception("Internal error ! No dispatcher !");
  char *yacsDriverPluginPath(getenv("YACS_DRIVER_PLUGIN_PATH"));
  if(!yacsDriverPluginPath)
    return ;
  void *handle(dlopen(yacsDriverPluginPath, RTLD_LAZY | RTLD_GLOBAL));
  if(!handle)
    {
      std::string message(dlerror());
      std::ostringstream oss; oss << "Error during load of \"" << yacsDriverPluginPath << "\" defined by the YACS_DRIVER_PLUGIN_PATH env var : " << message;
      throw YACS::Exception(oss.str());
    }
  DefineCustomObservers=(void (*)(YACS::ENGINE::Dispatcher *, YACS::ENGINE::ComposedNode *, YACS::ENGINE::Executor *))(dlsym(handle,SYMBOLE_NAME_1));
  if(!DefineCustomObservers)
    {
      std::ostringstream oss; oss << "Error during load of \"" << yacsDriverPluginPath << "\" ! Library has been correctly loaded but symbol " << SYMBOLE_NAME_1 << " does not exists !";
      throw YACS::Exception(oss.str());
    }
  CleanUpObservers=(void (*)())(dlsym(handle,SYMBOLE_NAME_2));
  if(!CleanUpObservers)
    {
      std::ostringstream oss; oss << "Error during load of \"" << yacsDriverPluginPath << "\" ! Library has been correctly loaded but symbol " << SYMBOLE_NAME_2 << " does not exists !";
      throw YACS::Exception(oss.str());
    }
  HandleOnLoadedPlugin=handle;
  DefineCustomObservers(disp,rootNode,executor);
#endif
}

void YACS::ENGINE::UnLoadObserversPluginIfAny()
{
#ifndef WIN32
  if(HandleOnLoadedPlugin)
    {
      CleanUpObservers();
      dlclose(HandleOnLoadedPlugin);
    }
#endif
}
