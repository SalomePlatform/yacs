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

#ifndef __OUTPUTDATASTREAMPORT_HXX__
#define __OUTPUTDATASTREAMPORT_HXX__

#include "YACSlibEngineExport.hxx"
#include "OutPort.hxx"
#include "DataStreamPort.hxx"
#include "ConversionException.hxx"

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class ElementaryNode;
    class InputDataStreamPort;

    class YACSLIBENGINE_EXPORT OutputDataStreamPort : public DataStreamPort, public OutPort
    {
      friend class ElementaryNode;
    protected:
      std::set<InputDataStreamPort *> _setOfInputDataStreamPort;
    public:
      static const char NAME[];
    public:
      OutputDataStreamPort(const OutputDataStreamPort& other, Node *newHelder);
      OutputDataStreamPort(const std::string& name, Node *node, TypeCode* type);
      virtual ~OutputDataStreamPort();
      virtual OutputDataStreamPort *clone(Node *newHelder) const;
      std::set<InPort *> edSetInPort() const;
      bool isAlreadyLinkedWith(InPort *withp) const;
      virtual std::string getNameOfTypeOfCurrentInstance() const;
      virtual bool addInPort(InPort *inPort) throw(Exception);
      virtual bool edAddInputDataStreamPort(InputDataStreamPort *port) throw(ConversionException);
      int edRemoveInputDataStreamPort(InputDataStreamPort *inPort, bool forward) throw(Exception);
      void edRemoveAllLinksLinkedWithMe() throw(Exception);
      int removeInPort(InPort *inPort, bool forward) throw(Exception);
      virtual std::string typeName() {return "YACS__ENGINE__OutputDataStreamPort";}
    private:
      bool isAlreadyInSet(InputDataStreamPort *inPort) const;
    };
  }
}

#endif
