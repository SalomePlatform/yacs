//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
%define DOCSTRING
"YACSGui_Swig docstring
All is needed to create and display a calculation schema from proc
pointer in Python console."
%enddef

%module(docstring=DOCSTRING) YACSGui_Swig

%{
#include "YACSGui_Swig.h"
#include "Proc.hxx"

using namespace YACS::ENGINE;
%}

class YACSGui_Swig
{
 public:
  YACSGui_Swig();
  ~YACSGui_Swig();

  void displayGraph(YACS::ENGINE::Proc* theProc);
};
