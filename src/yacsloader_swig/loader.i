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

// ----------------------------------------------------------------------------
//
%define LOADERDOCSTRING
"Module to load an calculation schema from a XML file."
%enddef

%module(docstring=LOADERDOCSTRING) loader

//work around SWIG bug #1863647
#if SWIG_VERSION >= 0x010336
#define SwigPyIterator loader_PySwigIterator
#else
#define PySwigIterator loader_PySwigIterator
#endif

%feature("autodoc", "1");

#ifdef DOXYGEN_IS_OK
%include docyacsloader.i
#endif

%include "engtypemaps.i"

// ----------------------------------------------------------------------------

%{
#include "parsers.hxx"
#include "LoadState.hxx"
#include "TypeCode.hxx"
%}

%types(YACS::ENGINE::Node *);
%types(YACS::ENGINE::InputPort *,YACS::ENGINE::OutputPort *,YACS::ENGINE::InputDataStreamPort *,YACS::ENGINE::OutputDataStreamPort *);
%types(YACS::ENGINE::InGate *,YACS::ENGINE::OutGate *,YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *,YACS::ENGINE::Port *);
%types(YACS::ENGINE::Container *, YACS::ENGINE::HomogeneousPoolContainer *);

%import "pilot.i"

/*
 * Ownership section
 */
//Take ownership : it is not the default (constructor) as it is a factory
%newobject YACS::YACSLoader::load;
/*
 * End of Ownership section
 */

%include "YACSloaderExport.hxx"
%include "parsers.hxx"
%import "xmlParserBase.hxx"
%include "LoadState.hxx"

