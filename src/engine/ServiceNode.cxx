#include "ServiceNode.hxx"
#include "Visitor.hxx"
#include "ComponentInstance.hxx"
#include "ComposedNode.hxx"
#include "Runtime.hxx"
#include <iostream>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

const char ServiceNode::KIND[]="";

std::string ServiceNode::getKind() const
{
  return KIND;
}

ServiceNode::ServiceNode(const std::string& name):ElementaryNode(name),_component(0)
{ 
}

ServiceNode::ServiceNode(const ServiceNode& other, ComposedNode *father)
           :ElementaryNode(other,father),_method(other._method),_component(0)
{
}

void ServiceNode::performDuplicationOfPlacement(const Node& other)
{
  const ServiceNode &otherC=*(dynamic_cast<const ServiceNode *>(&other));
  //if other has no component don't clone: this will not have one
  if(otherC._component)
    _component=otherC._component->clone();
}

ServiceNode::~ServiceNode()
{
  if(_component)
    _component->decrRef();
}

//! Load the component associated to the node
void ServiceNode::load()
{
  if(_component)
    {
      if(!_component->isLoaded())
        {
          try
            {
              _component->load();
            }
          catch(Exception& e)
            {
              _errorDetails=e.what();
              throw e;
            }
        }
    }
  else
    {
      std::string what("ServiceNode::load : a load operation requested on ServiceNode called \"");
      what+=_name; what+="\" with no component specified.";
      _errorDetails=what;
      throw Exception(what);
    }
}

void ServiceNode::accept(Visitor *visitor)
{
  visitor->visitServiceNode(this);
}

//! Return the associated component instance
ComponentInstance *ServiceNode::getComponent()
{
  return _component;
}

//! By definition of ServiceNode class.
bool ServiceNode::isDeployable() const
{
  return true;
}

//! Associate an existing component instance to this service node \b AND check the consistency regarding the deployment from root node point of view.
void ServiceNode::setComponent(ComponentInstance* compo) throw(Exception)
{
  if(compo)
    if(compo->getKind() != this->getKind())
      {
        //Not allowed
        std::string what("ServiceNode::setComponent : component instance kind not allowed ");
        throw Exception(what);
      }
  if(_component)
    {
      //The node is already associated with a component instance
      _component->decrRef();
      //Don't forget to unassociate
    }
  _component=compo;
  if(_component)
    {
      if(_father)
        try
          {
            getRootNode()->checkDeploymentTree(false);
          }
        catch(Exception& e)
          {
            _component=0;
            throw e;
          }
      _component->incrRef();
    }
  //assert(_component);
}

//! Associate a new component instance to this service node
/*!
 * A new component instance with type name ref is created (from runtime
 * factory createComponentInstance) and associated to the node.
 *
 */
void ServiceNode::setRef(const std::string& ref)
{
  _ref = ref;
  if(_component)
    {
      //The node is already associated with a component instance
      _component->decrRef();
      //Don't forget to unassociate
    }
  _component= getRuntime()->createComponentInstance(ref,getKind());
  assert(_component);
}

std::string ServiceNode::getRef()
{
  return _ref;
}
