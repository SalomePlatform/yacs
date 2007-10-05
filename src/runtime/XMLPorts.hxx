
#ifndef _XMLPORTS_HXX_
#define _XMLPORTS_HXX_

#include "InputPort.hxx"
#include "OutputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for XML Input Ports
 *
 * \ingroup Ports
 *
 * \see XmlNode
 */
    class InputXmlPort : public InputPort
    {
    public:
      InputXmlPort(const std::string& name, Node* node, TypeCode * type);
      InputXmlPort(const InputXmlPort& other, Node *newHelder);
      bool edIsManuallyInitialized() const;
      void edRemoveManInit();
      virtual void put(const void *data) throw (ConversionException);
      void put(const char *data) throw (ConversionException);
      InputPort *clone(Node *newHelder) const;
      virtual const char * getXml() const;
      void *get() const throw(Exception);
      bool isEmpty();
      virtual void exSaveInit();
      virtual void exRestoreInit();
      virtual std::string dump();
    protected:
      std::string _data;
      std::string _initData;
    };
    
/*! \brief Class for XML Output Ports
 *
 * \ingroup Ports
 *
 * \see XmlNode
 */
    class OutputXmlPort : public OutputPort
    {
    public:
      OutputXmlPort(const std::string& name,  Node* node, TypeCode * type);
      OutputXmlPort(const OutputXmlPort& other, Node *newHelder);
      virtual void put(const void *data) throw (ConversionException);
      void put(const char *data) throw (ConversionException);
      virtual const char * get() const throw (ConversionException);
      OutputPort *clone(Node *newHelder) const;
      virtual std::string dump();
    protected:
      std::string _data;
    };

  }
}

#endif
