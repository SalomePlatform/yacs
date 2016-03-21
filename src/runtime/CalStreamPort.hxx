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

#ifndef __CALSTREAMPORT_HXX__
#define __CALSTREAMPORT_HXX__

#include "yacsconfig.h"
#ifdef DSC_PORTS
#include "DSC_Engines.hh"
#endif

#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    /*! \brief Class for Input Calcium DataStream Ports
     *
     * \ingroup Ports
     *
     */
    class InputCalStreamPort : public InputDataStreamPort
    {
      public:
        static const char NAME[];
        InputCalStreamPort(const std::string& name, Node *node, TypeCode* type);
        InputCalStreamPort(const InputCalStreamPort& other, Node *newHelder);
        virtual ~InputCalStreamPort();
        void setProperty(const std::string& name, const std::string& value);
        std::string getNameOfTypeOfCurrentInstance() const;
        InputCalStreamPort * clone(Node *newHelder) const;
#ifdef DSC_PORTS
        virtual void initPortProperties();
#endif
        void setSchema(const std::string& value);
        std::string getSchema(){return _schema;};
        void setLevel(const std::string& value);
        int getLevel(){return _level;};
        void setDepend(const std::string& value);
        std::string getDepend(){return _depend;};
        void setDelta(const std::string& value);
        double getDelta(){return _delta;};
        void setAlpha(const std::string& value);
        double getAlpha(){return _alpha;};
        void setInterp(const std::string& value);
        std::string getInterp(){return _interp;};
        void setExtrap(const std::string& value);
        std::string getExtrap(){return _extrap;};
      protected:
        std::string _depend;
        std::string _schema;
        std::string _interp;
        std::string _extrap;
        int _level;
        double _delta;
        double _alpha;
    };

    /*! \brief Class for Output Calcium DataStream Ports
     *
     * \ingroup Ports
     *
     */
    class OutputCalStreamPort : public OutputDataStreamPort
    {
      public:
        static const char NAME[];
        OutputCalStreamPort(const std::string& name, Node *node, TypeCode* type);
        OutputCalStreamPort(const OutputCalStreamPort& other, Node *newHelder);
        virtual ~OutputCalStreamPort();
        void setDepend(const std::string& depend);
        void setLevel(const std::string& schema);
        std::string getDepend(){return _depend;};
        int getLevel(){return _level;};
        void setSchema(const std::string& schema);
        std::string getSchema(){return _schema;};
        void setProperty(const std::string& name, const std::string& value);
        virtual bool addInPort(InPort *inPort) throw(Exception);
        std::string getNameOfTypeOfCurrentInstance() const;
        OutputCalStreamPort * clone(Node *newHelder) const;
        virtual int removeInPort(InPort *inPort, bool forward) throw(Exception);
      protected:
        std::string _depend;
        std::string _schema;
        int _level;
        double _delta;
    };
  }
}

#endif
