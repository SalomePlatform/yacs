
#include "TypeConversions.hxx"
#include "CORBACppConv.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS
{
  namespace ENGINE
  {
    Any * convertCorbaCpp(const TypeCode *t, CORBA::Any *A)
    {
      return convertCorbaNeutral(t, A);
    }

    CorbaCpp::CorbaCpp(InputPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
    }

    //!Convert a CORBA::Any that is convertible to a YACS::ENGINE::Any * and send it to proxy port
    /*!
     *   \param data : CORBA::Any object as a void * pointer
     */

    void CorbaCpp::put(const void *data) throw(ConversionException)
    {
      put((CORBA::Any *)data);
    }

    //!Convert a CORBA::Any that is convertible to a YACS::ENGINE::Any * and send it to proxy port
    /*!
     *   \param data : CORBA::Any object
     */
    void CorbaCpp::put(CORBA::Any *data) throw(ConversionException)
    {
      DEBTRACE("CorbaCpp::put");
      YACS::ENGINE::Any *ob;
      ob= convertCorbaCpp(edGetType(),data);
      DEBTRACE("refcnt: " << ob->getRefCnt());
      DEBTRACE(_port->getName());
      _port->put(ob);
      // ob has been created in convertCorbaNeutral. _port has normally called incrRef
      ob->decrRef();
      DEBTRACE("after put refcnt: " << ob->getRefCnt())
    }

    int isAdaptableCppCorba(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptableNeutralCorba(t1, t2);
    }
  }
}

