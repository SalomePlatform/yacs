#ifndef __PYTHONCORBACONV_HXX__
#define __PYTHONCORBACONV_HXX__

#include <Python.h>

#include "CORBAPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {

    // --- adaptator ports Python->Corba for several types

    class PyCorbaInt : public ProxyPort
    {
    public:
      PyCorbaInt(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };

    class PyCorbaDouble : public ProxyPort
    {
    public:
      PyCorbaDouble(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };

    class PyCorbaString : public ProxyPort
    {
    public:
      PyCorbaString(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };

    class PyCorbaBool : public ProxyPort
    {
    public:
      PyCorbaBool(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };

    class PyCorbaObjref : public ProxyPort
    {
    public:
      PyCorbaObjref(InputCorbaPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    protected:
      PyObject * _pyorb;
      CORBA::ORB_ptr _orb;
    };

    class PyCorbaSequence : public ProxyPort
    {
    public:
      PyCorbaSequence(InputCorbaPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };

    class PyCorbaStruct : public ProxyPort
    {
    public:
      PyCorbaStruct(InputCorbaPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
    };

  }
}
#endif
