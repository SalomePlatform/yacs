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

#ifndef __FORLOOP_HXX__
#define __FORLOOP_HXX__

#include "YACSlibEngineExport.hxx"
#include "AnyInputPort.hxx"
#include "AnyOutputPort.hxx"
#include "Loop.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT ForLoop : public Loop
    {
    protected:
      static const char NAME_OF_INDEX[];
      static const char NAME_OF_NSTEPS_NUMBER[];
      AnyInputPort _nbOfTimesPort;
      AnyOutputPort _indexPort;
    public:
      ForLoop(const ForLoop& other, ComposedNode *father, bool editionOnly);
      ForLoop(const std::string& name);
      void exUpdateState();
      void exUpdateProgress();
      void init(bool start=true);
      InputPort *edGetNbOfTimesInputPort() { return &_nbOfTimesPort; }
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      InputPort* getInputPort(const std::string& name) const throw(Exception);
      OutPort *getOutPort(const std::string& name) const throw(Exception);
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      std::list<InputPort *> getLocalInputPorts() const;
      std::list<OutputPort *> getLocalOutputPorts() const;
      std::list<OutputPort *> getSetOfOutputPort() const;
      virtual void accept(Visitor *visitor);
      InputPort *getDecisionPort() const { return (InputPort *)&_nbOfTimesPort; }
      OutputPort *edGetIndexPort() { return &_indexPort; }
      virtual std::string typeName() {return "YACS__ENGINE__ForLoop";}
      std::string getProgress() const;
      std::list<ProgressWeight> getProgressWeight() const;
      int getNbSteps() const;

    protected:
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
      void checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed,
                        bool direction, LinkInfo& info) const;
      void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const;
    };
  }
}

#endif
