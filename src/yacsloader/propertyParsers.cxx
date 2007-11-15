
#include "propertyParsers.hxx"


namespace YACS
{
  propertytypeParser propertytypeParser::propertyParser;

  void propertytypeParser::buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      required("value",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "value")value(attr[i+1]);
      }
    }
  void propertytypeParser::name(const std::string& name){ _prop._name=name; }
  void propertytypeParser::value(const std::string& name){ _prop._value=name; }
  myprop propertytypeParser::post(){return _prop;}

}
