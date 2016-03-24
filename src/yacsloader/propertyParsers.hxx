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

#ifndef _PROPERTYPARSER_HXX_
#define _PROPERTYPARSER_HXX_

#include "parserBase.hxx"
#include "factory.hxx"

namespace YACS
{

/*! \brief Class for property parser.
 *
 *  Class used to parse a property.
 *  A property is a pair of name(string), value(string)
 *
   XML schema is
   \verbatim
     <xsd:complexType name="PropertyType">
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="value" type="xsd:string" use="required"/>
     </xsd:complexType>
   \endverbatim
 *
 */
struct propertytypeParser: parser
{
  static propertytypeParser propertyParser;
  virtual void buildAttr(const XML_Char** attr);
  virtual void name(const std::string& name);
  virtual void value(const std::string& name);
  myprop post();
  myprop _prop;
};

}

#endif
