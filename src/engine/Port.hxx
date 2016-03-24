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

#ifndef __PORT_HXX__
#define __PORT_HXX__

#include "YACSlibEngineExport.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Node;

/*! \brief Base class for all ports
 *
 * Not instanciable class that factorizes all basic data and behaviours relative 
 * to the in and out interfaces of all nodes.
 * End-user should neither instanciate a sub-class of 'Port' 
 * nor call other methods than accessor.
 *
 * \ingroup Ports
 */
    class YACSLIBENGINE_EXPORT Port
    {
    public:
      virtual ~Port();
      Node *getNode() const { return _node; }
      virtual std::string getNameOfTypeOfCurrentInstance() const;
      int getNumId() const { return _id; }
      virtual std::string typeName() {return "YACS__ENGINE__Port";}
      void modified();
    protected:
      Port(Node *node);
      Port(const Port& other, Node *newHelder);
    protected:
      Node        *_node;
      int          _id;
      static int   _total;
      static const char NAME[];
    };
  }
}

#endif
