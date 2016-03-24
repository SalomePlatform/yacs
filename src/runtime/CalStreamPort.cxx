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

#include "CalStreamPort.hxx"
#include "SalomeComponent.hxx"
#include "CORBANode.hxx"
#include "Calcium_Ports.hh"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char InputCalStreamPort::NAME[]="InputCalStreamPort";

InputCalStreamPort::InputCalStreamPort(const std::string& name, Node *node, TypeCode* type):
  InputDataStreamPort(name, node, type),
  DataPort(name, node, type),
  Port(node),_depend("TIME_DEPENDENCY"),_schema("TI_SCHEM"),_delta(-1.),_level(-1),_alpha(0.),
    _interp("L1_SCHEM"),_extrap("UNDEFINED_EXTRA_SCHEM")
{
}
InputCalStreamPort::InputCalStreamPort(const InputCalStreamPort& other, Node *newHelder):
  InputDataStreamPort(other,newHelder),
  DataPort(other,newHelder),
  Port(other,newHelder),_depend(other._depend),_schema(other._schema),_delta(other._delta),_level(other._level),
    _alpha(other._alpha),_interp(other._interp),_extrap(other._extrap)
{
}

InputCalStreamPort::~InputCalStreamPort()
{
}

void InputCalStreamPort::setDepend(const std::string& depend)
{
  DEBTRACE("InputCalStreamPort::setDepend: " << edGetNumberOfLinks());
  if(edGetNumberOfLinks() > 0)
    {
      throw Exception("Can not modify DependencyType property on a connected port");
    }
  if(depend != "TIME_DEPENDENCY" && depend != "ITERATION_DEPENDENCY")
    {
      throw Exception("DependencyType property must be TIME_DEPENDENCY or ITERATION_DEPENDENCY");
    }
  _depend=depend;
}

void InputCalStreamPort::setSchema(const std::string& schema)
{
  if(schema != "TI_SCHEM" && schema != "TF_SCHEM" && schema != "ALPHA_SCHEM")
    {
      throw Exception("DateCalSchem property must be TI_SCHEM, TF_SCHEM or ALPHA_SCHEM");
    }
  _schema=schema;
}
void InputCalStreamPort::setLevel(const std::string& value)
{
  DEBTRACE("InputCalStreamPort::setLevel: " << value);
  std::istringstream iss(value);
  int temp;
  if (!(iss >> temp)|| temp<1)
    throw Exception("StorageLevel property must be an integer > 0");
  _level=temp;
}
void InputCalStreamPort::setAlpha(const std::string& value)
{
  std::istringstream iss(value);
  double temp;
  if (!(iss >> temp)||temp<0. || temp >1.)
    throw Exception("Alpha property must be a float > 0 and < 1");
  _alpha=temp;
}
void InputCalStreamPort::setDelta(const std::string& value)
{
  std::istringstream iss(value);
  double temp;
  if (!(iss >> temp)||temp<0. || temp >1.)
    throw Exception("DeltaT property must be > 0 and < 1");
  _delta=temp;
}
void InputCalStreamPort::setInterp(const std::string& value)
{
  if(value != "L0_SCHEM" && value != "L1_SCHEM")
    {
      throw Exception("InterpolationSchem property must be L0_SCHEM or L1_SCHEM");
    }
  _interp=value;
}
void InputCalStreamPort::setExtrap(const std::string& value)
{
  if(value != "E0_SCHEM" && value != "E1_SCHEM")
    {
      throw Exception("ExtrapolationSchem property must be E0_SCHEM or E1_SCHEM");
    }
  _extrap=value;
}

void InputCalStreamPort::setProperty(const std::string& name, const std::string& value)
{
  DEBTRACE("InputCalStreamPort::setProperty: " << name << " " << value);
  if(name == "DependencyType")
    setDepend(value);
  else if(name == "DateCalSchem")
    setSchema(value);
  else if(name == "StorageLevel")
    setLevel(value);
  else if(name == "Alpha")
    setAlpha(value);
  else if(name == "DeltaT")
    setDelta(value);
  else if(name == "InterpolationSchem")
    setInterp(value);
  else if(name == "ExtrapolationSchem")
    setExtrap(value);
  InputDataStreamPort::setProperty(name,value);
}

