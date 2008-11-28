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
// File:      LineConn2d_Path.h
// Created:   11.08.05 15:08:00
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#ifndef LineConn2d_Path_HeaderFile
#define LineConn2d_Path_HeaderFile

#include <LineConn2d_Segment.h>
#include <LineConn2d.h>
#include <NCollection_List.hxx>

class LineConn2d_Model;

/**
 * Single path as temporary object created during the calculation of
 * connections. It can be treated as:
 * @li Segment having a definite orientation, origin and length. or
 * @li 2D box along that segment, with a width equal to the model tolerance, or
 * @li Node of the tree of Paths created during the calculations.
 */

class LineConn2d_Path : public LineConn2d_Segment
{
 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty Constructor.
  Standard_EXPORT LineConn2d_Path ();

  /// Constructor.
  Standard_EXPORT LineConn2d_Path  (const gp_XY&             theStart,
                                    const gp_XY&             theEnd,
                                    const LineConn2d_Model&  theModel,
                                    const LineConn2d::Orient theOri =
                                                        LineConn2d::Indefinite);
  /**
   * Query the Width.
   */
  Standard_EXPORT Standard_Real Width           () const;

  /**
   * Query the parent.
   */
  inline const LineConn2d_Path& Parent          () const
  { return * myParent; }

  /**
   * Query the orientation, according to the given Side
   * @param theSide
   *   Defines the side on which the orientation is calculated.
   *   @li Left : -1
   *   @li Front:  0
   *   @li Right: +1
   */
  Standard_EXPORT LineConn2d::Orient Orientation (const Standard_Integer theSide
                                                        = 0) const;

  /**
   * Query the limitation of the Path.
   * @return
   *   @li -1: limited on the left;
   *   @li  0: limited on the front or unlimited;
   *   @li  1: limited on the right.
   */
  inline Standard_Integer       Limitation      () const
  { return myLimitation; }

  /**
   * Query the direction according to the given Side.
   * @param theSide
   *   Defines the side on which the direction is calculated.
   *   @li Left : -1
   *   @li Front:  0
   *   @li Right: +1
   * @return
   *   Unit vector of the corresponding direction.
   */
  Standard_EXPORT gp_XY         Direction       (const Standard_Integer theSide
                                                   = 0) const;

  /**
   * Query the price associated with the end of the current Path.
   * The price is calculated by the method Evaluate().
   * The algorithm:
   * - the distance is calculated as sum of absolute distances along X and Y.
   * - the distance contribution to the price is percent of the half-perimeter
   *   of the Model box.
   * - each change of orientation gives the contribution of PriceOfBreak points.
   */
  inline Standard_Integer       Price           () const
  { return myPriceLen + myPriceDist; }

  /**
   * Create a Path in the given direction. The Path stops at an Object or
   * at the boundary of the Model.
   * @param outPath
   *   Created object.
   * @param theModel
   *   Used to provide the set of Objects, to properly stop the Path.
   * @param theStart
   *   Beginning of the Path.
   * @param theOrient
   *   Orientation of the Path.
   * @param isRoot
   *   True indicates the the root path is built.
   * @return
   *   True on success.
   */
  static Standard_EXPORT Standard_Boolean
                        createPath      (LineConn2d_Path&         outPath,
                                         const LineConn2d_Model&  theModel,
                                         const gp_XY&             theStart,
                                         const LineConn2d::Orient theOrient,
                                         const Standard_Boolean   isRoot);

  /**
   * Evaluate the price of the trajectory where the current Path is the last
   * one. This is done by incrementing the price of the Parent by the addition
   * due to this Path element.
   * <p>This method also checks if the path ends on theTarget, if yes then it
   * sets the flag myIsComplete. 
   * @param theTarget
   *   Target point of the path sequence (the end of the calculated connection) 
   */
  Standard_EXPORT Standard_Integer
                        Evaluate        (const gp_XY& theTarget);

  /**
   * Add the branch path. This method sets the Parent field in theBranch
   * @param theBranch
   *   Path added as branch to the current one.
   * @param theTarget
   *   Target point for price evaluation (method Evaluate is called with it).
   */
  Standard_EXPORT void  AddBranch       (LineConn2d_Path&       theBranch,
                                         const gp_XY&           theTarget);

  /**
   * Query if the Path is completed.
   */
  inline Standard_Boolean
                        IsComplete      () const
  { return myIsComplete; }

  /**
   * Project orthogonally the given point on the Path segment.
   * @param theResult
   *   Output parameter, the projection point belonging to the inside of
   *   the Path segment (only updated if the method returns True).
   * @param thePoint
   *   The given point to be projected.
   * @return
   *   True if the projection is found inside the Path segment
   */
  Standard_EXPORT Standard_Boolean ProjectPoint
                                        (gp_XY&         theResult,
                                         const gp_XY&   thePoint) const;

  /**
   * Price contributon of one path break (change of orientation)
   */
  static inline Standard_Integer
                        PriceOfBreak    ()
  { return 300; }

 protected:
  // ---------- PROTECTED METHODS ----------


 private:
  // ---------- PRIVATE FIELDS ----------
  const LineConn2d_Model                * myModel;
  const LineConn2d_Path                 * myParent;
  NCollection_List <LineConn2d_Path>    myBranches;
  Standard_Integer                      myPriceLen;
  Standard_Integer                      myPriceDist;
  Standard_Integer                      myLimitation; ///< -1, 0(def) or 1
  LineConn2d::Orient                    myOrient;
  Standard_Boolean                      myIsComplete;

  friend class LineConn2d_PathIterator;
};


#endif
