// Copyright (C) 2012-2016  CEA/DEN, EDF R&D
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

#include "YACSEvalSessionInternal.hxx"
#include "YACSEvalSession.hxx"

#include "PyStdout.hxx"
#include "AutoGIL.hxx"
#include "Exception.hxx"

YACSEvalSession::YACSEvalSessionInternal::YACSEvalSessionInternal():_orb(CORBA::ORB::_nil()),_sl(Engines::SalomeLauncher::_nil())
{
}

void YACSEvalSession::YACSEvalSessionInternal::checkSalomeLauncher()
{
  if(CORBA::is_nil(_sl))
    throw YACS::Exception("YACSEvalSessionInternal::checkSalomeLauncher : salome launcher is null !");
}

Engines::SalomeLauncher_var YACSEvalSession::YACSEvalSessionInternal::goFetchingSalomeLauncherInNS()
{
  if(!CORBA::is_nil(_sl))
    return _sl;
  int argc(0);
  _orb=CORBA::ORB_init(argc,0);
  if(CORBA::is_nil(_orb))
    throw YACS::Exception("YACSEvalSessionInternal contrctor : ORB is null !");
  //
  const char methName[]="goFetchingSalomeLauncherInNS";
  const char fetchPyCmd[]="import salome,CORBA\nsalome.salome_init()\nsl=salome.naming_service.Resolve(\"/SalomeLauncher\")\nif not CORBA.is_nil(sl):\n  return salome.orb.object_to_string(sl)\nelse:\n  raise Exception(\"Impossible to locate salome launcher !\")";
  YACS::ENGINE::AutoPyRef func(YACS::ENGINE::evalPy(methName,fetchPyCmd));
  YACS::ENGINE::AutoPyRef val(YACS::ENGINE::evalFuncPyWithNoParams(func));
  std::string ior(PyString_AsString(val));
  CORBA::Object_var obj(string_to_object(ior));
  if(CORBA::is_nil(obj))
    throw YACS::Exception("goFetchingSalomeLauncherInNS : fetched ior is NIL !");
  _sl=Engines::SalomeLauncher::_narrow(obj);
  checkSalomeLauncher();
  return _sl;
}

Engines::SalomeLauncher_var YACSEvalSession::YACSEvalSessionInternal::getNotNullSL()
{
  checkSalomeLauncher();
  return _sl;
}
