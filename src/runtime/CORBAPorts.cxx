
#include "CORBAPorts.hxx"
#include "RuntimeSALOME.hxx"
#include "TypeConversions.hxx"

#include <iostream>
#include <sstream>


using namespace YACS::ENGINE;
using namespace std;


InputCorbaPort::InputCorbaPort(const string& name,
			       Node *node,
			       TypeCode * type)
  : InputPort(name, node, type), Port(node)
{
  _impl="CORBA";
  _orb = getSALOMERuntime()->getOrb();
}

void InputCorbaPort::put(const void *data) throw (ConversionException)
{
  put((CORBA::Any *)data);
  _empty = false;
}

void InputCorbaPort::put(CORBA::Any *data) throw (ConversionException)
{
  cerr << "InputCorbaPort::put" << endl;
  // cerr << "addr data: " << data << endl;
  // cerr << "addr data.value(): " << data->value() << endl;
  switch(type()->kind())
    {
    case Double:
      CORBA::Double d;
      *data >>= d;
      cerr << "Double: " << d << endl;
      break;
    case Int:
      CORBA::Long l;
      *data >>= l;
      cerr << "Int: " << l << endl;
      break;
    case Sequence:
      break;
    default:
      break;
    }
  // on fait une copie du any (protection contre la destruction du any source)
  // la gestion des destructions est correctement faite par omniorb
  _data=*data;
  // cerr << "addr _data: " << &_data << endl;
  // cerr << "addr _data.value(): " << _data.value() << endl;
}

CORBA::Any * InputCorbaPort::getAny()
{
  // cerr << "_data: " << &_data << endl;
  // cerr << "_data.value(): " << _data.value() << endl;
  // cerr << "_data.NP_pd(): " << _data.NP_pd() << endl;
  // --- on retourne un pointeur sur le any interne
  return &_data;
}


OutputCorbaPort::OutputCorbaPort(const string& name,
				 Node *node,
				 TypeCode * type)
  : OutputPort(name, node, type), Port(node)
{
  _impl="CORBA";
  _orb = getSALOMERuntime()->getOrb();
}

void OutputCorbaPort::put(const void *data) throw (ConversionException)
{
  put((CORBA::Any *)data);
}

void OutputCorbaPort::put(CORBA::Any *data) throw (ConversionException)
{
  cerr << "OutputCorbaPort::put" << endl;
  // cerr << "addr data: " << data << endl;
  InputPort *p;
  // on fait une copie du any source
  // (protection contre la destruction de la source)
  _data=*data;
  set<InputPort *>::iterator iter;
  for(iter=_setOfInputPort.begin(); iter!=_setOfInputPort.end(); iter++)
    {
      p=*iter;
      // on pousse le pointeur mais put fait normalement une copie
      p->put(&_data);
    }
}

CORBA::Any * OutputCorbaPort::getAny()
{
  // cerr << "_data: " << &_data << endl;
  // cerr << "_data.value(): " << _data.value() << endl;
  // cerr << "_data.NP_pd(): " << _data.NP_pd() << endl;
  // on retourne un pointeur sur le any interne
  return &_data;
}

CORBA::Any * OutputCorbaPort::getAnyOut() 
{
  CORBA::Any* a=&_data;
  DynType kind=type()->kind();

  if(kind == Int)
    {
      a->replace(CORBA::_tc_long, (void*) 0);
    }
    else if(kind == String)
      {
        a->replace(CORBA::_tc_string, (void*) 0);
      }
    else if(kind == Double)
      {
        a->replace(CORBA::_tc_double, (void*) 0);
      }
    else if(kind == Objref)
      {
        a->replace(CORBA::_tc_Object, (void*) 0);
      }
    else if(kind == Sequence)
      {
        CORBA::TypeCode_var t;
        t = getCorbaTC(type());
        a->replace(t, (void*) 0);
       }
    else if(kind == Bool)
      {
        stringstream msg;
        msg << "Cannot set Any Out for Bool" << __FILE__ << ":" << __LINE__;
        throw Exception(msg.str());
      }
    else if(kind == None)
      {
        stringstream msg;
        msg << "Cannot set Any Out for None" << __FILE__ << ":" << __LINE__;
        throw Exception(msg.str());
      }
    else
      {
        stringstream msg;
        msg << "Cannot set Any Out for unknown type" << __FILE__
	    << ":" << __LINE__;
        throw Exception(msg.str());
      }
    
  // on retourne un pointeur sur le any interne reinitialisé
  // cerr << "getAnyOut::_data: " << a << endl;
  return a;
}

ostream& YACS::ENGINE::operator<<(ostream& os, const OutputCorbaPort& p)
{
  CORBA::Double l;
  p._data>>=l;
  os << p._name << " : " << l ;
  return os;
}

