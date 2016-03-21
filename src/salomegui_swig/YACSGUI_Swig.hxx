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

#ifndef YACSGUI_SWIG_HXX
#define YACSGUI_SWIG_HXX

#include <string>

class YACS_Swig
{
public:
  YACS_Swig();
  virtual ~YACS_Swig();
  virtual void activate(const std::string& module="YACS");
  virtual void real_activate(const std::string& module);
  virtual void loadSchema(const std::string& filename,bool edit=true, bool arrangeLocalNodes=false);
  virtual void real_loadSchema(const std::string& filename,bool edit, bool arrangeLocalNodes);
};

#endif // YACSGUI_SWIG_HXX

