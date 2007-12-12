// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#include "YACSGui_Observer.h"

#include "YACSGui_Graph.h"
#include "YACSGui_Module.h"
#include "YACSGui_Executor.h"
#include "YACSPrs_ElementaryNode.h"
#include "YACSGui_RunMode.h"
#include "YACSGui_TreeView.h"
#include "YACSGui_InputPanel.h"

#include "SALOME_Event.hxx"

#include <Node.hxx>
#include <ForEachLoop.hxx>

#include <qapplication.h>

#include <string>
#include <list>
#include <cassert>
#include "utilities.h"

using namespace YACS::ENGINE;
using namespace std;

YACSEvent::YACSEvent(std::pair<int,std::string> aYACSEvent)
  : QCustomEvent(YACS_EVENT), _event(aYACSEvent)
{
}


YACSGui_Observer::YACSGui_Observer(YACSGui_Graph* theGraph):
  Observer(),
  myGraph(theGraph)
{
}

void YACSGui_Observer::notifyObserver(YACS::ENGINE::Node* object, const std::string& event)
{
  MESSAGE("YACSGui_Observer::notifyObserver(YACS::ENGINE::Node* object, const std::string& event)");
  if ( !myGraph ) printf("==> null graph\n");

  if (event == "status")
    {
      printf("==> status\n");
      if ( myGraph && !myGraph->getItem(object) )
        {
	  printf("==> status 1\n");
          if ( dynamic_cast<YACS::ENGINE::ForEachLoop*>( object->getFather() )
               &&
               myGraph->getItem(object->getFather()) )
            {
	      printf("==> status 2\n");
              // transmit event to ForEachLoop node
              ProcessVoidEvent( new TVoidMemFunEvent<YACSPrs_ElementaryNode>( myGraph->getItem(object->getFather()), &YACSPrs_ElementaryNode::update ) );

              // transmit event from the last clone node to original loop body node
              if ( object == (dynamic_cast<YACS::ENGINE::ForEachLoop*>( object->getFather() ))->getNodes().back() )
                {
		  printf("==> status 3\n");
                  std::set<Node *> aChildren = dynamic_cast<YACS::ENGINE::ForEachLoop*>( object->getFather() )->edGetDirectDescendants();
                  for(std::set<Node *>::iterator iter=aChildren.begin();iter!=aChildren.end();iter++)
                    if ( myGraph->getItem(*iter) )
                      ProcessVoidEvent( new TVoidMemFun1ArgEvent<YACSPrs_ElementaryNode,YACS::ENGINE::Node*>( myGraph->getItem(*iter), 
                                                                                                              &YACSPrs_ElementaryNode::updateForEachLoopBody,
                                                                                                              object ) );
                }
            }
        }
      else if ( myGraph && myGraph->getItem(object) )
        {
	  printf("==> status 4\n");
          if ( dynamic_cast<YACS::ENGINE::ForEachLoop*>( object )
               &&
               object->getState() == YACS::TOACTIVATE )
            {
	      printf("==> status 5\n");
              std::vector<Node *> aCloneNodes = dynamic_cast<YACS::ENGINE::ForEachLoop*>(object)->getNodes();
              for(std::vector<Node *>::iterator iter=aCloneNodes.begin();iter!=aCloneNodes.end();iter++)
                myGraph->registerStatusObserverWithNode(*iter);	  
            }
          else
	    {
	      printf("==> status 6\n");
	      ProcessVoidEvent( new TVoidMemFunEvent<YACSPrs_ElementaryNode>( myGraph->getItem(object), &YACSPrs_ElementaryNode::update ) );
	    }
        }
    }
}

void YACSGui_Observer::notifyObserver(const int theID, const std::string& theEvent) 
{
  MESSAGE("YACSGui_Observer::notifyObserver(const int theID, const std::string& theEvent)");
  // Get node by its id
  YACS::ENGINE::Node* aNode = myGraph->getNodeById(theID);
  
  // Call notifyObserver() for the node
  if (aNode)
    notifyObserver(aNode, theEvent);
}

void YACSGui_Observer::notifyObserver(const std::string& theName, const std::string& theEvent)
{
  MESSAGE("YACSGui_Observer::notifyObserver(const std::string& theName, const std::string& theEvent)");
  // Get node by its name
  YACS::ENGINE::Node* aNode = myGraph->getNodeByName(theName);
  
  // Call notifyObserver() for the node
  if (aNode)
    notifyObserver(aNode, theEvent);
}

/*!
 * set node state in local (GUI) graph. 
 * setState emit a local event to the local dispatcher for gui events
 */ 
void YACSGui_Observer::setNodeState(const int theID, const int theState)
{
  MESSAGE("YACSGui_Observer::setNodeState(const int theID, const int theState)");
  // Get node by its id
  YACS::ENGINE::Node* aNode = myGraph->getNodeById(theID);
  
  // Set state
  if (aNode)
    aNode->setState(YACS::StatesForNode(theState));
}

