#ifndef _CORBACOMPONENT_HXX_
#define _CORBACOMPONENT_HXX_

#include "ComponentInstance.hxx"
#include <omniORB4/CORBA.h>

namespace YACS
{
  namespace ENGINE
  {
    class ServiceNode;

/*! \brief Class for CORBA component instance
 *
 * \ingroup Nodes
 *
 */
    class CORBAComponent : public ComponentInstance
    {
    public:
      CORBAComponent(const std::string& name);
      CORBAComponent(const CORBAComponent& other);
      virtual ~CORBAComponent();
      virtual void load();
      virtual void unload();
      virtual bool isLoaded();
      virtual ServiceNode* createNode(const std::string& name);
      virtual ComponentInstance* clone() const;
      virtual std::string getFileRepr() const;
      virtual CORBA::Object_ptr getCompoPtr();
    public:
      static const char KIND[];
      virtual std::string getKind() const;
    protected:
      CORBA::Object_var _objComponent;
    };
  }
}

#endif
