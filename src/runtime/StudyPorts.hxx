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
#ifndef _STUDYPORTS_HXX_
#define _STUDYPORTS_HXX_

#include "XMLPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {

/*! \brief Class for Study output Ports 
 *
 * \ingroup Ports
 *
 * \see StudyInNode
 */
    class OutputStudyPort: public OutputXmlPort
    {
    public:
      OutputStudyPort(const std::string& name,  Node* node, TypeCode* type);
      OutputStudyPort(const OutputStudyPort& other, Node *newHelder);
      void setData(const std::string& data);
      std::string getData();
      virtual void putIOR(const std::string& ior);
      virtual std::string dump();
      virtual std::string getPyObj();
      virtual std::string getAsString();
      virtual std::string typeName() {return "YACS__ENGINE__OutputStudyPort";}
    protected:
      std::string _storeData;
    };

/*! \brief Class for Study input Ports
 *
 * \ingroup Ports
 *
 * \see StudyOutNode
 */
    class InputStudyPort: public InputXmlPort
    {
    public:
      InputStudyPort(const std::string& name,  Node* node, TypeCode* type);
      InputStudyPort(const InputStudyPort& other, Node *newHelder);
      void setData(const std::string& data);
      std::string getData();
      virtual std::string getIOR();
      virtual std::string splitXML(const std::string& s);
      virtual std::string dump();
      virtual std::string getPyObj();
      virtual std::string getAsString();
      virtual std::string typeName() {return "YACS__ENGINE__InputStudyPort";}
    protected:
      std::string _storeData;
    };

  }
}


#endif
