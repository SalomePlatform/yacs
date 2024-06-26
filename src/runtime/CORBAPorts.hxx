// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef _CORBAPORTS_HXX_
#define _CORBAPORTS_HXX_

#include <omniORB4/CORBA.h>

#include "YACSRuntimeSALOMEExport.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "Mutex.hxx"

#include <Python.h>

#include <string>

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for CORBA Input Ports
 *
 * \ingroup Ports
 *
 * \see CORBANode
 */
    class YACSRUNTIMESALOME_EXPORT InputCorbaPort : public InputPort
    {
    public:
      InputCorbaPort(const std::string& name, Node *node, TypeCode * type);
      InputCorbaPort(const InputCorbaPort& other, Node *newHelder);
      virtual ~InputCorbaPort();
      bool edIsManuallyInitialized() const;
      void edRemoveManInit();
      virtual void put(const void *data);
      void put(CORBA::Any *data);
      void releaseData() override;
      InputPort *clone(Node *newHelder) const;
      void *get() const ;
      virtual bool isEmpty();
      virtual CORBA::Any * getAny();
      virtual std::string getAsString();
      virtual PyObject* getPyObj();
      virtual void exSaveInit();
      virtual void exRestoreInit();
      virtual std::string dump();
      virtual std::string typeName() {return "YACS__ENGINE__InputCorbaPort";}
      virtual std::string valToStr();
      virtual void valFromStr(std::string valstr);
    protected:
      CORBA::Any  _data;
      CORBA::Any *  _initData;
      CORBA::ORB_ptr _orb;
    private:
      YACS::BASES::Mutex _mutex;
    };

    class OutputCorbaPort;

    YACSRUNTIMESALOME_EXPORT std::ostream & operator<<(std::ostream &os,
                                                       const YACS::ENGINE::OutputCorbaPort& p);

    class YACSRUNTIMESALOME_EXPORT OutputCorbaPort : public OutputPort
    {
      friend std::ostream &operator<< ( std::ostream &os,
                                        const OutputCorbaPort& p);
    public:
      OutputCorbaPort(const std::string& name, Node *node, TypeCode * type);
      OutputCorbaPort(const OutputCorbaPort& other, Node *newHelder);
      virtual ~OutputCorbaPort();
      virtual void put(const void *data);
      void put(CORBA::Any *data);
      OutputPort *clone(Node *newHelder) const;
      virtual CORBA::Any * getAny();
      virtual CORBA::Any * getAnyOut();
      virtual std::string getAsString();
      virtual PyObject* getPyObj();
      virtual std::string dump();
      virtual std::string typeName() {return "YACS__ENGINE__OutputCorbaPort";}
      virtual std::string valToStr();
      virtual void valFromStr(std::string valstr);
    protected:
      CORBA::Any  _data;
      CORBA::ORB_ptr _orb;
    private:
      YACS::BASES::Mutex _mutex;
    };


  }
}

#endif
