// Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

#include "AbstractResourceModel.hxx"
#include <sstream>
#include <iostream>

AbstractResourceModel::AbstractResourceModel()
{
}

AbstractResourceModel::~AbstractResourceModel()
{
}

std::string AbstractResourceModel::getMaxDuration()const
{
  int hours = getMaxDurationHours();
  int minutes = getMaxDurationMinutes();
  if(hours<0)
    hours = 0;
  if(hours>99)
    hours = 99;
  if(minutes<0)
    minutes = 5;
  if(minutes > 59)
    minutes = 59;
  std::stringstream ss;
  // hours=0 && minutes=0 => use default values
  if(hours > 0 || minutes > 0)
  {
    if(hours<10)
      ss << "0";
    ss << hours << ":";
    if(minutes<10)
      ss << "0";
    ss << minutes;
  }
  return ss.str();
}
