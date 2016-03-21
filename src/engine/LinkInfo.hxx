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

#ifndef __LINKINFO_HXX__
#define __LINKINFO_HXX__

#include "YACSlibEngineExport.hxx"
#include "Exception.hxx"

#include <vector>
#include <list>
#include <map>
#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class InGate;
    class InPort;
    class OutGate;
    class OutPort;
    class ComposedNode;
    class CollectorSwOutPort;

    typedef enum
      {
        I_CF_USELESS   = 41,
        I_USELESS      = 42,
        I_BACK         = 43,//In loop context
        I_BACK_USELESS = 44,//In loop context - Not implemented yet
        I_BACK_CRAZY   = 45,//Out of loop context
        I_DFDS         = 46,
        I_ALL          = 49,
      } InfoReason;

    typedef enum
      {
        W_COLLAPSE                     = 141,
        W_COLLAPSE_AND_USELESS         = 142,
        W_COLLAPSE_EL                  = 143,
        W_COLLAPSE_EL_AND_USELESS      = 144,
        W_BACK_COLLAPSE                = 145,
        W_BACK_COLLAPSE_AND_USELESS    = 146,
        W_BACK_COLLAPSE_EL             = 147,
        W_BACK_COLLAPSE_EL_AND_USELESS = 148,
        W_ALL                          = 149
      } WarnReason;

    typedef enum
      {
        E_NEVER_SET_INPUTPORT     = 241,
        E_ONLY_BACKWARD_DEFINED   = 242,
        E_DS_LINK_UNESTABLISHABLE = 243,
        E_COLLAPSE_DFDS           = 244,
        E_COLLAPSE_DS             = 245,
        E_UNPREDICTABLE_FED       = 246,
        E_UNCOMPLETE_SW            =247,
        E_ALL                     = 249
      } ErrReason;

    /*!
     * \brief Class that deal with list of \b semantics links for high level analysis.
     */
    class YACSLIBENGINE_EXPORT LinkInfo
    {
    private:
      ComposedNode *_pov;
      unsigned int _level;
      unsigned char _levelOfInfo;
      std::vector<InPort *> _unsetInPort;
      std::vector<InPort *> _onlyBackDefined;
      std::set< std::pair<Node *, Node *> > _uselessLinks;
      std::map<InfoReason, std::vector< std::pair<OutPort *,InPort *> > > _infos;
      std::map<WarnReason, std::vector< std::vector< std::pair<OutPort *,InPort *> > > > _collapse;
      std::map<ErrReason, std::vector< std::pair<OutPort *,InPort *> > > _errors;
      std::vector<CollectorSwOutPort *> _errorsOnSwitchCases;
    public:
      LinkInfo(unsigned char level);
      void clearAll();
      void startCollapseTransac();
      void endCollapseTransac() throw(Exception);
      void setPointOfView(ComposedNode *pov);
      void pushInfoLink(OutPort *semStart, InPort *end, InfoReason reason);
      void pushWarnLink(OutPort *semStart, InPort *end, WarnReason reason);
      void pushErrLink(OutPort *semStart, InPort *end, ErrReason reason) throw(Exception);
      void pushErrSwitch(CollectorSwOutPort *collector) throw(Exception);
      void pushUselessCFLink(Node *start, Node *end);
      void takeDecision() const throw(Exception);
      //Typically methods for high level use.
      std::string getGlobalRepr() const;
      std::string getInfoRepr() const;
      std::string getWarnRepr() const;
      std::string getErrRepr() const;
      bool areWarningsOrErrors() const;
      unsigned getNumberOfInfoLinks(InfoReason reason) const;
      unsigned getNumberOfWarnLinksGrp(WarnReason reason) const;
      unsigned getNumberOfErrLinks(ErrReason reason) const;
      std::set< std::pair<Node *, Node *> > getInfoUselessLinks() const;
      std::pair<OutPort *, InPort *> getInfoLink(unsigned id, InfoReason reason) const;
      std::vector< std::pair<OutPort *, InPort *> > getWarnLink(unsigned id, WarnReason reason) const;
      std::pair<OutPort *, InPort *> getErrLink(unsigned id, ErrReason reason) const;
    protected:
      static std::string getStringReprOfI(InfoReason reason);
      static std::string getStringReprOfW(WarnReason reason);
      static std::string getStringReprOfE(ErrReason reason);
      static std::string printThereIsAre(unsigned val, const std::string& other);
    public:
      static const unsigned char ALL_STOP_ASAP       = 1;
      static const unsigned char ALL_DONT_STOP       = 2;
      static const unsigned char WARN_ONLY_DONT_STOP = 3;
    };
  }
}

#endif
