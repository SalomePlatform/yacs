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

#include "StudyPorts.hxx"
#include "TypeCode.hxx"
#include "Node.hxx"
#include "Proc.hxx"
#include "ComponentInstance.hxx"
#include "SalomeComponent.hxx"
#include "RuntimeSALOME.hxx"

#include "SALOMEDS_Attributes.hh"

#include <iostream>
#include <iomanip>
#include <cstdlib>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"


namespace YACS
{
namespace ENGINE
{

/*! \class YACS::ENGINE::OutputStudyPort
 *  \brief Class for Study output Ports
 *
 * \ingroup Ports
 *
 * \see StudyInNode
 */

OutputStudyPort::OutputStudyPort(const std::string& name,  Node* node, TypeCode* type)
  : OutputXmlPort(name, node, type),
    DataPort(name, node, type),
    Port(node)
{
}

OutputStudyPort::OutputStudyPort(const OutputStudyPort& other, Node *newHelder)
  : OutputXmlPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder),_storeData(other._storeData)
{
}

OutputPort* OutputStudyPort::clone(Node *newHelder) const
{
  return new OutputStudyPort(*this,newHelder);
}

void OutputStudyPort::setData(const std::string& data)
{
  _storeData = data;
  DEBTRACE( "OutputStudyPort::setData " << _storeData );
  modified();
}

std::string OutputStudyPort::getData()
{
  DEBTRACE("OutputStudyPort::getData " << _storeData);
  return _storeData;
}

void OutputStudyPort::putIOR(const std::string& ior)
{
  DEBTRACE("OutputStudyPort::putIOR " << ior);
  if(ior.substr(0,7) == "<value>")
    {
      put(ior.c_str());
    }
  else
    {
      int tk=edGetType()->kind();
      std::string value;
      switch(tk)
        {
        case Double:
          value="<value><double>"+ior+"</double></value>";
          break;
        case Int:
          value="<value><int>"+ior+"</int></value>";
          break;
        case String:
          value="<value><string>"+ior+"</string></value>";
          break;
        case Bool:
          value="<value><boolean>"+ior+"</boolean></value>";
          break;
        case Objref:
          value="<value><objref>"+ior+"</objref></value>";
          break;
        case Sequence:
        case Array:
        case Struct:
        default:
          break;
        }
      put(value.c_str());
    }
}

std::string OutputStudyPort::dump()
{
  DEBTRACE( "OutputStudyPort::dump " << _storeData );
  DEBTRACE( "OutputStudyPort::dump " << _data );
  //return "<value><string>"+_storeData+"</string></value>";
  return _data;
}

std::string OutputStudyPort::getPyObj()
{
  return getData();
}
std::string OutputStudyPort::getAsString()
{
  return getData();
}

void OutputStudyPort::getDataFromStudy(SALOMEDS::Study_var myStudy)
{
      std::string data = getData();
      DEBTRACE("data: " << data );
      //try an id
      SALOMEDS::SObject_var aSO = myStudy->FindObjectID(data.c_str());
      if(CORBA::is_nil(aSO))
        {
          //try a path
          aSO=myStudy->FindObjectByPath(data.c_str());
          if(CORBA::is_nil(aSO))
            {
              std::stringstream msg;
              msg << "Execution problem: no id or path: " << data << " in study " << myStudy->StudyId();
              throw Exception(msg.str());
            }
        }

      CORBA::String_var path=myStudy->GetObjectPath(aSO);
      DEBTRACE(path);
      CORBA::String_var id=aSO->GetID();
      DEBTRACE(id);
      //CORBA::Object_var sobj=aSO->GetObject();

      SALOMEDS::GenericAttribute_var aGAttr;
      CORBA::String_var value;

      if(edGetType()->kind()==Objref)
        {
          if ( aSO->FindAttribute( aGAttr, "AttributeIOR" ) )
            {
              SALOMEDS::AttributeIOR_var anAttr = SALOMEDS::AttributeIOR::_narrow( aGAttr );
              value=anAttr->Value();
              putIOR((const char*)value);
            }
          else
            {
              //Problem !!!
              std::string error="Execution problem: no AttributeIOR in study object: ";
              error=error+data;
              throw Exception(error);
            }
        }
      else if(edGetType()->kind()==Double )
        {
          if ( aSO->FindAttribute( aGAttr, "AttributeReal" ) )
            {
              SALOMEDS::AttributeReal_var anAttr = SALOMEDS::AttributeReal::_narrow( aGAttr );
              CORBA::Double d=anAttr->Value();
              std::stringstream msg;
              msg << "<value><double>" << std::setprecision(16) << d << "</double></value>";
              put(msg.str().c_str());
            }
          else
            {
              std::string error="Execution problem: no AttributeReal in study object: ";
              throw Exception(error+data);
            }
        }
      else if(edGetType()->kind()== Int)
        {
          if ( aSO->FindAttribute( aGAttr, "AttributeInteger" ) )
            {
              SALOMEDS::AttributeInteger_var anAttr = SALOMEDS::AttributeInteger::_narrow( aGAttr );
              CORBA::Long l=anAttr->Value();
              std::stringstream msg;
              msg << "<value><int>" << l << "</int></value>";
              put(msg.str().c_str());
            }
          else
            {
              std::string error="Execution problem: no AttributeInteger in study object: ";
              throw Exception(error+data);
            }
        }
      else
        {
          if ( aSO->FindAttribute( aGAttr, "AttributeComment" ) )
            {
              SALOMEDS::AttributeComment_var anAttr = SALOMEDS::AttributeComment::_narrow( aGAttr );
              value=anAttr->Value();
              DEBTRACE(value);
              putIOR((const char*)value);
            }
          else
            {
              std::string error="Execution problem: no AttributeComment in study object: ";
              throw Exception(error+data);
            }
        }
}




SALOMEDS::SObject_ptr findOrCreateSoWithName(SALOMEDS::Study_ptr study, SALOMEDS::StudyBuilder_ptr builder,
                                             SALOMEDS::SObject_ptr sobj, const std::string& name)
{
  SALOMEDS::ChildIterator_var anIterator= study->NewChildIterator(sobj);
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var namAttr ;
  SALOMEDS::SObject_var result=SALOMEDS::SObject::_nil();

  for (; anIterator->More(); anIterator->Next())
    {
      SALOMEDS::SObject_var anObj=anIterator->Value();
      if(anObj->FindAttribute(anAttr, "AttributeName"))
        {
          namAttr = SALOMEDS::AttributeName::_narrow( anAttr );
          CORBA::String_var value=namAttr->Value();
          if(name == (const char*)value)
            {
              result=anObj;
              break;
            }
        }
    }
  if(CORBA::is_nil(result))
    {
      //create it
      result = builder->NewObject( sobj );
      anAttr=builder->FindOrCreateAttribute(result,"AttributeName");
      namAttr = SALOMEDS::AttributeName::_narrow( anAttr );
      namAttr->SetValue(name.c_str());
    }
  return result._retn();
}

/*! \class YACS::ENGINE::InputStudyPort
 *  \brief Class for Study input Ports
 *
 * \ingroup Ports
 *
 * \see StudyOutNode
 */

InputStudyPort::InputStudyPort(const std::string& name,  Node* node, TypeCode* type)
  : InputXmlPort(name, node, type),
    DataPort(name, node, type),
    Port(node)
{
}

InputStudyPort::InputStudyPort(const InputStudyPort& other, Node *newHelder)
  : InputXmlPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder),_storeData(other._storeData)
{
}

