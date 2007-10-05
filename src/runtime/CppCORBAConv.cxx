
#include "TypeConversions.hxx"
#include "CppCORBAConv.hxx"
#include "RuntimeSALOME.hxx"

#include <iostream>
#include <sstream>

using namespace std;

namespace YACS
{
  namespace ENGINE
  {
    CORBA::Any *convertCppCorba(const TypeCode *t,Any *data)
    {
      return convertNeutralCorba(t, data);
    }

    void CppCorba::put(const void *data)  throw(ConversionException)
    {
      put((Any *)data);
    }

    //!Convert a C++ object into CORBA::Any object
    /*!
     *   \param data : python object
     */

    void CppCorba::put(Any *data)  throw(ConversionException)
    {
      CORBA::Any* a = convertCppCorba(edGetType(), data);
      _port->put(a);
      //delete Any that has been allocated by convertCppCorba
      delete a;
    }

    int isAdaptableCorbaCpp(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableCorbaNeutral(t1, t2);
    }
  }
}
