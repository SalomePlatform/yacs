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

#ifndef __INPORT_HXX__
#define __INPORT_HXX__

#include "YACSlibEngineExport.hxx"
#include "DataPort.hxx"

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class Loop;
    class OutPort;
    class ProxyPort;
    class OutputPort;
    class DynParaLoop;
    class ForEachLoop;
    class SplitterNode;
    class ComposedNode;
    class OptimizerLoop;
    class ElementaryNode;
    class CollectorSwOutPort;
    class OutputDataStreamPort;
    class InterceptorInputPort;

    class YACSLIBENGINE_EXPORT InPort : public virtual DataPort
    {
      friend class Loop;
      friend class OutPort;
      friend class ProxyPort;
      friend class OutputPort;
      friend class DynParaLoop;
      friend class ForEachLoop;
      friend class SplitterNode;
      friend class ComposedNode;
      friend class OptimizerLoop;
      friend class ElementaryNode; //for removeAllLinksWithMe
      friend class CollectorSwOutPort;
      friend class OutputDataStreamPort;
      friend class InterceptorInputPort;
    public:
      virtual InPort *getPublicRepresentant() { return this; }
      virtual int edGetNumberOfLinks() const;
      virtual std::set<OutPort *> edSetOutPort() const;
      virtual ~InPort();
      virtual std::string typeName() {return "YACS__ENGINE__InPort";}
    protected:
      InPort(const InPort& other, Node *newHelder);
      InPort(const std::string& name, Node *node, TypeCode* type);
      void edRemoveAllLinksLinkedWithMe() throw(Exception);
      virtual void edNotifyReferencedBy(OutPort *fromPort);
      virtual void edNotifyDereferencedBy(OutPort *fromPort);
      virtual void getAllRepresentants(std::set<InPort *>& repr) const;
    protected:
      std::set<OutPort *> _backLinks;
    };
  }
}

#endif
