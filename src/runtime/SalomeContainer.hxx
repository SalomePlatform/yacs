#ifndef __SALOMECONTAINER_HXX__
#define __SALOMECONTAINER_HXX__

#include "Container.hxx"
#include "Mutex.hxx"
#include <string>
#include <vector>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)

namespace YACS
{
  namespace ENGINE
  {
    class SalomeComponent;

    class SalomeContainer : public Container
    {
      friend class SalomeComponent;
    public:
      SalomeContainer();
      SalomeContainer(const SalomeContainer& other);
      //! For thread safety for concurrent load operation on same Container.
      void lock();
      //! For thread safety for concurrent load operation on same Container.
      void unLock();
      bool isAlreadyStarted() const;
      void start() throw (Exception);
      Container *clone() const;
      std::string getPlacementId() const;
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw (Exception);
      void setProperty(const std::string& name, const std::string& value);
      bool isAPaCOContainer() const;
      virtual void addComponentName(std::string name);
    protected:
      virtual ~SalomeContainer();
    protected:
      //! thread safety in Salome ???
      YACS::BASES::Mutex _mutex;
      Engines::Container_var _trueCont;
      std::vector<std::string> _componentNames;
    public:
      Engines::MachineParameters _params;
    };
  }
}

#endif
