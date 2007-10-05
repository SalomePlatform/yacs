
#ifndef _CPPPORTS_HXX_
#define _CPPPORTS_HXX_

#include "Any.hxx"

#include "InputPort.hxx"
#include "OutputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {

/*! \brief Class for C++ Ports
 *
 * \ingroup Ports
 *
 * \see CppNode
 */
    class InputCppPort : public InputPort
    {
    public:
      InputCppPort(const std::string& name, Node * node, TypeCode * type);
      InputCppPort(const InputCppPort& other, Node *newHelder);
      ~InputCppPort();
      bool edIsManuallyInitialized() const;
      void edRemoveManInit();
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
      InputPort *clone(Node *newHelder) const;
      virtual YACS::ENGINE::Any * getCppObj() const;
      void *get() const throw(Exception);
      virtual bool isEmpty();
      virtual void exSaveInit();
      virtual void exRestoreInit();
      virtual std::string dump();
    protected:
      YACS::ENGINE::Any* _data;
      YACS::ENGINE::Any* _initData;
    };

    class OutputCppPort : public OutputPort
    {
    public:
      OutputCppPort(const std::string& name, Node * node, TypeCode * type);
      OutputCppPort(const OutputCppPort& other, Node *newHelder);
      ~OutputCppPort();
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
      OutputPort *clone(Node *newHelder) const;
      virtual YACS::ENGINE::Any * get() const;
      virtual std::string dump();
    protected:
      YACS::ENGINE::Any* _data;
    };



  }
}

#endif
