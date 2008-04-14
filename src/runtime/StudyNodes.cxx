#include "RuntimeSALOME.hxx"
#include "StudyNodes.hxx"
#include "StudyPorts.hxx"
#include "Visitor.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOMEDS.hh"
#include "SALOMEDS_Attributes.hh"

#include <iostream>
#include <sstream>
#include <string>
#include <list>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

namespace YACS
{
namespace ENGINE
{

const char StudyInNode::IMPL_NAME[]="XML";

StudyInNode::StudyInNode(const std::string& name)
  : DataNode(name)
{
  _implementation=IMPL_NAME;
}

StudyInNode::StudyInNode(const StudyInNode& other, ComposedNode *father)
  : DataNode(other, father)
{
}

Node *StudyInNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new StudyInNode(*this,father);
}

OutputPort* StudyInNode::createOutputPort(const std::string& outputPortName, TypeCode* type)
{
  return new OutputStudyPort(outputPortName, this, type);
}

void StudyInNode::setData(OutputPort* port, std::string& data)
{
  OutputStudyPort *outp = dynamic_cast<OutputStudyPort *>(port);
  outp->setData(data);
}

void StudyInNode::execute()
{
  DEBTRACE("+++++++ StudyInNode::execute +++++++++++");
  SALOME_NamingService NS(getSALOMERuntime()->getOrb());
  CORBA::Object_var obj=NS.Resolve("/myStudyManager");
  if(CORBA::is_nil(obj)) 
    {
      _errorDetails="Execution problem: no naming service";
      throw Exception(_errorDetails);
    }

  SALOMEDS::StudyManager_var aStudyManager = SALOMEDS::StudyManager::_narrow(obj);
  if(CORBA::is_nil(aStudyManager)) 
    {
      _errorDetails="Execution problem: no naming service";
      throw Exception(_errorDetails);
    }

  int studyid=1;
  if (_propertyMap.find("StudyID") != _propertyMap.end())
    {
      // StudyId is specified
      studyid=atoi(_propertyMap["StudyID"].c_str());
    }

  SALOMEDS::Study_var myStudy =aStudyManager->GetStudyByID(studyid);
  if(CORBA::is_nil(myStudy)) 
    {
      std::stringstream msg;
      msg << "Execution problem: no study with id " << studyid;
      _errorDetails=msg.str();
      throw Exception(_errorDetails);
    }

  std::list<OutputPort *>::const_iterator iter;
  for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
      OutputStudyPort *outp = dynamic_cast<OutputStudyPort *>(*iter);
      std::string data = outp->getData();
      DEBTRACE("data: " << data );
      //try an id
      SALOMEDS::SObject_var aSO = myStudy->FindObjectID(data.c_str());
      if(CORBA::is_nil(aSO)) 
        {
          //try a path
          aSO=myStudy->FindObjectByPath(data.c_str());
          if(CORBA::is_nil(aSO)) 
            {
              _errorDetails="Execution problem: no id or path: ";
              _errorDetails=_errorDetails+data+" in study";
              throw Exception(_errorDetails);
            }
        }

      CORBA::String_var path=myStudy->GetObjectPath(aSO);
      DEBTRACE(path);
      CORBA::String_var id=aSO->GetID();
      DEBTRACE(id);
      //CORBA::Object_var sobj=aSO->GetObject();
      SALOMEDS::GenericAttribute_var aGAttr;

      CORBA::String_var value;
      if ( aSO->FindAttribute( aGAttr, "AttributeIOR" ) )
        {
          SALOMEDS::AttributeIOR_var anAttr = SALOMEDS::AttributeIOR::_narrow( aGAttr );
          value=anAttr->Value();
        }
      else
        {
          //Problem !!!
          _errorDetails="Execution problem: no AttributeIOR in study object: ";
          _errorDetails=_errorDetails+data;
          throw Exception(_errorDetails);
        }
      outp->putIOR((const char*)value);
    }
  DEBTRACE("+++++++ end StudyInNode::execute +++++++++++" );
}

