// Copyright (C) 2012-2024  CEA, EDF
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
// Author : Anthony Geay (EDF R&D)

#ifndef __YACSEVALSESSIONINTERNAL_HXX__
#define __YACSEVALSESSIONINTERNAL_HXX__

#include "YACSEvalSession.hxx"

#include "SALOMEconfig.h"
#include CORBA_CLIENT_HEADER(SALOME_Launcher)
#include "omniORB4/CORBA.h"

#include <string>

class YACSEvalSession::YACSEvalSessionInternal
{
public:
  YACSEvalSessionInternal();
  CORBA::Object_var string_to_object(const std::string& ior) { return _orb->string_to_object(ior.c_str()); }
  Engines::SalomeLauncher_var goFetchingSalomeLauncherInNS();
  Engines::SalomeLauncher_var getNotNullSL();
private:
  void checkSalomeLauncher();
private:
  CORBA::ORB_var _orb;
  Engines::SalomeLauncher_var _sl;
};

#endif
