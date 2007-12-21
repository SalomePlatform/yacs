
#ifndef _PORTPARSERS_HXX_
#define _PORTPARSERS_HXX_

#include "parserBase.hxx"
#include "propertyParsers.hxx"

#include "factory.hxx"

namespace YACS
{

/*! \brief Class for Inport parser.
 *
 *  This class is a base class for other inport parsers
 *
 *  XML schema:
   \verbatim
     <xsd:complexType name="InPortType">
       <xsd:sequence>
         <xsd:element name="property" type="PropertyType" minOccurs="0"/>
       </xsd:sequence>
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="type" type="xsd:string" use="required"/>
     </xsd:complexType>
   \endverbatim
 *
 */
template <class T=myinport>
struct inporttypeParser: parser
{
  static inporttypeParser<T> inportParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void name(const std::string& name);
  virtual void type(const std::string& type);
  virtual void property (const myprop& prop);
  virtual T& post();
protected:
    T _port;
};

/*! \brief Class for Outport parser.
 *
 *  This class is also used for OutputDataStream Port
 *  same XML schema as inporttypeParser
 */
template <class T=myoutport>
struct outporttypeParser:public inporttypeParser<T>
{
  static outporttypeParser<T> outportParser;
};

template <class T> inporttypeParser<T> inporttypeParser<T>::inportParser;
template <class T> outporttypeParser<T> outporttypeParser<T>::outportParser;

template <class T>
  void inporttypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "property")pp=&propertytypeParser::propertyParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
template <class T>
  void inporttypeParser<T>::onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "property")property(((propertytypeParser*)child)->post());
    }
template <class T>
  void inporttypeParser<T>::buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      required("type",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "type")type(attr[i+1]);
        }
    }
template <class T>
  void inporttypeParser<T>::pre ()
    {
      _port._name="";
      _port._type="";
      _port.clear();
    }
template <class T>
  void inporttypeParser<T>::name(const std::string& name)
    {
      _port._name=name;
    }
template <class T>
  void inporttypeParser<T>::type(const std::string& type)
    {
      _port._type=type;
    }
template <class T>
  void inporttypeParser<T>::property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
      _port.setProperty(prop._name,prop._value);
    }
template <class T>
  T& inporttypeParser<T>::post()
    {
      return _port;
    }

}

#endif