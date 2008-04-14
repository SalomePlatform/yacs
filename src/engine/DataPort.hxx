#ifndef __DATAPORT_HXX__
#define __DATAPORT_HXX__

#include "Port.hxx"
#include "Exception.hxx"
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class TypeCode;

    typedef enum
      {
        DATAFLOW,
        DATASTREAM
      } TypeOfChannel;

    class DataPort : public virtual Port
    {
    protected:
      TypeCode *_type;
      std::string _name;
    public:
      static const char NAME[];
    protected:
      virtual ~DataPort();
      DataPort(const DataPort& other, Node *newHelder);
      DataPort(const std::string& name, Node *node, TypeCode* type);
    public:
      TypeCode* edGetType() const { return _type; }
      void edSetType(TypeCode* type);
      std::string getName() const { return _name; }
      void setName( std::string theName ) { _name = theName; }
      std::string getNameOfTypeOfCurrentInstance() const;
      //! returns type of channel the port will use for data exchange on runtime : DATAFLOW or DATASTREAM.
      virtual TypeOfChannel getTypeOfChannel() const = 0;
      bool isDifferentTypeOf(const DataPort *other) const;
      virtual void edRemoveAllLinksLinkedWithMe() throw(Exception) = 0;
      virtual std::string typeName() {return "YACS__ENGINE__DataPort";}
    public:
      static DataPort *isCrossingType(const std::vector<DataPort *>& historyOfLink);
    };
  }
}

#endif
