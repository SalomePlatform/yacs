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

#include "PropertyInterface.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

/*! \class YACS::ENGINE::PropertyInterface
 *  \brief Class for adding property interface to another class
 *
 */

PropertyInterface::~PropertyInterface()
{
}

void PropertyInterface::setProperty(const std::string& name, const std::string& value)
{
  DEBTRACE("PropertyInterface::setProperty " << name << " " << value);
  _propertyMap[name]=value;
}

//! Get a property value given its name
/*!
 *  \param name : the property name
 *  \return the property value
 */
std::string PropertyInterface::getProperty(const std::string& name)
{
  DEBTRACE("PropertyInterface::getProperty " << name );
  return _propertyMap[name];
}

void PropertyInterface::setProperties(std::map<std::string,std::string> properties)
{
  DEBTRACE("PropertyInterface::setProperties " );
  _propertyMap.clear();
  std::map<std::string,std::string>::iterator it;
  for (it = properties.begin(); it != properties.end(); ++it)
    {
      setProperty(it->first, it->second); // setProperty virtual and derived
    }
}
