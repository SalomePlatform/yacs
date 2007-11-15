#ifndef _PRESETPARSERS_HXX_
#define _PRESETPARSERS_HXX_

#include "nodeParsers.hxx"

#include "factory.hxx"

#include "DataNode.hxx"

namespace YACS
{

/*! \brief Class for presetdata parser.
 *
 *  Its XML schema is:
 \verbatim
     <xsd:complexType name="PresetType">
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="type" type="xsd:string" use="required"/>
       <xsd:element name="value" type="ValueType" minOccurs="1" maxOccurs="1"/>
     </xsd:complexType>
 \endverbatim
 */
struct presetdatatypeParser: parser
{
  static presetdatatypeParser presetdataParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE("presetdatatypeParser::onStart");
      std::string element(el);
      this->maxcount("value",1,element);
      parser* pp=&parser::main_parser;
      if(element == "value")pp=&valuetypeParser::valueParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE("presetdatatypeParser::onEnd");
      std::string element(el);
      if(element == "value")value(((valuetypeParser*)child)->post());
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      DEBTRACE("presetdatatypeParser::buildAttr");
      required("name",attr);
      required("type",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "type")type(attr[i+1]);
      }
    }
  virtual void name (const std::string& name)
    {
      DEBTRACE("presetdatatypeParser::name");
      _param._name=name;
      _name=name;
    }
  virtual void type (const std::string& type)
    {
      DEBTRACE("presetdatatypeParser::type");
      _param._type=type;
      _type=type;
    }
    virtual void pre ()
    {
      DEBTRACE("presetdatatypeParser::pre");
    }
    virtual void value (const std::string& value)
    {
      DEBTRACE("presetdatatypeParser::value " << value);
      _param.clear();
      _param.setProperty("value",value);
    }
    virtual myoutport& post()
    {
      DEBTRACE("presetdatatypeParser::post");
      mincount("value",1);
      return _param;
    }
    myoutport _param;
    std::string _name;
    std::string _type;
};

/*! \brief Class for PresetNode parser.
 *
 *  Its XML schema is:
 \verbatim
     <xsd:complexType name="PresetType">
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:element name="parameter" type="ParameterType"/>
     </xsd:complexType>
 \endverbatim
 */
template <class T=ENGINE::DataNode*>
struct presettypeParser:public nodetypeParser<T>
{
  static presettypeParser<T> presetParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr);
  virtual void name (const std::string& name);
  virtual void parameter (myoutport& p);
  std::string _name;
};

template <class T> presettypeParser<T> presettypeParser<T>::presetParser;


template <class T>
void presettypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE("presettypeParser::onStart");
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "parameter")pp=&presetdatatypeParser::presetdataParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }

template <class T>
void presettypeParser<T>::onEnd(const char *el,parser* child)
    {
      DEBTRACE("presettypeParser::onEnd");
      std::string element(el);
      if(element == "parameter")parameter(((presetdatatypeParser*)child)->post());
    }

template <class T>
void presettypeParser<T>::buildAttr(const XML_Char** attr)
    {
      DEBTRACE("presettypeParser::buildAttr");
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2)
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
        }
    }

template <class T>
void presettypeParser<T>::name (const std::string& name)
    {
      DEBTRACE("presettypeParser::name");
      _name=name;
      this->_node = theRuntime->createDataNode("",_name);
    }

template <class T>
void presettypeParser<T>::parameter (myoutport& p)
{
    DEBTRACE("presettypeParser::parameter");
    ENGINE::OutputPort *port = this->_node->edAddOutputPort(p._name,currentProc->typeMap[p._type]);
    this->_node->setData(port,p._props["value"]);
}

}
#endif
