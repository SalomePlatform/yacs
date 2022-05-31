// Copyright (C) 2006-2022  CEA/DEN, EDF R&D
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

#include "NbBranches.hxx"

using namespace YACS::ENGINE;

const char NbBranchesAbstract::NAME_OF_NUMBER_OF_BRANCHES[]="nbBranches";

bool NbBranchesAbstract::IsBranchPortName(const std::string& name)
{
  return name == NAME_OF_NUMBER_OF_BRANCHES;
}

std::unique_ptr<NbBranchesAbstract> NbBranches::copy(Node *node) const
{
  return std::unique_ptr<NbBranchesAbstract>(new NbBranches(*this,node));
}

int NbBranches::getNumberOfBranches(int) const
{
  return this->getIntValue();
}

bool NbBranches::isMyName(const std::string& name) const
{
  return NbBranchesAbstract::IsBranchPortName(name);
}

void NbBranches::exInit(bool start)
{
  _nbOfBranches.exInit(start);
}

InputPort *NbBranches::getPort() const
{
  return const_cast<AnyInputPort *>(&_nbOfBranches);
}

bool NbBranches::isMultiplicitySpecified(unsigned& value) const
{
  if(_nbOfBranches.edIsManuallyInitialized())
    if(_nbOfBranches.edGetNumberOfLinks()==0)
    {
      value=_nbOfBranches.getIntValue();
      return true;
    }
  return false;
}

void NbBranches::forceMultiplicity(unsigned value)
{
  _nbOfBranches.edRemoveAllLinksLinkedWithMe();
  _nbOfBranches.edInit((int)value);
}

std::unique_ptr<NbBranchesAbstract> NoNbBranches::copy(Node *) const
{
  return std::unique_ptr<NbBranchesAbstract>(new NoNbBranches);
}

int NoNbBranches::getNumberOfBranches(int nbOfElems) const
{
  return nbOfElems;
}

void NoNbBranches::exInit(bool start)
{
}

InputPort *NoNbBranches::getPort() const
{
  return nullptr;
}

bool NoNbBranches::isMultiplicitySpecified(unsigned& value) const
{
  return false;
}

void NoNbBranches::forceMultiplicity(unsigned value)
{
  throw Exception("NoNbBranches::forceMultiplicity : impossible to be forced !");
}

int NoNbBranches::getIntValue() const
{
  throw Exception("NoNbBranches::getIntValue : no value stored !");
}
