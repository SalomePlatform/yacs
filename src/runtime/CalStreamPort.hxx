#ifndef __CALSTREAMPORT_HXX__
#define __CALSTREAMPORT_HXX__

#include "yacsconfig.h"
#ifdef DSC_PORTS
#include "DSC_Engines.hh"
#endif

#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    /*! \brief Class for Input Calcium DataStream Ports
     *
     * \ingroup Ports
     *
     */
    class InputCalStreamPort : public InputDataStreamPort
    {
      public:
        static const char NAME[];
        InputCalStreamPort(const std::string& name, Node *node, TypeCode* type);
        InputCalStreamPort(const InputCalStreamPort& other, Node *newHelder);
        virtual ~InputCalStreamPort();
        void setProperty(const std::string& name, const std::string& value);
        std::string getNameOfTypeOfCurrentInstance() const;
        InputCalStreamPort * clone(Node *newHelder) const;
#ifdef DSC_PORTS
        virtual void initPortProperties();
#endif
        void setSchema(const std::string& value);
        std::string getSchema(){return _schema;};
        void setLevel(const std::string& value);
        int getLevel(){return _level;};
        void setDepend(const std::string& value);
        std::string getDepend(){return _depend;};
        void setDelta(const std::string& value);
        double getDelta(){return _delta;};
        void setAlpha(const std::string& value);
        double getAlpha(){return _alpha;};
        void setInterp(const std::string& value);
        std::string getInterp(){return _interp;};
        void setExtrap(const std::string& value);
        std::string getExtrap(){return _extrap;};
      protected:
        std::string _depend;
        std::string _schema;
        std::string _interp;
        std::string _extrap;
        int _level;
        double _delta;
        double _alpha;
    };

    /*! \brief Class for Output Calcium DataStream Ports
     *
     * \ingroup Ports
     *
     */
    class OutputCalStreamPort : public OutputDataStreamPort
    {
      public:
        static const char NAME[];
        OutputCalStreamPort(const std::string& name, Node *node, TypeCode* type);
        OutputCalStreamPort(const OutputCalStreamPort& other, Node *newHelder);
        virtual ~OutputCalStreamPort();
        void setDepend(const std::string& depend);
        void setLevel(const std::string& schema);
        std::string getDepend(){return _depend;};
        int getLevel(){return _level;};
        void setSchema(const std::string& schema);
        std::string getSchema(){return _schema;};
        void setProperty(const std::string& name, const std::string& value);
        virtual bool addInPort(InPort *inPort) throw(Exception);
        std::string getNameOfTypeOfCurrentInstance() const;
        OutputCalStreamPort * clone(Node *newHelder) const;
        virtual int removeInPort(InPort *inPort, bool forward) throw(Exception);
      protected:
        std::string _depend;
        std::string _schema;
        int _level;
        double _delta;
    };
  }
}

#endif
