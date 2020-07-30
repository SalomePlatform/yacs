// Copyright (C) 2006-2020  CEA/DEN, EDF R&D
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

#include "Basics_Utils.hxx"
#include "SALOME_NamingService.hxx"
#include "SALOME_KernelServices.hxx"
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

  std::list<OutputPort *>::const_iterator iter;
  for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
      OutputStudyPort *outp = dynamic_cast<OutputStudyPort *>(*iter);
      try
        {
          outp->getDataFromStudy();
        }
      catch(Exception& e)
        {
          _errorDetails=e.what();
          throw;
        }
    }
  DEBTRACE("+++++++ end StudyInNode::execute +++++++++++" );
}

void StudyInNode::checkBasicConsistency() const
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
SALOMEDS::SObject_ptr findOrCreateSoWithName(SALOMEDS::StudyBuilder_ptr builder,
                                             SALOMEDS::SObject_ptr sobj, const std::string& name)
{
  SALOMEDS::ChildIterator_var anIterator= KERNEL::getStudyServant()->NewChildIterator(sobj);
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

  SALOMEDS::StudyBuilder_var aBuilder =KERNEL::getStudyServant()->NewBuilder() ;
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
      inp->putDataInStudy(aBuilder);
    }

  // save in file if ref is given
  if(_ref != "")
    {
      KERNEL::getStudyServant()->SaveAs(Kernel_Utils::decode_s( _ref ), false, false);
    }
  DEBTRACE("+++++++ end StudyOutNode::execute +++++++++++" );
}

void StudyOutNode::checkBasicConsistency() const
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
