#include "ContentOfDataFlow.hxx"

using namespace YACS::ENGINE;

const char ContentOfDataFlow::STRFORNULLREPR[]="NULL";

template< YACS::DynType type >
class TreatmentForNewContentDataFlow
{
public:
  static void perform(const std::string& input, ContentOfDataFlow::Type& ret)
  {
    ret=ContentOfDataFlow::createNewContent<type>(input);
  }
};

template< YACS::DynType fromType, YACS::DynType toType >
class TreatmentForConvertContentDataFlow
{
  public:
  static void perform(ContentOfDataFlow::Type input, ContentOfDataFlow::Type& ret)
  {
    ret=StaticAndDynamicTypeConverter<ContentOfDataFlow,fromType,toType>::convert(input);
  }
};

template< YACS::DynType fromType, YACS::DynType toType >
class TreatmentToCheckStaticallyTypes
{
  public:
  static void perform(ContentOfDataFlow::Type input, bool& ret)
  {
    ret=StaticTypeConverter<fromType,toType>::_staticallyCompatible;
  }
};

void ContentOfDataFlow::duplicate(ContentOfDataFlow::Type content)
{
  if(content!=neuter())
    {
      int *contentC=(int *)content;
      (*contentC)=(*contentC)+1;
    }
}

void ContentOfDataFlow::release(ContentOfDataFlow::Type& content)
{
  if(content!=neuter())
    {
      int *contentC=(int *)content;
      (*contentC)=(*contentC)-1;
      if((*contentC)==0)
	{
	  free(content);
	  content=neuter();
	}
    }
}

ContentOfDataFlow::Type ContentOfDataFlow::createNewContent(const std::string& content, DynType typeOfContent) throw(Exception)
{
  ContentOfDataFlow::Type ret;
  DynToStaticTypeDispatcher1<TreatmentForNewContentDataFlow>::perform(typeOfContent, content, ret);
  return ret;
}

ContentOfDataFlow::Type ContentOfDataFlow::convertContent(ContentOfDataFlow::Type content, DynType fromType, DynType toType) throw(ConversionException)
{
  if(fromType==toType)
    return content;
  else
    {
      ContentOfDataFlow::Type ret;
      DynToStaticTypeDispatcher2<TreatmentForConvertContentDataFlow>::perform(fromType, toType, content, ret);
      return ret;
    }
}

bool ContentOfDataFlow::isStaticallyCompatibleWith(DynType fromType, DynType toType) throw(Exception)
{
  bool ret;
  DynToStaticTypeDispatcher2<TreatmentToCheckStaticallyTypes>::perform(fromType, toType, ContentOfDataFlow::neuter(), ret);
  return ret;
}

std::string ContentOfDataFlow::getRepresentation(ContentOfDataFlow::Type content)
{
  if(content!=ContentOfDataFlow::neuter())
    {
      char *contentC=(char *)content;
      contentC+=sizeof(int);
      return std::string(contentC);
    }
  else
    return ContentOfDataFlow::STRFORNULLREPR;
}

ContentOfDataFlow::Type ContentOfDataFlow::allocate(int lgth)
{
  ContentOfDataFlow::Type ret=malloc(lgth+1+sizeof(int));
  int *ret2=(int *)ret;
  *ret2=1;
  return ret;
}
