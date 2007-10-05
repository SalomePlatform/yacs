#ifndef __PYTHONCPPCONV_HXX__
#define __PYTHONCPPCONV_HXX__

#include <Python.h>
#include "CppPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    //Proxy port to adapt C++ port to Python port

    class PyCpp : public ProxyPort
    {
    public:
      PyCpp(InputCppPort* p)
         : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data)  throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };
    int isAdaptableCppPyObject(const TypeCode *t1, const TypeCode *t2);
  }
}

#endif
