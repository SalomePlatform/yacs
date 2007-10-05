
#include "TypeConversions.hxx"
#include "CORBANeutralConv.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

CorbaNeutral::CorbaNeutral(InputPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

//!Convert a CORBA::Any that is convertible to a YACS::ENGINE::Any * and send it to proxy port
/*!
 *   \param data : CORBA::Any object as a void * pointer
 */

void CorbaNeutral::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any that is convertible to a YACS::ENGINE::Any * and send it to proxy port
/*!
 *   \param data : CORBA::Any object
 */
void CorbaNeutral::put(CORBA::Any *data) throw(ConversionException)
{
  DEBTRACE( "CorbaNeutral::put" )
  YACS::ENGINE::Any *ob;
  ob=convertCorbaNeutral(edGetType(),data);
  DEBTRACE("before put refcnt: " << ob->getRefCnt())
  DEBTRACE( _port->getName() )
  _port->put(ob);
  // ob has been created in convertCorbaNeutral. _port has normally called incRef 
  ob->decrRef();
  DEBTRACE("after put refcnt: " << ob->getRefCnt())
}
