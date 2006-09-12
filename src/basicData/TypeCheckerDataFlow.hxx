#ifndef __TYPECHECKERDATAFLOW_HXX__
#define __TYPECHECKERDATAFLOW_HXX__

#include "define.hxx"
#include "ConversionException.hxx"

#include <string>
#include <sstream>

namespace YACS
{
  namespace ENGINE
  {
    /**
     *
     * @ note : container in all this file is a concept that stores any data of any data into container::Type type.
     *          container concept must at least implement
     *                 - createNewContent method
     *                 - readContent method
     *          and container must define Type class that stores data independently of type.
     *          ContentOfDataFlow is one class fulfilling this concept !
     *
     */
    template< template< DynType type> class TREATMENTPERTYPE >
    class DynToStaticTypeDispatcher1
    {
    public:
      template<class  T, class U>
      static void perform(YACS::DynType myTypeDyn, T input, U& output) throw(Exception)
      {
	switch(myTypeDyn)
	  {
	  case Double:
	    TREATMENTPERTYPE<Double>::perform(input, output);
	    break;
	  case Int:
	    TREATMENTPERTYPE<Int>::perform(input, output);
	    break;
	  case String:
	    TREATMENTPERTYPE<String>::perform(input, output);
	    break;
	  case Bool:
	    TREATMENTPERTYPE<Bool>::perform(input, output);
	    break;
	  default:
	    throw Exception("Unknow type enum");
	  }
      }
    };

    template< template< DynType fromType, DynType toType> class TREATMENTPERTYPE >
    class DynToStaticTypeDispatcher2
    {
      template<DynType toType> class TREATMENTPERTYPEDOUBLE : public TREATMENTPERTYPE<Double,toType> { };
      template<DynType toType> class TREATMENTPERTYPEINT : public TREATMENTPERTYPE<Int,toType> { };
      template<DynType toType> class TREATMENTPERTYPEBOOL : public TREATMENTPERTYPE<Bool,toType> { };
      template<DynType toType> class TREATMENTPERTYPESTRING : public TREATMENTPERTYPE<String,toType> { };
    public:
      template<class  T, class U>
      static void perform(YACS::DynType myTypeDynFrom, YACS::DynType myTypeDynTo, T input, U& output) throw(Exception)
      {
	switch(myTypeDynFrom)
	  {
	  case Double:
#ifndef __OLD_GCC__
	    DynToStaticTypeDispatcher1<TREATMENTPERTYPEDOUBLE>::perform(myTypeDynTo, input, output);
#else
	    switch(myTypeDynTo)
	      {
	      case Double:
		TREATMENTPERTYPE<Double,Double>::perform(input, output);
		break;
	      case Int:
		TREATMENTPERTYPE<Double,Int>::perform(input, output);
		break;
	      case String:
		TREATMENTPERTYPE<Double,String>::perform(input, output);
		break;
	      case Bool:
		TREATMENTPERTYPE<Double,Bool>::perform(input, output);
		break;
	      default:
		throw Exception("Unknow type enum");
	      }
#endif
	    break;
	  case Int:
#ifndef __OLD_GCC__
	    DynToStaticTypeDispatcher1<TREATMENTPERTYPEINT>::perform(myTypeDynTo, input, output);
#else
	    switch(myTypeDynTo)
	      {
	      case Double:
		TREATMENTPERTYPE<Int,Double>::perform(input, output);
		break;
	      case Int:
		TREATMENTPERTYPE<Int,Int>::perform(input, output);
		break;
	      case String:
		TREATMENTPERTYPE<Int,String>::perform(input, output);
		break;
	      case Bool:
		TREATMENTPERTYPE<Int,Bool>::perform(input, output);
		break;
	      default:
		throw Exception("Unknow type enum");
	      }
#endif
	    break;
	  case String:
#ifndef __OLD_GCC__
	    DynToStaticTypeDispatcher1<TREATMENTPERTYPESTRING>::perform(myTypeDynTo, input, output);
#else
	    switch(myTypeDynTo)
	      {
	      case Double:
		TREATMENTPERTYPE<String,Double>::perform(input, output);
		break;
	      case Int:
		TREATMENTPERTYPE<String,Int>::perform(input, output);
		break;
	      case String:
		TREATMENTPERTYPE<String,String>::perform(input, output);
		break;
	      case Bool:
		TREATMENTPERTYPE<String,Bool>::perform(input, output);
		break;
	      default:
		throw Exception("Unknow type enum");
	      }
#endif
	    break;
	  case Bool:
#ifndef __OLD_GCC__
	    DynToStaticTypeDispatcher1<TREATMENTPERTYPESTRING>::perform(myTypeDynTo, input, output);
#else
	    switch(myTypeDynTo)
	      {
	      case Double:
		TREATMENTPERTYPE<Bool,Double>::perform(input, output);
		break;
	      case Int:
		TREATMENTPERTYPE<Bool,Int>::perform(input, output);
		break;
	      case String:
		TREATMENTPERTYPE<Bool,String>::perform(input, output);
		break;
	      case Bool:
		TREATMENTPERTYPE<Bool,Bool>::perform(input, output);
		break;
	      default:
		throw Exception("Unknow type enum");
	      }
#endif
	    break;
	  default:
	    throw Exception("Unknow type enum");
	  }
      }
    };

