// Copyright (C) 2006-2015  CEA/DEN, EDF R&D
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

#include "InlineNode.hxx"
#include "Visitor.hxx"
#include "Container.hxx"
#include <iostream>

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;


InlineNode::~InlineNode()
{
  if(_container)
    _container->decrRef();
}

void InlineNode::accept(Visitor *visitor)
{
  visitor->visitInlineNode(this);
}

//! Set the script (as a string) to execute
/*!
 * \param script: script to execute
 */
void InlineNode::setScript(const std::string& script) 
{ 
  _script=script; 
  modified();
}


InlineFuncNode::~InlineFuncNode()
{
}

void InlineFuncNode::accept(Visitor *visitor)
{
  visitor->visitInlineFuncNode(this);
}

/*!
 * \param fname: name of the function contained in the script to execute
 */
void InlineFuncNode::setFname(const std::string& fname)
{
  _fname=fname;
  modified();
}

void InlineFuncNode::checkBasicConsistency() const throw(YACS::Exception)
{
  InlineNode::checkBasicConsistency();
  if(_fname.empty() )
     {
       string mess = "Function name is not defined";
       throw Exception(mess);
     }
}

void InlineNode::setExecutionMode(const std::string& mode)
{
  if(mode == _mode)return;
  if(mode == "local"||mode == "remote")
    {
      _mode=mode;
      modified();
    }
}

std::string InlineNode::getExecutionMode()
{
  return _mode;
}

Container* InlineNode::getContainer()
{
  return _container;
}

void InlineNode::setContainer(Container* cont)
{
  if (cont == _container) return;
  if(_container)
    _container->decrRef();
  _container=cont;
  if(_container)
    _container->incrRef();
}

void InlineNode::performDuplicationOfPlacement(const Node& other)
{
  const InlineNode &otherC=*(dynamic_cast<const InlineNode *>(&other));
  //if other has no container don't clone: this will not have one
  if(otherC._container)
    _container=otherC._container->clone();
}

void InlineNode::performShallowDuplicationOfPlacement(const Node& other)
{
  const InlineNode &otherC=*(dynamic_cast<const InlineNode *>(&other));
  //if other has no container don't clone: this will not have one
  if(otherC._container)
    {
      _container=otherC._container;
      _container->incrRef();
    }
}

bool InlineNode::isDeployable() const
{
  if(_mode=="remote")
    return true;
  else
    return false;
}

