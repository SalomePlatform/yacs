#include "DataNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    DataNode::~DataNode() 
      {
      }
    void DataNode::setData(InputPort* port,const std::string& data) 
      {
      }
    void DataNode::setData(OutputPort* port,const std::string& data) 
      {
      }
    void DataNode::setRef(const std::string& ref) 
      {
        _ref=ref;
      }
    std::string DataNode::getRef() 
      {
        return _ref;
      }
  }
}

