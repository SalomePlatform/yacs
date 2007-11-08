#ifndef _SALOMECOMPONENT_HXX_
#define _SALOMECOMPONENT_HXX_

#include "ComponentInstance.hxx"
#include <omniORB4/CORBA.h>

namespace YACS
{
  namespace ENGINE
  {
    class ServiceNode;

/*! \brief Class for Salome component instance
 *
 *
 */
    class SalomeComponent : public ComponentInstance
    {
    public:
      SalomeComponent(const std::string& name);
      SalomeComponent(const SalomeComponent& other);
      virtual ~SalomeComponent();
      virtual void load();
      virtual void unload();
      virtual bool isLoaded();
      virtual ServiceNode* createNode(const std::string& name);
      virtual ComponentInstance* clone() const;
      virtual std::string getFileRepr() const;
      virtual CORBA::Object_ptr getCompoPtr(){return CORBA::Object::_duplicate(_objComponent);}
    public:
      static const char KIND[];
      virtual std::string getKind() const;
    protected:
      CORBA::Object_var _objComponent;
    };
  }
}

#endif
