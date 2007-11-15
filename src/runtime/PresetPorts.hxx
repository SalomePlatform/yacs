
#ifndef _PRESETPORTS_HXX_
#define _PRESETPORTS_HXX_

#include "XMLPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {

/*! \brief Class for PRESET output Ports 
 *
 * \ingroup Ports
 *
 * \see PresetNode
 */
    class OutputPresetPort: public OutputXmlPort
    {
    public:
      OutputPresetPort(const std::string& name,  Node* node, TypeCode* type);
      OutputPresetPort(const OutputPresetPort& other, Node *newHelder);
      void setData(std::string data);
      std::string getData();
    protected:
      std::string _storeData;
    };
  }
}


#endif
