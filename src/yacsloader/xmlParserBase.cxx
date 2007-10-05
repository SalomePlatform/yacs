
#include "xmlParserBase.hxx"
#include "Exception.hxx"

#include <stdexcept>
#include <iostream>
#include <cstdarg>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

// --- specific part for libxml2 ----------------------------------------------

#ifdef USE_LIBXML2
extern "C"
{
#include <libxml/parserInternals.h> // for xmlCreateFileParserCtxt
}
_xmlParserCtxt* xmlParserBase::_xmlParser;
void xmlParserBase::XML_SetUserData(_xmlParserCtxt* ctxt,
                                    xmlParserBase* parser)
{
  ctxt->userData = parser;
}
#endif

// --- specific part for expat ------------------------------------------------

#ifdef USE_EXPAT
XML_Parser xmlParserBase::_xmlParser;
#endif

// --- generic part -----------------------------------------------------------

using namespace std;
using YACS::Exception;

std::stack<xmlParserBase*> xmlParserBase::_stackParser;
std::list<xmlParserBase*>  xmlParserBase::_garbage;

/*! callback usable only with libxml2
 */

void XMLCALL xmlParserBase::start_document(void* userData)
{
  //DEBTRACE("xmlParserBase::start_document");
  xmlParserBase *currentParser = static_cast<xmlParserBase *> (userData);
}

/*! callback usable only with libxml2
 */

void XMLCALL xmlParserBase::end_document  (void* userData)
{
  //DEBTRACE("xmlParserBase::end_document");
  xmlParserBase *currentParser = static_cast<xmlParserBase *> (userData);
}

/*! callback called on start of an xml element: <name>
 */

void XMLCALL xmlParserBase::start_element (void* userData,
                                           const xmlChar* name,
                                           const xmlChar** p)
{
  //DEBTRACE("xmlParserBase::start_element " << name);
  cleanGarbage();
  xmlParserBase *currentParser = static_cast<xmlParserBase *> (userData);
  const XML_Char *aName = tochar(name);
  currentParser->incrCount(aName);
  currentParser->onStart(aName, p);
}


/*! callback called on end of an xml element: </name>
 */

void XMLCALL xmlParserBase::end_element   (void* userData,
                                           const xmlChar* name)
{
  //DEBTRACE("xmlParserBase::end_element");
  const XML_Char *aName = tochar(name);
  xmlParserBase *childParser = static_cast<xmlParserBase *> (userData);
  _garbage.push_back(_stackParser.top());
  DEBTRACE("xmlParserBase::end_element " << _garbage.size());
  _stackParser.pop();
  XML_SetUserData(_xmlParser, _stackParser.top());
  childParser->onEnd(aName);
  childParser->end();
 }


/*! callback called for significant characters inside tags: <tag>content</tag>
 *  or outside tags, like space or new line. 
 *  with expat get also the CDATA tags: <tag>![CDATA[content]]></tag>
 */

void XMLCALL xmlParserBase::characters    (void* userData,
                                           const xmlChar* ch,
                                           int len)
{
  //DEBTRACE("xmlParserBase::characters " << len);
  xmlParserBase *currentParser = (xmlParserBase *) (userData);
  string data((char*)ch,len);
  currentParser->charData(data);
}


/*! callback usable only with libxml2
 */

void XMLCALL xmlParserBase::comment       (void* userData,
                                           const xmlChar* value)
{
  //DEBTRACE("xmlParserBase::comment");
  xmlParserBase *currentParser = static_cast<xmlParserBase *> (userData);
}


/*! callback usable only with libxml2
 */

void XMLCALL xmlParserBase::warning       (void* userData,
                                           const char* fmt, ...)
{
  DEBTRACE("xmlParserBase::warning");
  xmlParserBase *currentParser = static_cast<xmlParserBase *> (userData);
  va_list args;
  va_start(args, fmt);
  string format = "%s";
  if (format == fmt)
    {
      char* parv;
      parv = va_arg(args, char*);
      cerr << parv ;
    }
  else cerr << __FILE__ << " [" << __LINE__ << "] : " 
            << "error format not taken into account: " << fmt << endl;
  va_end(args);
}


/*! callback usable only with libxml2
 */

