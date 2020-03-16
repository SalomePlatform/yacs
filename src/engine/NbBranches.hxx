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

#pragma once

#include "YACSlibEngineExport.hxx"
#include "AnyInputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class YACSLIBENGINE_EXPORT NbBranches
    {
      public:
        NbBranches(Node *node):_nbOfBranches(NAME_OF_NUMBER_OF_BRANCHES,node,Runtime::_tc_int) { }
        NbBranches(const NbBranches& other, Node *node):_nbOfBranches(other._nbOfBranches,node) { }
        void exInit(bool start);
        InputPort *getPort() const;
        bool isMultiplicitySpecified(unsigned& value) const;
        void forceMultiplicity(unsigned value);
        int getIntValue() const { return _nbOfBranches.getIntValue(); }
        static bool IsBranchPortName(const std::string& name);
      private:
        AnyInputPort _nbOfBranches;
        static const char NAME_OF_NUMBER_OF_BRANCHES[];
    };
  }
}
