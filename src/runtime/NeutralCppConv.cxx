#include "TypeConversions.hxx"
#include "NeutralCppConv.hxx"
#include "CppPorts.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS 
{
  namespace ENGINE
  {
    Any * convertNeutralCpp(const TypeCode *t, Any *data)
    {
      return convertNeutralNeutral(t, data);
    }

    NeutralCpp::NeutralCpp(InputCppPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
    }

    //!Convert a Any to a Any 
    /*!
     *   transition method from const void* to Any*
     *   \param data : const void * data
     */
    void NeutralCpp::put(const void *data) throw(ConversionException)
    {
      put((Any *)data);
    }

    //!Convert a Any to a Any 
    /*!
     *   \param data : Any object
     */
    void NeutralCpp::put(Any *data) throw(ConversionException)
    {
      Any *a = convertNeutralCpp(edGetType(),data);
      DEBTRACE( "before put refcnt: " << a->getRefCnt() );
      _port->put(a);
      DEBTRACE( "after put refcnt: " << a->getRefCnt() );
      //_port has called incRef
      a->decrRef();
    }

    int isAdaptableCppNeutral(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableNeutralNeutral(t1, t2);
    }

  }
}