    template<YACS::DynType baseTyp>
    class TypeDescriptorTraits
    {
    };

    template<class T>
    class TypeDescriptorTraitsInv
    {
    };

    template<>
    class TypeDescriptorTraits<YACS::Double>
    {
    public:
      typedef double ConcreteType;
      static const char _name[];
    };

    template<>
    class TypeDescriptorTraitsInv<double>
    {
    public:
      static const YACS::DynType _typeEnum=Double;
    };

    template<>
    class TypeDescriptorTraits<YACS::Int>
    {
    public:
      typedef int ConcreteType;
      static const char _name[];
    };

    template<>
    class TypeDescriptorTraitsInv<int>
    {
    public:
      static const YACS::DynType _typeEnum=Int;
    };

    template<>
    class TypeDescriptorTraits<YACS::Bool>
    {
    public:
      typedef bool ConcreteType;
      static const char _name[];
    };

    template<>
    class TypeDescriptorTraitsInv<bool>
    {
    public:
      static const YACS::DynType _typeEnum=Bool;
    };

    template<>
    class TypeDescriptorTraits<YACS::String>
    {
    public:
      typedef std::string ConcreteType;
      static const char _name[];
    };

    template<>
    class TypeDescriptorTraitsInv<std::string>
    {
    public:
      static const YACS::DynType _typeEnum=String;
    };

    /**
     *
     * This class has the responsability of the validity of the content of data regarding only statically its types from and to.
     *
     */
    template<DynType fromType, DynType toType >
    class StaticTypeConverter
    {
    public:
      enum { _staticallyCompatible=0 };
      static typename TypeDescriptorTraits<toType>::ConcreteType traduce(typename TypeDescriptorTraits<fromType>::ConcreteType input) throw(ConversionException)
      {
	throw ConversionException(TypeDescriptorTraits<fromType>::_name, TypeDescriptorTraits<toType>::_name);
      }
    };

    //Specialisation of the previously declared StaticTypeConverter class when 'fromType' and 'toType' are equal.
    template<DynType aType>
    class StaticTypeConverter<aType, aType>
    {
    public:
      enum { _staticallyCompatible=1 };
      static typename TypeDescriptorTraits<aType>::ConcreteType traduce(typename TypeDescriptorTraits<aType>::ConcreteType input) throw(ConversionException)
      {
	return input;
      }
    };

    template<>
    class StaticTypeConverter<YACS::Double, YACS::Int>
    {
    public:
      enum { _staticallyCompatible=1 };
      static TypeDescriptorTraits<YACS::Int>::ConcreteType traduce(TypeDescriptorTraits<YACS::Double>::ConcreteType input) throw(ConversionException)
      {
	return (TypeDescriptorTraits<YACS::Int>::ConcreteType) input;
      }
    };

    template<>
    class StaticTypeConverter<YACS::Int, YACS::Double>
    {
    public:
      enum { _staticallyCompatible=1 };
      static TypeDescriptorTraits<YACS::Double>::ConcreteType traduce(TypeDescriptorTraits<YACS::Int>::ConcreteType input) throw(ConversionException)
      {
	return (TypeDescriptorTraits<YACS::Double>::ConcreteType) input;
      }
    };

    template<>
    class StaticTypeConverter<YACS::Int, YACS::Bool>
    {
    public:
      enum { _staticallyCompatible=1 };
      static TypeDescriptorTraits<YACS::Bool>::ConcreteType traduce(TypeDescriptorTraits<YACS::Int>::ConcreteType input) throw(ConversionException)
      {
	if(input!=0)
	  return true;
	else
	  return false;
      }
    };

    template<>
    class StaticTypeConverter<YACS::Bool, YACS::Int>
    {
    public:
      enum { _staticallyCompatible=1 };
      static TypeDescriptorTraits<YACS::Int>::ConcreteType traduce(TypeDescriptorTraits<YACS::Bool>::ConcreteType input) throw(ConversionException)
      {
	if(input)
	  return 1;
	else
	  return 0;
      }
    };

    /**
     *
     * This class has the responsability of the validity of the content of data regarding statically and dynamically its types from and to.
     *
     */
    template<class container, DynType fromType, DynType toType >
    class StaticAndDynamicTypeConverter
    {
    public:
      static typename container::Type convert(typename container::Type source) throw(ConversionException)
      {
	if(!StaticTypeConverter<fromType, toType>::_staticallyCompatible)
	  throw ConversionException(TypeDescriptorTraits<fromType>::_name, TypeDescriptorTraits<toType>::_name);
	typename TypeDescriptorTraits<fromType>::ConcreteType sourceValue;
	container::readContent(source,sourceValue);
	typename TypeDescriptorTraits<toType>::ConcreteType targetValue=StaticTypeConverter<fromType, toType>::traduce(sourceValue);
	return container::createNewContent(targetValue);
      }
    };
  }
}

#endif
