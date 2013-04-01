// Copyright (C) 2006-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef __XMLPARSERBASE_HXX_
#define __XMLPARSERBASE_HXX_


// --- select only one of the following packages ------------------------------
// - libxml2 comes with gnome, so it's almost always already installed,
//   but may not not work (see below).
// - libexpat is a less common package, but light, and works fine.

// With standard installation of libxml2, C++ exception cannot be catched 
// during the parse process. This is required for normal use of yacs.
// libxml2 must be generated with configure --with-fexceptions ...
// (to be tested)
// Developpement and tests are done with libexpat.

//#define USE_LIBXML2
#define USE_EXPAT

// --- specific part for libxml2 ----------------------------------------------

#ifdef USE_LIBXML2
extern "C"
{
#include <libxml/parser.h>
}
#define XMLCALL
#define XML_Char char
inline XML_Char* tochar(const xmlChar *c) { return (XML_Char*)c; };
#endif

// --- specific part for expat ------------------------------------------------

#ifdef USE_EXPAT
  #include <expat.h>
#define xmlChar XML_Char
inline const XML_Char* tochar(const xmlChar *c) { return c; };
#endif


// --- generic part -----------------------------------------------------------

#include <string>
#include <map>
#include <stack>
#include <list>

class xmlParserBase;

//! \brief base class for parse an xml file, use a dedicated parser, runtime independant.

class xmlReader
{
public:
  xmlReader(xmlParserBase* parser);
  virtual void parse(std::string xmlFile);
protected:
  xmlParserBase* _rootParser;
};

//! \brief base class for xml parsers, runtime independant

class xmlParserBase
{
public:
#ifndef SWIG
  static void XMLCALL start_document(void* userData);
  static void XMLCALL end_document  (void* userData);
  static void XMLCALL start_element (void* userData,
                                     const xmlChar* name,
                                     const xmlChar** p);
  static void XMLCALL end_element   (void* userData,
                                     const xmlChar* name);
  static void XMLCALL characters    (void* userData,
                                     const xmlChar* ch,
                                     int len);
  static void XMLCALL comment       (void* userData,
                                     const xmlChar* value);
  static void XMLCALL warning       (void* userData,
                                     const char* fmt, ...);
  static void XMLCALL error         (void* userData,
                                     const char* fmt, ...);
  static void XMLCALL fatal_error   (void* userData,
                                     const char* fmt, ...);
  static void XMLCALL cdata_block   (void* userData,
                                     const xmlChar* value,
                                     int len);
#endif
  static void cleanGarbage();
  static int getGarbageSize() {return _garbage.size(); };
public:
  void setAttribute(std::string key, std::string value);
  std::string getAttribute(std::string key);
  virtual void addData(std::string value);
  virtual void init (const xmlChar** p, xmlParserBase* father=0);

  std::map< std::string, int > counts;

#ifdef USE_LIBXML2
  static _xmlParserCtxt* _xmlParser;
  static void XML_SetUserData(_xmlParserCtxt* ctxt,
                              xmlParserBase* parser);
#endif

#ifdef USE_EXPAT
  static XML_Parser _xmlParser;
#endif

  static std::stack<xmlParserBase*> _stackParser;

protected:
  void getAttributes(const xmlChar** p);

  virtual void onStart  (const XML_Char* elem, const xmlChar** p);
  virtual void onEnd    (const XML_Char* name);
  virtual void charData (std::string data);
  virtual void incrCount(const XML_Char *elem);
  virtual void end();
  virtual void stopParse(std::string what);

protected:
  std::map<std::string, std::string> _mapAttrib;
  static std::list<xmlParserBase*> _garbage;
  std::string _data;
  xmlParserBase *_father;
};

#endif
