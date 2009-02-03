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
#ifndef _PRESETPORTS_HXX_
#define _PRESETPORTS_HXX_

#include <Python.h>
#include "XMLPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {

/*! \brief Class for PRESET output Ports 
 *
 * \ingroup Ports
 *
 * \see PresetNode
 */
    class OutputPresetPort: public OutputXmlPort
    {
    public:
      OutputPresetPort(const std::string& name,  Node* node, TypeCode* type);
      OutputPresetPort(const OutputPresetPort& other, Node *newHelder);
      void setData(std::string data);
      std::string getData();
      virtual void checkBasicConsistency() const throw(Exception);
      virtual std::string dump();
      virtual std::string getAsString();
      virtual PyObject* getPyObj();
      virtual std::string typeName() {return "YACS__ENGINE__OutputPresetPort";}
    protected:
      std::string _storeData;
    };

/*! \brief Class for PRESET input Ports
 *
 * \ingroup Ports
 *
 * \see OutNode
 */
    class InputPresetPort: public InputXmlPort
    {
    public:
      InputPresetPort(const std::string& name,  Node* node, TypeCode* type);
      InputPresetPort(const InputPresetPort& other, Node *newHelder);
      void setData(std::string data);
      std::string getData();
      virtual std::string dump();
      virtual std::string getAsString();
      virtual PyObject* getPyObj();
      virtual std::string typeName() {return "YACS__ENGINE__InputPresetPort";}
    protected:
      std::string _storeData;
    };

  }
}


#endif
