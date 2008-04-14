#ifndef __NEUTRALPYTHONCONV_HXX__
#define __NEUTRALPYTHONCONV_HXX__

#include "PythonPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // --- adaptator ports Neutral->Python for several types

    class NeutralPyDouble : public ProxyPort
    {
    public:
      NeutralPyDouble(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPyInt : public ProxyPort
    {
    public:
      NeutralPyInt(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPyString : public ProxyPort
    {
    public:
      NeutralPyString(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPyBool : public ProxyPort
    {
    public:
      NeutralPyBool(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPyObjref : public ProxyPort
    {
    public:
      NeutralPyObjref(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralPySequence : public ProxyPort
    {
    public:
      NeutralPySequence(InputPyPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };
  }
}
#endif
