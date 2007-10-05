//#define REFCNT
#ifdef REFCNT
#define private public
#define protected public
#include <omniORB4/CORBA.h>
#include <omniORB4/internal/typecode.h>
#endif

#include "RuntimeSALOME.hxx"
#include "SalomeContainer.hxx"
#include "SalomeComponent.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOME_ContainerManager.hxx"

#include <sstream>
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

SalomeContainer::SalomeContainer():_trueCont(Engines::Container::_nil())
{
}

SalomeContainer::SalomeContainer(const SalomeContainer& other):Container(other),_trueCont(Engines::Container::_nil())
{
}

SalomeContainer::~SalomeContainer()
{
}

void SalomeContainer::lock()
{
  _mutex.lock();
}

void SalomeContainer::unLock()
{
  _mutex.unlock();
}

bool SalomeContainer::isAlreadyStarted() const
{
  if(CORBA::is_nil(_trueCont))
    return false;
  else
    return true;
}

void SalomeContainer::start() throw (Exception)
{
  CORBA::ORB_ptr orb=getSALOMERuntime()->getOrb();
  SALOME_NamingService ns(orb);
  CORBA::Object_var obj=ns.Resolve(SALOME_ContainerManager::_ContainerManagerNameInNS);
  Engines::ContainerManager_var contManager=Engines::ContainerManager::_narrow(obj);
  SALOME_LifeCycleCORBA LCC(&ns);
  Engines::MachineParameters params;
  LCC.preSet(params);
  params.hostname=CORBA::string_dup("");
  std::ostringstream stream;
  stream << (void *)(this);
  params.container_name=CORBA::string_dup(stream.str().c_str());
  try
    { 
      std::string policy=getProperty("policy");
      if(policy=="best")
        _trueCont=contManager->StartContainer(params,Engines::P_BEST);
      else if(policy=="first")
        _trueCont=contManager->StartContainer(params,Engines::P_FIRST);
      else
        _trueCont=contManager->StartContainer(params,Engines::P_CYCL);
    }
  catch(CORBA::COMM_FAILURE&)
    {
      throw Exception("SalomeContainer::start : Unable to launch container in Salome : CORBA Comm failure detected");
    }
  catch(CORBA::Exception&)
    {
      throw Exception("SalomeContainer::start : Unable to launch container in Salome : Unexpected CORBA failure detected");
    }
  if(CORBA::is_nil(_trueCont))
    throw Exception("SalomeContainer::start : Unable to launch container in Salome");
#ifdef REFCNT
    DEBTRACE(_trueCont->_PR_getobj()->pd_refCount );
#endif
}

Container *SalomeContainer::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (Container*) (this);
    }
  else
    return new SalomeContainer(*this);
}

std::string SalomeContainer::getPlacementId() const
{
  if(isAlreadyStarted())
    {
      const char *what="/";
      char *corbaStr=_trueCont->name();
      string ret(corbaStr);
      CORBA::string_free(corbaStr);
      //Salome FOREVER ...
      std::string::size_type i=ret.find_first_of(what,0);
      i=ret.find_first_of(what, i==std::string::npos ? i:i+1);
      if(i!=std::string::npos)
	return ret.substr(i+1);
      return ret;
    }
  else
    return "Not placed yet !!!";
}

void SalomeContainer::checkCapabilityToDealWith(const ComponentInstance *inst) const throw (Exception)
{
  if(inst->getKind()!=SalomeComponent::KIND)
    throw Exception("SalomeContainer::checkCapabilityToDealWith : SalomeContainer is not able to deal with this type of ComponentInstance.");
}

