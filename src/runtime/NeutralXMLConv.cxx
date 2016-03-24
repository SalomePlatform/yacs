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
#include "NeutralXMLConv.hxx"

#include <iostream>
#include <sstream>
#include "Any.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

NeutralXml::NeutralXml(InputXmlPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void NeutralXml::put(const void *data) throw(ConversionException)
{
  DEBTRACE(" NeutralXml::put(const void *data)");
  put((YACS::ENGINE::Any *)data);
}

//!Convert received Neutral (Any *) value to XML value and send it to proxy port
 /*!
  *   \param data : YACS::ENGINE::Any object
  */

void NeutralXml::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  DEBTRACE("NeutralXml::put " << data);
  std::string sss = convertNeutralXml(edGetType(),data);
  ((InputXmlPort*)_port)->put((const char*)sss.c_str());
}