void XMLCALL xmlParserBase::error         (void* userData,
                                           const char* fmt, ...)
{
  DEBTRACE("xmlParserBase::error");
  xmlParserBase *currentParser = static_cast<xmlParserBase *> (userData);
  va_list args;
  va_start(args, fmt);
  string format = "%s";
  if (format == fmt)
    {
      char* parv;
      parv = va_arg(args, char*);
      cerr << parv ;
      xmlParserBase *currentParser = (xmlParserBase *) userData;
      //cerr << currentParser->element << endl;
    }
  else cerr << __FILE__ << " [" << __LINE__ << "] : " 
            << "error format not taken into account: " << fmt << endl;
  va_end(args);
}


/*! callback usable only with libxml2
 */

void XMLCALL xmlParserBase::fatal_error   (void* userData,
                                           const char* fmt, ...)
{
  DEBTRACE("xmlParserBase::fatal_error");
  xmlParserBase *currentParser = static_cast<xmlParserBase *> (userData);
  va_list args;
  va_start(args, fmt);
  string format = "%s";
  if (format == fmt)
    {
      char* parv;
      parv = va_arg(args, char*);
      cerr << parv ;
    }
  else cerr << __FILE__ << " [" << __LINE__ << "] : " 
            << "error format not taken into account: " << fmt << endl;
  va_end(args);
}

/*! callback called for CDATA inside tags: <tag>![CDATA[content]]></tag>
 *  used only by libxml2
 */

void XMLCALL xmlParserBase::cdata_block   (void* userData,
                                           const xmlChar* value,
                                           int len)
{
  //DEBTRACE("xmlParserBase::cdata_block");
  xmlParserBase *currentParser = static_cast<xmlParserBase *> (userData);
  string data((char*)value,len);
  currentParser->charData(data);
}

void xmlParserBase::cleanGarbage()
{
  while (!_garbage.empty())
    {
      delete (_garbage.front());
      _garbage.pop_front();
    }
}

/*! Stores the tag attributes on a map. The map is an attribute of the parser
 *  object dedicated to the current xml tag
 */

void xmlParserBase::getAttributes(const xmlChar** p)
{
  if (p) while (*p)
    {
      string attributeName = (char*)*p;
      //cerr << "attribute name " << attributeName << endl;
      p++;
      string attributeValue = (char*)*p;
      //cerr << "attribute value " << attributeValue << endl;
      p++;
      _mapAttrib[attributeName] = attributeValue;
    }
}

/*! Stores an attribute (key, value) on the map attribute.
 *  used for attributes defined at another tag level (child tags). 
 */

void xmlParserBase::setAttribute(std::string key, std::string value)
{
  _mapAttrib[key] = value;
}

/*! Gets an attribute value given a string key.
 *  If the key does not exist, throws an Exception.
 */

std::string xmlParserBase::getAttribute(std::string key)
{
  if (_mapAttrib.find(key) == _mapAttrib.end())
    {
      string what = "Attribute does not exist: " + key;
      throw Exception(what);
    }
  return _mapAttrib[key];
}

/*! Add data on the data attribute of the parser object dedicated to an xml tag
 */

void xmlParserBase::addData(std::string value)
{
  //  DEBTRACE("xmlParserBase::addData()");
  _data += value;
}

/*! all parsers must know their father parser (father tag), in order
 *  to set values or attributes in father.
 */

void xmlParserBase::init (const xmlChar** p, xmlParserBase* father)
{
  _father = father;
}

/*! to be specialized for each kind of xml tag
 */

void xmlParserBase::onStart  (const XML_Char* elem, const xmlChar** p)
{
}

/*! to be specialized for each kind of xml tag
 */

void xmlParserBase::onEnd    (const XML_Char* name)
{
}

/*! to be specialized following the kind of xml tag
 */

void xmlParserBase::charData (std::string data)
{
}

/*! May be specialized for each kind of xml tag.
 *  Counts the number of tag occurences of a given type inside a context.
 */

void xmlParserBase::incrCount(const XML_Char *elem)
{
  if(counts.find(elem)==counts.end())
    counts[elem]=1;
  else
    counts[elem]=counts[elem]+1;
}

/*! to be specialized for each kind of xml tag
 */

void xmlParserBase::end()
{
}

/*! Throws an exception. different implementation with libxml2 and expat
 */

void xmlParserBase::stopParse(std::string what)
{
#ifdef USE_LIBXML2
  xmlStopParser(_xmlParser);
#endif
#ifdef USE_EXPAT
  throw Exception(what);
#endif  
}

