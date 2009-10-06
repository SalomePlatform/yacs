//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "SalomeOptimizerLoop.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

/*! \class YACS::ENGINE::SalomeOptimizerLoop
 *  \brief class to build optimization loops
 *
 * \ingroup Nodes
 */

SalomeOptimizerLoop::SalomeOptimizerLoop(const std::string& name, const std::string& algLibWthOutExt,
                                         const std::string& symbolNameToOptimizerAlgBaseInstanceFactory, bool algInitOnFile) throw(YACS::Exception) :
                     OptimizerLoop(name,algLibWthOutExt,symbolNameToOptimizerAlgBaseInstanceFactory,algInitOnFile)
{
}

SalomeOptimizerLoop::SalomeOptimizerLoop(const OptimizerLoop& other, ComposedNode *father, bool editionOnly): 
                     OptimizerLoop(other,father,editionOnly)
{
}

SalomeOptimizerLoop::~SalomeOptimizerLoop()
{
}

Node *SalomeOptimizerLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new SalomeOptimizerLoop(*this,father,editionOnly);
}

