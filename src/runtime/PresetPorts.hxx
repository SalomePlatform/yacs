
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
      virtual void checkBasicConsistency() const throw(Exception);
      virtual std::string dump();
      virtual std::string typeName() {return "YACS__ENGINE__OutputPresetPort";}
    protected:
      std::string _storeData;
    };

/*! \brief Class for PRESET input Ports
 *
 * \ingroup Ports
 *
 * \see OutNode
 */
    class InputPresetPort: public InputXmlPort
    {
    public:
      InputPresetPort(const std::string& name,  Node* node, TypeCode* type);
      InputPresetPort(const InputPresetPort& other, Node *newHelder);
      void setData(std::string data);
      std::string getData();
      virtual std::string dump();
      virtual std::string typeName() {return "YACS__ENGINE__InputPresetPort";}
    protected:
      std::string _storeData;
    };

  }
}


#endif
