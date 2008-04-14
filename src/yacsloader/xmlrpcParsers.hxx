
#ifndef _XMLRPCPARSERS_HXX_
#define _XMLRPCPARSERS_HXX_

#include "parserBase.hxx"
#include "factory.hxx"
#include <vector>
#include <string>

namespace YACS
{
/*! \brief Class for XML-RPC value parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *
 *  Its XML schema is:
   \verbatim
     <xsd:complexType name="ValueType">
       <xsd:choice >
         <xsd:element name="int" type="IntType"/>
         <xsd:element name="boolean" type="BooleanType"/>
         <xsd:element name="double" type="DoubleType"/>
         <xsd:element name="string" type="StringType"/>
         <xsd:element name="objref" type="ObjrefType"/>
         <xsd:element name="array" type="ArrayType"/>
         <xsd:element name="struct" type="StructType"/>
       </xsd:choice>
     </xsd:complexType>
   \endverbatim
 */
struct valuetypeParser: parser
{
  static valuetypeParser valueParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void pre ();
  virtual void int_ (const int& d);
  virtual void boolean (const bool& d);
  virtual void double_ (const double& d);
  virtual void string(const std::string& d);
  virtual void objref(const std::string& d);
  virtual void array (const std::string& d);
  virtual void struct_ (const std::string& d);
  virtual std::string post();
  std::string _data;
  std::vector<std::string> _v;
};

/*! \brief Class for XML-RPC data parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *
 *  Its XML schema is:
   \verbatim
     <xsd:complexType name="DataType">
       <xsd:element name="value" type="ValueType"/>
     </xsd:complexType>
   \endverbatim
 */
struct datatypeParser: parser
{
  static datatypeParser dataParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void pre ();
  virtual void value (const std::string& v);
  virtual std::string post();
  std::string _data;
  std::vector<std::string> _datas;
};

/*! \brief Class for XML-RPC member parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *
 *  Its XML schema is:
   \verbatim
     <xsd:complexType name="MemberDataType">
       <xsd:element name="name" type="StringType"/>
       <xsd:element name="value" type="ValueType"/>
     </xsd:complexType>
   \endverbatim
 */
struct memberdatatypeParser: parser
{
  static memberdatatypeParser memberdataParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void pre ();
  virtual void name (const std::string& v);
  virtual void value (const std::string& v);
  virtual std::string post();
  std::string _data;
  std::vector<std::string> _datas;
};

/*! \brief Class for XML-RPC struct parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *
 *  Its XML schema is:
   \verbatim
     <xsd:complexType name="StructDataType">
       <xsd:element name="member" type="MemberDataType" minOccurs="1"/>
     </xsd:complexType>
   \endverbatim
 */
struct structdatatypeParser: parser
{
  static structdatatypeParser structdataParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void pre ();
  virtual void member (const std::string& d);
  virtual std::string post();
  std::string _members;
  std::vector<std::string> _membersStack;
};

/*! \brief Class for XML-RPC array parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *
 *  Its XML schema is:
   \verbatim
     <xsd:complexType name="ArrayType">
       <xsd:element name="data" type="DataType" minOccurs="1" maxOccurs="1"/>
     </xsd:complexType>
   \endverbatim
 */
struct arraytypeParser: parser
{
  static arraytypeParser arrayParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void pre ();
  virtual void data (const std::string& d);
  virtual std::string post();
  std::vector<std::string> _arrays;
};

struct parametertypeParser: parser
{
  static parametertypeParser parameterParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
    virtual void pre ();
    virtual void tonode (const std::string& name);
    virtual void toport (const std::string& name);
    virtual void value (const std::string& name);
    virtual myparam& post();
    myparam _param;
};

}

#endif
