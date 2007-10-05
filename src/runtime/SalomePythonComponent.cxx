#include "SalomePythonComponent.hxx"
#include "SalomeComponent.hxx"
#include "SalomePythonNode.hxx"
#include "Exception.hxx"
#include "Container.hxx"

#include <sstream>

using namespace YACS::ENGINE;

const char SalomePythonComponent::KIND[]="SalomePy";

unsigned SalomePythonComponent::_cntForReprS = 0;

SalomePythonComponent::SalomePythonComponent(const std::string &name):ComponentInstance(name),_cntForRepr(_cntForReprS++)
{
}

SalomePythonComponent::SalomePythonComponent(const SalomePythonComponent& other):ComponentInstance(other),_cntForRepr(_cntForReprS++)
{
}

SalomePythonComponent::~SalomePythonComponent()
{
}

void SalomePythonComponent::load()
{
  if(_container)
    {
      _container->start();
      return;
    }
  //This component has no specified container : use default container policy
  //given by getStringValueToExportInInterp()
  //throw Exception("SalomePythonComponent::load : no container specified !!! To be implemented in executor to allocate default a Container in case of presenceOfDefaultContainer.");
}

void SalomePythonComponent::unload()
{
}

bool SalomePythonComponent::isLoaded()
{
  if(!_container)
    return false;
  else
    return _container->isAlreadyStarted();
}

std::string SalomePythonComponent::getKind() const
{
  //This is not a bug !!!! SalomeComponent NOT SalomePythonComponent. This is for Container assignation.
  return SalomeComponent::KIND;
}

ComponentInstance* SalomePythonComponent::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (ComponentInstance*) (this);
    }
  else
    return new SalomePythonComponent(*this);
}

ServiceNode *SalomePythonComponent::createNode(const std::string &name)
{
  ServiceNode* node=new SalomePythonNode(name);
  node->setComponent(this);
  return node;
}

std::string SalomePythonComponent::getFileRepr() const
{
  std::ostringstream stream;
  stream << "<ref>" << "SalomePythonComponent #" << _cntForRepr << "</ref>";
  return stream.str();
}

std::string SalomePythonComponent::getStringValueToExportInInterp() const
{
  if(!_container)
    return "localhost/FactoryServer";
  else
    return _container->getPlacementId();
}
