#ifndef __PYTHONINITCONV_HXX__
#define __PYTHONINITCONV_HXX__

#include <Python.h>
#include "PythonPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // Adaptator Port for initialization of python ports with python objects
    // need to check value type and eventually convert it

    class PyInit : public ProxyPort
    {
    public:
      PyInit(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };
  }
}
#endif