void StudyInNode::checkBasicConsistency() const throw(Exception)
{
  DEBTRACE("StudyInNode::checkBasicConsistency");
  if (! _setOfInputPort.empty())
    {
      std::string what = "StudyNode ";
      what += getName();
      what += " only accepts OutputStudyPort, no InputPort";
      throw Exception(what);
    }

  std::list<OutputPort *>::const_iterator iter;
  for(iter=_setOfOutputPort.begin();iter!=_setOfOutputPort.end();iter++)
    {
      OutputStudyPort *inp = dynamic_cast<OutputStudyPort*>(*iter);
      if (!inp)
        {
          std::string what("Output port: ");
          what += (*iter)->getName();
          what += " is not an OutputStudyPort. StudyNode ";
          what += getName();
          what += " only accepts OutputStudyPorts";
          throw Exception(what);
        }

      std::string data = inp->getData();
      DEBTRACE(data);
      if (data.empty())
        {
          std::string what("OutputStudyPort: ");
          what += (*iter)->getName();
          what += " is not initialised";
          throw Exception(what);
        }
    }
}

void StudyInNode::accept(Visitor *visitor)
{
  visitor->visitStudyInNode(this);
}

const char StudyOutNode::IMPL_NAME[]="XML";

StudyOutNode::StudyOutNode(const std::string& name)
  : DataNode(name)
{
  _implementation=IMPL_NAME;
}

StudyOutNode::StudyOutNode(const StudyOutNode& other, ComposedNode *father)
  : DataNode(other, father)
{
}

Node *StudyOutNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new StudyOutNode(*this,father);
}

InputPort* StudyOutNode::createInputPort(const std::string& inputPortName, TypeCode* type)
{
  return new InputStudyPort(inputPortName, this, type);
}

