#ifndef __INLINENODE_HXX__
#define __INLINENODE_HXX__

#include "ElementaryNode.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
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
    class InlineNode : public ElementaryNode 
    {
    protected:
      InlineNode(const InlineNode& other, ComposedNode *father)
        :ElementaryNode(other,father),_script(other._script) { }
      InlineNode(const std::string& name):ElementaryNode(name) { }
    public:
//! Set the script (as a string) to execute
/*!
 * \param script: script to execute
 */
      virtual void setScript(const std::string& script) { _script=script; }
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
    protected:
      std::string _script;
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
    class InlineFuncNode : public InlineNode
    {
    protected:
      InlineFuncNode(const InlineFuncNode& other, ComposedNode *father)
        :InlineNode(other,father),_fname(other._fname) { }
      InlineFuncNode(const std::string& name):InlineNode(name) { }
    public:
//! Set the function name to use in node execution
/*!
 * \param fname: name of the function contained in the script to execute
 */
      virtual void setFname(const std::string& fname) { _fname=fname; }
      virtual std::string getFname() { return _fname; }
      void accept(Visitor *visitor);
      virtual ~InlineFuncNode();
      virtual std::string typeName() {return "YACS__ENGINE__InlineFuncNode";}
    protected:
      std::string _fname;
    };
  }
}

#endif