/*!
 * set node state in local (GUI) graph. 
 * setState emit a local event to the local dispatcher for gui events
 */ 
void YACSGui_Observer::setNodeState(const std::string& theName, const int theState)
{
  MESSAGE("YACSGui_Observer::setNodeState " << theName << " " << theState);
  // Get node by its name
  YACS::ENGINE::Node* aNode = 0; 
  
  if (theName != "proc")
    aNode = myGraph->getNodeByName(theName);
  else
    aNode = myGraph->getProc();
  
  // Set state
  if (aNode)
    aNode->setState(YACS::StatesForNode(theState));
}




Observer_i::Observer_i(YACS::ENGINE::Proc* guiProc,
                       YACSGui_Module* guiMod,
                       YACSGui_Executor* guiExec)
{
  MESSAGE("Observer_i::Observer_i");
  _guiProc = guiProc;
  _guiMod = guiMod;
  _guiExec = guiExec;
  _engineProc = YACSGui_ORB::ProcExec::_nil();
  myImpl = 0;
}

Observer_i::~Observer_i()
{
}

void Observer_i::setConversion()
{
  MESSAGE("Observer_i::setConversion");
  assert(!CORBA::is_nil(_engineProc));
  YACSGui_ORB::stringArray_var engineNames;
  YACSGui_ORB::longArray_var engineIds;
  //MESSAGE("---");
  _engineProc->getIds(engineIds.out(), engineNames.out());
  int iLength = engineIds->length();
  int nLength = engineNames->length();
  if (nLength < iLength) iLength = nLength;
  for(int i=0; i<iLength; i++)
    {
      string aName = "";
      aName = engineNames[i];
      int iEng = engineIds[i];
      //MESSAGE("--- " << aName << " " << iEng);
      if (aName != "_root_")
        {
          int iGui = _guiProc->getChildByName(aName)->getNumId();
          //MESSAGE("---");
          _guiToEngineMap[iGui] = iEng;
          _engineToGuiMap[iEng] = iGui;
        }
      else
        {
          int iGui = _guiProc->getNumId();
          _guiToEngineMap[iGui] = iEng;
          _engineToGuiMap[iEng] = iGui;
        }
    }
}

