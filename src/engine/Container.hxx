#ifndef __CONTAINER_HXX__
#define __CONTAINER_HXX__

#include "Exception.hxx"
#include "RefCounter.hxx"

#include <list>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class ComponentInstance;
    /*!
     * This is an abstract class, that represents an abstract process in which ComponentInstances can be launched and run.
     */
    class Container : public RefCounter
    {
    protected:
      Container();
      virtual ~Container();
    public:
      //Execution only methods
      virtual bool isAlreadyStarted() const = 0;
      virtual void start() throw(Exception) = 0;
      virtual std::string getPlacementId() const = 0;
      //Edition only methods
      virtual void attachOnCloning() const;
      virtual void dettachOnCloning() const;
      bool isAttachedOnCloning() const;
      //! \b WARNING ! clone behaviour \b MUST be in coherence with what is returned by isAttachedOnCloning() method
      virtual Container *clone() const = 0;
      virtual bool isSupportingRTODefNbOfComp() const;
      virtual void checkCapabilityToDealWith(const ComponentInstance *inst) const throw(Exception) = 0;
      virtual void setProperty(const std::string& name,const std::string& value);
      virtual std::string getProperty(const std::string& name);
    protected:
      mutable bool _isAttachedOnCloning;
      std::map<std::string,std::string> _propertyMap;
    };
  }
}

#endif
