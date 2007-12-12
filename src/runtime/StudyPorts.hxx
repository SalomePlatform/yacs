#ifndef _STUDYPORTS_HXX_
#define _STUDYPORTS_HXX_

#include "XMLPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {

/*! \brief Class for Study output Ports 
 *
 * \ingroup Ports
 *
 * \see StudyInNode
 */
    class OutputStudyPort: public OutputXmlPort
    {
    public:
      OutputStudyPort(const std::string& name,  Node* node, TypeCode* type);
      OutputStudyPort(const OutputStudyPort& other, Node *newHelder);
      void setData(const std::string& data);
      std::string getData();
      virtual void putIOR(const std::string& ior);
    protected:
      std::string _storeData;
    };

/*! \brief Class for Study input Ports
 *
 * \ingroup Ports
 *
 * \see StudyOutNode
 */
    class InputStudyPort: public InputXmlPort
    {
    public:
      InputStudyPort(const std::string& name,  Node* node, TypeCode* type);
      InputStudyPort(const InputStudyPort& other, Node *newHelder);
      void setData(const std::string& data);
      std::string getData();
      virtual std::string getIOR();
      virtual std::string splitXML(const std::string& s);
    protected:
      std::string _storeData;
    };

  }
}


#endif
