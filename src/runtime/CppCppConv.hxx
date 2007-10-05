#ifndef CPPCPPCONV_HXX_
#define CPPCPPCONV_HXX_

#include "CppPorts.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputCppPort;
/*! \brief Class for conversion from C++ Output port to C++ Input port
 *
 * \ingroup Ports
 *
 */
    class CppCpp : public ProxyPort
    {
    public:
      CppCpp(InputCppPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(Any *data) throw(ConversionException);
    };

  }
}



#endif /*CPPCPPCONV_HXX_*/
