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

#include "TypeConversions.hxx"
#include "CppXMLConv.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS 
{
  namespace ENGINE 
  {
    std::string convertCppXml(const TypeCode *t, void *ob)
    {
      return convertNeutralXml(t, (Any *) ob);
    }

    CppXml::CppXml(InputXmlPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
      DEBTRACE("proxy port from C++ to XML");
    }

    //!Convert a YACS::ENGINE::Any that is convertible to Xml::char * and send it to proxy port
    /*!
     *   \param data : YACS::ENGINE::Any object as a void * pointer
     */

    void CppXml::put(const void *data) throw(ConversionException)
    {
      put((YACS::ENGINE::Any *)data);
    }

    //!Convert a YACS::ENGINE::Any that is convertible to Xml::char * and send it to proxy port
    /*!
     *   \param data : YACS::ENGINE::Any object
     */
    void CppXml::put(YACS::ENGINE::Any *data) throw(ConversionException)
    {
      DEBTRACE("CppXml::put");
      std::string s = convertCppXml(edGetType(), data);
      DEBTRACE(s);
      DEBTRACE(_port->getName());
      ((InputXmlPort*)_port)->put(s.c_str());
      DEBTRACE("End CppXml::put");
    }

    int isAdaptableXmlCpp(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableXmlNeutral(t1, t2);
    }
  }
}