void StudyOutNode::setData(InputPort* port, std::string& data)
{
  InputStudyPort *inp = dynamic_cast<InputStudyPort *>(port);
  inp->setData(data);
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

void StudyOutNode::execute()
{
  DEBTRACE("+++++++ StudyOutNode::execute +++++++++++");
  SALOME_NamingService NS(getSALOMERuntime()->getOrb());
  CORBA::Object_var obj=NS.Resolve("/myStudyManager");
  if(CORBA::is_nil(obj))
    {
      _errorDetails="Execution problem: no naming service";
      throw Exception(_errorDetails);
    }

  SALOMEDS::StudyManager_var aStudyManager = SALOMEDS::StudyManager::_narrow(obj);
  if(CORBA::is_nil(aStudyManager))
    {
      _errorDetails="Execution problem: no naming service";
      throw Exception(_errorDetails);
    }

  int studyid=1;
  if (_propertyMap.find("StudyID") != _propertyMap.end())
    {
      // StudyId is specified
      studyid=atoi(_propertyMap["StudyID"].c_str());
    }

  SALOMEDS::Study_var myStudy =aStudyManager->GetStudyByID(studyid);
  if(CORBA::is_nil(myStudy))
    {
      //open a new one
      std::stringstream msg;
      msg << "Study" << studyid;
      myStudy=aStudyManager->NewStudy(msg.str().c_str());
      if(CORBA::is_nil(myStudy))
        {
          _errorDetails="Execution problem: can not create new study " + msg.str();
          throw Exception(_errorDetails);
        }
    }
  DEBTRACE(myStudy->StudyId());

  SALOMEDS::StudyBuilder_var aBuilder =myStudy->NewBuilder() ;
  if(CORBA::is_nil(aBuilder))
    {
      _errorDetails="Execution problem: can not create StudyBuilder";
      throw Exception(_errorDetails);
    }

  SALOMEDS::GenericAttribute_var aGAttr;
  SALOMEDS::SObject_var aSO ;
  SALOMEDS::AttributeName_var anAttr ;
  SALOMEDS::AttributeIOR_var iorAttr ;

  std::list<InputPort *>::const_iterator iter;
  for(iter = _setOfInputPort.begin(); iter != _setOfInputPort.end(); iter++)
    {
      InputStudyPort *inp = dynamic_cast<InputStudyPort *>(*iter);
      std::string data = inp->getData();
      DEBTRACE("data: " << data );
      //try to find an id
      aSO = myStudy->FindObjectID(data.c_str());
      if(CORBA::is_nil(aSO)) 
        {
          // the id does not exist. Try to create it by id
          aSO=myStudy->CreateObjectID(data.c_str());
          if(!CORBA::is_nil(aSO)) 
            {
              aGAttr=aBuilder->FindOrCreateAttribute(aSO,"AttributeName");
              anAttr = SALOMEDS::AttributeName::_narrow( aGAttr );
              anAttr->SetValue(inp->getName().c_str());
            }
        }
      if(CORBA::is_nil(aSO)) 
        {
          // try a path
          aSO=myStudy->FindObjectByPath(data.c_str());
        }
      if(CORBA::is_nil(aSO)) 
        {
          //try to create it by path
          std::string name;
          std::string::size_type begin = data.find_first_not_of("/");
          std::string::size_type pos=data.find_first_of("/", begin);
          if (pos != std::string::npos)
            name=data.substr(begin,pos-begin);
          else
            name=data.substr(begin);
          name="/"+name;
          DEBTRACE(name);
          aSO=myStudy->FindObjectByPath(name.c_str());
          if(CORBA::is_nil(aSO)) 
            {
              DEBTRACE("Create an entry " << name);
              //create a container component
              aSO=aBuilder->NewComponent(name.c_str());
              if(CORBA::is_nil(aSO)) 
                {
                  std::cerr << "Execution problem: can not create component: " + data << std::endl;
                  continue;
                }
              aGAttr=aBuilder->FindOrCreateAttribute(aSO,"AttributeIOR");
              iorAttr = SALOMEDS::AttributeIOR::_narrow( aGAttr );
              iorAttr->SetValue(name.c_str());
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
          continue;
        }
      std::string value=inp->getIOR();
      DEBTRACE(value);
      aGAttr=aBuilder->FindOrCreateAttribute(aSO,"AttributeIOR");
      iorAttr = SALOMEDS::AttributeIOR::_narrow( aGAttr );
      iorAttr->SetValue(value.c_str());
    }

  // save in file if ref is given
  if(_ref != "")
    {
      aStudyManager->SaveAs(_ref.c_str(),myStudy, false);
    }
  DEBTRACE("+++++++ end StudyOutNode::execute +++++++++++" );
}

void StudyOutNode::checkBasicConsistency() const throw(Exception)
{
  DEBTRACE("StudyOutNode::checkBasicConsistency");
  if (! _setOfOutputPort.empty())
    {
      std::string what = "StudyNode ";
      what += getName();
      what += " only accepts InputStudyPort, no OutputPort";
      throw Exception(what);
    }

  std::list<InputPort *>::const_iterator iter;
  for(iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    {
      InputStudyPort *inp = dynamic_cast<InputStudyPort*>(*iter);
      if (!inp)
        {
          std::string what("Input port: ");
          what += (*iter)->getName();
          what += " is not an InputStudyPort. StudyNode ";
          what += getName();
          what += " only accepts InputStudyPorts";
          throw Exception(what);
        }
      inp->checkBasicConsistency();

      std::string data = inp->getData();
      DEBTRACE(data);
      if (data.empty())
        {
          std::string what("InputStudyPort: ");
          what += (*iter)->getName();
          what += " is not initialised";
          throw Exception(what);
        }
    }

}

void StudyOutNode::accept(Visitor *visitor)
{
  visitor->visitStudyOutNode(this);
}

} //end namespace ENGINE
} //end namespace YACS
