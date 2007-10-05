#ifndef __CONTAINERTEST_HXX__
#define __CONTAINERTEST_HXX__

#include "Container.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class ContainerTest : public Container
    {
    public:
      ContainerTest();
      std::string getPlacementInfo() const;
      // implementation of compulsary methods
      bool isAlreadyStarted() const;
      void start() throw(Exception);
      Container *clone() const;
      std::string getPlacementId() const { return ""; }
      static void initAllContainers();
    protected:
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw(Exception);
    protected:
      bool _alreadyStarted;
      unsigned _myCounter;
      static unsigned _counter;
      static const char SUPPORTED_COMP_KIND[];
    };

    class ContainerTest2 : public Container
    {
    public:
      ContainerTest2();
      // implementation of compulsary methods
      bool isAlreadyStarted() const;
      void start() throw(Exception);
      Container *clone() const;
      std::string getPlacementId() const { return ""; }
      static void initAllContainers();
    protected:
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw(Exception);
    protected:
      bool _alreadyStarted;
      unsigned _myCounter;
      static unsigned _counter;
      static const char SUPPORTED_COMP_KIND[];
    };
  }
}

#endif
