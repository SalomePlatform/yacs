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
      if (!attr)
        return;
      DEBTRACE("presetdatatypeParser::buildAttr");
      required("name",attr);
      required("type",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "type")type(attr[i+1]);
        if(std::string(attr[i]) == "ref")ref(attr[i+1]);
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

  virtual void ref (const std::string& ref)
  {
    _ref=ref;
  }

  virtual void pre ()
  {
    DEBTRACE("presetdatatypeParser::pre");
    _ref="undef";
    _param.clear();
  }
  virtual void value (const std::string& value)
  {
    DEBTRACE("presetdatatypeParser::value " << value);
    _param.setProperty("value",value);
  }
  virtual myoutport& post()
  {
    DEBTRACE("presetdatatypeParser::post");
    //a parameter can have a ref attribute OR one value element
    if(_ref == "undef")
      mincount("value",1);
    else
      _param.setProperty("value",_ref);
    return _param;
  }
  myoutport _param;
  std::string _name;
  std::string _type;
  std::string _ref;
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
  virtual void create ();
  virtual void pre ();
  virtual void name (const std::string& name);
  virtual void kind (const std::string& kind);
  virtual void parameter (myoutport& p);
  std::string _name;
  std::string _kind;
};

template <class T> presettypeParser<T> presettypeParser<T>::presetParser;


template <class T>
void presettypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE("presettypeParser::onStart");
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "parameter")pp=&presetdatatypeParser::presetdataParser;
      if(element == "property")pp=&propertytypeParser::propertyParser;
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
      if(element == "property")this->property(((propertytypeParser*)child)->post());
    }

template <class T>
void presettypeParser<T>::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      DEBTRACE("presettypeParser::buildAttr");
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2)
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "kind")kind(attr[i+1]);
        }
      create();
    }

template <class T>
void presettypeParser<T>::pre ()
{
  _kind="";
}

template <class T>
void presettypeParser<T>::name (const std::string& name)
{
  _name=name;
}

template <class T>
void presettypeParser<T>::kind (const std::string& kind)
{
  _kind=kind;
}

template <class T>
void presettypeParser<T>::create ()
{
  this->_node = theRuntime->createInDataNode(_kind,_name);
}

template <class T>
void presettypeParser<T>::parameter (myoutport& p)
{
  DEBTRACE("presettypeParser::parameter");
  if(currentProc->typeMap.count(p._type)==0)
    {
      //Check if the typecode is defined in the runtime
      YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(p._type);
      if(t==0)
      {
        std::string msg="Unknown Type: ";
        msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
        this->logError(msg);
        return;
      }
      else
      {
        currentProc->typeMap[p._type]=t;
        t->incrRef();
      }
    }
  ENGINE::OutputPort *port = this->_node->edAddOutputPort(p._name,currentProc->typeMap[p._type]);
  this->_node->setData(port,p._props["value"]);
}

}
#endif