//! process YACS events in main thread (see postEvent)
bool Observer_i::event(QEvent *e)
{
  MESSAGE("Observer_i::event");
  if (e->type() == YACS_EVENT)
    {
      YACSEvent *ye = (YACSEvent*)e;      
      pair <int, string> myEvent = ye->getYACSEvent();
      int numid = myEvent.first;
      string event = myEvent.second;

      if (event == "executor") // --- Executor notification: state
        {
          MESSAGE("Observer_i::run executor");
          int execState = _engineProc->getExecutorState();

          YACSGui_RunMode* theRunMode = _guiMod->getRunMode(_guiExec);
	  YACSGui_RunTreeView* aRunTV = dynamic_cast<YACSGui_RunTreeView*>(_guiMod->activeTreeView());

	  list<string> nextSteps;
	  if ( theRunMode || aRunTV )
	    if ((execState == YACS::WAITINGTASKS) || (execState == YACS::PAUSED))
	      {
		YACSGui_ORB::stringArray_var nstp = _engineProc->getTasksToLoad();
		for (CORBA::ULong i=0; i<nstp->length(); i++)
		  nextSteps.push_back(nstp[i].in());
	      }
	  
	  if ( theRunMode )
            {
              theRunMode->onNotifyStatus(execState);
	      theRunMode->onNotifyNextSteps(nextSteps);
            }
	  
          if ( aRunTV )
            {
	      aRunTV->onNotifyStatus(execState);
                         
	      if ( YACSGui_InputPanel* anIP = _guiMod->getInputPanel() )
		if ( YACSGui_SchemaPage* aSPage = dynamic_cast<YACSGui_SchemaPage*>( anIP->getPage( YACSGui_InputPanel::SchemaId ) ) )
		  aSPage->onNotifyNextSteps(nextSteps);
            }
	}
      else                     // --- Node notification
        {
          if (!myImpl)
            return true;
          if (_engineToGuiMap.count(numid) == 0)
            return true;
          int iGui = _engineToGuiMap[numid];
          
          if (YACS::ENGINE::Node::idMap.count(iGui) == 0)
            return true;
          YACS::ENGINE::Node* aNode= YACS::ENGINE::Node::idMap[iGui];
          
          string aName = _guiProc->getChildName(aNode);    
          if (aName == "")
            return true;
          
          if (event == "status") // --- Node notification: status
            {
              MESSAGE("Observer_i::run status");
              int aState = _engineProc->getNodeState(numid);
              if (aState < 0)
                return true;

              YACSGui_RunMode* theRunMode = _guiMod->getRunMode(_guiExec);
              if (theRunMode)
                theRunMode->onNotifyNodeStatus(iGui, aState);

	      if ( YACSGui_RunTreeView* aRunTV = dynamic_cast<YACSGui_RunTreeView*>(_guiMod->activeTreeView()) )
		aRunTV->onNotifyNodeStatus(iGui, aState);

	      if ( YACSGui_InputPanel* anIP = _guiMod->getInputPanel() )
	      {
		anIP->onNotifyNodeStatus(iGui, aState);
		
		list<string> anInPortsNames;
		list<string> anInPortsValues;
		list<InPort*> IPs = aNode->getSetOfInPort();
		list<InPort*>::iterator itIP = IPs.begin();
		for ( ; itIP!=IPs.end(); itIP++ )
		{
		  int aEngineNodeId = numid;
		  int aGuiNodeId = iGui;
		  if ( dynamic_cast<ComposedNode*>(aNode) )
		  {
		    Node* aChildNode = (*itIP)->getNode();
		    if ( aNode != aChildNode )
		    { // the event is emitted for a child node of a composed node
		      // (with numid corresponded to a composed node)
		      aGuiNodeId = aChildNode->getNumId();
		      aEngineNodeId = _guiToEngineMap[aGuiNodeId];
		    }
		  }
		  string aDump = _engineProc->getInPortValue(aEngineNodeId,(*itIP)->getName().c_str());
		  QString aValue(aDump);
		  aValue = aValue.right(aValue.length()-(aValue.find(">",aValue.find(">")+1)+1));
		  aValue = aValue.left(aValue.find("<"));
		  if ( aValue.isEmpty() ) aValue = QString("<?>");
		  
		  anInPortsNames.push_back((*itIP)->getName());
		  anInPortsValues.push_back(string(aValue.latin1()));
		  
		  _guiMod->getGraph(_guiProc)->updateNodePrs(aGuiNodeId,
							     (*itIP)->getName(),
							     string(aValue.latin1()));
		}
		anIP->onNotifyInPortValues(iGui,anInPortsNames,anInPortsValues);
		
		list<string> anOutPortsNames;
		list<string> anOutPortsValues;
		list<OutPort*> OPs = aNode->getSetOfOutPort();
		list<OutPort*>::iterator itOP = OPs.begin();
		for ( ; itOP!=OPs.end(); itOP++ )
		{
		  int aEngineNodeId = numid;
		  int aGuiNodeId = iGui;
		  if ( dynamic_cast<ComposedNode*>(aNode) )
		  {
		    Node* aChildNode = (*itOP)->getNode();
		    if ( aNode != aChildNode )
		    { // the event is emitted for a child node of a composed node
		      // (with numid corresponded to a composed node)
		      aGuiNodeId = aChildNode->getNumId();
		      aEngineNodeId = _guiToEngineMap[aGuiNodeId];
		    }
		  }
		  string aDump = _engineProc->getOutPortValue(aEngineNodeId,(*itOP)->getName().c_str());
		  QString aValue(aDump);
		  aValue = aValue.right(aValue.length()-(aValue.find(">",aValue.find(">")+1)+1));
		  aValue = aValue.left(aValue.find("<"));
		  if ( aValue.isEmpty() ) aValue = QString("< ? >");
		  
		  anOutPortsNames.push_back((*itOP)->getName());
		  anOutPortsValues.push_back(string(aValue.latin1()));
		  
		  _guiMod->getGraph(_guiProc)->updateNodePrs(aGuiNodeId,
							     (*itOP)->getName(),
							     string(aValue.latin1()));
		}
		anIP->onNotifyOutPortValues(iGui,anOutPortsNames,anOutPortsValues);
	      }

              myImpl->setNodeState(aName, aState);
            }
        }
      return true;
    }
  MESSAGE("--- Wrong event ---");
  return false;
}

//! CORBA servant implementation
/*!
 * post event for treatment by main thread and to avoid deadlocks
 * in SALOME Engine with CORBA callbacks.
 */
void Observer_i::notifyObserver(CORBA::Long numid, const char* event)
{
  MESSAGE("Observer_i::notifyObserver " << numid << " " << event );
  pair<int,string> myEvent(numid, event);
  YACSEvent* evt = new YACSEvent(myEvent);
  QApplication::postEvent(this, evt);  // Qt will delete it when done
}

void Observer_i::SetImpl(YACSGui_Observer* theImpl)
{
  MESSAGE("Observer_i::SetImpl");
  myImpl = theImpl;
}

void Observer_i::SetRemoteProc(YACSGui_ORB::ProcExec_ptr engineProc)
{
  _engineProc = YACSGui_ORB::ProcExec::_duplicate(engineProc);
}
