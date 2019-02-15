// Copyright (C) 2006-2019  CEA/DEN, EDF R&D
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


#include "ComplexWeight.hxx"

#include "Exception.hxx"

using namespace YACS::ENGINE;

#ifdef WIN32
#include <algorithm>
#endif

ComplexWeight::ComplexWeight()
{
  setToZero();
}

ComplexWeight::ComplexWeight(double elementaryWeight, double loopWeight, int nbCoresByIteration): _elementaryWeight(elementaryWeight), _bootWeight(false)
{
  _loopWeight.push_back(std::pair<double,int>(loopWeight,nbCoresByIteration));
}

double ComplexWeight::getSimpleLoopWeight() const
{
  if(_loopWeight.size()!=1)
    throw Exception("ComplexWeight::getSimpleLoopWeight : can not get loop weight. Node contain multiple loop weights!");
  return (*(_loopWeight.begin())).first;
}

double ComplexWeight::calculateTotalLength(int nbOfCoresAllocated) const
{
  if (isDefaultValue())
    throw Exception("ComplexWeight::calculateTotalLength : can not calculate total weight with default value!");
  double totalWeight(0);
  if (hasValidElementaryWeight())
    totalWeight+=_elementaryWeight;
  for(std::vector< std::pair<double,int> >::const_iterator it=_loopWeight.begin();it!=_loopWeight.end();it++)
    {
      if ((*it).second>0)
        totalWeight+=(*it).first*((double)(*it).second)/((double)nbOfCoresAllocated);
	}
  return totalWeight;
}

void ComplexWeight::setLoopWeight(double loopWeight, int nbCoresByIteration) 
{
  bool found(false);
  int i(0), rankFound(0);
  _bootWeight=false;
  for(std::vector<std::pair<double,int> >::const_iterator it=_loopWeight.begin();it!=_loopWeight.end();it++,i++)
    {
      if ((*it).second==nbCoresByIteration)
        {
          found=true;
          rankFound=i;
        }
        
    }
  if (found)
    _loopWeight.erase(_loopWeight.begin()+rankFound);
  _loopWeight.push_back(std::pair<double,int>(loopWeight,nbCoresByIteration));
  if ((_loopWeight.size()>1) && ((*(_loopWeight.begin())).second==-1))
    _loopWeight.erase(_loopWeight.begin());  
}

int ComplexWeight::getNbCoresConsoLoopMax() const
{
  int nbCoresPerShotLoopMax(0);
  for(std::vector< std::pair<double,int> >::const_iterator it=_loopWeight.begin();it!=_loopWeight.end();it++)
    if ((*it).second>nbCoresPerShotLoopMax)nbCoresPerShotLoopMax=(*it).second;
  return nbCoresPerShotLoopMax;
}

void ComplexWeight::max(ComplexWeight &other)
{
  _elementaryWeight=std::max(_elementaryWeight,other._elementaryWeight);
  
  double allLoopWeight1(0);
  double allLoopWeight2(0);
  for(std::vector< std::pair<double,int> >::const_iterator it=_loopWeight.begin();it!=_loopWeight.end();it++)
    {
      if (((*it).first>0) && ((*it).second>0))
        allLoopWeight1+=(*it).first * (*it).second;
    }
  for(std::vector< std::pair<double,int> >::const_iterator it=other._loopWeight.begin();it!=other._loopWeight.end();it++)
    {
      if (((*it).first>0) && ((*it).second>0))
        allLoopWeight2+=(*it).first * (*it).second;
    }
  if (allLoopWeight2>allLoopWeight1)
    _loopWeight=other._loopWeight;  
}

void ComplexWeight::addWeight(const ComplexWeight *other)
{
  bool found;
  if ((!_bootWeight) && ((other->isUnsetLoopWeight() && this->isDefaultValueLoop()) || (this->isUnsetLoopWeight() && other->isDefaultValueLoop())))
    this->unsetLoop();
  else
    {
	  for(std::vector< std::pair<double,int> >::const_iterator it=other->_loopWeight.begin();it!=other->_loopWeight.end();it++)
	    {
	      found=false;
	      for(std::vector< std::pair<double,int> >::iterator it2=_loopWeight.begin();it2!=_loopWeight.end();it2++)	
	        {
	          if ((*it).second == (*it2).second)
	            {
	              if (((*it2).first>=0) && ((*it).first>=0))
	                this->setLoopWeight((*it2).first+(*it).first, (*it2).second);
	              else if ((*it).first>=0)
	                this->setLoopWeight((*it).first, (*it2).second);
	              found=true;
	              continue;
	            }            
	        }
	      if ((!found) && ((*it).second!=-1))
	        this->setLoopWeight((*it).first, (*it).second);
	    }
	  if ((_loopWeight.size()>1) && ((*(_loopWeight.begin())).second==-1))
	    _loopWeight.erase(_loopWeight.begin());
	}

  if ((!_bootWeight) && ((other->isUnsetElementaryWeight() && this->isDefaultValueElementary()) || (this->isUnsetElementaryWeight() && other->isDefaultValueElementary())))
    this->unsetElementary();
  else
    {
	  if (other->hasValidElementaryWeight())
	    {
	      if (hasValidElementaryWeight())
	        _elementaryWeight+=other->_elementaryWeight;
	      else
	        _elementaryWeight=other->_elementaryWeight;
	    }
    }
  if (!other->_bootWeight)
    _bootWeight=false;
}




