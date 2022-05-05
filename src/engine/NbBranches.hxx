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

#pragma once

#include "YACSlibEngineExport.hxx"
#include "AnyInputPort.hxx"

#include <memory>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class YACSLIBENGINE_EXPORT NbBranchesAbstract
    {
      public:
        virtual std::unique_ptr<NbBranchesAbstract> copy(Node *node) const = 0;
        virtual int getNumberOfBranches(int nbOfElems) const = 0;
        virtual bool isMyName(const std::string& name) const = 0;
        virtual void exInit(bool start) = 0;
        virtual InputPort *getPort() const = 0;
        virtual bool isMultiplicitySpecified(unsigned& value) const = 0;
        virtual void forceMultiplicity(unsigned value) = 0;
        virtual int getIntValue() const = 0;
        static bool IsBranchPortName(const std::string& name);
      protected:
        static const char NAME_OF_NUMBER_OF_BRANCHES[];
    };

    class YACSLIBENGINE_EXPORT NbBranches : public NbBranchesAbstract
    {
      public:
        NbBranches(Node *node):_nbOfBranches(NAME_OF_NUMBER_OF_BRANCHES,node,Runtime::_tc_int) { }
        NbBranches(const NbBranches& other, Node *node):_nbOfBranches(other._nbOfBranches,node) { }
        bool isMyName(const std::string& name) const override;
        std::unique_ptr<NbBranchesAbstract> copy(Node *node) const override;
        int getNumberOfBranches(int nbOfElems) const override;
        void exInit(bool start) override;
        InputPort *getPort() const override;
        bool isMultiplicitySpecified(unsigned& value) const override;
        void forceMultiplicity(unsigned value) override;
        int getIntValue() const override { return _nbOfBranches.getIntValue(); }
      private:
        AnyInputPort _nbOfBranches;
    };

    class YACSLIBENGINE_EXPORT NoNbBranches : public NbBranchesAbstract
    {
      public:
        NoNbBranches() = default;
        std::unique_ptr<NbBranchesAbstract> copy(Node *node) const override;
        int getNumberOfBranches(int nbOfElems) const override;
        bool isMyName(const std::string& name) const override { return false; }
        void exInit(bool start) override;
        InputPort *getPort() const override;
        bool isMultiplicitySpecified(unsigned& value) const override;
        void forceMultiplicity(unsigned value) override;
        int getIntValue() const override;
    };
  }
}
