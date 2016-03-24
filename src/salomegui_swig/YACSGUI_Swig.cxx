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

#include "YACSGUI_Swig.hxx"
#include <iostream>

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"

#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"

#include "SALOME_Event.h"
#include "Yacsgui.hxx"

/*! \class YACS_Swig
 *  \brief Interface to YACS GUI
 *
 *  The YACS_Swig serves as an interface to the YACS GUI to activate it
 *  or to load a schema.
 *  It is wrapped with SWIG so it can be used from python to drive YACS GUI.
 */

YACS_Swig::YACS_Swig()
{
}

YACS_Swig::~YACS_Swig()
{
}

//! Activate a module
/*!
 * \param module the module name (default "YACS")
 */
void YACS_Swig::activate(const std::string& module)
{
  class TEvent: public SALOME_Event
  {
    YACS_Swig* _obj;
    std::string _module;
    public:
      TEvent(YACS_Swig* obj,const std::string& module) {_obj=obj;_module=module;};
      virtual void Execute() {
        _obj->real_activate(_module);
      };
  };
  ProcessVoidEvent(new TEvent(this,module));
}

void YACS_Swig::real_activate(const std::string& module)
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>(SUIT_Session::session()->activeApplication());
  if (!app) return;
  SalomeApp_Study* ActiveStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  if (!ActiveStudy) return;
  app->activateModule(QString::fromStdString(module));
}

//! Load a schema in edit mode or in run mode
/*!
 * \param filename the schema file to load
 * \param edit the loading mode. true for edition (default), false for run
 */
void YACS_Swig::loadSchema(const std::string& filename,bool edit, bool arrangeLocalNodes)
{
  class TEvent: public SALOME_Event
  {
    YACS_Swig* _obj;
    std::string fn;
    bool ed;
    bool ar;
    public:
      TEvent(YACS_Swig* obj,const std::string& filename,bool edit,bool arrangeLocalNodes) {
        _obj=obj;
        fn=filename;
        ed=edit;
        ar=arrangeLocalNodes;
      };
      virtual void Execute() {
        _obj->real_loadSchema(fn,ed,ar);
      };
  };
  ProcessVoidEvent(new TEvent(this,filename,edit,arrangeLocalNodes));
}

void YACS_Swig::real_loadSchema(const std::string& filename,bool edit, bool arrangeLocalNodes)
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>(SUIT_Session::session()->activeApplication());
  if (!app) return;
  app->activateModule( "YACS" );
  CAM_Module* module = app->module("YACS");
  Yacsgui* appMod = dynamic_cast<Yacsgui*>(module);
  if (appMod)
    appMod->loadSchema(filename,edit,arrangeLocalNodes);
}
