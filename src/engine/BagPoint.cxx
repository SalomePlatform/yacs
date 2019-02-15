// Copyright (C) 2015-2019  CEA/DEN, EDF R&D
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

#include "BagPoint.hxx"
#include "Exception.hxx"
#include "LinkedBlocPoint.hxx"
#include "ForkBlocPoint.hxx"

#include <sstream>
#include <algorithm>

using namespace YACS::ENGINE;

BagPoint::BagPoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father):BlocPoint(nodes,father)
{
}

AbstractPoint *BagPoint::getUnique()
{
  if(_nodes.size()!=1)
    throw YACS::Exception("BagPoint::getUnique : invalid call !");
  else
    {
      AbstractPoint *ret(*_nodes.begin());
      if(!ret)
        throw YACS::Exception("BagPoint::getUnique : Ooops !");
      return ret;
    }
}

const AbstractPoint *BagPoint::getUnique() const
{
  if(_nodes.size()!=1)
    throw YACS::Exception("BagPoint::getUnique const : invalid call !");
  else
    {
      AbstractPoint *ret(*_nodes.begin());
      if(!ret)
        throw YACS::Exception("BagPoint::getUnique : Ooops !");
      return ret;
    }
}

AbstractPoint *BagPoint::getUniqueAndReleaseIt()
{
  AbstractPoint *ret(getUnique());
  getOutPoint(ret);
  return ret;
}

void BagPoint::accept(PointVisitor *pv)
{
  if(_nodes.size()!=1)
    throw YACS::Exception("BagPoint::accept : simplification has failed !");
  AbstractPoint *ret(*_nodes.begin());
  if(!ret)
    throw YACS::Exception("BagPoint::accept : Ooops !");
  ret->accept(pv);
}

Node *BagPoint::getFirstNode()
{
  return getUnique()->getFirstNode();
}

Node *BagPoint::getLastNode()
{
  return getUnique()->getLastNode();
}

int BagPoint::getMaxLevelOfParallelism() const
{
  return getUnique()->getMaxLevelOfParallelism();
}

void BagPoint::getWeightRegardingDPL(ComplexWeight *weight)
{
  getUnique()->getWeightRegardingDPL(weight);
}

void BagPoint::partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const
{
  getUnique()->partitionRegardingDPL(pd,zeMap);
}

std::string BagPoint::getRepr() const
{
  std::ostringstream oss;
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
    oss << (*it)->getRepr() << " - ";
  return oss.str();
}

#include <iostream>

void BagPoint::replaceInMe(BlocPoint *aSet)
{
  const std::list<AbstractPoint *>& pts(aSet->getListOfPoints());
  for(std::list<AbstractPoint *>::const_iterator it0=pts.begin();it0!=pts.end();it0++)
    {
      std::list<AbstractPoint *>::iterator it1(std::find(_nodes.begin(),_nodes.end(),*it0));
      if(it1==_nodes.end())
        throw Exception("SetOfPoints::replaceInMe : internal error !");
      _nodes.erase(it1);
    }
  _nodes.push_back(aSet);
}

void BagPoint::deal1(bool& somethingDone)
{
  somethingDone=false;
  for(std::list<AbstractPoint *>::iterator it=_nodes.begin();it!=_nodes.end();it++)
    {
      if(!(*it)->isSimplyLinkedBeforeAfter(this))
        if(!(*it)->isSimplyLinkedAfterNullBefore(this) && !(*it)->isSimplyLinkedBeforeNullAfter(this))
           continue;
      LinkedBlocPoint *try0((*it)->tryAsLink(this));
      if(try0)
        {
          replaceInMe(try0);
          somethingDone=true;
          break;
        }
    }
}

void BagPoint::deal2(bool& somethingDone)
{
  somethingDone=false;
  for(std::list<AbstractPoint *>::iterator it=_nodes.begin();it!=_nodes.end();it++)
    {
      if(!(*it)->isSimplyLinkedBeforeAfter(this))
        continue;
      ForkBlocPoint *try1((*it)->tryAsFork(this));
      if(try1)
        {
          replaceInMe(try1);
          somethingDone=true;
          break;
        }
    }
}

void BagPoint::deal2Bis(bool& somethingDone)
{
  somethingDone=false;
  for(std::list<AbstractPoint *>::iterator it=_nodes.begin();it!=_nodes.end();it++)
    {
      if(!(*it)->isSimplyLinkedAfterNullBefore(this))
        continue;
      ForkBlocPoint *try1((*it)->tryAsForkBis(this));
      if(try1)
        {
          replaceInMe(try1);
          somethingDone=true;
          break;
        }
    }
}

void BagPoint::deal2Ter(bool& somethingDone)
{
  somethingDone=false;
  for(std::list<AbstractPoint *>::iterator it=_nodes.begin();it!=_nodes.end();it++)
    {
      if(!(*it)->isSimplyLinkedBeforeNullAfter(this))
        continue;
      ForkBlocPoint *try1((*it)->tryAsForkTer(this));
      if(try1)
        {
          replaceInMe(try1);
          somethingDone=true;
          break;
        }
      }
}

