// Copyright (C) 2015-2016  CEA/DEN, EDF R&D
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

#include "BlocPoint.hxx"
#include "Node.hxx"

using namespace YACS::ENGINE;

BlocPoint::BlocPoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father):AbstractPoint(father),_nodes(nodes)
{
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
    (*it)->setFather(this);
}

AbstractPoint *BlocPoint::findPointWithNode(Node *node)
{
  for(std::list<AbstractPoint *>::iterator it=_nodes.begin();it!=_nodes.end();it++)
    {
      AbstractPoint *ret((*it)->findPointWithNode(node));
      if(ret)
        return AbstractPoint::GetDirectSonOf(this,ret);
    }
  return 0;
}

AbstractPoint *BlocPoint::getNodeAfter(Node *node)
{
  OutGate *oug(node->getOutGate());
  std::list<InGate *> fl(oug->edSetInGate());
  if(fl.size()>=1)
    {
      AbstractPoint *ret(0);
      IsCommonDirectSonOf(this,fl,ret);
      return ret;
    }
  else
    return 0;
}

AbstractPoint *BlocPoint::getNodeB4(Node *node)
{
  InGate *ing(node->getInGate());
  std::list<OutGate *> bl(ing->getBackLinks());
  if(bl.size()>=1)
    {
      AbstractPoint *ret(0);
      IsCommonDirectSonOf(this,bl,ret);
      return ret;
    }
  else
    return 0;
}

bool BlocPoint::contains(Node *node)
{
  for(std::list<AbstractPoint *>::iterator it=_nodes.begin();it!=_nodes.end();it++)
    {
      if((*it)->contains(node))
        return true;
    }
  return false;
}

int BlocPoint::getNumberOfNodes() const
{
  int ret(0);
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
    ret+=(*it)->getNumberOfNodes();
  return ret;
}

BlocPoint::~BlocPoint()
{
  for(std::list<AbstractPoint *>::iterator it=_nodes.begin();it!=_nodes.end();it++)
    delete *it;
}
