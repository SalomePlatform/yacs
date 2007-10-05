#ifndef __CORBACORBACONV_HXX__
#define __CORBACORBACONV_HXX__

#include <omniORB4/CORBA.h>
#include "InputPort.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputCorbaPort;
/*! \brief Class for conversion from CORBA Output port to CORBA Input port
 *
 * \ingroup Ports
 *
 */
    class CorbaCorba : public ProxyPort
    {
    public:
      CorbaCorba(InputCorbaPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

  }
}
#endif
