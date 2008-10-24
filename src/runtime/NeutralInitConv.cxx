#include "TypeConversions.hxx"
#include "NeutralInitConv.hxx"
#include "TypeCode.hxx"

#include <iostream>
#include <sstream>
#include "Any.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

NeutralInit::NeutralInit(InputPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void NeutralInit::put(const void *data) throw(ConversionException)
{
  DEBTRACE(" NeutralInit::put(const void *data)");
  put((YACS::ENGINE::Any *)data);
}

//!Initialize Neutral proxy port with received Any object after type check
 /*!
  *   \param data : YACS::ENGINE::Any object
  */

void NeutralInit::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  DEBTRACE("NeutralInit::put " << data);
  if(!edGetType()->isAdaptable(data->getType()))
    throw ConversionException("Value type is not compatible with port type");
  _port->put((const void *)data);
}
