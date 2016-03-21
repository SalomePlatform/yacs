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

#ifndef __LOADSTATE_HXX_
#define __LOADSTATE_HXX_

#include "YACSloaderExport.hxx"
#include "xmlParserBase.hxx"

#include "define.hxx"
#include "Exception.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
    class Runtime;

    //! Load state from a file into a Proc
    /*!
     * \param p: the Proc
     * \param xmlStateFile: the file name
     */
    YACSLOADER_EXPORT void loadState(YACS::ENGINE::Proc *p,const std::string& xmlStateFile);

    /*! \brief class for parse an xml file, use a dedicated parser, to load a
     *  saved state of a SALOME execution.
     */

    class YACSLOADER_EXPORT stateLoader: public xmlReader
    {
    public:
      stateLoader(xmlParserBase* parser,
                  YACS::ENGINE::Proc* p);
      virtual void parse(std::string xmlState);
    protected:
      Proc* _p;
      Runtime* _runtime;
    };

    typedef enum
      {
        XMLNOCONTEXT  = 0,
        XMLINGRAPH    = 1,
        XMLINNODE     = 2,
        XMLINPORT     = 3,
        XMLINVALUE    = 4,
        XMLDONE       = 5,
        XMLFATALERROR = 6
      } XMLReadState;

    //! \brief specialized parser to load SALOME execution saved states.
    /*! this base class must be derived to build specific parsers for each tag
     *  defined in the xml file
     */

    class YACSLOADER_EXPORT stateParser: public xmlParserBase
    {
    public:
      static XMLReadState _state;
      static std::string _what;

      static void setProc(Proc* p);
      static void setRuntime(Runtime* runtime);

    public:
      virtual void init(const xmlChar** p, xmlParserBase* father=0);

    protected:
      virtual void onStart (const XML_Char* elem, const xmlChar** p);
      virtual void onEnd   (const XML_Char* name);
      virtual void charData(std::string data);

    protected:
      static std::stack<XMLReadState> _stackState;
      static Proc* _p;
      static Runtime* _runtime;
      static std::map<std::string, YACS::StatesForNode> _nodeStateValue;
      static std::map<std::string, YACS::StatesForNode> _nodeStates;
    };

    class YACSLOADER_EXPORT graphParser: public stateParser
    {
    public:
      virtual void init(const xmlChar** p, xmlParserBase* father=0);
      virtual void onStart (const XML_Char* elem, const xmlChar** p);
      virtual void onEnd   (const XML_Char* name);
    };


    class YACSLOADER_EXPORT nodeParser: public stateParser
    {
    public:
      virtual void init(const xmlChar** p, xmlParserBase* father=0);
      virtual void onStart (const XML_Char* elem, const xmlChar** p);
      virtual void onEnd   (const XML_Char* name);
      std::string _nodeName;
      std::string _nodeState;
    };

    class YACSLOADER_EXPORT attrParser: public stateParser
    {
    public:
      virtual void init(const xmlChar** p, xmlParserBase* father=0);
      virtual void onStart (const XML_Char* elem, const xmlChar** p);
      virtual void charData(std::string data);
      virtual void onEnd   (const XML_Char* name);
      std::string _attrValue;
    };


    class YACSLOADER_EXPORT portParser: public stateParser
    {
    public:
      virtual void init(const xmlChar** p, xmlParserBase* father=0);
      virtual void onStart (const XML_Char* elem, const xmlChar** p);
      virtual void onEnd   (const XML_Char* name);
      virtual void addData(std::string value);
    };

    class YACSLOADER_EXPORT valueParser: public stateParser
    {
    public:
      virtual void init(const xmlChar** p, xmlParserBase* father=0);
      virtual void onStart (const XML_Char* elem, const xmlChar** p);
      virtual void onEnd   (const XML_Char* name);
      virtual void addData(std::string value);
    };

    class YACSLOADER_EXPORT arrayParser: public stateParser
    {
    public:
      virtual void init(const xmlChar** p, xmlParserBase* father=0);
      virtual void onStart (const XML_Char* elem, const xmlChar** p);
      virtual void onEnd   (const XML_Char* name);
      virtual void addData(std::string value);
    };

    class YACSLOADER_EXPORT dataParser: public stateParser
    {
    public:
      virtual void init(const xmlChar** p, xmlParserBase* father=0);
      virtual void onStart (const XML_Char* elem, const xmlChar** p);
      virtual void onEnd   (const XML_Char* name);
      virtual void addData(std::string value);
      std::list<std::string> _dataList;
    };

    class YACSLOADER_EXPORT simpleTypeParser: public stateParser
    {
    public:
      virtual void init(const xmlChar** p, xmlParserBase* father=0);
      virtual void onStart (const XML_Char* elem, const xmlChar** p);
      virtual void onEnd   (const XML_Char* name);
      virtual void charData(std::string data);
    };

  }
}
#endif
