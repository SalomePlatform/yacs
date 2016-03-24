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

#ifndef __INGATE_HXX__
#define __INGATE_HXX__

#include "YACSlibEngineExport.hxx"
#include "Port.hxx"
#include "define.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class OutGate;

    class YACSLIBENGINE_EXPORT InGate : public Port
    {
      friend class Bloc;
      friend class Node;
    protected:
      static const char NAME[];
    private:
      std::list< std::pair<OutGate *, bool> > _backLinks;
    public:
      InGate(Node *node);
      virtual ~InGate();
      std::string getNameOfTypeOfCurrentInstance() const;
      void exNotifyFromPrecursor(OutGate *fromgate);
      std::list< std::pair<OutGate *, bool> >& edMapOutGate() { return _backLinks; }
      void edAppendPrecursor(OutGate *fromgate);
      void edRemovePrecursor(OutGate *fromgate);
      int getNumberOfBackLinks() const;
      void edDisconnectAllLinksToMe();
      void exNotifyFailed();
      void exNotifyDisabled();
      void exReset();
      bool exIsReady() const;
      std::list<OutGate *> getBackLinks();
      void setPrecursorDone(OutGate *fromgate);
      virtual std::string typeName() {return "YACS__ENGINE__InGate";}
    };
  }
}

#endif
