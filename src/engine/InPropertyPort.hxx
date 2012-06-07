// Copyright (C) 2006-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef __INPROPERTYPORT_HXX__
#define __INPROPERTYPORT_HXX__

#include <string>
#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class OutPort;

   /*! \brief class for Input Property Ports
    *
    * \ingroup Ports
    *
    */
    class YACSLIBENGINE_EXPORT InPropertyPort : public InputPort
    {
      friend class Runtime; // for port creation
      friend class OutPort;
      friend class Node;

    public:
      static const char NAME[];
    public:
      virtual ~InPropertyPort();

      std::string getNameOfTypeOfCurrentInstance() const;
      //! returns the final physical port behind 'this'.
#ifdef NOCOVARIANT
      virtual InPort *getPublicRepresentant() { return this; }
#else
      virtual InPropertyPort *getPublicRepresentant() { return this; }
#endif
      virtual std::string typeName() {return "YACS__ENGINE__InPropertyPort";}
      virtual void exNewPropertyValue(const std::string & name, const std::string & value);

      virtual void exSaveInit();
      virtual void exRestoreInit();
      virtual InPropertyPort *clone(Node *newHelder) const;
      virtual void *get() const;
      virtual void put(const void *data) throw(ConversionException);
      virtual void put(YACS::ENGINE::Any *data);

    protected:
      InPropertyPort(const InPropertyPort& other, Node *newHelder);
      InPropertyPort(const std::string& name, Node *node, TypeCode* type, bool canBeNull = false);

      YACS::ENGINE::Any* _property_data;
      YACS::ENGINE::Any* _init_property_data;
    };

  }
}

#endif
