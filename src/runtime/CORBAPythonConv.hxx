#ifndef __CORBAPYTHONCONV_HXX__
#define __CORBAPYTHONCONV_HXX__

#include <omniORB4/CORBA.h>

#include "PythonPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {

    // --- convertisseurs Corba->Python pour les différents types

    class CorbaPyDouble : public ProxyPort
    {
    public:
      CorbaPyDouble(InputPyPort* p)
	: ProxyPort(p), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

    class CorbaPyInt : public ProxyPort
    {
    public:
      CorbaPyInt(InputPyPort* p)
	: ProxyPort(p), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

    class CorbaPyString : public ProxyPort
    {
    public:
      CorbaPyString(InputPyPort* p)
	: ProxyPort(p), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

    class CorbaPyObjref : public ProxyPort
    {
    public:
      CorbaPyObjref(InputPyPort* p)
	: ProxyPort(p), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

    class CorbaPySequence : public ProxyPort
    {
    public:
      CorbaPySequence(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    protected:
      DynamicAny::DynAnyFactory_ptr _dynfactory;
    };

  }
}
#endif
