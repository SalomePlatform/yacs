// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef CPPCPPCONV_HXX_
#define CPPCPPCONV_HXX_

#include "CppPorts.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputCppPort;
/*! \brief Class for conversion from C++ Output port to C++ Input port
 *
 * \ingroup Ports
 *
 */
    class CppCpp : public ProxyPort
    {
    public:
      CppCpp(InputCppPort* p);
      virtual void put(const void *data);
      void put(Any *data);
    };

  }
}



#endif /*CPPCPPCONV_HXX_*/
