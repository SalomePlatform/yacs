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

#ifndef _CORBANODE_HXX_
#define _CORBANODE_HXX_

#include "YACSRuntimeSALOMEExport.hxx"
#include "ServiceNode.hxx"
#include "Mutex.hxx"
#include "yacsconfig.h"
#ifdef DSC_PORTS
#include "DSC_Engines.hh"
#endif
#include <omniORB4/CORBA.h>
#include <list>

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for CORBA Service Node
 *
 * \ingroup Nodes
 *
 * \see InputCorbaPort
 * \see OutputCorbaPort
 */
    class YACSRUNTIMESALOME_EXPORT CORBANode : public ServiceNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      CORBANode(const CORBANode& other,ComposedNode *father);
      CORBANode(const std::string& name);
      virtual void execute();
      virtual ServiceNode* createNode(const std::string& name);
      virtual std::string getKind() const;
      static const char KIND[];
      virtual std::string typeName() {return "YACS__ENGINE__CORBANode";}
    public:
      static const char IMPL_NAME[];
    };

/*! \brief Class for Salome component Service Node
 *
 * \ingroup Nodes
 *
 * \see InputCorbaPort
 * \see OutputCorbaPort
 */
    class YACSRUNTIMESALOME_EXPORT SalomeNode : public ServiceNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      SalomeNode(const SalomeNode& other,ComposedNode *father);
      SalomeNode(const std::string& name);
      virtual ~SalomeNode();
      virtual void execute();
      virtual ServiceNode* createNode(const std::string& name);
      virtual std::string getKind() const;
      static const char KIND[];
      virtual std::string typeName() {return "YACS__ENGINE__SalomeNode";}
#ifdef DSC_PORTS
      virtual void initService();
      virtual void connectService();
      virtual void disconnectService();
      virtual void cleanNodes();
      std::list<Engines::ConnectionManager::connectionId> ids;
      virtual void addDatastreamPortToInitMultiService(const std::string & port_name,
                                                       int number);
#endif
      virtual void shutdown(int level);
      virtual std::string getContainerLog();

    private:
      YACS::BASES::Mutex _mutex;
#ifdef DSC_PORTS
      Engines::Superv_Component::seq_multiple_param _param;
#endif
    };
  }
}

#endif
