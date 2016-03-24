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

#include "RuntimeSALOME.hxx"
#include "StudyNodes.hxx"
#include "StudyPorts.hxx"
#include "Visitor.hxx"
#include "TypeCode.hxx"
#include "SalomeProc.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOMEDS.hh"
#include "SALOMEDS_Attributes.hh"

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <stdlib.h>

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

void StudyInNode::setData(OutputPort* port, const std::string& data)
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
  if (getProperty("StudyID") != "")
    {
      // StudyId is specified
      studyid=atoi(getProperty("StudyID").c_str());
    }
  else
    {
      Proc* p=getProc();
      if(p)
        {
          std::string value=p->getProperty("DefaultStudyID");
          if(!value.empty())
            studyid= atoi(value.c_str());
        }
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
      try
        {
          outp->getDataFromStudy(myStudy);
        }
      catch(Exception& e)
        {
          _errorDetails=e.what();
          throw;
        }
    }
  DEBTRACE("+++++++ end StudyInNode::execute +++++++++++" );
}

void StudyInNode::checkBasicConsistency() const throw(YACS::Exception)
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

void StudyOutNode::setData(InputPort* port, const std::string& data)
{
  InputStudyPort *inp = dynamic_cast<InputStudyPort *>(port);
  inp->setData(data);
}

/*
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
*/

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
  if (getProperty("StudyID") != "")
    {
      // StudyId is specified
      studyid=atoi(getProperty("StudyID").c_str());
    }
  else
    {
      Proc* p=getProc();
      if(p)
        {
          std::string value=p->getProperty("DefaultStudyID");
          if(!value.empty())
            studyid= atoi(value.c_str());
        }
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
      inp->putDataInStudy(myStudy,aBuilder);
    }

  // save in file if ref is given
  if(_ref != "")
    {
      aStudyManager->SaveAs(_ref.c_str(),myStudy, false);
    }
  DEBTRACE("+++++++ end StudyOutNode::execute +++++++++++" );
}

void StudyOutNode::checkBasicConsistency() const throw(YACS::Exception)
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
