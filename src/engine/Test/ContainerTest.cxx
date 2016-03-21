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

#include "ContainerTest.hxx"
#include "ComponentInstance.hxx"
#include "ToyNode.hxx"
#include <sstream>

using namespace std;
using namespace YACS::ENGINE;

unsigned ContainerTest::_counter = 0;

const char ContainerTest::SUPPORTED_COMP_KIND[] = "TESTKIND1";

unsigned ContainerTest2::_counter = 0;

const char ContainerTest2::SUPPORTED_COMP_KIND[] = "TESTKIND2";

ContainerTest::ContainerTest():_alreadyStarted(false),_myCounter(_counter++)
{
}

std::string ContainerTest::getKind() const
{
  return std::string();
}

std::string ContainerTest::getPlacementInfo() const
{
  ostringstream stream;
  stream << "Cont#_" << _myCounter;
  return stream.str();
}

bool ContainerTest::isAlreadyStarted(const Task *askingNode) const
{
  return _alreadyStarted;
}

void ContainerTest::start(const Task *askingNode) throw(YACS::Exception)
{
  if(_alreadyStarted)
    throw Exception("ContainerTest already started !!!!");
  _alreadyStarted=true;
}

Container *ContainerTest::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (Container *) this;
    }
  else
    return new ContainerTest;
}

Container *ContainerTest::cloneAlways() const
{
  return new ContainerTest;
}

void ContainerTest::checkCapabilityToDealWith(const ComponentInstance *inst) const throw(YACS::Exception)
{
  if(inst->getKind()!=SUPPORTED_COMP_KIND)
    throw Exception("ContainerTest not compatible with this type of instance.");
}

void ContainerTest::initAllContainers()
{
  _counter=0;
}

ContainerTest2::ContainerTest2():_alreadyStarted(false),_myCounter(_counter++)
{
}

std::string ContainerTest2::getKind() const
{
  return std::string();
}

bool ContainerTest2::isAlreadyStarted(const Task *askingNode) const
{
  return _alreadyStarted;
}

void ContainerTest2::start(const Task *askingNode) throw(YACS::Exception)
{
  if(_alreadyStarted)
    throw Exception("ContainerTest already started !!!!");
  _alreadyStarted=true;
}

Container *ContainerTest2::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (Container *) this;
    }
  else
    return new ContainerTest2;
}

Container *ContainerTest2::cloneAlways() const
{
  return new ContainerTest2;
}

void ContainerTest2::initAllContainers()
{
  _counter=0;
}

void ContainerTest2::checkCapabilityToDealWith(const ComponentInstance *inst) const throw(YACS::Exception)
{
  if(inst->getKind()!=SUPPORTED_COMP_KIND)
    throw Exception("ContainerTest not compatible with this type of instance.");
}
