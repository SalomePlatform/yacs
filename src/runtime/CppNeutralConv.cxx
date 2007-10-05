
#include "TypeConversions.hxx"
#include "CppNeutralConv.hxx"

#include <iostream>
#include <sstream>
#include "Any.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS
{
  namespace ENGINE 
  {
    Any* convertCppNeutral(const TypeCode *t, Any *a)
    {
      return convertNeutralNeutral(t, a);
    }

    CppNeutral::CppNeutral(InputPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
    }

    void CppNeutral::put(const void *data) throw(ConversionException)
    {
      DEBTRACE(" CppNeutral::put(const void *data)");
      put((Any *)data);
    }

    //! Send received C++/Neutral (Any *) value to proxy port
    /*!
     *   \param data : Neutral (Any *) value
     */

    void CppNeutral::put(Any *data) throw(ConversionException)
    {
      DEBTRACE("CppNeutral::put " << data);
      Any *ob;
      ob=convertCppNeutral(edGetType(), data);
      DEBTRACE("before put refcnt: " << ob->getRefCnt());
      _port->put(ob);
      DEBTRACE( "after put refcnt: " << ob->getRefCnt() );
      //_port has called incRef
      ob->decrRef();
    }

    int isAdaptableNeutralCpp(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableNeutralNeutral(t1, t2);
    }
  }
}