InputPort* InputStudyPort::clone(Node *newHelder) const
{
  return new InputStudyPort(*this,newHelder);
}

void InputStudyPort::setData(const std::string& data)
{
  _storeData = data;
  DEBTRACE( "InputStudyPort::setData " << _storeData );
  modified();
}

std::string InputStudyPort::getData()
{
  DEBTRACE("InputStudyPort::getData " << _storeData);
  return _storeData;
}

std::string InputStudyPort::getIOR()
{
  switch(edGetType()->kind())
   {
   case Objref:
     return splitXML(_data);
   default:
     return _data;
   }
}

std::string InputStudyPort::splitXML(const std::string& s)
{
  // <value><tag>content</tag></value>
  std::string::size_type begin = s.find_first_of("<"); // <value>
  begin = s.find_first_of(">",begin); // end of <value>
  begin = s.find_first_of("<",begin); // beginning of <tag>
  begin = s.find_first_of(">",begin); // end of <tag>
  begin = s.find_first_not_of("> ",begin); // beginning of content
  std::string::size_type last = s.find_first_of("<",begin); // beginning of </tag>
  return s.substr(begin,last-begin);
}

std::string InputStudyPort::dump()
{
  DEBTRACE( "InputStudyPort::dump " << _storeData );
  DEBTRACE( "InputStudyPort::dump " << _data );
  //return "<value><string>"+_storeData+"</string></value>";
  return _data;
}
std::string InputStudyPort::getPyObj()
{
  return getData();
}
std::string InputStudyPort::getAsString()
{
  return getData();
}

