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

#ifndef __CONTAINERTEST_HXX__
#define __CONTAINERTEST_HXX__

#include "Container.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class ContainerTest : public Container
    {
    public:
      ContainerTest();
      std::string getKind() const;
      std::string getPlacementInfo() const;
      // implementation of compulsary methods
      bool isAlreadyStarted(const Task *askingNode) const;
      void start(const Task *askingNode) throw(Exception);
      Container *clone() const;
      Container *cloneAlways() const;
      //
      void lock() { }
      void unLock() { }
      void clearProperties() { }
      void addComponentName(const std::string& name) { }
      void shutdown(int level) { }
      void setProperty(const std::string& name,const std::string& value) { }
      std::string getProperty(const std::string& name) const { return ""; }
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const { return std::map<std::string,std::string>(); }
      std::map<std::string,std::string> getProperties() const { return std::map<std::string,std::string>(); }
      //
      std::string getPlacementId(const Task *askingNode) const { return ""; }
      std::string getFullPlacementId(const Task *askingNode) const { return ""; }
      static void initAllContainers();
    protected:
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw(Exception);
    protected:
      bool _alreadyStarted;
      unsigned _myCounter;
      static unsigned _counter;
      static const char SUPPORTED_COMP_KIND[];
    };

    class ContainerTest2 : public Container
    {
    public:
      ContainerTest2();
      std::string getKind() const;
      // implementation of compulsary methods
      bool isAlreadyStarted(const Task *askingNode) const;
      void start(const Task *askingNode) throw(Exception);
      Container *clone() const;
      Container *cloneAlways() const;
      //
      void lock() { }
      void unLock() { }
      void clearProperties() { }
      void addComponentName(const std::string& name) { }
      void shutdown(int level) { }
      void setProperty(const std::string& name,const std::string& value) { }
      std::string getProperty(const std::string& name) const { return ""; }
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const { return std::map<std::string,std::string>(); }
      std::map<std::string,std::string> getProperties() const { return std::map<std::string,std::string>(); }
      //
      std::string getPlacementId(const Task *askingNode) const { return ""; }
      std::string getFullPlacementId(const Task *askingNode) const { return ""; }
      static void initAllContainers();
    protected:
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw(Exception);
    protected:
      bool _alreadyStarted;
      unsigned _myCounter;
      static unsigned _counter;
      static const char SUPPORTED_COMP_KIND[];
    };
  }
}

#endif
