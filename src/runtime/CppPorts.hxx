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

#ifndef _CPPPORTS_HXX_
#define _CPPPORTS_HXX_


#include "InputPort.hxx"
#include "OutputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Any;

/*! \brief Class for C++ Ports
 *
 * \ingroup Ports
 *
 * \see CppNode
 */
    class InputCppPort : public InputPort
    {
    public:
      InputCppPort(const std::string& name, Node * node, TypeCode * type);
      InputCppPort(const InputCppPort& other, Node *newHelder);
      ~InputCppPort();
      bool edIsManuallyInitialized() const;
      void edRemoveManInit();
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
      InputPort *clone(Node *newHelder) const;
      virtual YACS::ENGINE::Any * getCppObj() const;
      void *get() const throw(Exception);
      virtual bool isEmpty();
      virtual void exSaveInit();
      virtual void exRestoreInit();
      virtual std::string dump();
    protected:
      YACS::ENGINE::Any* _data;
      YACS::ENGINE::Any* _initData;
    };

    class OutputCppPort : public OutputPort
    {
    public:
      OutputCppPort(const std::string& name, Node * node, TypeCode * type);
      OutputCppPort(const OutputCppPort& other, Node *newHelder);
      ~OutputCppPort();
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
      OutputPort *clone(Node *newHelder) const;
      virtual YACS::ENGINE::Any * get() const;
      virtual std::string dump();
    protected:
      YACS::ENGINE::Any* _data;
    };



  }
}

#endif
