#ifndef _CORBANODE_HXX_
#define _CORBANODE_HXX_

#include "ServiceNode.hxx"
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
    class CORBANode : public ServiceNode 
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
    class SalomeNode : public ServiceNode 
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
      std::list<Engines::ConnectionManager::connectionId> ids;
#endif
      virtual std::string getContainerLog();
    };
  }
}

#endif
