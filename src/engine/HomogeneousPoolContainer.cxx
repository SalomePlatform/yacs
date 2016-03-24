// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

#include "HomogeneousPoolContainer.hxx"
#include "Exception.hxx"

using namespace YACS::ENGINE;

const char HomogeneousPoolContainer::SIZE_OF_POOL_KEY[]="SizeOfPool";

const char HomogeneousPoolContainer::INITIALIZE_SCRIPT_KEY[]="InitializeScriptKey";

void HomogeneousPoolContainer::attachOnCloning() const
{
  _isAttachedOnCloning=true;
}

void HomogeneousPoolContainer::setAttachOnCloningStatus(bool val) const
{
  _isAttachedOnCloning=true;
  if(val)
    return ;
  else
    throw Exception("An HomogeneousPoolContainer cannot be detached on cloning #2 !");
}

void HomogeneousPoolContainer::dettachOnCloning() const
{
  _isAttachedOnCloning=true;
  throw Exception("An HomogeneousPoolContainer cannot be detached on cloning !");
}

/*!
 * By definition an HomogeneousPoolContainer instance is attached on cloning.
 */
bool HomogeneousPoolContainer::isAttachedOnCloning() const
{
  return true;
}

HomogeneousPoolContainer::HomogeneousPoolContainer()
{
  _isAttachedOnCloning=true;
}

HomogeneousPoolContainer::~HomogeneousPoolContainer()
{
}
