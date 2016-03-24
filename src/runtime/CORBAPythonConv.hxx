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

#ifndef __CORBAPYTHONCONV_HXX__
#define __CORBAPYTHONCONV_HXX__

#include <omniORB4/CORBA.h>
#include "InputPort.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputPyPort;

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyDouble : public ProxyPort
    {
    public:
      CorbaPyDouble(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyInt : public ProxyPort
    {
    public:
      CorbaPyInt(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyString : public ProxyPort
    {
    public:
      CorbaPyString(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * Convert boolean data
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyBool : public ProxyPort
    {
    public:
      CorbaPyBool(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyObjref : public ProxyPort
    {
    public:
      CorbaPyObjref(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

/*! \brief Class for conversion from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPySequence : public ProxyPort
    {
    public:
      CorbaPySequence(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    protected:
      DynamicAny::DynAnyFactory_ptr _dynfactory;
    };

/*! \brief Class for conversion of struct objects from CORBA Output port to Python Input port
 *
 * \ingroup AdaptorPorts
 *
 */
    class CorbaPyStruct : public ProxyPort
    {
    public:
      CorbaPyStruct(InputPyPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

  }
}
#endif
