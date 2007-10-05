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

#include "YACSPrs_ForEachLoopNode.h"
#include "YACSPrs_Def.h"

#include "SUIT_ResourceMgr.h"

#include <qpainter.h>

#include <ForEachLoop.hxx>

using namespace YACS::ENGINE;

/*!
  Constructor
*/
YACSPrs_ForEachLoopNode::YACSPrs_ForEachLoopNode( SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACS::ENGINE::Node* theNode ):
  YACSPrs_LoopNode(theMgr, theCanvas, theNode, false)
{
  //updatePorts(); // will be called in moveBy(...) function
  moveBy(3*TITLE_HEIGHT/2, 3*TITLE_HEIGHT/2);
}

/*!
  Destructor
*/
YACSPrs_ForEachLoopNode::~YACSPrs_ForEachLoopNode()
{
}

int YACSPrs_ForEachLoopNode::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_ForEachLoopNode;
}

void YACSPrs_ForEachLoopNode::updatePorts()
{
  bool aDisp = isVisible();
  if (aDisp) hide();
  
  // ForEachLoop has 2 input ports and 2 output ports.
  // Input ports : 1) 'nbBranches' port, its type is 'int', its value is a number of iterations;
  //               2) 'SmplsCollection' port, its type is 'sequence' of elelmnts of the given type,
  //                  it is a list of values of the given type.
  // Output ports : 1) 'SmplPrt' port, its type is the given type, its value is a value of the
  //                   list item, which is processed at the current moment.
  //                2) 'Body' label port, this 'label' port connects with help of label link to 
  //                   'Master' hook of the node, which is set as an internal node of the loop.

  bool withCreate = false;
  if ( myPortList.isEmpty() ) withCreate = true;

  QRect r = getBodyRect();
  int aPRectWidth = (int)(r.width()/2) - 2*PORT_MARGIN;
  if ( aPRectWidth < PORT_WIDTH ) aPRectWidth = PORT_WIDTH;
  
  int ix = r.x() + PORT_MARGIN + 1;
  int iy = r.y() + PORT_MARGIN;// + 1;
  int ox = ix + aPRectWidth + 2*PORT_MARGIN;
  int oy = r.y() + PORT_MARGIN;// + 1;

  if ( withCreate )
  { // create (and update)
    ForEachLoop* aFELoop = dynamic_cast<ForEachLoop*>( myEngine );
    if ( aFELoop )
    { // create 2 input and 1 output ports
      // 'nbBranches'
      YACSPrs_InOutPort* anIn1Port = new YACSPrs_InOutPort(myMgr,canvas(),aFELoop->edGetNbOfBranchesPort(),this);
      anIn1Port->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT));
      anIn1Port->setColor(nodeSubColor());
      anIn1Port->setStoreColor(nodeSubColor());
      myPortList.append(anIn1Port);
      // 'SmplsCollection'
      YACSPrs_InOutPort* anIn2Port = new YACSPrs_InOutPort(myMgr,canvas(),aFELoop->edGetSeqOfSamplesPort(),this);
      anIn2Port->setPortRect(QRect(ix, iy+PORT_HEIGHT+PORT_SPACE, aPRectWidth, PORT_HEIGHT));
      anIn2Port->setColor(nodeSubColor());
      anIn2Port->setStoreColor(nodeSubColor());
      myPortList.append(anIn2Port);

      myPortHeight += 2*PORT_HEIGHT+PORT_SPACE;

      // 'SmplPtr'
      YACSPrs_InOutPort* anOutPort = new YACSPrs_InOutPort(myMgr,canvas(),aFELoop->edGetSamplePort(),this);
      anOutPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT));
      anOutPort->setColor(nodeSubColor());
      anOutPort->setStoreColor(nodeSubColor());
      myPortList.append(anOutPort);
        
      // get a set of internal loop nodes (in fact ForEachLoop has 2 internal nodes: _node and _initNode,
      // but only _node was initialized in engine, in all examples _initNode is null)
      std::set<Node*> aNodes = aFELoop->edGetDirectDescendants();
      std::set<Node*>::iterator aNodesIter = aNodes.begin();
      for (; aNodesIter != aNodes.end(); aNodesIter++)
      { // output label port
	YACSPrs_LabelPort* anOutPort = new YACSPrs_LabelPort(myMgr,canvas(),*aNodesIter,this);
	anOutPort->setPortRect(QRect(ox, oy+PORT_HEIGHT+PORT_SPACE, aPRectWidth, PORT_HEIGHT));
	anOutPort->setColor(nodeSubColor().dark(140));
	anOutPort->setStoreColor(nodeSubColor().dark(140));
	myPortList.append(anOutPort);
	oy += PORT_HEIGHT+PORT_SPACE;
      }
      if ( aNodes.size() > 1 )
	myPortHeight += (aNodes.size()-1)*PORT_HEIGHT + (aNodes.size()-2)*PORT_SPACE;
    }
  }
  else
  { // only update
    YACSPrs_Port* aPort;
    for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
    { 
      YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
      if ( anIOPort )
      {
	if ( !anIOPort->isGate() )
	{  
	  if ( anIOPort->isInput() )
	  { // input data (i.e. not Gate) ports
	    anIOPort->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT), !isSelfMoving(), myArea);
	    iy += PORT_HEIGHT+PORT_SPACE;
	  }
	  else
	  { // output data (i.e. not Gate) ports
	    anIOPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT), !isSelfMoving(), myArea);
	    oy += PORT_HEIGHT+PORT_SPACE;
	  }
	}
      }
      else
      { // not YACSPrs_InOutPort => it is YACSPrs_LabelPort (output!) => we not need to dynamic cast
	aPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT), !isSelfMoving(), myArea);
	oy += PORT_HEIGHT+PORT_SPACE;
      }
    }
  }
    
  // can update gates only after body height will be defined
  updateGates(withCreate);

  if (aDisp) show();
}

