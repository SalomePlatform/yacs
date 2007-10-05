#ifndef __CORBAPYTHONCONV_HXX__
#define __CORBAPYTHONCONV_HXX__

#include <omniORB4/CORBA.h>
#include "InputPort.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputPyPort;

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyDouble : public ProxyPort
    {
    public:
      CorbaPyDouble(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyInt : public ProxyPort
    {
    public:
      CorbaPyInt(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyString : public ProxyPort
    {
    public:
      CorbaPyString(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * Convert boolean data
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyBool : public ProxyPort
    {
    public:
      CorbaPyBool(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyObjref : public ProxyPort
    {
    public:
      CorbaPyObjref(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPySequence : public ProxyPort
    {
    public:
      CorbaPySequence(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    protected:
      DynamicAny::DynAnyFactory_ptr _dynfactory;
    };

/*! \brief Class for conversion of struct objects from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyStruct : public ProxyPort
    {
    public:
      CorbaPyStruct(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

  }
}
#endif
