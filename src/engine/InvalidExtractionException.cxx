#include "InvalidExtractionException.hxx"

using namespace YACS::ENGINE;

const char InvalidExtractionException::TYPEOFEXCEPTION[]="Invalid runtime of YACS::Any struct : having ";


InvalidExtractionException::InvalidExtractionException(DynType kindExpected, DynType myKind):Exception(TYPEOFEXCEPTION)
{
  _what=TYPEOFEXCEPTION;
  _what+=TypeCode::getKindRepr(kindExpected);
  _what+=" and you want "; _what+=TypeCode::getKindRepr(myKind);
}