//! Increments time iteration.
/*!
 *  Note : use not null argument of this function only for update original body node of ForEachLoop node.
 *
 * \param theEngine : the engine of clone node from which we have to update presentation of this node.
 */
void YACSPrs_ForEachLoopNode::nextTimeIteration(YACS::ENGINE::Node* theEngine)
{
  bool nullifyOnToActivate = false;
  if ( !theEngine ) theEngine = myEngine;
  else nullifyOnToActivate = true;
  
  if ( theEngine
       &&
       ( theEngine->getState() == YACS::INITED || theEngine->getEffectiveState() == YACS::INITED
	 ||
	 ( nullifyOnToActivate && ( theEngine->getState() == YACS::TOACTIVATE || theEngine->getEffectiveState() == YACS::TOACTIVATE) ) ) )
    setTimeIteration( 0. );
  else if ( theEngine &&
	    theEngine->getState() != YACS::INITED && theEngine->getEffectiveState() != YACS::INITED &&
	    !isFinished() ) {
    if ( ForEachLoop* aLoop = dynamic_cast<ForEachLoop*>( theEngine ) )
      setTimeIteration( aLoop->getExecCurrentId()-1. >= 0 ? aLoop->getExecCurrentId()-1. : 0. );
  }
}

//! Returns the progress bar percentage.
/*!
 *  Note : use not null argument of this function only for update original body node of ForEachLoop node.
 *
 * \param theEngine : the engine of clone node from which we have to update presentation of this node.
 */
double YACSPrs_ForEachLoopNode::getPercentage(YACS::ENGINE::Node* theEngine) const
{
  if ( !theEngine ) theEngine = myEngine;
  
  if ( !theEngine ) return 0.;

  if ( theEngine->getState() == YACS::INITED || theEngine->getEffectiveState() == YACS::INITED ||
       theEngine->getState() == YACS::TOLOAD || theEngine->getEffectiveState() == YACS::TOLOAD )
    return 0.;
  if ( theEngine->getState() == YACS::DONE )
    return 100.;
  if ( ForEachLoop* aLoop = dynamic_cast<ForEachLoop*>( theEngine ) ) {
    SeqAnyInputPort* aCollection = dynamic_cast<SeqAnyInputPort*>( aLoop->edGetSeqOfSamplesPort() );
    if ( aCollection && !aCollection->isEmpty() )
      return 100. / aCollection->getNumberOfElements() * getTimeIteration();
  }
  return 0.;
}
