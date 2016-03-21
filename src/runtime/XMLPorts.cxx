// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "XMLPorts.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

InputXmlPort::InputXmlPort(const std::string& name, Node * node, TypeCode * type)
  : InputPort(name, node, type), DataPort(name, node, type), Port(node), _initData(""), _data("")
{
}

InputXmlPort::InputXmlPort(const InputXmlPort& other, Node *newHelder):InputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder),_initData(other._initData),_data(other._data)
{
}

bool InputXmlPort::edIsManuallyInitialized() const
{
  return _initData != "";
}

void InputXmlPort::edRemoveManInit()
{
  _initData="";
  InputPort::edRemoveManInit();
}

const char *InputXmlPort::getXml() const
{
  DEBTRACE(_data);
  return _data.c_str();
}

void *InputXmlPort::get() const throw(YACS::Exception)
{
  return (void *) _data.c_str();
}

void InputXmlPort::put(const void *data) throw (ConversionException)
{
  DEBTRACE("put(void *)");
  put((const char*)data);
}

void InputXmlPort::put(const char *data) throw (ConversionException)
{
  DEBTRACE(data);
  _data = data;
}

bool InputXmlPort::isEmpty()
{
  return _data.empty();
}

InputPort *InputXmlPort::clone(Node *newHelder) const
{
  return new InputXmlPort(*this,newHelder);
}

//! Save the current data value for further reinitialization of the port
/*!
 *
 */
void InputXmlPort::exSaveInit()
{
  _initData=_data;
}

//! Restore the saved data value to current data value
/*!
 * If no data has been saved (_initData == 0) don't restore
 */
void InputXmlPort::exRestoreInit()
{
  if(_initData!="")return;
  _data=_initData;
}

std::string InputXmlPort::dump()
{
  return _data;
}

std::string InputXmlPort::valToStr()
{
  return _data;
}

void InputXmlPort::valFromStr(std::string valstr)
{
  _data = valstr;
}


OutputXmlPort::OutputXmlPort(const std::string& name, Node* node, TypeCode * type)
  : OutputPort(name, node, type), DataPort(name, node, type), Port(node)
{
}

OutputXmlPort::OutputXmlPort(const OutputXmlPort& other, Node *newHelder):OutputPort(other,newHelder),DataPort(other,newHelder),
                                                                          Port(other,newHelder),_data(other._data)
{
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
  DEBTRACE(data);
  InputPort *p;
  _data=data;
  OutputPort::put(data);
}

OutputPort *OutputXmlPort::clone(Node *newHelder) const
{
  return new OutputXmlPort(*this,newHelder);
}

std::string OutputXmlPort::dump()
{
  return _data;
}

std::string OutputXmlPort::valToStr()
{
  return _data;
}

void OutputXmlPort::valFromStr(std::string valstr)
{
  _data = valstr;
}

