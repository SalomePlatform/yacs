%define PYPILOTDOCSTRING
"This module provides 2 classes PyObserver and PyCatalogLoader that can be subclassed in Python (uses director feature)"
%enddef

%module(directors="1",docstring=PYPILOTDOCSTRING) pypilot

%feature("autodoc", "1");

%include "engtypemaps.i"

%{
#include "Dispatcher.hxx"
#include "Catalog.hxx"
#include "TypeCode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class PyObserver:public Observer
    {
    public:
      virtual void notifyObserver(Node* object,const std::string& event)
      {
        //YACS engine processing is pure C++ : need to take the GIL
        PyGILState_STATE gstate = PyGILState_Ensure();

        try
          {
            pynotify(object,event);
            if (PyErr_Occurred()) 
              {
                // print the exception and clear it
                PyErr_Print();
                //do not propagate the python exception (ignore it)
              }
          }
        catch (...)
          {
            //ignore this exception:probably Swig::DirectorException
            if (PyErr_Occurred()) 
                PyErr_Print();
          }
        PyGILState_Release(gstate);
      }
      virtual void pynotify(Node* object,const std::string& event)
      {
        //To inherit (Python class)
        std::cerr << "pynotify " << event << object << std::endl;
      }
    };

    class PyCatalogLoader:public CatalogLoader
    {
    protected:
      virtual CatalogLoader* newLoader(const std::string& path) {return 0;}
    };
  }
}

%}

%init
%{
  // init section
#ifdef OMNIORB
  PyObject* omnipy = PyImport_ImportModule((char*)"_omnipy");
  if (!omnipy)
  {
    PyErr_SetString(PyExc_ImportError,(char*)"Cannot import _omnipy");
    return;
  }
  PyObject* pyapi = PyObject_GetAttrString(omnipy, (char*)"API");
  api = (omniORBPYAPI*)PyCObject_AsVoidPtr(pyapi);
  Py_DECREF(pyapi);
#endif
%}

/*
 * Director section : classes that can be subclassed in python
 */
%feature("director") YACS::ENGINE::PyObserver;
%feature("nodirector") YACS::ENGINE::PyObserver::notifyObserver;
%feature("director") YACS::ENGINE::PyCatalogLoader;
%feature("nodirector") YACS::ENGINE::PyCatalogLoader::newLoader;

/*
%feature("director:except") {
  if ($error != NULL) {
    // print the exception and clear it
    PyErr_Print();
    //do not propagate the python exception (ignore it)
    //throw Swig::DirectorMethodException();
  }
}
*/
/*
 * End of Director section
 */

%types(YACS::ENGINE::Node *,YACS::ENGINE::Proc *,YACS::ENGINE::ForLoop *,YACS::ENGINE::Bloc *, YACS::ENGINE::Switch *);
%types(YACS::ENGINE::WhileLoop *,YACS::ENGINE::ForEachLoop *,YACS::ENGINE::ComposedNode *,YACS::ENGINE::InlineNode *);
%types(YACS::ENGINE::InlineFuncNode *,YACS::ENGINE::ServiceInlineNode *,YACS::ENGINE::ServiceNode *);
%types(YACS::ENGINE::ElementaryNode *);

%types(YACS::ENGINE::InputPort *,YACS::ENGINE::OutputPort *);
%types(YACS::ENGINE::InputDataStreamPort *,YACS::ENGINE::OutputDataStreamPort *);
%types(YACS::ENGINE::InGate *,YACS::ENGINE::OutGate *);
%types(YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *,YACS::ENGINE::Port *);

%types(YACS::ENGINE::Observer *);

%import "pilot.i"
%import "Dispatcher.hxx"
%import "Catalog.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class PyObserver:public Observer
    {
    public:
      virtual void pynotify(Node* object,const std::string& event);
    };

    class PyCatalogLoader:public CatalogLoader
    {
    protected:
      virtual CatalogLoader* newLoader(const std::string& path);
    };
  }
}

