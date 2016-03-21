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

#ifndef _PROC_HXX_
#define _PROC_HXX_

#include "YACSlibEngineExport.hxx"
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

    class YACSLIBENGINE_EXPORT Proc : public Bloc
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
      virtual TypeCode *getTypeCode(const std::string& name);
      virtual void setTypeCode(const std::string& name,TypeCode *t);
      virtual Container *createContainer(const std::string& name, const std::string& kind="");
      virtual ComponentInstance *createComponentInstance(const std::string& componame,
                                                         const std::string& name="",
                                                         const std::string& kind="");
      virtual void addComponentInstance(ComponentInstance* inst, const std::string& name="",bool resetCtr=false);
      virtual void removeComponentInstance(ComponentInstance* inst);
      virtual void removeContainer(Container* cont);
      virtual void accept(Visitor *visitor);
      virtual Proc *getProc();
      virtual const Proc *getProc() const;
      virtual void updateContainersAndComponents();

      YACS::StatesForNode getNodeState(int numId);
      std::string getNodeProgress(int numId);
      int getGlobalProgressPercent();
      std::string getInPortValue(int nodeNumId, std::string portName);
      std::string setInPortValue(std::string nodeName, std::string portName, std::string value);
      std::string getOutPortValue(int nodeNumId, std::string portName);
      std::string getNodeErrorDetails(int nodeNumId);
      std::string getNodeErrorReport(int nodeNumId);
      std::string getNodeContainerLog(int nodeNumId);
      std::string getXMLState(int numId);
      std::list<int> getNumIds();
      std::list<std::string> getIds();
      virtual Logger *getLogger(const std::string& name);

      virtual void writeDot(std::ostream &os) const;
      void setName(const std::string& name); // Used by GUI to display graph name
      virtual std::string typeName() {return "YACS__ENGINE__Proc";}

      friend std::ostream & operator<< ( std::ostream &os, const Proc& p);
      std::map<std::string, Node*> nodeMap;
      std::map<std::string, ServiceNode*> serviceMap;
      std::map<std::string, InlineNode*> inlineMap;
      std::map<std::string, TypeCode*> typeMap;
      std::map<std::string, Container*> containerMap;
      std::map<std::string, ComponentInstance*> componentInstanceMap;
      std::vector<std::string> names;

      typedef std::map<std::string, Logger*> LoggerMap;
      LoggerMap _loggers;
      virtual bool getEdition(){return _edition;}
      virtual void setEdition(bool edition);
      virtual void modified();
      virtual void saveSchema(const std::string& xmlSchemaFile);
      virtual void saveState(const std::string& xmlStateFile);
    protected:
      void removeContainers();
    protected:
      bool _edition;
      int _compoinstctr;

    };
  }
}

#endif
