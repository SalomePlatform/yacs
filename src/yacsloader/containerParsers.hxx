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

#ifndef _CONTAINERPARSER_HXX_
#define _CONTAINERPARSER_HXX_

#include "parserBase.hxx"
#include "codeParsers.hxx"
#include "factory.hxx"

namespace YACS
{

/*! \brief Class for machine parser.
 *
 *  Class used to parse computer adress on which container must be started
 *
 *  XML schema is 
   \verbatim
     <xsd:complexType name="MachineType">
       <xsd:attribute name="name" type="xsd:string" use="required"/>
     </xsd:complexType>
   \endverbatim
 */
struct machinetypeParser: parser
{
  static machinetypeParser machineParser;
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void name(const std::string& name);
  machine post();
  machine _mach;
};

/*! \brief Class for container parser
 *
 *  Class used to parse container description
 *
 *  XML schema is 
   \verbatim
     <xsd:complexType name="ContainerType">
       <xsd:sequence>
         <xsd:element name="machine" type="MachineType" minOccurs="0" maxOccurs="unbounded"/>
         <xsd:element name="property" type="PropertyType" minOccurs="0" maxOccurs="unbounded"/>
       </xsd:sequence>
       <xsd:attribute name="name" type="xsd:string" use="required"/>
     </xsd:complexType>
   \endverbatim
 */
struct containertypeParser: parser
{
  static containertypeParser containerParser;
  virtual void buildAttr(const XML_Char** attr);
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void pre ();
  virtual void name(const std::string& name);
  virtual void machine_(const machine& m);
  virtual void property (const myprop& prop);
  virtual void initializescriptkey(const myfunc& f);
  mycontainer post();
  mycontainer _container;
};

/*! \brief Class for loading parser
 *
 *  Class used to parse service node loading information
 *
 *  XML schema is 
   \verbatim
     <xsd:complexType name="LoadType">
       <xsd:attribute name="container" type="xsd:string" use="required"/>
     </xsd:complexType>
   \endverbatim
 */
struct loadtypeParser: parser
{
  static loadtypeParser loadParser;
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void container(const std::string& name);
  loadon post();
  loadon _loadon;
};

}

#endif
