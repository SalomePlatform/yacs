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

#ifndef __INLINENODE_HXX__
#define __INLINENODE_HXX__

#include "YACSlibEngineExport.hxx"
#include "ElementaryNode.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Container;

/*! \brief Class for calculation node (script) inlined (and executed) in the schema
 *
 * \ingroup Nodes
 *
 * This node is like a script. It has no state if it is executed several times.
 * Each execution the string _script is executed within a clean context.
 *
 * \see ServiceNode
 * \see ElementaryNode
 */
    class YACSLIBENGINE_EXPORT InlineNode : public ElementaryNode 
    {
    protected:
      InlineNode(const InlineNode& other, ComposedNode *father)
        :ElementaryNode(other,father),_script(other._script),_mode(other._mode),_container(0) { }
      InlineNode(const std::string& name):ElementaryNode(name),_mode(LOCAL_STR),_container(0) { }
    public:
      virtual void setScript(const std::string& script);
      virtual std::string getScript(){return _script;}

//! Return a new InlineNode node by making a copy of this node
/*!
 * \param name: name of the new node
 * \return the new node built by cloning.
 */
      virtual InlineNode* cloneNode(const std::string& name)
      { throw Exception("Not implemented");};
      virtual void accept(Visitor *visitor);
      virtual ~InlineNode();
      virtual std::string typeName() {return "YACS__ENGINE__InlineNode";}
      virtual void setExecutionMode(const std::string& mode);
      virtual std::string getExecutionMode();
      virtual void setContainer(Container* container);
      virtual Container* getContainer();
      void performDuplicationOfPlacement(const Node& other);
      void performShallowDuplicationOfPlacement(const Node& other);
      bool isDeployable() const;
      int getMaxLevelOfParallelism() const;
    public:
      static const char LOCAL_STR[];
      static const char REMOTE_STR[];
    protected:
      std::string _script;
      std::string _mode;
      Container* _container;
    };

/*! \brief Class for calculation node (function) inlined (and executed) in the schema
 *
 * \ingroup Nodes
 *
 * This node is like a function. It can have a state. The first time the node
 * is executed, the string _script is executed in a clean context followed by the
 * execution of the function _fname. Next times, the function _fname is executed 
 * within the preserved context.
 *
 * \see ServiceNode
 * \see ElementaryNode
 */
    class YACSLIBENGINE_EXPORT InlineFuncNode : public InlineNode
    {
    protected:
      InlineFuncNode(const InlineFuncNode& other, ComposedNode *father)
        :InlineNode(other,father),_fname(other._fname) { }
      InlineFuncNode(const std::string& name):InlineNode(name) { }
    public:
      //! Set the function name to use in node execution
      virtual void setFname(const std::string& fname);
      virtual std::string getFname() { return _fname; }
      void accept(Visitor *visitor);
      virtual ~InlineFuncNode();
      virtual std::string typeName() { return "YACS__ENGINE__InlineFuncNode"; }
      virtual void checkBasicConsistency() const throw(Exception);
    protected:
      std::string _fname;
    };
  }
}

#endif
