#ifndef __DATAPORT_HXX__
#define __DATAPORT_HXX__

#include "Port.hxx"

namespace YACS
{
  namespace ENGINE
  {
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
    public:
      static DataPort *isCrossingType(const std::vector<DataPort *>& historyOfLink);
    };
  }
}

#endif
