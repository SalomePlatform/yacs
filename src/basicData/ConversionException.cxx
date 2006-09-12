#include "ConversionException.hxx"

using namespace YACS::ENGINE;

const char ConversionException::TYPEOFEXCEPTION[]="Conversion between types failed : ";


ConversionException::ConversionException(const std::string& content, const std::string& expectedType):Exception(TYPEOFEXCEPTION)
{
  _what=TYPEOFEXCEPTION;
  _what+=content;
  _what+=" -> Expecting type ";
  _what+=expectedType;
}
