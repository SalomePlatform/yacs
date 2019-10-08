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
#include "NotSimpleCasePoint.hxx"

#include "Bloc.hxx"
#include "LinkInfo.hxx"

#include <sstream>
#include <algorithm>

using namespace YACS::ENGINE;

BagPoint::BagPoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father):BlocPoint(nodes,father)
{
}

void BagPoint::accept(PointVisitor *pv)
{
  for(auto it : _nodes)
    {
      if(!it)
        throw YACS::Exception("BagPoint::accept : Ooops !");
      it->accept(pv);
    }
}

AbstractPoint *BagPoint::expandNonSimpleCaseOn(NotSimpleCasePoint *pathologicalPt, const std::set<Node *>& uncatchedNodes)
{
  throw YACS::Exception("BagPoint::expandNonSimpleCaseOn : Ooops !");
}

AbstractPoint *BagPoint::deepCopy(AbstractPoint *father) const
{
  BagPoint *ret(new BagPoint);
  ret->deepCopyFrom(*this);
  ret->setFather(father);
  return ret;
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

void BagPoint::deal2Quatro(bool& somethingDone)
{
  somethingDone=false;
  for(std::list<AbstractPoint *>::iterator it=_nodes.begin();it!=_nodes.end();it++)
    {
      if(!(*it)->isNullBeforeNullAfter(this))
        continue;
      ForkBlocPoint *try1((*it)->tryAsForkQuatro(this));
      if(try1)
        {
          replaceInMe(try1);
          somethingDone=true;
          break;
        }
      }
}

/*!
 * This method remove 1, 2 or 3 points from this by flagging them as pathological nodes.
 * By doing so, we hope to simplify enough to have a standart case.
 */
void BagPoint::dealNotSimpleCase(bool& somethingDone)
{
  somethingDone=false;
  for(auto it=_nodes.begin();it!=_nodes.end();it++)
    {
      std::vector<AbstractPoint *> v{*it};
      AbstractPoint::TryAsNotSimpleCase((*it)->getFather(),v,_nodes,somethingDone);
      if(somethingDone)
        break;
    }
  if(somethingDone)
    return ;
  //
  for(auto it=_nodes.cbegin();it!=_nodes.cend();it++)
    {
      auto it2(it); it2++;
      for(;it2!=_nodes.cend();it2++)
        {
          std::vector<AbstractPoint *> v{*it,*it2};
          AbstractPoint::TryAsNotSimpleCase((*it)->getFather(),v,_nodes,somethingDone);
          if(somethingDone)
            return;
        }
    }
  if(somethingDone)
    return ;
  //
  for(auto it=_nodes.cbegin();it!=_nodes.cend();it++)
    {
      auto it2(it); it2++;
      for(;it2!=_nodes.cend();it2++)
        {
          auto it3(it2); it3++;
          for(;it3!=_nodes.cend();it3++)
            {
              std::vector<AbstractPoint *> v{*it,*it2,*it3};
              AbstractPoint::TryAsNotSimpleCase((*it)->getFather(),v,_nodes,somethingDone);
              if(somethingDone)
                return;
            }
        }
    }
}

/*!
 * Append a single pathologicalPt point instance to AbstractPoint instances contained in \a this.
 * To do that we start to locate in graph dependancy in \a this with ENGINE::Node cannot run in parallel with \a pathologicalPt. These nodes stored in \a nodesBeforeAfter.
 *
 * By walking across Points in \a this if a Point has no intersection with \a nodesBeforeAfter this Point is converted to a ForkBlocPoint.
 */
void BagPoint::expandNonSimpleCaseOn(AbstractPoint *pathologicalPt)
{
  NotSimpleCasePoint *pathologicalPt2(dynamic_cast<NotSimpleCasePoint *>(pathologicalPt));
  if(!pathologicalPt2)
    throw YACS::Exception("BagPoint::expandNonSimpleCaseOn : pathologicalPt is expected to be a NotSimpleCasePoint !");
  std::list<AbstractPoint *> nodes2;
  std::for_each(_nodes.begin(),_nodes.end(),[&nodes2](AbstractPoint *elt) { if(!dynamic_cast<NotSimpleCasePoint *>(elt)) nodes2.push_back(elt); });
  if(nodes2.size()!=1)
    throw YACS::Exception("BagPoint::expandNonSimpleCaseOn : Internal error only one AbstractPoint is expected !");
  
  Node *firstNode(pathologicalPt->getFirstNode()),*lastNode(pathologicalPt->getLastNode());
  //
  ComposedNode *parent(firstNode->getFather());
  Bloc *parentc(dynamic_cast<Bloc *>(parent));
  if(!parentc)
    throw YACS::Exception("BagPoint::expandNonSimpleCaseOn : internal error, Bloc expected");
  //
  std::set<Node *> nodesBefore, nodesAfter;
  {
    std::map<Node *, std::set<Node *> > accelStr;
    LinkInfo info(E_ALL);
    parentc->findAllNodesStartingFrom<false>(firstNode,nodesBefore,accelStr,info);
  }
  {
    std::map<Node *, std::set<Node *> > accelStr;
    LinkInfo info(E_ALL);
    parentc->findAllNodesStartingFrom<true>(lastNode,nodesAfter,accelStr,info);
  }
  //
  std::set<Node *> nodesBeforeAfter(nodesBefore.begin(),nodesBefore.end());
  nodesBeforeAfter.insert(nodesAfter.begin(),nodesAfter.end());
  //std::for_each(nodesBeforeAfter.begin(),nodesBeforeAfter.end(),[](Node *elt) { std::cerr << elt->getName() << " "; }); std::cerr << std::endl;
  //
  AbstractPoint *onlyOne(*nodes2.begin());
  AbstractPoint *res(onlyOne->expandNonSimpleCaseOn(pathologicalPt2,nodesBeforeAfter));
  if(res!=onlyOne)
    throw YACS::Exception("BagPoint::expandNonSimpleCaseOn : unexpected situation !");// impossible because by construction pathologicalPt cannot be merged to this. If it were possible, algo in simplify would be simply it !
  {// remove delete pathologicalPt and remove it from this->_nodes
    std::list<AbstractPoint *> nodes3;
    std::for_each(_nodes.begin(),_nodes.end(),[&nodes3,pathologicalPt](AbstractPoint *elt) { if(elt!=pathologicalPt) nodes3.push_back(elt); else delete elt; });
    _nodes = nodes3;
  }
}

/*!
 * All NotSimpleCasePoint instances in this->_nodes are going to be scanned recursively accross this->_nodes.
 * To do that NotSimpleCasePoint instance are added incrementaly.
 */
void BagPoint::expandNonSimpleCase()
{
  std::vector<AbstractPoint *> pathologicalPt;
  std::for_each(_nodes.begin(),_nodes.end(),[&pathologicalPt](AbstractPoint *elt) { if(dynamic_cast<NotSimpleCasePoint *>(elt)) pathologicalPt.push_back(elt); });
  if(pathologicalPt.empty())
    throw YACS::Exception("BagPoint::expandNonSimpleCase : Not found any pathological case !");
  for(auto it : pathologicalPt)
    this->expandNonSimpleCaseOn(it);
  
}
