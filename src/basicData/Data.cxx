#include "Data.hxx"

using namespace YACS::ENGINE;

const char Data::UNRECOGNIZEDTYPENAME[]="Unrecognized type";

Data::Data(DynType typeOfContent):_edType(typeOfContent),_exContent(ContentOfDataFlow::neuter())
{
}

Data::Data(const std::string& content, DynType typeOfContent) throw(Exception):_edType(typeOfContent)
{
  _exContent=ContentOfDataFlow::createNewContent(content,typeOfContent);
}

Data::Data(const Data& other):_edType(other._edType)
{
  ContentOfDataFlow::duplicate(other._exContent);
  _exContent=other._exContent;
}

Data::~Data()
{
  ContentOfDataFlow::release(_exContent);
}

Data &Data::operator =(const Data& other) throw(ConversionException)
{
  if(&other!=this)
    {
      if(other._exContent==ContentOfDataFlow::neuter())
	{
	  ContentOfDataFlow::release(_exContent);
	  _exContent=ContentOfDataFlow::neuter();
	  return *this;
	}
      if(other._edType==_edType)
	{
	  ContentOfDataFlow::duplicate(other._exContent);
	  ContentOfDataFlow::release(_exContent);
	  _exContent=other._exContent;
	}
      else
	{
	  ContentOfDataFlow::release(_exContent);
	  _exContent=ContentOfDataFlow::convertContent(other._exContent,other._edType,_edType);
	}
    }
  return *this;
}

std::string Data::edGetRepresentation() const
{
  std::ostringstream stream;
  stream << _edType << "_" << ContentOfDataFlow::getRepresentation(_exContent);
  return stream.str();
}

void Data::edSetNewType(DynType newTypeOfContent) throw(ConversionException)
{
  if(_edType==newTypeOfContent)
    return;
  ContentOfDataFlow::Type newContent=ContentOfDataFlow::convertContent(_exContent,_edType,newTypeOfContent);
  ContentOfDataFlow::release(_exContent);
  _exContent=newContent;
  _edType=newTypeOfContent;
}

void Data::exSetContent(const std::string& content) throw(ConversionException)
{
  ContentOfDataFlow::release(_exContent);
  _exContent=ContentOfDataFlow::createNewContent(content,_edType);
}

void Data::edInitToType(DynType typeOfContent)
{
  ContentOfDataFlow::release(_exContent);
  _exContent=ContentOfDataFlow::neuter();
  _edType=typeOfContent;
}

void Data::exInit()
{
  ContentOfDataFlow::release(_exContent);
  _exContent=ContentOfDataFlow::neuter();
}

// Part of Data::edGetType implementation
template< YACS::DynType type >
class TreatmentForNameOfType
{
public:
  static void perform(int input, std::string& ret)
  {
    ret=TypeDescriptorTraits<type>::_name;
  }
};

std::string Data::edGetTypeInPrintableForm(DynType type)
{
  try
    {
      int fake1;
      std::string ret;
      DynToStaticTypeDispatcher1<TreatmentForNameOfType>::perform(type, fake1, ret);
      return ret;
    }
  catch(Exception& e)
    {
      return UNRECOGNIZEDTYPENAME;
    }
}

bool Data::isStaticallyCompatibleWith(const Data& other) const throw(Exception)
{
  return areStaticallyCompatible(_edType, other._edType);
}

bool Data::areStaticallyCompatible(DynType type1, DynType type2) throw(Exception)
{
  return ContentOfDataFlow::isStaticallyCompatibleWith(type1, type2);
}

bool Data::empty() const
{
  return _exContent==ContentOfDataFlow::neuter();
}
