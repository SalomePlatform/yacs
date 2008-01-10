// ----------------------------------------------------------------------------
%define LOADERDOCSTRING
"Module to load an calculation schema from a XML file."
%enddef

%module(docstring=LOADERDOCSTRING) loader

//work around SWIG bug #1863647
#define PySwigIterator loader_PySwigIterator

%feature("autodoc", "1");

%include "engtypemaps.i"

// ----------------------------------------------------------------------------

%{
#include "parsers.hxx"
#include "LoadState.hxx"
%}

%types(YACS::ENGINE::Node *);
%types(YACS::ENGINE::InputPort *,YACS::ENGINE::OutputPort *,YACS::ENGINE::InputDataStreamPort *,YACS::ENGINE::OutputDataStreamPort *);
%types(YACS::ENGINE::InGate *,YACS::ENGINE::OutGate *,YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *,YACS::ENGINE::Port *);

%import "pilot.i"

/*
 * Ownership section
 */
//Take ownership : it is not the default (constructor) as it is a factory
%newobject YACS::YACSLoader::load;
/*
 * End of Ownership section
 */

%include "parsers.hxx"
%import "xmlParserBase.hxx"
%include "LoadState.hxx"

