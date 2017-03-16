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

#ifndef __BLOC_HXX__
#define __BLOC_HXX__

#include "YACSlibEngineExport.hxx"
#include "StaticDefinedComposedNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT Bloc : public StaticDefinedComposedNode
    {
    protected:
      std::list<Node *> _setOfNode;//OWNERSHIP OF ALL NODES
      //! For internal calculations
      mutable std::map<Node *,std::set<Node *> > *_fwLinks;
      //! For internal calculations
      mutable std::map<Node *,std::set<Node *> > *_bwLinks;
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
    public:
      Bloc(const Bloc& other, ComposedNode *father, bool editionOnly);
      Bloc(const std::string& name);
      virtual ~Bloc();
      bool isFinished();
      int getNumberOfCFLinks() const;
      void init(bool start=true);
      void getReadyTasks(std::vector<Task *>& tasks);
      void exUpdateState();
      //Node* DISOWNnode is a SWIG notation to indicate that the ownership of the node is transfered to C++
      bool edAddChild(Node *DISOWNnode) throw(Exception);
      void edRemoveChild(Node *node) throw(Exception);
      std::list<Node *> getChildren() const { return _setOfNode; }
      std::list<Node *> edGetDirectDescendants() const { return _setOfNode; }
      std::vector< std::list<Node *> > splitIntoIndependantGraph() const;
      Node *getChildByShortName(const std::string& name) const throw(Exception);
      virtual void writeDot(std::ostream &os) const;
      void accept(Visitor *visitor);
      template<bool direction>
      void findAllPathsStartingFrom(Node *start, std::list< std::vector<Node *> >& vec, std::map<Node *, std::set<Node *> >& accelStr) const;
      template<bool direction>
      void findAllNodesStartingFrom(Node *start, std::set<Node *>& result, std::map<Node *, std::set<Node *> >& accelStr, LinkInfo& info) const;
      virtual std::string typeName() { return "YACS__ENGINE__Bloc"; }
      int getMaxLevelOfParallelism() const;
      void removeRecursivelyRedundantCL();
    protected:
      bool areAllSubNodesFinished() const;
      bool areAllSubNodesDone() const;
      bool isNameAlreadyUsed(const std::string& name) const;
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
      std::vector< std::pair<OutGate *, InGate *> > getSetOfInternalCFLinks() const;
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
      YACS::Event updateStateOnFailedEventFrom(Node *node, const Executor *execInst);
      void initComputation() const;
      void performCFComputationsOnlyOneLevel(LinkInfo& info) const;
      void performCFComputations(LinkInfo& info) const;
      void destructCFComputations(LinkInfo& info) const;
      void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const;
      bool areLinked(Node *start, Node *end, bool fw) const;
      bool arePossiblyRunnableAtSameTime(Node *start, Node *end) const;
      void checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const;
      static void verdictForOkAndUseless1(const std::map<Node *,std::list <OutPort *> >& pool, InputPort *end, const std::vector<Node *>& candidates,
                                          unsigned char& alreadyFed, bool direction, LinkInfo& info);
      static void verdictForCollapses(const std::map<Node *,std::list <OutPort *> >& pool, InputPort *end, const std::set<Node *>& candidates,
                                      unsigned char& alreadyFed, bool direction, LinkInfo& info);
      void seekOkAndUseless1(std::vector<Node *>& okAndUseless1, std::set<Node *>& allNodes) const;
      void seekUseless2(std::vector<Node *>& useless2, std::set<Node *>& allNodes) const;
    private:
      static void findUselessLinksIn(const std::list< std::vector<Node *> >& res , LinkInfo& info);
      template<bool direction>
      static unsigned appendIfAlreadyFound(std::list< std::vector<Node *> >& res, const std::vector<Node *>& startRes, Node *node, std::map<Node *, std::set<Node *> >& fastFinder);
      static void updateWithNewFind(const std::vector<Node *>& path, std::map<Node *, std::set<Node *> >& fastFinder);
    };

    template<bool direction>
    struct CFDirectionVisTraits
    {   
    };
    
    template<>
    struct CFDirectionVisTraits<true>
    {
      typedef std::list< std::pair<InGate *,bool> >::iterator Iterator;
      typedef std::list< std::pair<InGate *,bool> >& Nexts;
      static Nexts getNexts(Node *node) { return node->getOutGate()->edMapInGate(); }
    };

    
    template<>
    struct CFDirectionVisTraits<false>
    {
      typedef std::list< std::pair<OutGate *,bool> >::iterator Iterator;
      typedef std::list< std::pair<OutGate *,bool> >& Nexts;
      static Nexts getNexts(Node *node) { return node->getInGate()->edMapOutGate(); }
    };

    /*!
     * Internal method for CF computation. Given 'fastFinder' it searched 'node' to see if an already found path in 'res' go through 'node'.
     * If true all paths are deduced and append to res and 'fastFinder' is updated for next turn.
     */
    template<bool direction>
    unsigned Bloc::appendIfAlreadyFound(std::list< std::vector<Node *> >& res, const std::vector<Node *>& startRes, Node *node, std::map<Node *, std::set<Node *> >& fastFinder)
    {
      std::map<Node *, std::set<Node *> >::iterator iter=fastFinder.find(node);
      if(iter==fastFinder.end())
        return 0;
      unsigned ret=0;
      std::vector<std::pair<std::set<Node *>::iterator, std::set<Node *>::iterator > > li;
      std::pair<std::set<Node *>::iterator, std::set<Node *>::iterator> ipr(((*iter).second).begin(),((*iter).second).end());
      li.push_back(ipr);
      std::vector<Node *> work(startRes);
      std::list< std::vector<Node *> >::iterator where=res.end(); where--;
      std::list< std::vector<Node *> >::iterator updates=where;
      while(!li.empty())
        {
          if(li.back().first!=li.back().second)
            {
              work.push_back(*(li.back().first));
              if(CFDirectionVisTraits<direction>::getNexts(work.back()).empty())
                {
                  where=res.insert(where,work);
                  ret++;
                  li.back().first++;
                  work.pop_back();
                }
              else
                {
                  std::set<Node *>& s=fastFinder[*(li.back().first)];
                  std::pair<std::set<Node *>::iterator, std::set<Node *>::iterator> pr(s.begin(),s.end());
                  li.push_back(pr);
                }
            }
          else
            {
              work.pop_back();
              li.pop_back();
              if(!li.empty())
                li.back().first++;
            }
        }
      updates--;
      for(unsigned i=0;i<ret;i++,updates--)
        updateWithNewFind(*updates,fastFinder);
      return ret;
    }

    template<bool direction>
    void Bloc::findAllNodesStartingFrom(Node *start, std::set<Node *>& result, 
                                        std::map<Node *, std::set<Node *> >& accelStr, LinkInfo& info) const
    {
      std::list< std::vector<Node *> > li;
      findAllPathsStartingFrom<direction>(start,li,accelStr);
      for(std::list< std::vector<Node *> >::const_iterator iter=li.begin();iter!=li.end();iter++)
        for(std::vector<Node *>::const_iterator iter2=(*iter).begin()+1;iter2!=(*iter).end();iter2++)
          result.insert(*iter2);
      if(direction)
        findUselessLinksIn(li,info);
    }

    /*!
     * Method for CF computation.DFS visitor is used.
     * if direction is true forward visiting is performed, if false backward is performed.
     * \param start \b must be a direct descendant of 'this'.
     * \param vec :
     * \param accelStr
     */
    template<bool direction>
    void Bloc::findAllPathsStartingFrom(Node *start, std::list< std::vector<Node *> >& vec, 
                                        std::map<Node *, std::set<Node *> >& accelStr) const
    {
      initComputation();
      Node *current=start;
      int caseId=0;
      int idInCase=0;
      vec.push_back(std::vector<Node *>());
      typename CFDirectionVisTraits<direction>::Iterator iter;
      std::list< std::vector<Node *> >::iterator curLine=vec.begin();
      while(start->_colour!=YACS::Black)
        {
          (*curLine).push_back(current);
          idInCase++;
          //
          if(CFDirectionVisTraits<direction>::getNexts(current).empty())
            {
              
              vec.push_back(std::vector<Node *>((*curLine)));
              updateWithNewFind(*curLine,accelStr);
              current->_colour=YACS::Black;
              curLine++;
              if(idInCase>1)
                {
                  idInCase-=2;
                  current=(*curLine)[idInCase];
                  (*curLine).pop_back();
                  (*curLine).pop_back();
                }
              continue;
            }
          if(current->_colour==YACS::Black)
            {
              appendIfAlreadyFound<direction>(vec,(*curLine),current,accelStr);
              curLine=vec.end(); curLine--;
              current->_colour=YACS::Black;
              if(idInCase>1)
                {
                  idInCase-=2;
                  current=(*curLine)[idInCase];
                  (*curLine).pop_back();
                  (*curLine).pop_back();
                }
              continue;
            }
          for(iter=CFDirectionVisTraits<direction>::getNexts(current).begin();iter!=CFDirectionVisTraits<direction>::getNexts(current).end();iter++)
            if(!(*iter).second)
              break;
          if(iter==CFDirectionVisTraits<direction>::getNexts(current).end())
            {//Fail this branch should be forgotten go rev
              current->_colour=YACS::Black;
              (*curLine).pop_back();
              if(idInCase>1)
                {
                  idInCase-=2;
                  current=(*curLine)[idInCase];
                  (*curLine).pop_back();
                }
            }
          else
            {
              //Nothing to signal continuing in this direction hoping to find
              current=(*iter).first->getNode();
              (*iter).second=true;
            }
        }
      vec.pop_back();
    }
  }
}


#endif
