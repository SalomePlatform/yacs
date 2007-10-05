#include "TypeConversions.hxx"
#include "CORBACORBAConv.hxx"
#include "CORBAPorts.hxx"

using namespace YACS::ENGINE;
using namespace std;

CorbaCorba::CorbaCorba(InputCorbaPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

//!Convert a CORBA::Any to a CORBA::Any 
/*!
 *   transition method from const void* to CORBA::Any*
 *   \param data : const void * data
 */
void CorbaCorba::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any to a CORBA::Any 
/*!
 *   \param data : CORBA::Any object
 */
void CorbaCorba::put(CORBA::Any *data) throw(ConversionException)
{
  CORBA::Any *a = convertCorbaCorba(edGetType(),data);
  _port->put(a);
}
