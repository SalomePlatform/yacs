//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "StudyPorts.hxx"
#include "TypeCode.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"


namespace YACS
{
namespace ENGINE
{

OutputStudyPort::OutputStudyPort(const std::string& name,  Node* node, TypeCode* type)
  : OutputXmlPort(name, node, type),
    DataPort(name, node, type),
    Port(node)
{
}

OutputStudyPort::OutputStudyPort(const OutputStudyPort& other, Node *newHelder)
  : OutputXmlPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder)
{
}

void OutputStudyPort::setData(const std::string& data)
{
  _storeData = data;
  DEBTRACE( "OutputStudyPort::setData " << _storeData );
  modified();
}

std::string OutputStudyPort::getData()
{
  DEBTRACE("OutputStudyPort::getData " << _storeData);
  return _storeData;
}

void OutputStudyPort::putIOR(const std::string& ior)
{
  DEBTRACE("OutputStudyPort::putIOR " << ior);
  if(ior.substr(0,7) == "<value>")
    {
      put(ior.c_str());
    }
  else
    {
      int tk=edGetType()->kind();
      std::string value;
      switch(tk)
        {
        case Double:
          value="<value><double>"+ior+"</double></value>";
          break;
        case Int:
          value="<value><int>"+ior+"</int></value>";
          break;
        case String:
          value="<value><string>"+ior+"</string></value>";
          break;
        case Bool:
          value="<value><boolean>"+ior+"</boolean></value>";
          break;
        case Objref:
          value="<value><objref>"+ior+"</objref></value>";
          break;
        case Sequence:
        case Array:
        case Struct:
        default:
          break;
        }
      put(value.c_str());
    }
}

std::string OutputStudyPort::dump()
{
  DEBTRACE( "OutputStudyPort::dump " << _storeData );
  DEBTRACE( "OutputStudyPort::dump " << _data );
  //return "<value><string>"+_storeData+"</string></value>";
  return _data;
}

std::string OutputStudyPort::getPyObj()
{
  return getData();
}
std::string OutputStudyPort::getAsString()
{
  return getData();
}


InputStudyPort::InputStudyPort(const std::string& name,  Node* node, TypeCode* type)
  : InputXmlPort(name, node, type),
    DataPort(name, node, type),
    Port(node)
{
}

InputStudyPort::InputStudyPort(const InputStudyPort& other, Node *newHelder)
  : InputXmlPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder)
{
}

void InputStudyPort::setData(const std::string& data)
{
  _storeData = data;
  DEBTRACE( "InputStudyPort::setData " << _storeData );
  modified();
}

std::string InputStudyPort::getData()
{
  DEBTRACE("InputStudyPort::getData " << _storeData);
  return _storeData;
}

std::string InputStudyPort::getIOR()
{
  switch(edGetType()->kind())
   {
   case Objref:
     return splitXML(_data);
   default:
     return _data;
   }
}

std::string InputStudyPort::splitXML(const std::string& s)
{
  // <value><tag>content</tag></value>
  std::string::size_type begin = s.find_first_of("<"); // <value>
  begin = s.find_first_of(">",begin); // end of <value>
  begin = s.find_first_of("<",begin); // beginning of <tag>
  begin = s.find_first_of(">",begin); // end of <tag>
  begin = s.find_first_not_of("> ",begin); // beginning of content
  std::string::size_type last = s.find_first_of("<",begin); // beginning of </tag>
  return s.substr(begin,last-begin);
}

std::string InputStudyPort::dump()
{
  DEBTRACE( "InputStudyPort::dump " << _storeData );
  DEBTRACE( "InputStudyPort::dump " << _data );
  //return "<value><string>"+_storeData+"</string></value>";
  return _data;
}
std::string InputStudyPort::getPyObj()
{
  return getData();
}
std::string InputStudyPort::getAsString()
{
  return getData();
}

}
}
