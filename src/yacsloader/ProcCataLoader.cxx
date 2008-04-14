#include "ProcCataLoader.hxx"
#include "parsers.hxx"
#include "Proc.hxx"
#include "Logger.hxx"
#include "TypeCode.hxx"

#include <set>
#include <iostream>
#include <sstream>
#include <stdexcept>


//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

ProcCataLoader::ProcCataLoader(YACS::YACSLoader* xmlLoader,const std::string& path):CatalogLoader(path),_xmlLoader(xmlLoader)
{
  //path should be a file path
}

ProcCataLoader::~ProcCataLoader()
{
  DEBTRACE ("ProcCataLoader::~ProcCataLoader");
}

void ProcCataLoader::loadCata(Catalog* cata)
{
  DEBTRACE("ProcCataLoader::load");
  Proc* p;
  try
    {
      p=_xmlLoader->load(_path.c_str());
      if(p==0)
        {
          std::string msg="the file is probably not a YACS schema file";
          cata->setErrors(msg);
          std::cerr << msg << std::endl;
          return; 
        }
    }
  catch (YACS::Exception& e)
    {
      std::string msg="Caught a YACS exception";
      msg=msg + e.what();
      std::cerr << msg << std::endl;
      cata->setErrors(msg);
      return ;
    }
  catch (const std::invalid_argument& e)
    {
      cata->setErrors(e.what());
      return ;
    }
  catch (const std::ios_base::failure&)
    {
      std::string msg="Caught an io failure exception";
      std::cerr << msg << std::endl;
      cata->setErrors(msg);
      return ;
    }

  //Get the parser logger
  Logger* logger=p->getLogger("parser");
  //Print errors logged if any
  if(!logger->isEmpty())
    {
      std::string msg=logger->getStr();
      std::cerr << msg << std::endl;
      cata->setErrors(msg);
    }

  std::map<std::string,TypeCode*>& typeMap=cata->_typeMap;
  std::map<std::string,Node*>& nodeMap=cata->_nodeMap;
  std::map<std::string,ComposedNode*>& composednodeMap=cata->_composednodeMap;
  std::map<std::string,ComponentDefinition*>& componentMap=cata->_componentMap;

  std::map<std::string,TypeCode*>::iterator it=p->typeMap.begin();
  while(it != p->typeMap.end())
    {
      typeMap[it->first]=it->second;
      it->second->incrRef();
      it++;
    }

  std::list<Node *> s=p->getChildren();
  for(std::list<Node *>::iterator iter=s.begin();iter!=s.end();iter++)
    {
      YACS::ENGINE::ComposedNode * cnode= dynamic_cast<YACS::ENGINE::ComposedNode *>(*iter);
      if(cnode)
        composednodeMap[cnode->getName()]=(YACS::ENGINE::ComposedNode *) cnode->clone(0);
      else 
        nodeMap[(*iter)->getName()]=(*iter)->clone(0);
    }
  delete p;
}
