#ifndef __DATA_HXX__
#define __DATA_HXX__

#include "define.hxx"
#include "ContentOfDataFlow.hxx"
#include "ConversionException.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    /**
     *
     * Manage Data exchanged through dataflow pipe between 2 nodes. This class minimizes duplications of data.
     * WARNING : Data has a type defined on constructor or after having called edSetNewType method.
     *          All the methods that change the content of data check the validity regarding _edType attribute.
     *
     */
    class Data
    {
    private:
      DynType _edType;
      ContentOfDataFlow::Type _exContent;
      static const char Data::UNRECOGNIZEDTYPENAME[];
    public:
      Data(DynType typeOfContent);
      Data(const std::string& content, DynType typeOfContent) throw(Exception);
      template<class T>
      Data(T val);
      Data(const Data& other);
      ~Data();
      Data &operator =(const Data& other) throw(ConversionException);
      template<class T>
      Data &operator =(T val) throw(ConversionException);
      std::string edGetRepresentation() const;
      void edSetNewType(DynType newTypeOfContent) throw(ConversionException);
      void exSetContent(const std::string& content) throw(ConversionException);
      template<class T>
      T exGet() const throw(ConversionException);
      void edInitToType(DynType typeOfContent);
      void exInit();
      DynType edGetType() const { return _edType; }
      static std::string edGetTypeInPrintableForm(DynType type);
      bool isStaticallyCompatibleWith(const Data& other) const throw(Exception);
      static bool areStaticallyCompatible(DynType type1, DynType type2) throw(Exception);
      bool empty() const;
    };

    template<class T>
    Data::Data(T val)
    {
      _edType=TypeDescriptorTraitsInv<T>::_typeEnum;
      _exContent=ContentOfDataFlow::createNewContent(val);
    }

    template<class T>
    Data &Data::operator =(T val) throw(ConversionException)
    {
      ContentOfDataFlow::release(_exContent);
      typename ContentOfDataFlow::Type content=ContentOfDataFlow::createNewContent(val);
      if(_edType==TypeDescriptorTraitsInv<T>::_typeEnum)
	{
	  _exContent=content;
	}
      else
	{
	  _exContent=ContentOfDataFlow::convertContent(content,TypeDescriptorTraitsInv<T>::_typeEnum,_edType);
	  ContentOfDataFlow::release(content);
	}
      return *this;
    }
    
    template<class T>
    T Data::exGet() const throw(ConversionException)
    {
      T ret;
      ContentOfDataFlow::readContent(_exContent,ret);
      return ret;
    }
  }
}

#endif