// ----------------------------------------------------------------------------

#ifdef USE_LIBXML2

/*! libxml2 parser initialisation
 * \param parser dedicated parser
 */

xmlReader::xmlReader(xmlParserBase* parser): _rootParser(parser)
{
}

/*! libxml2 parse
 * \param xmlFile file to parse
 */

void xmlReader::parse(std::string xmlFile)
{
  _rootParser->init(0);
  _rootParser->_stackParser.push(_rootParser);

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
      xmlParserBase::start_document, // startDocument
      xmlParserBase::end_document,   // endDocument
      xmlParserBase::start_element,  // startElement
      xmlParserBase::end_element,    // endElement
      0,  // reference
      xmlParserBase::characters,     // characters
      0,  // ignorableWhitespace
      0,  // processingInstruction
      xmlParserBase::comment,        // comment
      xmlParserBase::warning,        // warning
      xmlParserBase::error,          // error
      xmlParserBase::fatal_error,    // fatalError
      0,  // getParameterEntity
      xmlParserBase::cdata_block,    // cdataBlock
      0  // externalSubset
    };

  // --- sequence from libxml++, to have a libxml context

  _xmlParserCtxt* saxContext;
  saxContext = xmlCreateFileParserCtxt(xmlFile.c_str());
  if (!saxContext)
    {
      _rootParser->cleanGarbage();
      string what = "problem while trying to open the file for parsing " + xmlFile;
      throw Exception(what);
    }
  xmlSAXHandlerPtr old_sax = saxContext->sax;
  saxContext->sax = &baseHandler;
  _rootParser->_xmlParser = saxContext;
  saxContext->userData = _rootParser;

  xmlParseDocument(saxContext);
  _rootParser->cleanGarbage();
  DEBTRACE("xmlParserBase::end of parse, garbage size = " << _rootParser->getGarbageSize());
}
#endif

// ----------------------------------------------------------------------------

#ifdef USE_EXPAT

#define SIZEBUF        8192
char Buffer[SIZEBUF];

/*! expat parser initialisation
 * \param parser dedicated parser
 */

xmlReader::xmlReader(xmlParserBase* parser): _rootParser(parser)
{
  xmlParserBase::_xmlParser= XML_ParserCreate(NULL);
  if (! _rootParser ) 
    {
      cerr << "Couldn't allocate memory for parser" << endl;
      throw Exception("Couldn't allocate memory for parser");
    }
}

/*! expat parse
 * \param xmlFile file to parse
 */

void xmlReader::parse(std::string xmlFile)
{
  FILE* fin=fopen(xmlFile.c_str(),"r");
  if (! fin) 
    {
      std::cerr << "Couldn't open schema file" << std::endl;
      throw std::invalid_argument("Couldn't open schema file");
      //throw Exception("Couldn't open schema file");
    }

  XML_SetElementHandler(xmlParserBase::_xmlParser,
                        xmlParserBase::start_element,
                        xmlParserBase::end_element);
  XML_SetCharacterDataHandler(xmlParserBase::_xmlParser,
                              xmlParserBase::characters );
  XML_SetUserData(xmlParserBase::_xmlParser, _rootParser);
  _rootParser->init(0);
  _rootParser->_stackParser.push(_rootParser);

  try
    {
      for (;;) 
        {
          int done;
          int len;

          len = fread(Buffer, 1, SIZEBUF, fin);
          if (ferror(fin)) 
            {
              std::cerr << "Read error" << std::endl;
              throw Exception("Read error");
            }
          done = feof(fin);

          if (XML_Parse(xmlParserBase::_xmlParser, Buffer, len, done) == XML_STATUS_ERROR) 
            {
              throw Exception(XML_ErrorString(XML_GetErrorCode(xmlParserBase::_xmlParser)));
            }

          if (done)
            break;
        }
      XML_ParserFree (xmlParserBase::_xmlParser);
      xmlParserBase::_xmlParser=0;
      _rootParser->cleanGarbage();
      DEBTRACE("xmlParserBase::end of parse, garbage size = " << _rootParser->getGarbageSize());
    }
  catch(Exception& e)
    {
      _rootParser->cleanGarbage();
      //get line number from XML parser
      cerr << "Error at line: " << XML_GetCurrentLineNumber(xmlParserBase::_xmlParser) << endl;
      throw e;
    }
}
#endif

// ----------------------------------------------------------------------------
