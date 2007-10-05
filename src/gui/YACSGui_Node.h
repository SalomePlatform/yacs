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

#ifndef YACSGui_Node_HeaderFile
#define YACSGui_Node_HeaderFile

#include <YACSGui_Graph.h>

#include <Node.hxx>

class SUIT_ResourceMgr;

/*! Interface of a driver that creates specific graphic items for some type of 
 *  graph nodes. It also updates any parameters of existing graphic items
 *  before they are redrawn by the viewer.
 *  Drivers for each kind of node should be derived from YACSGui_Node,
 *  with their own implementation of update() method (it should become
 *  a pure virtual function in the final version).
 */
class YACSGui_Node
{
 public:
  YACSGui_Node(YACSGui_Graph*);
  virtual ~YACSGui_Node();

  virtual void        update(YACS::ENGINE::Node*, YACSPrs_ElementaryNode*&);

protected:
  YACSGui_Graph*      graph() const { return myGraph; }
  SUIT_ResourceMgr*   resMgr() const;

private:
  YACSGui_Graph*      myGraph;
};

/*! Interface of a driver that creates specific graphic items for Service 
 *  graph nodes. It also updates any parameters of existing graphic items
 *  before they are redrawn by the viewer.
 */
class YACSGui_ServiceNode : public YACSGui_Node
{
 public:
  YACSGui_ServiceNode(YACSGui_Graph*);
  virtual ~YACSGui_ServiceNode();

  virtual void        update(YACS::ENGINE::Node*, YACSPrs_ElementaryNode*&);

};

/*! Interface of a driver that creates specific graphic items for Inline 
 *  graph nodes. It also updates any parameters of existing graphic items
 *  before they are redrawn by the viewer.
 */
class YACSGui_InlineNode : public YACSGui_Node
{
 public:
  YACSGui_InlineNode(YACSGui_Graph*);
  virtual ~YACSGui_InlineNode();

  virtual void        update(YACS::ENGINE::Node*, YACSPrs_ElementaryNode*&);

};

/*! Interface of a driver that creates specific graphic items for If 
 *  graph nodes. It also updates any parameters of existing graphic items
 *  before they are redrawn by the viewer.
 */
class YACSGui_IfNode : public YACSGui_Node
{
 public:
  YACSGui_IfNode(YACSGui_Graph*);
  virtual ~YACSGui_IfNode();

  virtual void        update(YACS::ENGINE::Node*, YACSPrs_ElementaryNode*&);

};

/*! Interface of a driver that creates specific graphic items for Switch 
 *  graph nodes. It also updates any parameters of existing graphic items
 *  before they are redrawn by the viewer.
 */
class YACSGui_SwitchNode : public YACSGui_Node
{
 public:
  YACSGui_SwitchNode(YACSGui_Graph*);
  virtual ~YACSGui_SwitchNode();

  virtual void        update(YACS::ENGINE::Node*, YACSPrs_ElementaryNode*&);

};

/*! Interface of a driver that creates specific graphic items for ForLoop and WhileLoop
 *  graph nodes. It also updates any parameters of existing graphic items
 *  before they are redrawn by the viewer.
 */
class YACSGui_LoopNode : public YACSGui_Node
{
 public:
  YACSGui_LoopNode(YACSGui_Graph*);
  virtual ~YACSGui_LoopNode();

  virtual void        update(YACS::ENGINE::Node*, YACSPrs_ElementaryNode*&);

};

/*! Interface of a driver that creates specific graphic items for ForEachLoop
 *  graph nodes. It also updates any parameters of existing graphic items
 *  before they are redrawn by the viewer.
 */
class YACSGui_ForEachLoopNode : public YACSGui_Node
{
 public:
  YACSGui_ForEachLoopNode(YACSGui_Graph*);
  virtual ~YACSGui_ForEachLoopNode();

  virtual void        update(YACS::ENGINE::Node*, YACSPrs_ElementaryNode*&);

};

/*! Interface of a driver that creates specific graphic items for Bloc
 *  graph nodes. It also updates any parameters of existing graphic items
 *  before they are redrawn by the viewer.
 */
class YACSGui_BlocNode : public YACSGui_Node
{
 public:
  YACSGui_BlocNode(YACSGui_Graph*);
  virtual ~YACSGui_BlocNode();

  virtual void        update(YACS::ENGINE::Node*, YACSPrs_ElementaryNode*&);

  int level(YACS::ENGINE::Node* theEngine);
};

#endif
