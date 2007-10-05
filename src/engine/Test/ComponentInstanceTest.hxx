#ifndef __COMPONENTINTANCETEST_HXX__
#define __COMPONENTINTANCETEST_HXX__

#include "ComponentInstance.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class ComponentInstanceTest1 : public ComponentInstance
    {
    public:
      ComponentInstanceTest1(const ComponentInstanceTest1& other);
      ComponentInstanceTest1(const std::string& name);
      void load();
      void unload();
      bool isLoaded();
      std::string getKind() const;
      ServiceNode* createNode(const std::string& name);
      ComponentInstance *clone() const;
    protected:
      bool _loaded;
    };

    class ComponentInstanceTest2 : public ComponentInstance
    {
    public:
      ComponentInstanceTest2(const ComponentInstanceTest2& other);
      ComponentInstanceTest2(const std::string& name);
      void load();
      void unload();
      bool isLoaded();
      std::string getKind() const;
      ServiceNode* createNode(const std::string& name);
      ComponentInstance *clone() const;
    protected:
      bool _loaded;
    };
  }
}

#endif
