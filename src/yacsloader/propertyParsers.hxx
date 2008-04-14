
#ifndef _PROPERTYPARSER_HXX_
#define _PROPERTYPARSER_HXX_

#include "parserBase.hxx"
#include "factory.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

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
