// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

/*! \class YACS::ENGINE::ServiceNode
 *  \brief Class for calculation node associated with a component service
 *
 * \ingroup Nodes
 *
 * \see InlineNode
 * \see ElementaryNode
 */

const char ServiceNode::KIND[]="";

//! Return the service node kind
/*!
 * A runtime can provide several implementations of a service node.
 * Each implementation has a different kind. A ComponentInstance can be
 * associated to a ServiceNode with the same kind.
 */
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

void ServiceNode::performShallowDuplicationOfPlacement(const Node& other)
{
  const ServiceNode &otherC=*(dynamic_cast<const ServiceNode *>(&other));
  //if other has no component don't clone: this will not have one
  if(otherC._component)
    {
      _component=otherC._component;
      _component->incrRef();
    }
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
      if(!_component->isLoaded(this))
        {
          try
            {
              _component->load(this);
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

const ComponentInstance *ServiceNode::getComponent() const
{
  return _component;
}

//! Return the associated container
Container *ServiceNode::getContainer()
{
  if(_component)return _component->getContainer();
  return 0;
}

//! By definition of ServiceNode class.
bool ServiceNode::isDeployable() const
{
  return true;
}

//! Associate an existing component instance to this service node \b AND check the consistency regarding the deployment from root node point of view.
void ServiceNode::setComponent(ComponentInstance* compo) throw(YACS::Exception)
{
  DEBTRACE("ServiceNode::setComponent " << compo);
  if(compo)
    {
      DEBTRACE(compo->getInstanceName());
      if(compo->getKindForNode() != this->getKind())
        {
          //Not allowed
          std::string what("ServiceNode::setComponent : component instance kind not allowed ");
          throw Exception(what);
        }
    }

  ComponentInstance* oldcompo=_component;
  std::string oldref=_ref;

  _component=compo;
  _ref=compo->getCompoName();
  DEBTRACE(_component->getInstanceName());
  if(_component)
    {
      if(_father)
        try
          {
            getRootNode()->checkDeploymentTree(false);
          }
        catch(Exception& e)
          {
            // Impossible to associate compo to this node. Keep the old component instance and throws exception
            _component=oldcompo;
            _ref=oldref;
            throw e;
          }
      _component->incrRef();
    }

  if(oldcompo)
    oldcompo->decrRef();
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
  YASSERT(_component);
}

std::string ServiceNode::getRef()
{
  return _ref;
}
