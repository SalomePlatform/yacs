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

#ifndef _DATAPARSERS_HXX_
#define _DATAPARSERS_HXX_

#include "parserBase.hxx"

namespace YACS
{
/*! \brief Class for string parser.
 *
 *  Class used to parse string
 */
struct stringtypeParser:parser
{
  static stringtypeParser stringParser;
  std::string post();
};

/*! \brief Class for double parser.
 *
 *  Class used to parse double 
 */
struct doubletypeParser:parser
{
  static doubletypeParser doubleParser;
  double post();
};

/*! \brief Class for integer parser.
 *
 *  Class used to parse integer
 */
struct inttypeParser:parser
{
  static inttypeParser intParser;
  int post();
};

/*! \brief Class for boolean parser.
 *
 *  Class used to parse bool
 */
struct booltypeParser:parser
{
  static booltypeParser boolParser;
  bool post();
};

}

#endif
