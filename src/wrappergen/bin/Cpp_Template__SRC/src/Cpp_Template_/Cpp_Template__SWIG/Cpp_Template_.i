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


