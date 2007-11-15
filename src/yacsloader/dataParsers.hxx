#ifndef _DATAPARSERS_HXX_
#define _DATAPARSERS_HXX_

#include "parserBase.hxx"

namespace YACS
{
/*! \brief Class for string parser.
 *
 *  Class used to parse string
 */
struct stringtypeParser:parser
{
  static stringtypeParser stringParser;
  std::string post();
};

/*! \brief Class for double parser.
 *
 *  Class used to parse double 
 */
struct doubletypeParser:parser
{
  static doubletypeParser doubleParser;
  double post();
};

/*! \brief Class for integer parser.
 *
 *  Class used to parse integer
 */
struct inttypeParser:parser
{
  static inttypeParser intParser;
  int post();
};

/*! \brief Class for boolean parser.
 *
 *  Class used to parse bool
 */
struct booltypeParser:parser
{
  static booltypeParser boolParser;
  bool post();
};

}

#endif
