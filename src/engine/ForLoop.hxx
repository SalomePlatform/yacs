//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __FORLOOP_HXX__
#define __FORLOOP_HXX__

#include "AnyInputPort.hxx"
#include "Loop.hxx"

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for for loop node
 *
 * \ingroup Nodes
 *
 *   This kind of loop makes a fixed number of steps and stops
 *
 */
    class ForLoop : public Loop
    {
    protected:
      static const char NAME_OF_NSTEPS_NUMBER[];
      AnyInputPort _nbOfTimesPort;
    public:
      ForLoop(const ForLoop& other, ComposedNode *father, bool editionOnly);
      ForLoop(const std::string& name);
      void exUpdateState();
      void init(bool start=true);
      InputPort *edGetNbOfTimesInputPort() { return &_nbOfTimesPort; }
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      InputPort* getInputPort(const std::string& name) const throw(Exception);
      std::list<InputPort *> getLocalInputPorts() const;
      virtual void accept(Visitor *visitor);
      InputPort *getDecisionPort() const { return (InputPort *)&_nbOfTimesPort; }
      virtual std::string typeName() {return "YACS__ENGINE__ForLoop";}
    protected:
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
    };
  }
}

#endif
