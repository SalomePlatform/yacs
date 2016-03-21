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

#ifndef _XMLPORTS_HXX_
#define _XMLPORTS_HXX_

#include "YACSRuntimeSALOMEExport.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for XML Input Ports
 *
 * \ingroup Ports
 *
 * \see XmlNode
 */
    class YACSRUNTIMESALOME_EXPORT InputXmlPort : public InputPort
    {
    public:
      InputXmlPort(const std::string& name, Node* node, TypeCode * type);
      InputXmlPort(const InputXmlPort& other, Node *newHelder);
      bool edIsManuallyInitialized() const;
      void edRemoveManInit();
      virtual void put(const void *data) throw (ConversionException);
      void put(const char *data) throw (ConversionException);
      InputPort *clone(Node *newHelder) const;
      virtual const char * getXml() const;
      void *get() const throw(Exception);
      bool isEmpty();
      virtual void exSaveInit();
      virtual void exRestoreInit();
      virtual std::string dump();
      virtual std::string valToStr();
      virtual void valFromStr(std::string valstr);
    protected:
      std::string _data;
      std::string _initData;
    };
    
/*! \brief Class for XML Output Ports
 *
 * \ingroup Ports
 *
 * \see XmlNode
 */
    class YACSRUNTIMESALOME_EXPORT OutputXmlPort : public OutputPort
    {
    public:
      OutputXmlPort(const std::string& name,  Node* node, TypeCode * type);
      OutputXmlPort(const OutputXmlPort& other, Node *newHelder);
      virtual void put(const void *data) throw (ConversionException);
      void put(const char *data) throw (ConversionException);
      virtual const char * get() const throw (ConversionException);
      OutputPort *clone(Node *newHelder) const;
      virtual std::string dump();
      virtual std::string valToStr();
      virtual void valFromStr(std::string valstr);
    protected:
      std::string _data;
    };

  }
}

#endif
