
#include "dataParsers.hxx"
#include "Cstr2d.hxx"
#include <sstream>

namespace YACS
{
  stringtypeParser stringtypeParser::stringParser;
  doubletypeParser doubletypeParser::doubleParser;
  inttypeParser inttypeParser::intParser;
  booltypeParser booltypeParser::boolParser;

std::string stringtypeParser::post()
    {
      return _content;
    }

  double doubletypeParser::post()
    {
      return Cstr2d(_content.c_str());
//       std::istringstream s(_content.c_str());
//       double a;
//       if (!(s >> a))
//         throw YACS::Exception::Exception("problem in conversion from string to double");
//       std::cerr << "--------------_content s a "<< _content.c_str() << " " << s.str() << " " << a << std::endl;
//       return a;
    }

  int inttypeParser::post()
    {
      return atoi(_content.c_str());
    }

  bool booltypeParser::post()
    {
      DEBTRACE( _content )             
      if(_content == "true")return true;
      if(_content == "false")return false;
      std::stringstream temp(_content);
      bool b ;
      temp >> b;
      //std::cerr << b << std::endl;
      return b;
    }

}
