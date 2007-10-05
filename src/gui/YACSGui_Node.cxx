//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <YACSGui_Node.h>
#include <YACSGui_Graph.h>

#include <YACSPrs_ElementaryNode.h>
#include <YACSPrs_ServiceNode.h>
#include <YACSPrs_InlineNode.h>
#include <YACSPrs_IfNode.h>
#include <YACSPrs_SwitchNode.h>
#include <YACSPrs_LoopNode.h>
#include <YACSPrs_ForEachLoopNode.h>
#include <YACSPrs_BlocNode.h>

#include <ComposedNode.hxx>
#include <Bloc.hxx>
#include <Proc.hxx>

#include <QxGraph_Canvas.h>
#include <QxGraph_Prs.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

/*!
 * =========================== YACSGui_Node ===========================
 !*/

//! Constructor
/*!   
*/
YACSGui_Node::YACSGui_Node(YACSGui_Graph* theGraph) : myGraph(theGraph)
{
}

//! Destructor
/*!
*/
YACSGui_Node::~YACSGui_Node()
{
}

//! Returns the resource manager object
/*!
 */
SUIT_ResourceMgr* YACSGui_Node::resMgr() const
{
  return SUIT_Session::session()->resourceMgr();
}

//! Updates a node presentation
/*!
 *  New graphic items should be created with a null canvas pointer.
 *  They can be added to a canvas by QxGraph_Prs::show() and removed from it by hide().
 */
void YACSGui_Node::update(YACS::ENGINE::Node* theEngine, YACSPrs_ElementaryNode*& theItem )
{
  if ( !myGraph )
    return;

  if ( theItem )
  {
    // TODO - Here any parameters of exisiting items should be updated
    // ...
    theItem->update();
  }
}

/*!
 * =========================== YACSGui_ServiceNode ===========================
 !*/
 
//! Constructor
/*!   
*/
YACSGui_ServiceNode::YACSGui_ServiceNode(YACSGui_Graph* theGraph) : YACSGui_Node(theGraph)
{
}

//! Destructor
/*!
*/
YACSGui_ServiceNode::~YACSGui_ServiceNode()
{
}

//! Updates a node presentation
/*!
 *  New graphic items should be created with a null canvas pointer.
 *  They can be added to a canvas by QxGraph_Prs::show() and removed from it by hide().
 */
void YACSGui_ServiceNode::update(YACS::ENGINE::Node* theEngine, YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    theItem = new YACSPrs_ServiceNode( resMgr(), 0, theEngine);
    graph()->registerStatusObserverWithNode(theEngine);
  }
  else
  {
    // TODO - Here any parameters of exisiting items should be updated
    // ...
    theItem->update();
  }
}

/*!
 * =========================== YACSGui_InlineNode ===========================
 !*/

//! Constructor
/*!   
*/
YACSGui_InlineNode::YACSGui_InlineNode(YACSGui_Graph* theGraph) : YACSGui_Node(theGraph)
{
}

//! Destructor
/*!
*/
YACSGui_InlineNode::~YACSGui_InlineNode()
{
}

//! Updates a node presentation
/*!
 *  New graphic items should be created with a null canvas pointer.
 *  They can be added to a canvas by QxGraph_Prs::show() and removed from it by hide().
 */
void YACSGui_InlineNode::update(YACS::ENGINE::Node* theEngine, YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    theItem = new YACSPrs_InlineNode( resMgr(), 0, theEngine);
    graph()->registerStatusObserverWithNode(theEngine);
  }
  else
  {
    // TODO - Here any parameters of exisiting items should be updated
    // ...
    theItem->update();
  }
}

/*!
 * =========================== YACSGui_IfNode ===========================
 !*/

//! Constructor
/*!   
*/
YACSGui_IfNode::YACSGui_IfNode(YACSGui_Graph* theGraph) : YACSGui_Node(theGraph)
{
}

//! Destructor
/*!
*/
YACSGui_IfNode::~YACSGui_IfNode()
{
}

//! Updates a node presentation
/*!
 *  New graphic items should be created with a null canvas pointer.
 *  They can be added to a canvas by QxGraph_Prs::show() and removed from it by hide().
 */
void YACSGui_IfNode::update(YACS::ENGINE::Node* theEngine, YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    theItem = new YACSPrs_IfNode( resMgr(), 0, theEngine);
    graph()->registerStatusObserverWithNode(theEngine);
  }
  else
  {
    // TODO - Here any parameters of exisiting items should be updated
    // ...
    theItem->update();
  }
}

/*!
 * =========================== YACSGui_SwitchNode ===========================
 !*/

//! Constructor
/*!   
*/
YACSGui_SwitchNode::YACSGui_SwitchNode(YACSGui_Graph* theGraph) : YACSGui_Node(theGraph)
{
}

//! Destructor
/*!
*/
YACSGui_SwitchNode::~YACSGui_SwitchNode()
{
}

