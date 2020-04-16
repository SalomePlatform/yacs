// Copyright (C) 2006-2020  CEA/DEN, EDF R&D
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

%module Cpp_Template_SWIG

%{
#include <stdlib.h>
#include "Cpp_Template_.hxx"
%}

/*
   Initialisation block due to the LocalTraceCollector mechanism

 */

%init %{
    if (getenv("SALOME_trace") == NULL )
	setenv("SALOME_trace","local",0);
%}

%include "std_vector.i"
%include "std_string.i"
%include "Cpp_Template_.hxx"


