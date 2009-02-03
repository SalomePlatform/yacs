//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __CORBACPPCONV_HXX__
#define __CORBACPPCONV_HXX__

#include <omniORB4/CORBA.h>

#include "InputPort.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // Adaptator Ports Corba->C++

    class CorbaCpp : public ProxyPort
    {
    public:
      CorbaCpp(InputPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };
    int isAdaptableCppCorba(const TypeCode *t1,const TypeCode *t2);
  }
}
#endif
