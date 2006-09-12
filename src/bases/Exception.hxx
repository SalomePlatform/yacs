#ifndef __EXCEPTION_HXX__
#define __EXCEPTION_HXX__

#include <string>
#include <exception>

namespace YACS
{
  class Exception : public std::exception
  {
  protected:
    std::string _what;
  public:
    Exception(const std::string& what);
    const char *what( void ) const throw ();
    ~Exception() throw ();
  };
}

#endif
