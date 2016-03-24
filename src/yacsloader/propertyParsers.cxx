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

#include "propertyParsers.hxx"


namespace YACS
{
  propertytypeParser propertytypeParser::propertyParser;

  void propertytypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      required("value",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "value")value(attr[i+1]);
      }
    }
  void propertytypeParser::name(const std::string& name){ _prop._name=name; }
  void propertytypeParser::value(const std::string& name){ _prop._value=name; }
  myprop propertytypeParser::post(){return _prop;}

}
