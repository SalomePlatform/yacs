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

#include "YACSPrs_toString.h"
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

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

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
  DEBTRACE("YACSGui_Observer::notifyObserver " << object << ":" << event);
  if ( !myGraph ) printf("==> null graph\n");

  if (event == "status")
    {
      DEBTRACE("==> status");
      if ( myGraph && !myGraph->getItem(object) )
        {
	  DEBTRACE("==> status 1");
          if ( dynamic_cast<YACS::ENGINE::ForEachLoop*>( object->getFather() )
               &&
               myGraph->getItem(object->getFather()) )
            {
	      DEBTRACE("==> status 2");
              // transmit event to ForEachLoop node
              ProcessVoidEvent( new TVoidMemFunEvent<YACSPrs_ElementaryNode>( myGraph->getItem(object->getFather()), &YACSPrs_ElementaryNode::update ) );

              // transmit event from the last clone node to original loop body node
              if ( object == (dynamic_cast<YACS::ENGINE::ForEachLoop*>( object->getFather() ))->getNodes().back() )
                {
		  DEBTRACE("==> status 3");
                  std::list<Node *> aChildren = dynamic_cast<YACS::ENGINE::ForEachLoop*>( object->getFather() )->edGetDirectDescendants();
                  for(std::list<Node *>::iterator iter=aChildren.begin();iter!=aChildren.end();iter++)
                    if ( myGraph->getItem(*iter) )
                      ProcessVoidEvent( new TVoidMemFun1ArgEvent<YACSPrs_ElementaryNode,YACS::ENGINE::Node*>( myGraph->getItem(*iter), 
                                                                                                              &YACSPrs_ElementaryNode::updateForEachLoopBody,
                                                                                                              object ) );
                }
            }
        }
      else if ( myGraph && myGraph->getItem(object) )
        {
	  DEBTRACE("==> status 4");
          if ( dynamic_cast<YACS::ENGINE::ForEachLoop*>( object )
               &&
               object->getState() == YACS::TOACTIVATE )
            {
	      DEBTRACE("==> status 5");
              std::vector<Node *> aCloneNodes = dynamic_cast<YACS::ENGINE::ForEachLoop*>(object)->getNodes();
              for(std::vector<Node *>::iterator iter=aCloneNodes.begin();iter!=aCloneNodes.end();iter++)
                myGraph->registerStatusObserverWithNode(*iter);	  
            }
          else
	    {
	      DEBTRACE("==> status 6");
	      ProcessVoidEvent( new TVoidMemFunEvent<YACSPrs_ElementaryNode>( myGraph->getItem(object), &YACSPrs_ElementaryNode::update ) );
	    }
        }
    }
}

void YACSGui_Observer::notifyObserver(const int theID, const std::string& theEvent) 
{
  DEBTRACE("YACSGui_Observer::notifyObserver(const int theID, const std::string& theEvent)");
  // Get node by its id
  YACS::ENGINE::Node* aNode = myGraph->getNodeById(theID);
  
  // Call notifyObserver() for the node
  if (aNode)
    notifyObserver(aNode, theEvent);
}

void YACSGui_Observer::notifyObserver(const std::string& theName, const std::string& theEvent)
{
  DEBTRACE("YACSGui_Observer::notifyObserver(const std::string& theName, const std::string& theEvent)");
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
  DEBTRACE("YACSGui_Observer::setNodeState(const int theID, const int theState)");
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
  DEBTRACE("YACSGui_Observer::setNodeState " << theName << " " << theState);
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
  DEBTRACE("Observer_i::Observer_i");
  _guiProc = guiProc;
  _guiMod = guiMod;
  _guiExec = guiExec;
  _engineProc = YACS_ORB::ProcExec::_nil();
  myImpl = 0;
}

Observer_i::~Observer_i()
{
}

