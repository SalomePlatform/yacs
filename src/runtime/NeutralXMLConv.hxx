#ifndef __NEUTRALXMLCONV_HXX__
#define __NEUTRALXMLCONV_HXX__

#include "XMLPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Any;

/*! \brief Class for conversion from  Neutral Output port to XML Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class NeutralXml : public ProxyPort
    {
    public:
      NeutralXml(InputXmlPort* p);
      virtual void put(const void *data)  throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };
  }
}

#endif
