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

#include "NbBranches.hxx"

using namespace YACS::ENGINE;

const char NbBranches::NAME_OF_NUMBER_OF_BRANCHES[]="nbBranches";

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

bool NbBranches::IsBranchPortName(const std::string& name)
{
  return name == NAME_OF_NUMBER_OF_BRANCHES;
}