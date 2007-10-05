#ifndef NEUTRALCPPCONV_HXX_
#define NEUTRALCPPCONV_HXX_

#include "InputPort.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputCppPort;
/*! \brief Class for conversion from Neutral Output port to Xml Input port
 *
 * \ingroup Ports
 *
 */
    class NeutralCpp : public ProxyPort
    {
    public:
      NeutralCpp(InputCppPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };
    int isAdaptableCppNeutral(const TypeCode * t1, const TypeCode * t2);
  }
}


#endif /*NEUTRALCPPCONV_HXX_*/
