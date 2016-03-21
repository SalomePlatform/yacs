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

#ifndef __INPUTDATASTREAMPORT_HXX__
#define __INPUTDATASTREAMPORT_HXX__

#include "YACSlibEngineExport.hxx"
#include "InPort.hxx"
#include "DataStreamPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    /*! \brief Class for Input DataStream Ports
     *
     * \ingroup Ports
     *
     */
    class OutputDataStreamPort;
    class YACSLIBENGINE_EXPORT InputDataStreamPort : public DataStreamPort, public InPort
    {
    public:
      static const char NAME[];
    protected:
      std::set<OutputDataStreamPort *> _setOfOutputDataStreamPort;
    public:
      InputDataStreamPort(const InputDataStreamPort& other, Node *newHelder);
      InputDataStreamPort(const std::string& name, Node *node, TypeCode* type);
      virtual ~InputDataStreamPort();
      virtual std::string getNameOfTypeOfCurrentInstance() const;
      virtual InputDataStreamPort *clone(Node *newHelder) const;
      virtual std::string typeName() {return "YACS__ENGINE__InputDataStreamPort";}

      virtual void edAddOutputDataStreamPort(OutputDataStreamPort *port);
      std::set<OutputDataStreamPort *> getConnectedOutputDataStreamPort() {return _setOfOutputDataStreamPort;}

    private:
      bool isAlreadyInSet(OutputDataStreamPort *inPort) const;
    };
  }
}

#endif
