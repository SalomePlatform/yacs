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

#ifndef __INPUTPORT_HXX__
#define __INPUTPORT_HXX__

#include "YACSlibEngineExport.hxx"
#include "Any.hxx"
#include "InPort.hxx"
#include "Runtime.hxx"
#include "DataFlowPort.hxx"
#include "ConversionException.hxx"
#include "yacsconfig.h"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class OutPort;
/*! \brief Base class for Input Ports
 *
 * \ingroup Ports
 *
 */
    class YACSLIBENGINE_EXPORT InputPort : public DataFlowPort, public InPort
    {
      friend class Runtime; // for port creation
      friend class OutPort;
    public:
      static const char NAME[];
    public:
      virtual ~InputPort();

      std::string getNameOfTypeOfCurrentInstance() const;
      //! returns the final physical port behind 'this'.
#ifdef NOCOVARIANT
      virtual InPort *getPublicRepresentant() { return this; }
#else
      virtual InputPort *getPublicRepresentant() { return this; }
#endif
      virtual bool isIntermediate() const { return false; }
      virtual bool edIsManuallyInitialized() const;
      //!soon deprecated
      bool edIsInitialized() const;

      template<class T>
      void edInit(T value);
      void edInit(Any *value);
      void edInit(const std::string& impl,const void* value);
      virtual void edRemoveManInit();
      virtual void checkBasicConsistency() const throw(Exception);
      virtual void exInit(bool start);
      virtual void exSaveInit() = 0;
      virtual void exRestoreInit() = 0;
      virtual InputPort *clone(Node *newHelder) const = 0;
      virtual bool isEmpty();

      virtual void *get() const = 0;
      virtual void put(const void *data) throw(ConversionException) = 0;
      virtual std::string dump();
      virtual std::string getHumanRepr();
      virtual void setStringRef(std::string strRef);
      virtual std::string typeName() {return "YACS__ENGINE__InputPort";}
      bool canBeNull() const;
    protected:
      InputPort(const InputPort& other, Node *newHelder);
      InputPort(const std::string& name, Node *node, TypeCode* type, bool canBeNull = false);
    protected:
      Any *_initValue;
      std::string _stringRef;
      bool _canBeNull;
    };

/*! \brief Base class for Proxy Input Ports
 *
 * \ingroup Ports
 *
 */
    class YACSLIBENGINE_EXPORT ProxyPort : public InputPort
    {
    public:
      ProxyPort(InputPort* p);
      ~ProxyPort();
      
      void edRemoveAllLinksLinkedWithMe() throw(Exception);
      InputPort *clone(Node *newHelder) const;
      void edNotifyReferencedBy(OutPort *fromPort);
      void edNotifyDereferencedBy(OutPort *fromPort);
      std::set<OutPort *> edSetOutPort() const;
#ifdef NOCOVARIANT
      InPort *getPublicRepresentant();
#else
      InputPort *getPublicRepresentant();
#endif
      void *get() const;
      virtual void put(const void *data) throw(ConversionException) ;
      int edGetNumberOfLinks() const;
      bool isIntermediate() const { return true; }
      void exRestoreInit();
      void exSaveInit();
      void getAllRepresentants(std::set<InPort *>& repr) const;
      virtual std::string typeName() {return "YACS__ENGINE__ProxyPort";}
    protected:
      InputPort* _port;
    };

    template<class T>
    void InputPort::edInit(T value)
    { 
      Any* any=AtomAny::New(value);
      try
      {
        edInit(any);
        any->decrRef();
      }
      catch(ConversionException&)
      {
        any->decrRef();
        throw;
      }
    }
  }
}

#endif
