#ifndef __NEUTRALINITCONV_HXX__
#define __NEUTRALINITCONV_HXX__

#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Any;

/*! \brief Class for initialization of a Neutral port with YACS::ENGINE::Any* object
 *
 * \ingroup AdaptorPorts
 *
 */
    class NeutralInit : public ProxyPort
    {
    public:
      NeutralInit(InputPort* p);
      virtual void put(const void *data)  throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };
  }
}

#endif
