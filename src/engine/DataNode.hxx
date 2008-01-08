#ifndef __DATANODE_HXX__
#define __DATANODE_HXX__

#include "ElementaryNode.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for data parameters specification
 *
 * \ingroup Nodes
 *
 */
    class DataNode : public ElementaryNode 
    {
    protected:
      DataNode(const DataNode& other, ComposedNode *father)
        :ElementaryNode(other,father){ }
      DataNode(const std::string& name):ElementaryNode(name) { }
    public:
      virtual void setData(InputPort* port ,std::string& data);
      virtual void setData(OutputPort* port ,std::string& data);
      virtual void setRef(std::string& ref);
      virtual std::string getRef();
      virtual ~DataNode();
      virtual std::string typeName() {return "YACS__ENGINE__DataNode";}
    protected:
      std::string _ref;
    };
  }
}

#endif
