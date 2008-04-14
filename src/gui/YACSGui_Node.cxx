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

#include <Python.h>
#include <PythonNode.hxx>

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
#include <guiObservers.hxx>
#include <CORBANode.hxx>
#include <CppNode.hxx>
#include <SalomePythonNode.hxx>
#include <XMLNode.hxx>
#include <Switch.hxx>
#include <ForLoop.hxx>
#include <WhileLoop.hxx>
//#include <OptimizerLoop.hxx>
#include <ForEachLoop.hxx>

#include <QxGraph_Canvas.h>
#include <QxGraph_Prs.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace YACS::HMI;

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
void YACSGui_Node::update(YACS::ENGINE::Node* theEngine, 
			  YACS::HMI::SubjectComposedNode* theParent, 
			  YACSPrs_ElementaryNode*& theItem )
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
void YACSGui_ServiceNode::update(YACS::ENGINE::Node* theEngine, 
				 YACS::HMI::SubjectComposedNode* theParent,
				 YACSPrs_ElementaryNode*& theItem )
{
  DEBTRACE("YACSGui_ServiceNode::update " << theItem);
  if ( !graph() )
    return;

  if ( !theItem ) {
    SubjectNode* aSNode = theParent->getChild(theEngine);
    DEBTRACE(aSNode);
    if ( aSNode )
    {
      theItem = new YACSPrs_ServiceNode( resMgr(), graph()->getCanvas(), aSNode );
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
void YACSGui_InlineNode::update(YACS::ENGINE::Node* theEngine,
				YACS::HMI::SubjectComposedNode* theParent,
				YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    SubjectNode* aINode = theParent->getChild(theEngine);
    if ( aINode )
    {
      theItem = new YACSPrs_InlineNode( resMgr(), graph()->getCanvas(), aINode );
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
void YACSGui_IfNode::update(YACS::ENGINE::Node* theEngine,
			    YACS::HMI::SubjectComposedNode* theParent,
			    YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    SubjectComposedNode* aCNode = dynamic_cast<SubjectComposedNode*>( theParent->getChild(theEngine) );
    if ( aCNode )
    {
      theItem = new YACSPrs_IfNode( resMgr(), graph()->getCanvas(), aCNode );
      graph()->registerStatusObserverWithNode(theEngine);
      graph()->createChildNodesPresentations( aCNode );
    }
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
void YACSGui_SwitchNode::update(YACS::ENGINE::Node* theEngine,
				YACS::HMI::SubjectComposedNode* theParent,
				YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    SubjectSwitch* aSNode = dynamic_cast<SubjectSwitch*>( theParent->getChild(theEngine) );
    if ( aSNode )
    {
      theItem = new YACSPrs_SwitchNode( resMgr(), graph()->getCanvas(), aSNode );
      graph()->registerStatusObserverWithNode(theEngine);
      graph()->createChildNodesPresentations( aSNode );
      aSNode->attach( graph() );
    }
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
void YACSGui_LoopNode::update(YACS::ENGINE::Node* theEngine,
			      YACS::HMI::SubjectComposedNode* theParent,
			      YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    SubjectComposedNode* aLNode = dynamic_cast<SubjectComposedNode*>( theParent->getChild(theEngine) );
    if ( aLNode )
    {
      theItem = new YACSPrs_LoopNode( resMgr(), graph()->getCanvas(), aLNode );
      graph()->registerStatusObserverWithNode(theEngine);
      graph()->createChildNodesPresentations( aLNode );
      aLNode->attach( graph() );
    }
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
void YACSGui_ForEachLoopNode::update(YACS::ENGINE::Node* theEngine,
				     YACS::HMI::SubjectComposedNode* theParent,
				     YACSPrs_ElementaryNode*& theItem )
{
  if ( !graph() )
    return;

  if ( !theItem ) {
    SubjectComposedNode* aFELNode = dynamic_cast<SubjectComposedNode*>( theParent->getChild(theEngine) );
    if ( aFELNode )
    {
      theItem = new YACSPrs_ForEachLoopNode( resMgr(), graph()->getCanvas(), aFELNode );
      graph()->registerStatusObserverWithNode(theEngine);
      graph()->createChildNodesPresentations( aFELNode );
      aFELNode->attach( graph() );
    }
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
void YACSGui_BlocNode::update(YACS::ENGINE::Node* theEngine,
			      YACS::HMI::SubjectComposedNode* theParent,
			      YACSPrs_ElementaryNode*& theItem )
{
  DEBTRACE("YACSGui_BlocNode::update");
  if ( !graph() )
    return;

  if ( !theItem )
  {
    DEBTRACE("YACSGui_BlocNode::update");
    YACS::ENGINE::Bloc* aBloc = dynamic_cast<YACS::ENGINE::Bloc*>( theEngine );
    if ( aBloc )
    {
      SubjectBloc* aBNode = dynamic_cast<SubjectBloc*>( theParent->getChild(theEngine) );
      if ( aBNode )
      {
	// create children presentations (and, of course, subjects)
	graph()->createChildNodesPresentations( aBNode );

	// collect created children presentations in the list
	std::set<YACSPrs_ElementaryNode*> aNodePrsSet;
	std::set<YACS::ENGINE::Node*> aNodeSet;
	graph()->getAllBlocChildren(aBloc, aNodeSet);
	for ( std::set<YACS::ENGINE::Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
	  if ( graph()->getItem( *it ) )
	    aNodePrsSet.insert( graph()->getItem( *it ) );
	
	YACSPrs_BlocNode* aBlocPrs = new YACSPrs_BlocNode( resMgr(), graph()->getCanvas(),
							   aBNode,
							   YACSPrs_BlocNode::Expanded,
							   level(theEngine) );
	aBlocPrs->setChildren( aNodePrsSet );
	
	theItem = aBlocPrs;
	
	graph()->registerStatusObserverWithNode(theEngine);
	aBNode->attach( graph() );
      }
    }
  }
  else
  {
    // TODO - Here any parameters of exisiting items should be updated
    // ...

    // NB: this case is actual if the content of the block node (i.e. the set of child nodes) was changed
    YACSPrs_BlocNode* aBlocPrs = dynamic_cast<YACSPrs_BlocNode*>( theItem );
    YACS::ENGINE::Bloc* aBloc = dynamic_cast<YACS::ENGINE::Bloc*>( theEngine );
    DEBTRACE("YACSGui_BlocNode::update "<< aBlocPrs <<","<< aBloc);
    if ( aBlocPrs && aBloc )
    {
      // get old set of children: make it independent from the block
      std::set<YACSPrs_ElementaryNode*> anOldChildren = aBlocPrs->getChildren();
      DEBTRACE(anOldChildren.size());
      for ( std::set<YACSPrs_ElementaryNode*>::iterator it = anOldChildren.begin(); it != anOldChildren.end(); it++ )
      {
        DEBTRACE(*it);
	(*it)->setIsInBloc(false);
	(*it)->setSelfMoving(true);
      }
      
      // collect presentations of the new children in the list: set new children list to the block
      std::set<YACSPrs_ElementaryNode*> aNodePrsSet;
      std::set<YACS::ENGINE::Node*> aNodeSet;
      graph()->getAllBlocChildren(aBloc, aNodeSet);
      DEBTRACE(aNodeSet.size());
      for ( std::set<YACS::ENGINE::Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
	if ( graph()->getItem( *it ) )
	  aNodePrsSet.insert( graph()->getItem( *it ) );
      
      DEBTRACE(aNodePrsSet.size());
      aBlocPrs->setChildren( aNodePrsSet );

      // remove control links of the new children
      for ( std::set<YACSPrs_ElementaryNode*>::iterator childIt = aNodePrsSet.begin(); childIt != aNodePrsSet.end(); childIt++ )
      {  
	YACSPrs_ElementaryNode* aPrs = *childIt;
	if( anOldChildren.count( aPrs ) )
	  continue;
	std::list<SubjectControlLink*> aControlLinks = aPrs->getSEngine()->getSubjectControlLinks();
	for( std::list<SubjectControlLink*>::iterator iter = aControlLinks.begin(); iter != aControlLinks.end(); iter++ )
	{
	  SubjectControlLink* aControlLink = *iter;
	  if( SubjectNode* anOutNode = aControlLink->getSubjectOutNode() )
	    if( YACSPrs_ElementaryNode* aParentPrs = graph()->getItem( anOutNode->getNode() ) )
	      aParentPrs->removeLinkPrs( aControlLink );
	}
      }

      Bloc* aFather = aBloc;
      bool isNeedToArrange = true;
      if ( !anOldChildren.empty() || aNodeSet.empty() )
	isNeedToArrange = graph()->isNeededToIncreaseBlocSize(aFather);
      while ( isNeedToArrange && aFather && !dynamic_cast<Proc*>(aFather) )
      {
	graph()->arrangeNodesWithinBloc(aFather);
	aFather = dynamic_cast<Bloc*>(aFather->getFather());
	isNeedToArrange = graph()->isNeededToIncreaseBlocSize(aFather);
      }
      graph()->getCanvas()->update();

      theItem = aBlocPrs;
    }
    
    //theItem->update();
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
