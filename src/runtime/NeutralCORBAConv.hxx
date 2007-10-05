#ifndef __NEUTRALCORBACONV_HXX__
#define __NEUTRALCORBACONV_HXX__

#include "ConversionException.hxx"
#include "CORBAPorts.hxx"
#include "Any.hxx"

namespace YACS
{
  namespace ENGINE
  {

    // --- adaptator ports Neutral->Corba for several types

    class NeutralCorbaInt : public ProxyPort
    {
    public:
      NeutralCorbaInt(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralCorbaDouble : public ProxyPort
    {
    public:
      NeutralCorbaDouble(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralCorbaBool : public ProxyPort
    {
    public:
      NeutralCorbaBool(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralCorbaSequence : public ProxyPort
    {
    public:
      NeutralCorbaSequence(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralCorbaString : public ProxyPort
    {
    public:
      NeutralCorbaString(InputCorbaPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };

    class NeutralCorbaObjref : public ProxyPort
    {
    public:
      NeutralCorbaObjref(InputCorbaPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };
  }
}
#endif
