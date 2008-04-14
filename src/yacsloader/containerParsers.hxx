
#ifndef _CONTAINERPARSER_HXX_
#define _CONTAINERPARSER_HXX_

#include "parserBase.hxx"
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