//! Updates a node presentation
/*!
 *  New graphic items should be created with a null canvas pointer.
 *  They can be added to a canvas by QxGraph_Prs::show() and removed from it by hide().
 */
void YACSGui_SwitchNode::update(YACS::ENGINE::Node* theEngine, YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    theItem = new YACSPrs_SwitchNode( resMgr(), 0, theEngine);
    graph()->registerStatusObserverWithNode(theEngine);
  }
  else
  {
    // TODO - Here any parameters of exisiting items should be updated
    // ...
    theItem->update();
  }
}

/*!
 * =========================== YACSGui_LoopNode ===========================
 !*/

//! Constructor
/*!   
*/
YACSGui_LoopNode::YACSGui_LoopNode(YACSGui_Graph* theGraph) : YACSGui_Node(theGraph)
{
}

//! Destructor
/*!
*/
YACSGui_LoopNode::~YACSGui_LoopNode()
{
}

//! Updates a node presentation
/*!
 *  New graphic items should be created with a null canvas pointer.
 *  They can be added to a canvas by QxGraph_Prs::show() and removed from it by hide().
 */
void YACSGui_LoopNode::update(YACS::ENGINE::Node* theEngine, YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    theItem = new YACSPrs_LoopNode( resMgr(), 0, theEngine);
    graph()->registerStatusObserverWithNode(theEngine);
  }
  else
  {
    // TODO - Here any parameters of exisiting items should be updated
    // ...
    theItem->update();
  }
}

/*!
 * =========================== YACSGui_ForEachLoopNode ===========================
 !*/

//! Constructor
/*!   
*/
YACSGui_ForEachLoopNode::YACSGui_ForEachLoopNode(YACSGui_Graph* theGraph) : YACSGui_Node(theGraph)
{
}

//! Destructor
/*!
*/
YACSGui_ForEachLoopNode::~YACSGui_ForEachLoopNode()
{
}

//! Updates a node presentation
/*!
 *  New graphic items should be created with a null canvas pointer.
 *  They can be added to a canvas by QxGraph_Prs::show() and removed from it by hide().
 */
void YACSGui_ForEachLoopNode::update(YACS::ENGINE::Node* theEngine, YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    theItem = new YACSPrs_ForEachLoopNode( resMgr(), 0, theEngine);
    graph()->registerStatusObserverWithNode(theEngine);
  }
  else
  {
    // TODO - Here any parameters of exisiting items should be updated
    // ...
    theItem->update();
  }
}

/*!
 * =========================== YACSGui_BlocNode ===========================
 !*/

//! Constructor
/*!   
*/
YACSGui_BlocNode::YACSGui_BlocNode(YACSGui_Graph* theGraph) : YACSGui_Node(theGraph)
{
}

//! Destructor
/*!
*/
YACSGui_BlocNode::~YACSGui_BlocNode()
{
}

//! Updates a node presentation
/*!
 *  New graphic items should be created with a null canvas pointer.
 *  They can be added to a canvas by QxGraph_Prs::show() and removed from it by hide().
 */
void YACSGui_BlocNode::update(YACS::ENGINE::Node* theEngine, YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem )
  {
    YACSPrs_BlocNode* aBlocPrs = new YACSPrs_BlocNode( resMgr(), 0, theEngine,
						       YACSPrs_BlocNode::Expanded, level(theEngine) );
    YACS::ENGINE::Bloc* aBloc = dynamic_cast<YACS::ENGINE::Bloc*>( theEngine );
    if ( aBloc )
    {
      std::set<YACSPrs_ElementaryNode*> aNodePrsSet;
      std::set<YACS::ENGINE::Node*> aNodeSet;
      graph()->getAllBlocChildren(aBloc, aNodeSet);
      for ( std::set<YACS::ENGINE::Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
      {
	if ( !graph()->getItem( *it ) ) graph()->update( *it );
	aNodePrsSet.insert( graph()->getItem( *it ) );
      }
      aBlocPrs->setChildren( aNodePrsSet );

      theItem = aBlocPrs;

      graph()->registerStatusObserverWithNode(theEngine);
    }
  }
  else
  {
    // TODO - Here any parameters of exisiting items should be updated
    // ...
    theItem->update();
  }
}

//! Returns a nesting level of Bloc node
int YACSGui_BlocNode::level(YACS::ENGINE::Node* theEngine)
{
  //YACS::ENGINE::Bloc* aBloc = dynamic_cast<YACS::ENGINE::Bloc*>( theEngine );
  //if ( !aBloc ) return -1;

  int aLevel = 3;
  YACS::ENGINE::ComposedNode* aFather = theEngine->getFather();
  while ( !dynamic_cast<YACS::ENGINE::Proc*>( aFather ) )
  {
    if ( dynamic_cast<YACS::ENGINE::Bloc*>( aFather ) ) aLevel += 4;
    aFather = aFather->getFather();
  }
  return aLevel;
}
