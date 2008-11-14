#include "ConversionException.hxx"

using namespace YACS::ENGINE;

const char ConversionException::TYPEOFEXCEPTION[]="Conversion between types failed : ";


ConversionException::ConversionException(const std::string& what,bool prefix):Exception("")
{
  if(prefix)
    _what=TYPEOFEXCEPTION;
  _what+=what;
}

ConversionException::~ConversionException() throw ()
{
}
