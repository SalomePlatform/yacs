
#ifndef _XMLPORTS_HXX_
#define _XMLPORTS_HXX_

#include "InputPort.hxx"
#include "OutputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputXmlPort : public InputPort
    {
    public:
      InputXmlPort(const std::string& name, Node* node, TypeCode * type);
      virtual void put(const void *data) throw (ConversionException);
      void put(const char *data) throw (ConversionException);
      virtual const char * getXml() const;
    protected:
      std::string _data;
    };
    
    class OutputXmlPort : public OutputPort
    {
    public:
      OutputXmlPort(const std::string& name,  Node* node, TypeCode * type);
      virtual void put(const void *data) throw (ConversionException);
      void put(const char *data) throw (ConversionException);
      virtual const char * get() const throw (ConversionException);
    protected:
      std::string _data;
    };

  }
}

#endif