void Observer_i::setConversion()
{
  DEBTRACE("Observer_i::setConversion");
  assert(!CORBA::is_nil(_engineProc));
  YACS_ORB::stringArray_var engineNames;
  YACS_ORB::longArray_var engineIds;
  //DEBTRACE("---");
  _engineProc->getIds(engineIds.out(), engineNames.out());
  int iLength = engineIds->length();
  int nLength = engineNames->length();
  if (nLength < iLength) iLength = nLength;
  for(int i=0; i<iLength; i++)
    {
      string aName = "";
      aName = engineNames[i];
      int iEng = engineIds[i];
      //DEBTRACE("--- " << aName << " " << iEng);
      if (aName != "_root_")
        {
          int iGui = _guiProc->getChildByName(aName)->getNumId();
          //DEBTRACE("---");
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
  DEBTRACE("Observer_i::event");
  if (e->type() == YACS_EVENT)
    {
      YACSEvent *ye = (YACSEvent*)e;      
      pair <int, string> myEvent = ye->getYACSEvent();
      int numid = myEvent.first;
      string event = myEvent.second;

      if (event == "executor") // --- Executor notification: state
        {
          DEBTRACE("Observer_i::run executor");
          int execState = _engineProc->getExecutorState();

          YACSGui_RunMode* theRunMode = _guiMod->getRunMode(_guiExec);
	  YACSGui_RunTreeView* aRunTV = dynamic_cast<YACSGui_RunTreeView*>(_guiMod->activeTreeView());

	  list<string> nextSteps;
	  if ( theRunMode || aRunTV )
	    if ((execState == YACS::WAITINGTASKS) || (execState == YACS::PAUSED))
	      {
		YACS_ORB::stringArray_var nstp = _engineProc->getTasksToLoad();
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
              DEBTRACE("Observer_i::run status");
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
		
		map<int, map<string, string> > aGuiNodeId2InPortsValues;
		list<InputPort*> IPs ;
		if ( dynamic_cast<ComposedNode*>(aNode) )
                  IPs=aNode->getLocalInputPorts();
                else
                  IPs = aNode->getSetOfInputPort();
		list<InputPort*>::iterator itIP = IPs.begin();
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
                  std::string aValue = _engineProc->getInPortValue(aEngineNodeId,(*itIP)->getName().c_str());
                  DEBTRACE("on status change, inport value: " << (*itIP)->getName() << " " << aValue);
		  
		  if ( aGuiNodeId2InPortsValues.find(aGuiNodeId) == aGuiNodeId2InPortsValues.end() )
		  {
		    map<string, string> aPName2PValue;
		    aPName2PValue.insert( make_pair((*itIP)->getName(),aValue));
		    aGuiNodeId2InPortsValues.insert( make_pair( aGuiNodeId, aPName2PValue ) );
		  }
		  else
		    aGuiNodeId2InPortsValues[aGuiNodeId].insert( make_pair((*itIP)->getName(),aValue));
		  
		  _guiMod->getGraph(_guiProc)->updateNodePrs(aGuiNodeId,true,
							     (*itIP)->getName(),
							     aValue);
		}
		map<int, map<string, string> >::iterator itMI = aGuiNodeId2InPortsValues.begin();
		for( ; itMI!=aGuiNodeId2InPortsValues.end(); itMI++ )
		  anIP->onNotifyInPortValues((*itMI).first,(*itMI).second);

		map<int, map<string, string> > aGuiNodeId2OutPortsValues;
		list<OutputPort*> OPs;
		if ( dynamic_cast<ComposedNode*>(aNode) )
                  OPs=aNode->getLocalOutputPorts();
                else
                  OPs = aNode->getSetOfOutputPort();
		list<OutputPort*>::iterator itOP = OPs.begin();
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
                  std::string aValue = _engineProc->getOutPortValue(aEngineNodeId,(*itOP)->getName().c_str());
                  DEBTRACE("on status change, outport value: " << (*itOP)->getName() << " " << aValue);
		  
		  if ( aGuiNodeId2OutPortsValues.find(aGuiNodeId) == aGuiNodeId2OutPortsValues.end() )
		  {
		    map<string, string> aPName2PValue;
		    aPName2PValue.insert( make_pair((*itOP)->getName(),aValue) );
		    aGuiNodeId2OutPortsValues.insert( make_pair( aGuiNodeId, aPName2PValue ) );
		  }
		  else
		    aGuiNodeId2OutPortsValues[aGuiNodeId].insert( make_pair((*itOP)->getName(),aValue) );

		  _guiMod->getGraph(_guiProc)->updateNodePrs(aGuiNodeId,false,
							     (*itOP)->getName(),
							     aValue);
		}
		map<int, map<string, string> >::iterator itMO = aGuiNodeId2OutPortsValues.begin();
		for( ; itMO!=aGuiNodeId2OutPortsValues.end(); itMO++ )
		  anIP->onNotifyOutPortValues((*itMO).first,(*itMO).second);
	      }

              myImpl->setNodeState(aName, aState);
            }
        }
      return true;
    }
  DEBTRACE("--- Wrong event ---");
  return false;
}

//! CORBA servant implementation
/*!
 * post event for treatment by main thread and to avoid deadlocks
 * in SALOME Engine with CORBA callbacks.
 */
void Observer_i::notifyObserver(CORBA::Long numid, const char* event)
{
  DEBTRACE("Observer_i::notifyObserver " << numid << " " << event );
  pair<int,string> myEvent(numid, event);
  YACSEvent* evt = new YACSEvent(myEvent);
  QApplication::postEvent(this, evt);  // Qt will delete it when done
}

void Observer_i::SetImpl(YACSGui_Observer* theImpl)
{
  DEBTRACE("Observer_i::SetImpl");
  myImpl = theImpl;
}

void Observer_i::SetRemoteProc(YACS_ORB::ProcExec_ptr engineProc)
{
  _engineProc = YACS_ORB::ProcExec::_duplicate(engineProc);
}
