#include "TypeCheckerDataStream.hxx"

namespace YACS
{
  namespace ENGINE
  {
    template< template< StreamType type> class TREATMENTPERTYPE >
    class DynToStaticStreamTypeDispatcher1
    {
    public:
      template<class  T, class U>
      static void perform(YACS::StreamType myTypeDyn, T input, U& output) throw(Exception)
      {
	switch(myTypeDyn)
	  {
	  case SDouble:
	    TREATMENTPERTYPE<SDouble>::perform(input, output);
	    break;
	  default:
	    throw Exception("Unknow stream type enum");
	  }
      }
    };

    template< template< StreamType fromType, StreamType toType> class TREATMENTPERTYPE >
    class DynToStaticStreamTypeDispatcher2
    {
      template<StreamType toType> class TREATMENTPERTYPESDOUBLE : public TREATMENTPERTYPE<SDouble,toType> { };
    public:
      template<class  T, class U>
      static void perform(YACS::StreamType myTypeDynFrom, YACS::StreamType myTypeDynTo, T input, U& output) throw(Exception)
      {
	switch(myTypeDynFrom)
	  {
	  case SDouble:
	    DynToStaticStreamTypeDispatcher1<TREATMENTPERTYPESDOUBLE>::perform(myTypeDynTo, input, output);
	    break;
	  default:
	    throw Exception("Unknow stream type enum");
	  }
      }
    };

    template<YACS::StreamType baseTyp>
    class StreamTypeDescriptorTraits
    {
    };

    template<>
    class StreamTypeDescriptorTraits<YACS::SDouble>
    {
    public:
      static const char _name[];
    };

    /**
     *
     * This class has the responsability of the validity of the content of stream data regarding only statically its types from and to.
     *
     */
    template< StreamType fromType, StreamType toType >
    class StaticStreamTypeConverter
    {
    public:
      enum { _staticallyCompatible=0 };
    };

    template<StreamType type>
    class StaticStreamTypeConverter<type,type>
    {
    public:
      enum { _staticallyCompatible=1 };
    };
  }
}

using namespace YACS::ENGINE;

const char StreamTypeDescriptorTraits<YACS::SDouble>::_name[]="SDouble";

const char TypeCheckerDataStream::UNRECOGNIZEDTYPENAME[]="Unrecognized type";

//Part of TypeCheckerDataStream::areStaticallyCompatible implementation
template< YACS::StreamType fromType, YACS::StreamType toType >
class TreatmentToCheckStaticallyTypes
{
public:
  static void perform(int input, bool& ret)
  {
    ret=StaticStreamTypeConverter<fromType,toType>::_staticallyCompatible;
  }
};

bool TypeCheckerDataStream::areStaticallyCompatible(YACS::StreamType type1, YACS::StreamType type2) throw(Exception)
{
  bool ret;
  int fake;
  //DynToStaticStreamTypeDispatcher2<TreatmentToCheckStaticallyTypes>::perform(type1, type2, fake, ret);
  return ret;
}

// Part of TypeCheckerDataStream::edGetTypeInPrintableForm implementation

template< YACS::StreamType type >
class TreatmentForNameOfStreamType
{
public:
  static void perform(int input, std::string& ret)
  {
    ret=StreamTypeDescriptorTraits<type>::_name;
  }
};

std::string TypeCheckerDataStream::edGetTypeInPrintableForm(YACS::StreamType type)
{
  try
    {
      int fake1;
      std::string ret;
      DynToStaticStreamTypeDispatcher1< TreatmentForNameOfStreamType >::perform(type, fake1, ret);
      return ret;
    }
  catch(Exception& e)
    {
      return UNRECOGNIZEDTYPENAME;
    }
}
