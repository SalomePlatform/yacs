#ifndef __TYPECHECKERDATASTREAM_HXX__
#define __TYPECHECKERDATASTREAM_HXX__

#include "define.hxx"
#include "Exception.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class TypeCheckerDataStream
    {
    public:
      static bool areStaticallyCompatible(YACS::StreamType type1, YACS::StreamType type2) throw(Exception);
      static std::string edGetTypeInPrintableForm(YACS::StreamType type);
    private:
      static const char UNRECOGNIZEDTYPENAME[];
    };
  }
}

#endif
