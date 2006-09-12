#include "Exception.hxx"

using namespace YACS;

Exception::Exception(const std::string& what):_what(what)
{
}

const char *Exception::what( void ) const throw ()
{
  return _what.c_str();
}

Exception::~Exception() throw ()
{
}
