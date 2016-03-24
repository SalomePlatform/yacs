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

#ifndef __OUTGATE_HXX__
#define __OUTGATE_HXX__

#include "YACSlibEngineExport.hxx"
#include "Port.hxx"
#include "Exception.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class InGate;
    class ElementaryNode;
    
    class YACSLIBENGINE_EXPORT OutGate : public Port
    {
      friend class ElementaryNode;
    protected:
      std::list< std::pair< InGate *, bool > > _setOfInGate;
    public:
      static const char NAME[];
    public:
      OutGate(Node *node);
      std::string getNameOfTypeOfCurrentInstance() const;
      void exReset();
      void exNotifyDone();
      void exNotifyFailed();
      void exNotifyDisabled();
      void edDisconnectAllLinksFromMe();
      bool edAddInGate(InGate *inGate);
      std::list< std::pair< InGate *, bool> >& edMapInGate() { return _setOfInGate; }
      std::list<InGate *> edSetInGate() const;
      void edRemoveInGate(InGate *inGate, bool coherenceWithInGate=true) throw(Exception);
      int getNbOfInGatesConnected() const;
      bool isAlreadyInSet(InGate *inGate) const;
      virtual std::string typeName() {return "YACS__ENGINE__OutGate";}
    protected:
      void edRemoveInGateOneWay(InGate *inGate);
    };
  }
}

#endif
