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

%define DOCSTRING
"Module that defines the YACS_Swig class to interact with the YACS GUI

To get a YACS_Swig object use ImportComponentGUI function from salome module :
  >>> yg=salome.ImportComponentGUI('YACS')"
%enddef

%module(docstring=DOCSTRING) libYACS_Swig

%{
#include "YACSGUI_Swig.hxx"

class PyAllowThreadsGuard {
   public:
    PyAllowThreadsGuard() { _save = PyEval_SaveThread(); }
    ~PyAllowThreadsGuard() { PyEval_RestoreThread(_save); }
   private:
    PyThreadState *_save;
};

%}

%include std_string.i

%ignore real_activate;
%ignore real_loadSchema;

/*
  managing C++ exception in the Python API
*/
%exception
{
  PyAllowThreadsGuard guard;
  $action
}

#ifdef DOXYGEN_IS_OK
%include doc.i 
#endif
%include "YACSGUI_Swig.hxx"
