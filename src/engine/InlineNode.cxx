//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "InlineNode.hxx"
#include "Visitor.hxx"

using namespace YACS::ENGINE;
using namespace std;


InlineNode::~InlineNode() { }

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



InlineFuncNode::~InlineFuncNode() { }

void InlineFuncNode::accept(Visitor *visitor)
{
  visitor->visitInlineFuncNode(this);
}

void InlineFuncNode::setFname(const std::string& fname)
{
  _fname=fname;
  modified();
}

void InlineFuncNode::checkBasicConsistency() const throw(Exception)
{
  InlineNode::checkBasicConsistency();
  if(_fname.empty() )
     {
       string mess = "Function name is not defined";
       throw Exception(mess);
     }
}
