// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "parsers.hxx"

#include <iostream>
#include <stdexcept>
#include <cstdio>

#include "Runtime.hxx"
#include "Proc.hxx"
#include "ProcCataLoader.hxx"
#include "Logger.hxx"

#include "rootParser.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/parserInternals.h>
YACS::ENGINE::Runtime* theRuntime=0;

#define BUFFSIZE        8192

char Buff[BUFFSIZE];

extern YACS::ENGINE::Proc* currentProc;
extern _xmlParserCtxt* saxContext;
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
  parser::main_parser.init();
  parser::main_parser._stackParser.push(&parser::main_parser);
  xmlSAXHandler baseHandler =
  {
    0,  // internal_subset,
    0,  // isStandalone
    0,  // hasInternalSubset
    0,  // hasExternalSubset
    0,  // resolveEntity
    0,  // getEntity
    0,  // entityDecl
    0,  // notationDecl
    0,  // attributeDecl
    0,  // elementDecl
    0,  // unparsedEntityDecl
    0,  // setDocumentLocator
    parser::start_document, // startDocument
    parser::end_document,   // endDocument
    parser::start_element,  // startElement
    parser::end_element,    // endElement
    0,  // reference
    parser::characters,     // characters
    0,  // ignorableWhitespace
    0,  // processingInstruction
    parser::comment,        // comment
    parser::warning,  // warning
    parser::error,  // error
    parser::fatal_error,  // fatalError
    0,  // getParameterEntity
    parser::cdata_block,    // cdataBlock
    0  // externalSubset
  };

  FILE* fin=fopen(file,"r");
  if (! fin)
  {
    std::cerr << "Couldn't open schema file" << std::endl;
    throw std::invalid_argument("Couldn't open schema file");
  }

  saxContext = xmlCreateFileParserCtxt(file);

  if (!saxContext)
  {
    std::cerr << "Couldn't allocate memory for parser" << std::endl;
    throw Exception("Couldn't allocate memory for parser");
  }
  saxContext->sax = &baseHandler;
  parser::main_parser.SetUserDataAndPush(&YACS::roottypeParser::rootParser);
  if ( !_defaultParsersMap.empty() )
    roottypeParser::rootParser.setDefaultMap(&_defaultParsersMap);
  else
    roottypeParser::rootParser.setDefaultMap(0);

  parser::main_parser._file = file;
  currentProc = 0;

  try
  {
    if ( xmlParseDocument(saxContext) == -1 )
    {
      if(!currentProc)
        throw Exception("Basic error during parsing.");
      YACS::ENGINE::Logger* logger = currentProc->getLogger("parser");
      logger->fatal( saxContext->lastError.message, file, saxContext->input->line );
    }
    saxContext = 0;
    return currentProc;
  }
  catch(Exception& e)
  {
    if(!currentProc)
      throw e;
    YACS::ENGINE::Logger* logger = currentProc->getLogger("parser");
    logger->fatal(e.what(), file, saxContext->input->line);
    saxContext = 0;
    return currentProc;
  }
}

YACSLoader::~YACSLoader()
{
}

}
