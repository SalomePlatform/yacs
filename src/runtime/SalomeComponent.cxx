#include "RuntimeSALOME.hxx"
#include "SalomeComponent.hxx"
#include "SalomeContainer.hxx"
#include "CORBANode.hxx"

#ifdef SALOME_KERNEL
#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#endif

#include <omniORB4/CORBA.h>
#include <iostream>
#include <sstream>

using namespace YACS::ENGINE;
using namespace std;

const char SalomeComponent::KIND[]="Salome";

//! SalomeComponent constructor
SalomeComponent::SalomeComponent(const std::string& name): ComponentInstance(name)
{
  _objComponent=CORBA::Object::_nil();
}

//! SalomeComponent copy constructor
SalomeComponent::SalomeComponent(const SalomeComponent& other):ComponentInstance(other)
{
  _objComponent=CORBA::Object::_nil();
}

SalomeComponent::~SalomeComponent()
{
}

std::string SalomeComponent::getKind() const
{
  return KIND;
}

//! Unload the component 
void SalomeComponent::unload()
{
  //Not implemented
  std::cerr << "SalomeComponent::unload : not implemented " << std::endl;
}

//! Is the component instance already loaded ?
bool SalomeComponent::isLoaded()
{
  if(CORBA::is_nil(_objComponent))
    return false;
  else
    return true;
}

#ifdef SALOME_KERNEL
//! Load the component 
void SalomeComponent::load()
{
  if(_container)
    {
      SalomeContainer *containerC=(SalomeContainer *)_container;
      containerC->lock();//To be sure
      if(!_container->isAlreadyStarted())
        {
          try
            {
              _container->start();
            }
          catch(Exception& e)
            {
              containerC->unLock();
              throw e;
            }
        }
      containerC->unLock();
      containerC->lock();//To be sure
      const char* componentName=_name.c_str();
      //char *val2=CORBA::string_dup("");
      // does not work with python components
      // does not make a strict load but a find or load component
      //   _objComponent=containerC->_trueCont->load_impl(componentName,val2);
      bool isLoadable = containerC->_trueCont->load_component_Library(componentName);
      if (isLoadable) 
	if (containerC->isAPaCOContainer()) {
	  std::string compo_paco_name(componentName);
	  compo_paco_name = compo_paco_name + "@PARALLEL@";
	  char * c_paco_name = CORBA::string_dup(compo_paco_name.c_str());
	  _objComponent=containerC->_trueCont->create_component_instance(c_paco_name, 0);
	}
	else
	  _objComponent=containerC->_trueCont->create_component_instance(componentName, 0);

      if(CORBA::is_nil(_objComponent))
        {
          containerC->unLock();
          throw Exception("SalomeComponent::load : Error while trying to create a new component.");
        }
      containerC->unLock();
      return;
    }
  //throw Exception("SalomeComponent::load : no container specified !!! To be implemented in executor to allocate default a Container in case of presenceOfDefaultContainer.");
  //This component has no specified container : use default container policy
  SALOME_NamingService ns(getSALOMERuntime()->getOrb());
  SALOME_LifeCycleCORBA LCC(&ns);
  Engines::MachineParameters params;
  LCC.preSet(params);
  params.hostname="localhost";
  params.container_name ="FactoryServer";
  _objComponent=LCC.LoadComponent(params,_name.c_str());
}
#else
void SalomeComponent::load()
{
  throw Exception("YACS has been built without SALOME support");
}
#endif

//! Create a ServiceNode with this component instance and no input or output port
/*!
 *   \param name : node name
 *   \return       a new SalomeNode node
 */
ServiceNode* SalomeComponent::createNode(const std::string& name)
{
   SalomeNode* node=new SalomeNode(name);
   node->setComponent(this);
   return node;
}

//! Clone the component instance
ComponentInstance* SalomeComponent::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (ComponentInstance*) (this);
    }
  else
    return new SalomeComponent(*this);
}

std::string SalomeComponent::getFileRepr() const
{
  ostringstream stream;
  stream << "<component>" << getName() << "</component>";
  return stream.str();
}

void SalomeComponent::setContainer(Container *cont)
{
  if (cont == _container) return;

  if(cont)
    cont->checkCapabilityToDealWith(this);

  if(_container)
    _container->decrRef();
  _container=cont;
  if(_container)
  {
    _container->incrRef();
    ((SalomeContainer*)_container)->addComponentName(_name);
  }
}

