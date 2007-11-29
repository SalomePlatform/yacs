#ifndef _PROC_HXX_
#define _PROC_HXX_

#include "Bloc.hxx"
#include <string>
#include <iostream>
#include <list>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class TypeCode;
    class TypeCodeObjref;
    class InlineNode;
    class ServiceNode;
    class Container;
    class ComponentInstance;
    class Logger;

    class Proc: public Bloc
    {
    public:
      Proc(const std::string& name);
      virtual ~Proc();
      virtual TypeCode *createType(const std::string& name, const std::string& kind);
      virtual TypeCode *createInterfaceTc(const std::string& id, const std::string& name, 
                                         std::list<TypeCodeObjref *> ltc);
      virtual TypeCode *createSequenceTc (const std::string& id, const std::string& name, 
                                         TypeCode *content);
      virtual TypeCode *createStructTc (const std::string& id, const std::string& name);
      virtual TypeCode* getTypeCode(const std::string& name);
      virtual void setTypeCode(const std::string& name,TypeCode *t);
      virtual void accept(Visitor *visitor);

      YACS::StatesForNode getNodeState(int numId);
      std::string getInPortValue(int nodeNumId, std::string portName);
      std::string getOutPortValue(int nodeNumId, std::string portName);
      std::string getErrorDetails(int nodeNumId);
      std::string getXMLState(int numId);
      std::list<int> getNumIds();
      std::list<std::string> getIds();
      virtual Logger *getLogger(const std::string& name);

      virtual void writeDot(std::ostream &os);
      void setName(const std::string& name); // Used by GUI to display graph name
      friend std::ostream & operator<< ( std::ostream &os, const Proc& p);
      std::map<std::string, Node*> nodeMap;
      std::map<std::string, ServiceNode*> serviceMap;
      std::map<std::string, InlineNode*> inlineMap;
      std::map<std::string, TypeCode*> typeMap;
      std::map<std::string, Container*> containerMap;
      std::map<std::pair<std::string,int>, ComponentInstance*> componentInstanceMap;
      std::vector<std::string> names;

      typedef std::map<std::string, Logger*> LoggerMap;
      LoggerMap _loggers;

    };
  }
}

#endif
