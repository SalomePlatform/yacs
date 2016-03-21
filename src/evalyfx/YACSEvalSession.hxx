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

#ifndef __YACSEVALSESSION_HXX__
#define __YACSEVALSESSION_HXX__

#include "YACSEvalYFXExport.hxx"

#include <string>

struct _object;
typedef _object PyObject;

class YACSEvalSession
{
public:
  YACSEVALYFX_EXPORT YACSEvalSession();
  YACSEVALYFX_EXPORT ~YACSEvalSession();
  YACSEVALYFX_EXPORT void launch();
  YACSEVALYFX_EXPORT bool isLaunched() const { return _isLaunched; }
  YACSEVALYFX_EXPORT void checkLaunched() const;
  YACSEVALYFX_EXPORT int getPort() const;
  YACSEVALYFX_EXPORT std::string getCorbaConfigFileName() const;
private:
  static std::string GetPathToAdd();
public:
  static const char KERNEL_ROOT_DIR[];
  static const char CORBA_CONFIG_ENV_VAR_NAME[];
private:
  bool _isLaunched;
  int _port;
  std::string _corbaConfigFileName;
  PyObject *_salomeInstanceModule;
  PyObject *_salomeInstance;
};

#endif
