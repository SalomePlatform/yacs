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

#ifndef _TYPEPARSERS_HXX_
#define _TYPEPARSERS_HXX_

#include "parserBase.hxx"
#include "TypeCode.hxx"
#include "factory.hxx"

namespace YACS
{

/*! \brief Class for type parser.
 *
 *  Class used to parse a type definition (class TypeCode in implementation)
 *  with a name and a kind (reserved to atomic types)
 
   XML schema is 
   \verbatim
     <xsd:complexType name="TypeType">
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="kind" type="xsd:string" use="required"/>
     </xsd:complexType>
   \endverbatim
 */
struct typetypeParser: parser
{
  static typetypeParser typeParser;
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void name(const std::string& name);
  virtual void kind(const std::string& name);
  virtual mytype post();
  std::string _name;
  std::string _kind;
};

/*! \brief Class for sequence parser.
 *
 *  Class used to parse a sequence (type) definition (class TypeCodeSeq in implementation)
 *
   XML schema is 
 
 \verbatim
     <xsd:complexType name="SequenceType">
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="content" type="xsd:string" use="required"/>
     </xsd:complexType>
 \endverbatim
 
 */
struct seqtypeParser:public parser
{
  static seqtypeParser seqParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr);
  void name(const std::string& name);
  void content(const std::string& name);
  YACS::ENGINE::TypeCode* post();
  YACS::ENGINE::TypeCode* _contentType;
  std::string _name;
};

/*! \brief Class for objref parser.
 *
 *  Class used to parse a objref (type) definition (class TypeCodeObjref in implementation)
 
   XML schema is 
   \verbatim
     <xsd:complexType name="ObjrefType">
       <xsd:sequence>
         <xsd:element name="base" type="xsd:string" minOccurs="0" maxOccurs="unbounded"/>
       </xsd:sequence>
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="id" type="xsd:string" />
     </xsd:complexType>
   \endverbatim
 
 */
struct objtypeParser: parser
{
  static objtypeParser objParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void name(const std::string& name);
  virtual void id(const std::string& name);
  virtual void base(const std::string& name);
  virtual YACS::ENGINE::TypeCode * post();
  std::string _name;
  std::string _id;
  std::list<YACS::ENGINE::TypeCodeObjref *> _ltc;
};

/*! \brief Class for member parser.
 *
 *  Class used to parse a struct member
 *  A struct member is a pair of name(string), type(string)
 
   XML schema is
   \verbatim
     <xsd:complexType name="MemberType">
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="type" type="xsd:string" use="required"/>
     </xsd:complexType>
   \endverbatim
 
 */
struct membertypeParser: parser
{
  static membertypeParser memberParser;
  virtual void buildAttr(const XML_Char** attr);
  virtual void name(const std::string& name);
  virtual void type(const std::string& name);
  myprop post();
  myprop _prop;
};

/*! \brief Class for struct parser.
 *
 *  Class used to parse a struct (type) definition (class TypeCodeStruct in implementation)
 *
 *  XML schema is 
   \verbatim
     <xsd:complexType name="StructType">
       <xsd:sequence>
         <xsd:element name="member" type="MemberType" minOccurs="0" maxOccurs="unbounded"/>
       </xsd:sequence>
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="id" type="xsd:string" />
     </xsd:complexType>
   \endverbatim
 *
 */
struct structtypeParser: parser
{
  static structtypeParser structParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void name(const std::string& name);
  virtual void id(const std::string& name);
  virtual void member (const myprop& prop);
  virtual YACS::ENGINE::TypeCode * post();
  std::string _name;
  std::string _id;
  std::vector<myprop> _members;
};

}

#endif
