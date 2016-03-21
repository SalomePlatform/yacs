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

#include "Cstr2d.hxx"
#include "Exception.hxx"

#include <iostream>
#include <sstream>

//! replacement for atof witch does not work everywhere
/*!
 * When using xml parser (parser.cxx) from YACSGui_XMLDriver (in GUI context),
 * instruction like double d = atof(content); where content = "0.8"
 * gives d = 0 .
 * the same binary code called from outside GUI context works fine...
 */ 
 
double Cstr2d(const char* s)
{
  std::istringstream ss(s);
  double d;
  if (!(ss >> d))
    {
      std::stringstream msg;
      msg << "problem in conversion from string to double: " << s ;
      throw YACS::Exception(msg.str());
    }
  return d;
}