void InputStudyPort::putDataInStudy(SALOMEDS::Study_var myStudy,SALOMEDS::StudyBuilder_var aBuilder)
{
  SALOMEDS::GenericAttribute_var aGAttr;
  SALOMEDS::SObject_var aSO ;
  SALOMEDS::AttributeName_var anAttr ;
  SALOMEDS::AttributeIOR_var iorAttr ;
  SALOMEDS::SComponent_var       aFather;

  std::string data = getData();
  DEBTRACE("data: " << data );
  //try to find an existing id (i:j:k...)
  aSO = myStudy->FindObjectID(data.c_str());
  if(CORBA::is_nil(aSO))
    {
      // the id does not exist. Try to create it by id
      aSO=myStudy->CreateObjectID(data.c_str());
      if(!CORBA::is_nil(aSO))
        {
          aGAttr=aBuilder->FindOrCreateAttribute(aSO,"AttributeName");
          anAttr = SALOMEDS::AttributeName::_narrow( aGAttr );
          anAttr->SetValue(getName().c_str());
        }
    }

  if(CORBA::is_nil(aSO))
    {
      //try to publish the object with a given path
      std::string name; // the component instance name
      std::string objname; // the object name (eventually with "/")
      std::string::size_type begin = data.find_first_not_of("/");
      std::string::size_type pos=data.find_first_of("/", begin);
      if (pos != std::string::npos)
        {
          name=data.substr(begin,pos-begin);
          objname=data.substr(pos+1);
        }
      else
        {
          name=data.substr(begin);
          objname="";
        }
      std::string pname="/"+name;
      DEBTRACE(pname);
      DEBTRACE(objname);

      Proc* proc=getNode()->getProc();
      if(proc->componentInstanceMap.count(name)!=0)
        {
          // There is a component instance with this name. Is it a Salome component or not ?
          ComponentInstance* compo=proc->componentInstanceMap[name];
          if(SalomeComponent* scompo=dynamic_cast<SalomeComponent*>(compo))
            {
                  //It's a Salome component, make it the right way : component name, component instance reference, ...
              CORBA::Object_var compovar= scompo->getCompoPtr();
              SALOMEDS::Driver_var aDriver = SALOMEDS::Driver::_narrow(compovar);
              if ( !CORBA::is_nil( aDriver ) )
                {
                  //It's a Salome component that implements the Driver interface. Use it to publish in study
                  CORBA::ORB_ptr orb;
                  CORBA::Object_var anObject;
                  try
                    {
                          orb = getSALOMERuntime()->getOrb();
                          anObject=orb->string_to_object(getIOR().c_str());
                    }
                  catch ( ... ) 
                    {
                      std::cerr << "Execution problem: can not get the object to publish" << std::endl;
                      return;
                    }
                  if ( aDriver->CanPublishInStudy( anObject ) ) 
                    {
                          //It's fine use the driver to publish
                          SALOMEDS::SObject_var aTmpSO; // initialized to nil
                          try 
                            {
                              aTmpSO = aDriver->PublishInStudy(myStudy, aTmpSO, anObject,objname.c_str() );
                              return;
                            }
                          catch ( ... ) 
                            {
                              std::cerr << "Execution problem: error in PublishInStudy" << std::endl;
                              return;
                            }
                    }
                }
            }
        }

      // Does component entry exist ?
      aSO=myStudy->FindObjectByPath(pname.c_str());
      if(CORBA::is_nil(aSO))
        {
          // We have not been able to publish the object with Salome Driver, make it the light way
          aFather=aBuilder->NewComponent(name.c_str());
          if(CORBA::is_nil(aFather))
            {
              std::cerr << "Execution problem: can not create component: " + name << std::endl;
              return;
            }
          aGAttr=aBuilder->FindOrCreateAttribute(aFather,"AttributeName");
          anAttr = SALOMEDS::AttributeName::_narrow( aGAttr );
          anAttr->SetValue(name.c_str());
          aSO=myStudy->FindObjectByPath(pname.c_str());
        }

      begin=data.find_first_not_of("/",pos);
      while (begin != std::string::npos)
        {
          pos = data.find_first_of("/", begin);
          if (pos != std::string::npos)
            name=data.substr(begin,pos-begin);
          else
            name=data.substr(begin);
          aSO=findOrCreateSoWithName(myStudy,aBuilder,aSO,name);
          begin=data.find_first_not_of("/",pos);
        }
    }

  if(CORBA::is_nil(aSO))
    {
      std::cerr << "Execution problem: can not create id or path: " + data + " in study" << std::endl;
      return;
    }

  std::string value;
  SALOMEDS::AttributeComment_var commentAttr ;
  SALOMEDS::AttributeReal_var realAttr ;
  SALOMEDS::AttributeInteger_var intAttr ;
  SALOMEDS::AttributeString_var stringAttr ;
  double d;
  long v;
  switch(edGetType()->kind())
    {
      case Objref:
             value=getIOR();
             DEBTRACE(value);
             aGAttr=aBuilder->FindOrCreateAttribute(aSO,"AttributeIOR");
             iorAttr = SALOMEDS::AttributeIOR::_narrow( aGAttr );
             iorAttr->SetValue(value.c_str());
             break;
      case Double:
             value=splitXML(dump());
             DEBTRACE(value);
             aGAttr=aBuilder->FindOrCreateAttribute(aSO,"AttributeReal");
             realAttr = SALOMEDS::AttributeReal::_narrow( aGAttr );
             d=atof(value.c_str());
             realAttr->SetValue(d);
             break;
      case Int:
             value=splitXML(dump());
             DEBTRACE(value);
             aGAttr=aBuilder->FindOrCreateAttribute(aSO,"AttributeInteger");
             intAttr = SALOMEDS::AttributeInteger::_narrow( aGAttr );
             v=atol(value.c_str());
             intAttr->SetValue(v);
             break;
      case String:
      case Bool:
             value=splitXML(dump());
             DEBTRACE(value);
             aGAttr=aBuilder->FindOrCreateAttribute(aSO,"AttributeComment");
             commentAttr = SALOMEDS::AttributeComment::_narrow( aGAttr );
             commentAttr->SetValue(value.c_str());
             break;
      default:
             value=dump();
             DEBTRACE(value);
             aGAttr=aBuilder->FindOrCreateAttribute(aSO,"AttributeComment");
             commentAttr = SALOMEDS::AttributeComment::_narrow( aGAttr );
             commentAttr->SetValue(value.c_str());
    }
}

} //end namespace ENGINE
} //end namespace YACS
