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

#include "XMLNode.hxx"
#include "XMLPorts.hxx"
#include "Mutex.hxx"
#include "TypeCode.hxx"
#include "AutoLocker.hxx"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#include <io.h>
#define chmod _chmod
#endif

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char XmlNode::IMPL_NAME[]="XML";
const char XmlNode::KIND[]="xmlsh";
static YACS::BASES::Mutex MUTEX;

XmlNode::XmlNode(const XmlNode& other, ComposedNode *father)
  : _script(other._script), ServiceNode(other, father)
{
  _implementation=IMPL_NAME;
  _ref = other._ref;
}

XmlNode::XmlNode(const std::string& name)
  : ServiceNode(name)
{
  _implementation=IMPL_NAME;
}

Node *XmlNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new XmlNode(*this,father);
}

void XmlNode::setRef(const std::string& ref)
{
  //No component instance here
  _ref=ref;
}

void XmlNode::setScript(const std::string& script)
{
  _script=script;
}

std::string XmlNode::getScript() const
{
  return _script;
}

std::string XmlNode::getKind() const
{
  return KIND;
}

void XmlNode::execute()
{
  DEBTRACE("execute");
  char dir[]="yacsXXXXXX";
  // add a lock around mkdtemp (seems not thread safe)
  {
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&MUTEX);
#ifdef WIN32
    char mdir [512+1];
    GetTempPath(MAX_PATH+1, mdir);
    CreateDirectory(mdir, NULL);
#else
    char* mdir=mkdtemp(dir);
#endif
    if(mdir==NULL)
      {
        perror("mkdtemp failed");
        std::cerr << "Problem in mkdtemp " << dir << " " << mdir << std::endl;
        throw Exception("Execution problem in mkdtemp");
      }
  }
  std::string sdir(dir);
  std::string input=sdir+"/input";
  std::ofstream f(input.c_str());
  f<<"<methodCall> <methodName>" << _method << "</methodName> <params>"<<std::endl;
  DEBTRACE("---------------XmlNode::inputs---------------");
  list<InputPort *>::iterator iter;
  for(iter = _setOfInputPort.begin(); iter != _setOfInputPort.end(); iter++)
    {
      InputXmlPort *p=(InputXmlPort *)*iter;
      DEBTRACE("port name: " << p->getName());
      DEBTRACE("port kind: " << p->edGetType()->kind());
      const char* ob=p->getXml();
      DEBTRACE("Xml: " << ob );
      f<<"<param>" << ob << "</param>"<<std::endl;
    }
  f<<"</params>"<<std::endl;
  f<<"</methodCall>"<<std::endl;
  f.close();
  DEBTRACE("--------------XmlNode::calculation---------------" << _ref );
  std::string call=sdir+"/run.sh";
  std::ofstream run(call.c_str());
  run << "#!/bin/sh" << std::endl;
  run << "cd " << sdir << std::endl;
  if(_ref[0]=='/' || _ref[0]=='~')
    run << _ref << "> stdout 2>&1 " << std::endl;
  else
    run << "../"<<_ref << "> stdout 2>&1 " << std::endl;
  //run << "cat stdout" << std::endl;
  run.close();
  chmod(call.c_str(),00777);

  std::string call2="/bin/sh "+call;
  int ret=system(call2.c_str());
  if(ret)
    {
      std::cerr << "Problem: " << ret << std::endl;
      DEBTRACE("Problem: " << ret);
      throw Exception("Execution problem");
    }
  std::string output=sdir+"/output";
  xmlDocPtr doc; 
  doc = xmlReadFile(output.c_str(), NULL, 0);
  if (doc == NULL) 
    {
      DEBTRACE("Failed to parse " << output);
      throw Exception("Execution problem");
    }
  xmlNodePtr cur;
  cur = xmlDocGetRootElement(doc);
  if (cur == NULL) 
    {
      DEBTRACE("empty document " );
      xmlFreeDoc(doc);
      throw Exception("Execution problem");
    }
  if (xmlStrcmp(cur->name, (const xmlChar *) "methodResponse")) 
    {
      DEBTRACE("document of the wrong type, root node != methodResponse");
      xmlFreeDoc(doc);
      throw Exception("Execution problem");
    }
  cur = cur->xmlChildrenNode;
  xmlBufferPtr buf=xmlBufferCreate();
  list<OutputPort *>::iterator iter2;
  iter2 = _setOfOutputPort.begin(); 
  OutputXmlPort *p;
  p=(OutputXmlPort *)*iter2;
  int nres=0;

  while (cur != NULL) 
    {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"fault")))
        {
          DEBTRACE("exception in shell" );
          xmlFreeDoc(doc);
          throw Exception("Execution problem");
        }
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"params")))
        {
          xmlNodePtr cur0 = cur->xmlChildrenNode;
          while (cur0 != NULL)
            {
              if ((!xmlStrcmp(cur0->name, (const xmlChar *)"param")))
                {
                  xmlNodePtr cur1 = cur0->xmlChildrenNode;
                  while (cur1 != NULL)
                    {
                      if ((!xmlStrcmp(cur1->name, (const xmlChar *)"value")))
                        {
                          xmlNodePtr cur2=cur1->xmlChildrenNode;
                          while (cur2 != NULL)
                            {
                              if ((!xmlStrcmp(cur2->name, (const xmlChar *)"int")))
                                {
                                  //got an int
                                  if(getNumberOfOutputPorts()!=1)
                                    {
                                      //mismatch
                                      xmlBufferFree(buf);
                                      xmlFreeDoc(doc);
                                      throw Exception("Execution problem:mismatch in output numbers");
                                    }
                                  xmlBufferEmpty(buf);
                                  xmlNodeDump(buf,doc,cur1,0,0);
                                  DEBTRACE(xmlBufferContent(buf));
                                  p->put(xmlBufferContent(buf));
                                }
                              if ((!xmlStrcmp(cur2->name, (const xmlChar *)"double")))
                                {
                                  //got an double
                                  if(getNumberOfOutputPorts()!=1)
                                    {
                                      //mismatch
                                      xmlBufferFree(buf);
                                      xmlFreeDoc(doc);
                                      throw Exception("Execution problem:mismatch in output numbers");
                                    }
                                  xmlBufferEmpty(buf);
                                  xmlNodeDump(buf,doc,cur1,0,0);
                                  DEBTRACE(xmlBufferContent(buf));
                                  p->put(xmlBufferContent(buf));
                                }
                              if ((!xmlStrcmp(cur2->name, (const xmlChar *)"string")))
                                {
                                  //got an string
                                  if(getNumberOfOutputPorts()!=1)
                                    {
                                      //mismatch
                                      xmlBufferFree(buf);
                                      xmlFreeDoc(doc);
                                      throw Exception("Execution problem:mismatch in output port numbers");
                                    }
                                  xmlBufferEmpty(buf);
                                  xmlNodeDump(buf,doc,cur1,0,0);
                                  DEBTRACE(xmlBufferContent(buf));
                                  p->put(xmlBufferContent(buf));
                                }
                              if ((!xmlStrcmp(cur2->name, (const xmlChar *)"boolean")))
                                {
                                  //got an boolean
                                  if(getNumberOfOutputPorts()!=1)
                                    {
                                      //mismatch
                                      xmlBufferFree(buf);
                                      xmlFreeDoc(doc);
                                      throw Exception("Execution problem:mismatch in output port numbers");
                                    }
                                  xmlBufferEmpty(buf);
                                  xmlNodeDump(buf,doc,cur1,0,0);
                                  DEBTRACE(xmlBufferContent(buf));
                                  p->put(xmlBufferContent(buf));
                                }
                              if ((!xmlStrcmp(cur2->name, (const xmlChar *)"objref")))
                                {
                                  //got an objref
                                  if(getNumberOfOutputPorts()!=1)
                                    {
                                      //mismatch
                                      xmlBufferFree(buf);
                                      xmlFreeDoc(doc);
                                      throw Exception("Execution problem:mismatch in output port numbers");
                                    }
                                  xmlBufferEmpty(buf);
                                  xmlNodeDump(buf,doc,cur1,0,0);
                                  DEBTRACE(xmlBufferContent(buf));
                                  p->put(xmlBufferContent(buf));
                                }
                              if ((!xmlStrcmp(cur2->name, (const xmlChar *)"struct")))
                                {
                                  //got an struct
                                  if(getNumberOfOutputPorts()!=1)
                                    {
                                      //mismatch
                                      xmlBufferFree(buf);
                                      xmlFreeDoc(doc);
                                      throw Exception("Execution problem:mismatch in output port numbers");
                                    }
                                  xmlBufferEmpty(buf);
                                  xmlNodeDump(buf,doc,cur1,0,0);
                                  DEBTRACE(xmlBufferContent(buf));
                                  p->put(xmlBufferContent(buf));
                                }
                              if ((!xmlStrcmp(cur2->name, (const xmlChar *)"array")))
                                {
                                  //got a tuple of results or only one result (but a list)
                                  if(getNumberOfOutputPorts()==1)
                                    {
                                      //It's a one result list
                                      xmlBufferEmpty(buf);
                                      xmlNodeDump(buf,doc,cur1,0,0);
                                      DEBTRACE(xmlBufferContent(buf));
                                      p->put(xmlBufferContent(buf));
                                    }
                                  else
                                    {
                                      //It's a list of results
                                      xmlNodePtr cur3=cur2->xmlChildrenNode;
                                      while (cur3 != NULL)
                                        {
                                          if ((!xmlStrcmp(cur3->name, (const xmlChar *)"data")))
                                            {
                                              xmlNodePtr cur4=cur3->xmlChildrenNode;
                                              while (cur4 != NULL)
                                                {
                                                  if ((!xmlStrcmp(cur4->name, (const xmlChar *)"value")))
                                                    {
                                                      nres++;
                                                      if(nres > getNumberOfOutputPorts())
                                                        {
                                                          //mismatch
                                                          xmlBufferFree(buf);
                                                          xmlFreeDoc(doc);
                                                          throw Exception("Execution problem:mismatch in output port numbers");
                                                        }
                                                      xmlBufferEmpty(buf);
                                                      xmlNodeDump(buf,doc,cur4,0,0);
                                                      DEBTRACE(xmlBufferContent(buf));
                                                      p=(OutputXmlPort *)*iter2;
                                                      p->put(xmlBufferContent(buf));
                                                      iter2++;
                                                    }
                                                  cur4 = cur4->next;
                                                } // end while value
                                              break;
                                            }
                                          cur3 = cur3->next;
                                        } // end while data
                                    }
                                  break;
                                }
                              cur2 = cur2->next;
                            } // end while array
                          break;
                        }
                      cur1 = cur1->next;
                    } // end while value
                }
              cur0 = cur0->next;
            }// end while param
        }
      cur = cur->next;
    }
  xmlBufferFree(buf);
  xmlFreeDoc(doc);
}


