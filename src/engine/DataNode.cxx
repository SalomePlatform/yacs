#include "DataNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    DataNode::~DataNode() 
      {
      }
    void DataNode::setData(InputPort* port,std::string& data) 
      {
      }
    void DataNode::setData(OutputPort* port,std::string& data) 
      {
      }
    void DataNode::setRef(std::string& ref) 
      {
        _ref=ref;
      }
    std::string DataNode::getRef() 
      {
        return _ref;
      }
  }
}

