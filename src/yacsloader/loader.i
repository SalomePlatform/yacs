// ----------------------------------------------------------------------------
%define LOADERDOCSTRING
"Loader docstring
loads an XML file."
%enddef

%module(docstring=LOADERDOCSTRING) loader

%feature("autodoc", "0");

%include std_string.i

// ----------------------------------------------------------------------------

%{
#include <stdexcept>
#include <iostream>
#include <fstream>

#include "Proc.hxx"
#include "Exception.hxx"
#include "parsers.hxx"
#include "LoadState.hxx"

using namespace YACS::ENGINE;
using namespace std;

%}

%exception load {
   try {
      $action
   } catch(YACS::Exception& _e) {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   } catch(std::invalid_argument& _e) {
      PyErr_SetString(PyExc_IOError ,_e.what());
      return NULL;
   }
}

namespace YACS
{
  class YACSLoader
  {
    public:
        YACSLoader();
%feature("autodoc", "load(self, file) -> YACS::ENGINE::Proc");
%feature("docstring") "loads a XML file which name is given by file argument" ;
        virtual YACS::ENGINE::Proc* load(char *);
  };
}

class xmlParserBase
{
};

namespace YACS
{
  namespace ENGINE
  {
    class stateParser: public xmlParserBase
    {
    };

    class stateLoader: public xmlReader
    {
    public:
      stateLoader(xmlParserBase* parser,
                  YACS::ENGINE::Proc* p);
      virtual void parse(std::string xmlState);
    };
  }
}
