
#include "XMLPorts.hxx"

#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

InputXmlPort::InputXmlPort(const string& name, Node * node, TypeCode * type)
  : InputPort(name, node, type), Port(node)
{
  _impl="XML";
}

const char * InputXmlPort::getXml() const
{
  return _data.c_str();
}

void InputXmlPort::put(const void *data) throw (ConversionException)
{
  put((const char*)data);
  _empty = false;
}

void InputXmlPort::put(const char *data) throw (ConversionException)
{
  cerr << "InputXmlPort::put" << endl;
  _data = data;
  cerr << "data: " << data << endl;
}



OutputXmlPort::OutputXmlPort(const string& name, Node* node, TypeCode * type)
  : OutputPort(name, node, type), Port(node)
{
  _impl="XML";
}

const char * OutputXmlPort::get() const throw (ConversionException)
{
  return _data.c_str();
}

void OutputXmlPort::put(const void *data) throw (ConversionException)
{
  put((const char*)data);
}

void OutputXmlPort::put(const char *data)  throw (ConversionException)
{
  cerr << "OutputXmlPort::put-------" << getName() << endl;
  InputPort *p;
  cerr << "data: " << data << endl;
  _data=data;
  set<InputPort *>::iterator iter;
  for(iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    {
      p=*iter;
      p->put((void*)data);
    }
}
