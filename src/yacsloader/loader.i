// ----------------------------------------------------------------------------
%define LOADERDOCSTRING
"Loader docstring
loads an XML file."
%enddef

%module(docstring=LOADERDOCSTRING) loader

%feature("autodoc", "1");

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
#include "ProcCataLoader.hxx"

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

/*
 * Ownership section
 */
//Take ownership : it is not the default (constructor) as it is a factory
%newobject YACS::YACSLoader::load;
/*
 * End of Ownership section
 */

%ignore parser;

%include "parsers.hxx"
%import "xmlParserBase.hxx"
%include "LoadState.hxx"
%import "Catalog.hxx"
%include "ProcCataLoader.hxx"

/*
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
*/