#ifdef DSC_PORTS
void InputCalStreamPort::initPortProperties()
{

  CORBA::Object_var objComponent=((SalomeComponent*)((SalomeNode*)_node)->getComponent())->getCompoPtr();
  Engines::Superv_Component_var compo=Engines::Superv_Component::_narrow(objComponent);
  Ports::PortProperties_var port_properties=compo->get_port_properties(getName().c_str());
  CORBA::Any a;
  std::string prop;
  try
    {
      DEBTRACE(_level);
      if(_level>0)
        {
          prop="StorageLevel";
          a <<= (CORBA::Long)_level;
          port_properties->set_property(prop.c_str(),a);
        }

      if(_schema != "TI_SCHEM")
        {
          prop="DateCalSchem";
          if(_schema == "TF_SCHEM")
            a <<= Ports::Calcium_Ports::TF_SCHEM;
          else if(_schema == "ALPHA_SCHEM")
            a <<= Ports::Calcium_Ports::ALPHA_SCHEM;
          port_properties->set_property(prop.c_str(),a);
        }

      if(_schema == "ALPHA_SCHEM" && _alpha > 0.)
        {
          prop="Alpha";
          a <<= (CORBA::Double)_alpha;
          port_properties->set_property(prop.c_str(),a);
        }

      if(_delta >= 0.)
        {
          prop="DeltaT";
          a <<= (CORBA::Double)_delta;
          port_properties->set_property(prop.c_str(),a);
        }
      if(_interp == "L0_SCHEM" )
        {
          prop="InterpolationSchem";
          a <<= Ports::Calcium_Ports::L0_SCHEM;
          port_properties->set_property(prop.c_str(),a);
        }
      if(_extrap != "UNDEFINED_EXTRA_SCHEM" )
        {
          prop="ExtrapolationSchem";
          if(_extrap == "E0_SCHEM" )
            a <<= Ports::Calcium_Ports::E0_SCHEM;
          else if(_extrap == "E1_SCHEM" )
            a <<= Ports::Calcium_Ports::E1_SCHEM;
          port_properties->set_property(prop.c_str(),a);
        }
    }
  catch(Ports::NotDefined& ex)
    {
      throw Exception("Property "+prop+ " not defined on that port: "+getName());
    }
}
#endif

std::string InputCalStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

InputCalStreamPort *InputCalStreamPort::clone(Node *newHelder) const
{
  return new InputCalStreamPort(*this,newHelder);
}

const char OutputCalStreamPort::NAME[]="OutputCalStreamPort";

OutputCalStreamPort::OutputCalStreamPort(const std::string& name, Node *node, TypeCode* type):
  OutputDataStreamPort(name, node, type),
  DataPort(name, node, type),
  Port(node),_depend("TIME_DEPENDENCY"),_schema("TI_SCHEM"),_level(-1)
{
}

OutputCalStreamPort::OutputCalStreamPort(const OutputCalStreamPort& other, Node *newHelder):
  OutputDataStreamPort(other,newHelder),
  DataPort(other,newHelder),
  Port(other,newHelder),
  _depend(other._depend),_schema(other._schema),_level(other._level)
{
}

OutputCalStreamPort::~OutputCalStreamPort()
{
}

void OutputCalStreamPort::setDepend(const std::string& depend)
{
  DEBTRACE("OutputCalStreamPort::setDepend: " << edGetNumberOfOutLinks());
  if(edGetNumberOfOutLinks() > 0)
    {
      throw Exception("Can not modify DependencyType property on a connected port");
    }
  if(depend != "TIME_DEPENDENCY" && depend != "ITERATION_DEPENDENCY")
    {
      throw Exception("DependencyType property must be TIME_DEPENDENCY or ITERATION_DEPENDENCY");
    }
  _depend=depend;
}

void OutputCalStreamPort::setSchema(const std::string& schema)
{
  if(schema != "TI_SCHEM" && schema != "TF_SCHEM" && schema != "ALPHA_SCHEM")
    {
      throw Exception("DateCalSchem property must be TI_SCHEM, TF_SCHEM or ALPHA_SCHEM");
    }
  _schema=schema;
}
void OutputCalStreamPort::setLevel(const std::string& level)
{
  _level =atoi(level.c_str());
}

void OutputCalStreamPort::setProperty(const std::string& name, const std::string& value)
{
  if(name == "DependencyType")
    setDepend(value);
  else if(name == "DateCalSchem")
    setSchema(value);
  else if(name == "StorageLevel")
    setLevel(value);
  OutputDataStreamPort::setProperty(name,value);
}

std::string OutputCalStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

OutputCalStreamPort *OutputCalStreamPort::clone(Node *newHelder) const
{
  return new OutputCalStreamPort(*this,newHelder);
}

bool OutputCalStreamPort::addInPort(InPort *inPort) throw(YACS::Exception)
{
  DEBTRACE("OutputCalStreamPort::addInPort " << InputCalStreamPort::NAME );
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputCalStreamPort::NAME)
    {
      string what="not compatible type of port requested during building of link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }

  InputCalStreamPort* port=static_cast<InputCalStreamPort*>(inPort);
  if(port->getDepend() != _depend)
    {
      std::string what= "incompatible DependencyType properties: "+_depend+" != "+ port->getDepend();
      throw Exception(what);
    }
  bool ret;
  ret= edAddInputDataStreamPort(port);
  return ret;
}


int OutputCalStreamPort::removeInPort(InPort *inPort, bool forward) throw(YACS::Exception)
{
  DEBTRACE("OutputCalStreamPort::removeInPort");
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputCalStreamPort::NAME && !forward)
    {
      string what="not compatible type of port requested during destruction of for link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  return edRemoveInputDataStreamPort(static_cast<InputDataStreamPort *>(inPort),forward);
}

