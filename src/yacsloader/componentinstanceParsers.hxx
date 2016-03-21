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

#ifndef _COMPONENTINSTANCEPARSER_HXX_
#define _COMPONENTINSTANCEPARSER_HXX_

#include "parserBase.hxx"
#include "factory.hxx"

namespace YACS
{

/*! \brief Class for componentinstance parser
 *
 *  Class used to parse ComponentInstance description
 *
 *  XML schema is 
   \verbatim
     <xsd:complexType name="ComponentInstanceType">
       <xsd:sequence>
         <xsd:element name="component" type="xsd:string"/>
         <xsd:element name="property" type="PropertyType" minOccurs="0" maxOccurs="unbounded"/>
       </xsd:sequence>
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="kind" type="xsd:string"/>
     </xsd:complexType>
   \endverbatim
 */
struct componentinstancetypeParser: parser
{
  static componentinstancetypeParser componentinstanceParser;
  virtual void buildAttr(const XML_Char** attr);
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void pre ();
  virtual void name(const std::string& name);
  virtual void kind(const std::string& kind);
  virtual void component (const std::string& name);
  virtual void property (const myprop& prop);
  virtual void load (const loadon& l);
  mycomponentinstance post();
  mycomponentinstance _componentinstance;
  std::string _container;
};

}

#endif
