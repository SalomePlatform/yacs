// Copyright (C) 2006-2014  CEA/DEN, EDF R&D
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

#include "Container.hxx"
#include "ComponentInstance.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

Container::Container():_isAttachedOnCloning(false),_proc(0)
{
}

Container::~Container()
{
}

/*!
 * By calling this method the current container 'this' is not destined to be deeply copied on clone call.
 */
void Container::attachOnCloning() const
{
  _isAttachedOnCloning=true;
}

/*!
 * By calling this method the current container 'this' will be deeply copied on clone call.
 */
void Container::dettachOnCloning() const
{
  _isAttachedOnCloning=false;
}

bool Container::isAttachedOnCloning() const
{
  return _isAttachedOnCloning;
}

/*!
 * This method informs about the capability of the container to deal with CT an unpredictably number of components.
 * By default : True
 */
bool Container::isSupportingRTODefNbOfComp() const
{
  return true;
}

void Container::setProperties(const std::map<std::string,std::string>& properties)
{
  for (std::map<std::string,std::string>::const_iterator it=properties.begin();it!=properties.end();++it)
    setProperty((*it).first,(*it).second);
}

