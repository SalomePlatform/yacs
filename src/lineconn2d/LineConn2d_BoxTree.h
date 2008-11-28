//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File:      LineConn2d_BoxTree.h
// Created:   12.08.05 14:45:10
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#ifndef LineConn2d_BoxTree_HeaderFile
#define LineConn2d_BoxTree_HeaderFile

#include <NCollection_UBTree.hxx>
#include <NCollection_List.hxx>
#include <LineConn2d_Box.h>

class LineConn2d_Object;

/**
 * Implementation of AABB-tree of bounding boxes for quick selection
 * of entities.
 */

typedef NCollection_UBTree <const LineConn2d_Object *, LineConn2d_Box>
                LineConn2d_BoxTree;

/**
 * Specialization of Selector class defined for NCollection_UBTree.
 */

class LineConn2d_BoxTreeSelector : public LineConn2d_BoxTree::Selector
{
 public:
  /**
   *    Constructor. Takes Box as parameter
   */
  inline                LineConn2d_BoxTreeSelector(const LineConn2d_Box& theBox)
    : myBox (theBox) {}
  
  /**
   *    Constructor. Takes Segment as parameter
   */
  Standard_EXPORT       LineConn2d_BoxTreeSelector (const LineConn2d_Segment&);
  
  /**
   *    Query the list of elements.
   */
  inline const NCollection_List<const LineConn2d_Object *>&
                        GetObjects () const
    { return myList; }

  /**
   * Redefined from the base class
   */
  Standard_EXPORT Standard_Boolean
                        Reject  (const LineConn2d_Box& theBox) const;

  /**
   * Redefined from the base class
   */
  Standard_EXPORT Standard_Boolean
                        Accept  (const LineConn2d_Object * const& theObj);
  
 protected:
  NCollection_List<const LineConn2d_Object *>     myList;
  LineConn2d_Box                                  myBox;
};

#endif
