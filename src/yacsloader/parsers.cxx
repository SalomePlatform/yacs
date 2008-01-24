
#include "parsers.hxx"

#include <expat.h>
#include <iostream>
#include <stdexcept>

#include "Runtime.hxx"
#include "Proc.hxx"
#include "ProcCataLoader.hxx"
#include "Logger.hxx"

#include "rootParser.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

YACS::ENGINE::Runtime* theRuntime=0;

#define BUFFSIZE        8192

char Buff[BUFFSIZE];

extern YACS::ENGINE::Proc* currentProc;
extern XML_Parser p ;

namespace YACS
{

YACSLoader::YACSLoader()
{
  _defaultParsersMap.clear();

  theRuntime = ENGINE::getRuntime();
}

void YACSLoader::registerProcCataLoader()
{
  YACS::ENGINE::ProcCataLoader* factory= new YACS::ENGINE::ProcCataLoader(this);
  theRuntime->setCatalogLoaderFactory("proc",factory);
}

ENGINE::Proc* YACSLoader::load(const char * file)
{
  DEBTRACE("YACSLoader::load: " << file);
  FILE* fin=fopen(file,"r");
  if (! fin) 
    {
      std::cerr << "Couldn't open schema file" << std::endl;
      throw std::invalid_argument("Couldn't open schema file");
    }

  p = XML_ParserCreate(NULL);
  if (! p) 
    {
      std::cerr << "Couldn't allocate memory for parser" << std::endl;
      throw Exception("Couldn't allocate memory for parser");
    }
  XML_SetElementHandler(p, parser::start,parser::end);
  XML_SetCharacterDataHandler(p,parser::charac );

  parser::main_parser.SetUserDataAndPush(&YACS::roottypeParser::rootParser);

  // OCC: san -- Allow external parsers for handling of unknown elements
  // and attributes. This capability is used by YACS GUI to read
  // graph presentation data
  if ( !_defaultParsersMap.empty() )
    roottypeParser::rootParser.setDefaultMap(&_defaultParsersMap);
  else
    roottypeParser::rootParser.setDefaultMap(0);
  
  parser::main_parser._file=file;

  currentProc=0;

  try
    {
      for (;;) 
        {
          int done;
          int len;

          len = fread(Buff, 1, BUFFSIZE, fin);
          if (ferror(fin)) 
            {
              std::cerr << "Read error" << std::endl;
              throw Exception("Read error");
            }
          done = feof(fin);

          if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) 
            {
              if(currentProc==0)
                break;
              YACS::ENGINE::Logger* logger=currentProc->getLogger("parser");
              logger->fatal(XML_ErrorString(XML_GetErrorCode(p)),file,XML_GetCurrentLineNumber(p));
              break;
            }

          if (done)
            break;
        }
      XML_ParserFree (p);
      p=0;
      return currentProc;
    }
  catch(Exception& e)
    {
      //get line number from XML parser
      YACS::ENGINE::Logger* logger=currentProc->getLogger("parser");
      logger->fatal(e.what(),file,XML_GetCurrentLineNumber(p));
      XML_ParserFree (p);
      p=0;
      return currentProc;
    }
}

YACSLoader::~YACSLoader()
{
}

}
