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

#ifndef NEUTRALCPPCONV_HXX_
#define NEUTRALCPPCONV_HXX_

#include "InputPort.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputCppPort;
/*! \brief Class for conversion from Neutral Output port to Xml Input port
 *
 * \ingroup Ports
 *
 */
    class NeutralCpp : public ProxyPort
    {
    public:
      NeutralCpp(InputCppPort* p);
      virtual void put(const void *data);
      void put(YACS::ENGINE::Any *data);
    };
    int isAdaptableCppNeutral(const TypeCode * t1, const TypeCode * t2);
  }
}


#endif /*NEUTRALCPPCONV_HXX_*/
