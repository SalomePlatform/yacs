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

#ifndef __WHILELOOP_HXX__
#define __WHILELOOP_HXX__

#include "YACSlibEngineExport.hxx"
#include "Loop.hxx"
#include "ConditionInputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for a while loop
 *
 * \ingroup Nodes
 *
 *  This node makes steps while the condition is true
 *
 */
    class YACSLIBENGINE_EXPORT WhileLoop : public Loop
    {
    protected:
      static const char NAME_OF_INPUT_CONDITION[];
      ConditionInputPort _conditionPort;
    public:
      WhileLoop(const WhileLoop& other, ComposedNode *father, bool editionOnly);
      WhileLoop(const std::string& name);
      void exUpdateState();
      void init(bool start=true);
      InputPort *edGetConditionPort() { return &_conditionPort; }
      InputPort* getInputPort(const std::string& name) const throw(Exception);
      std::list<InputPort *> getLocalInputPorts() const;
      virtual void accept(Visitor *visitor);
      InputPort *getDecisionPort() const { return (InputPort *)&_conditionPort; }
      virtual std::string typeName() {return "YACS__ENGINE__WhileLoop";}
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      void checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                                InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd) throw(Exception);
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
    };
  }
}

#endif
