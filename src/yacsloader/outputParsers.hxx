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

#ifndef _OUTPUTPARSERS_HXX_
#define _OUTPUTPARSERS_HXX_

#include "nodeParsers.hxx"

#include "factory.hxx"

#include "DataNode.hxx"

namespace YACS
{

/*! \brief Class for outputdata parser.
 *
 *  Its XML schema is:
 \verbatim
     <xsd:complexType name="OutputDataType">
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="type" type="xsd:string" use="required"/>
       <xsd:attribute name="ref" type="xsd::string" />
     </xsd:complexType>
 \endverbatim
 */
struct outputdatatypeParser: parser
{
  static outputdatatypeParser outputdataParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE("outputdatatypeParser::onStart");
      std::string element(el);
      parser* pp=&parser::main_parser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      DEBTRACE("outputdatatypeParser::buildAttr");
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
      DEBTRACE("outputdatatypeParser::name");
      _param._name=name;
    }
  virtual void type (const std::string& type)
    {
      DEBTRACE("outputdatatypeParser::type");
      _param._type=type;
    }
  virtual void ref (const std::string& ref)
    {
      _param.setProperty("ref",ref);
    }

  virtual void pre ()
    {
      DEBTRACE("outputdatatypeParser::pre");
      _param.clear();
    }
  virtual myoutport& post()
    {
      return _param;
    }
  myoutport _param;
};

/*! \brief Class for OutNode parser.
 *
 *  Its XML schema is:
 \verbatim
     <xsd:complexType name="OutNodeType">
       <xsd:attribute name="name" type="xsd:string" use="required"/>
       <xsd:attribute name="kind" type="xsd:string" />
       <xsd:attribute name="ref" type="xsd:string" />
       <xsd:element name="parameter" type="OutputDataType"/>
     </xsd:complexType>
 \endverbatim
 */
template <class T=ENGINE::DataNode*>
struct outnodetypeParser:public nodetypeParser<T>
{
  static outnodetypeParser<T> outnodeParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void name (const std::string& name);
  virtual void kind (const std::string& kind);
  virtual void ref (const std::string& ref);
  virtual void create ();
  virtual void parameter (myoutport& p);
  virtual T post();
  std::string _name;
  std::string _kind;
  std::string _ref;
};

template <class T> outnodetypeParser<T> outnodetypeParser<T>::outnodeParser;


template <class T>
void outnodetypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE("outnodetypeParser::onStart");
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "parameter")pp=&outputdatatypeParser::outputdataParser;
      if(element == "property")pp=&propertytypeParser::propertyParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }

template <class T>
void outnodetypeParser<T>::onEnd(const char *el,parser* child)
    {
      DEBTRACE("outnodetypeParser::onEnd");
      std::string element(el);
      if(element == "parameter")parameter(((outputdatatypeParser*)child)->post());
      if(element == "property")this->property(((propertytypeParser*)child)->post());
    }

template <class T>
void outnodetypeParser<T>::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      DEBTRACE("outnodetypeParser::buildAttr");
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2)
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "kind")kind(attr[i+1]);
          if(std::string(attr[i]) == "ref")ref(attr[i+1]);
        }
      create();
    }

template <class T>
void outnodetypeParser<T>::pre ()
{
  this->_node=0;
  _kind="";
  _ref="";
}

template <class T>
void outnodetypeParser<T>::name (const std::string& name)
{
  _name=name;
}
template <class T>
void outnodetypeParser<T>::kind (const std::string& kind)
{
  _kind=kind;
}
template <class T>
void outnodetypeParser<T>::ref (const std::string& ref)
{
  _ref=ref;
}

template <class T>
void outnodetypeParser<T>::create ()
{
  this->_node = theRuntime->createOutDataNode(_kind,_name);
}

template <class T>
void outnodetypeParser<T>::parameter (myoutport& p)
{
  DEBTRACE("outnodetypeParser::parameter");
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
  ENGINE::InputPort *port = this->_node->edAddInputPort(p._name,currentProc->typeMap[p._type]);
  this->_node->setData(port,p._props["ref"]);
}

template <class T>
T outnodetypeParser<T>::post()
{
  this->_node->setRef(_ref);
  return this->_node;
}

}
#endif
