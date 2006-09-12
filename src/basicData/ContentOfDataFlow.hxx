#ifndef __CONTENTOFDATAFLOW_HXX__
#define __CONTENTOFDATAFLOW_HXX__

#include "define.hxx"
#include "TypeCheckerDataFlow.hxx"
#include "ConversionException.hxx"

#include <string>
#include <cstring>
#include <sstream>
#include <stdlib.h>

namespace YACS
{
  namespace ENGINE
  {
    class ContentOfDataFlow
    {
    public:
      typedef void * Type;

      static void duplicate(Type content);
      static void release(Type& content);
      static Type createNewContent(const std::string& content, DynType typeOfContent) throw(Exception);
      static Type convertContent(Type content, DynType fromType, DynType toType) throw(ConversionException);
      static bool isStaticallyCompatibleWith(DynType fromType, DynType toType) throw(Exception);
      template<class T>
      static Type createNewContent(T value);
      template<YACS::DynType typ>
      static Type createNewContent(const std::string& content) throw(Exception);
      template<class T>
      static void readContent(Type content, T& value) throw(Exception);
      static Type neuter() { return 0; }
      static std::string getRepresentation(Type content);
    private:
      static Type allocate(int lgth);
    private:
      static const char STRFORNULLREPR[];
    };

    template<class T>
    ContentOfDataFlow::Type ContentOfDataFlow::createNewContent(T value)
    {
      std::ostringstream stream;
      stream << value;
      int length=stream.str().length();
      ContentOfDataFlow::Type ret=allocate(length);
      int *retC=(int *)ret;
      memcpy(retC+1,stream.str().c_str(),length+1);
      return ret;
    }

    template<class T>
    void ContentOfDataFlow::readContent(ContentOfDataFlow::Type content, T& value) throw(Exception)
    {
      if(content==ContentOfDataFlow::neuter())
	throw Exception("no content to read");
      char *contentC=(char *)content;
      contentC+=sizeof(int);
      const DynType typ=TypeDescriptorTraitsInv<T>::_typeEnum;
      std::istringstream stream(contentC);
      stream >> value;
      if(stream.fail() || !stream.eof())
	{
	  std::string what="Content not recognized as "; what+=TypeDescriptorTraits<TypeDescriptorTraitsInv<T>::_typeEnum>::_name;
	  throw Exception(what);
	}
    }

    template<YACS::DynType typ>
    ContentOfDataFlow::Type ContentOfDataFlow::createNewContent(const std::string& content) throw(Exception)
    {
      std::istringstream stream(content);
      typename TypeDescriptorTraits<typ>::ConcreteType value;
      stream >> value;
      if(!stream.fail() && stream.eof())
	return createNewContent(value);
      else
	{
	  std::string what(content);
	  what+=" is not recognized as "; what+=TypeDescriptorTraits<typ>::_name;
	  throw Exception(what);
	}
    }
  }
}

#endif
