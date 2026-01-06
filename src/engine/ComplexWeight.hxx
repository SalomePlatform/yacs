// Copyright (C) 2006-2026  CEA, EDF
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

#ifndef __COMPLEXWEIGHT_HXX__
#define __COMPLEXWEIGHT_HXX__

#include "YACSlibEngineExport.hxx"

#include <vector>

#ifdef WIN32
#ifdef max
#undef max
#endif
#endif


namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT ComplexWeight
    {
    public:
      ComplexWeight();
      ComplexWeight(double elementaryWeight, double loopWeight, int nbCoresByIteration);
      ComplexWeight(const ComplexWeight& other) {_bootWeight=other._bootWeight; _loopWeight=other._loopWeight; _elementaryWeight=other._elementaryWeight;}
      ~ComplexWeight() {};
      std::vector<std::pair<double,int> > getLoopWeight() const {return _loopWeight;}
      double getSimpleLoopWeight() const;
      double getElementaryWeight() const {return _elementaryWeight;}
      double calculateTotalLength(int nbOfCoresAllocated) const;
      void setDefaultElementary() {setToZero(); setLoopWeight(0.,0);}
      void setDefaultLoop() {setToZero(); setElementaryWeight(0.);}
      bool isDefaultValue() const { return ((isDefaultValueElementary()) && (isDefaultValueLoop()));}
      bool isUnsetLoopWeight() const {return ((*(_loopWeight.begin())).second==0);}
      bool isUnsetElementaryWeight() const {return (_elementaryWeight==0);}
      bool hasValidLoopWeight() const { return (((*(_loopWeight.begin())).first>0) && ((*(_loopWeight.begin())).second!=-1));}
      bool hasValidElementaryWeight() const { return (_elementaryWeight>=0);}
      void setLoopWeight(double loopWeight, int nbCoresByIteration);
      void setElementaryWeight(double elementaryWeight) {_bootWeight=false; _elementaryWeight=elementaryWeight;}
      void setToZero() {_bootWeight=true; unsetElementary(); unsetLoop(); }
      int getNbCoresConsoLoopMax() const;
      void max(ComplexWeight &other);
      void addWeight(const ComplexWeight *other);
    protected:
      bool _bootWeight;
      // _loopWeight: vect<pair(weight,nbcorePerIteration)>, for first element of vector: nbcorePerIteration<0 -> unset, nbcorePerIteration==0 -> no loopweight (means no loop inside)
      std::vector<std::pair<double,int> > _loopWeight; 
      double _elementaryWeight;
    private:
      void unsetLoop() {_loopWeight.clear(); _loopWeight.push_back(std::pair<double,int>(-1.,-1));}
      void unsetElementary(){_elementaryWeight=-1.;}
      bool isDefaultValueLoop() const {return ((*(_loopWeight.begin())).second==-1);}
      bool isDefaultValueElementary() const {return (_elementaryWeight<0);}
    };
  }
}

#endif















