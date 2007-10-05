#ifndef __PYTHONXMLCONV_HXX__
#define __PYTHONXMLCONV_HXX__

#include <Python.h>
#include "XMLPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // Adaptator Ports Python->Xml for several types

    class PyXml : public ProxyPort
    {
    public:
      PyXml(InputXmlPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };
  }
}
#endif
