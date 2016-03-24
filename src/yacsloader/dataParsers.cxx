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

#include "dataParsers.hxx"
#include "Cstr2d.hxx"
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

namespace YACS
{
  stringtypeParser stringtypeParser::stringParser;
  doubletypeParser doubletypeParser::doubleParser;
  inttypeParser inttypeParser::intParser;
  booltypeParser booltypeParser::boolParser;

std::string stringtypeParser::post()
    {
      return _content;
    }

  double doubletypeParser::post()
    {
      if(_content=="")return 0;
      return Cstr2d(_content.c_str());
//       std::istringstream s(_content.c_str());
//       double a;
//       if (!(s >> a))
//         throw YACS::Exception::Exception("problem in conversion from string to double");
//       std::cerr << "--------------_content s a "<< _content.c_str() << " " << s.str() << " " << a << std::endl;
//       return a;
    }

  int inttypeParser::post()
    {
      return atoi(_content.c_str());
    }

  bool booltypeParser::post()
    {
      DEBTRACE( _content )             
      if(_content == "true")return true;
      if(_content == "false")return false;
      std::stringstream temp(_content);
      bool b ;
      temp >> b;
      //std::cerr << b << std::endl;
      return b;
    }

}
