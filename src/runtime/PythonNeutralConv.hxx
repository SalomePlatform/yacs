#ifndef __PYTHONNEUTRALCONV_HXX__
#define __PYTHONNEUTRALCONV_HXX__

#include <Python.h>
#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    //Proxy port to adapt Neutral port to Python port

    class PyNeutral : public ProxyPort
    {
    public:
      PyNeutral(InputPort* p);
      virtual void put(const void *data)  throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };
  }
}

#endif
