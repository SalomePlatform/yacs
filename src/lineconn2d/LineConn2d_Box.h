// File:      LineConn2d_Box.h
// Created:   03.08.05 20:16:45
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#ifndef LineConn2d_Box_HeaderFile
#define LineConn2d_Box_HeaderFile

#include <gp_XY.hxx>

class LineConn2d_Segment;
class LineConn2d_Object;

///  Simple class implementing 2D Bounding Box type
//

class LineConn2d_Box 
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline LineConn2d_Box ()
    : myCenter (::RealLast(), ::RealLast()),
      myHSize  (-::RealLast(), -::RealLast())
  {}

  /**
   * Constructor.
   * @param theCenter
   *   Center of the created box
   * @param theHSize
   *   Half-diagonal of the box, both X and Y should be non-negative
   */
  inline LineConn2d_Box (const gp_XY& theCenter,
                         const gp_XY& theHSize)
    : myCenter (theCenter),
      myHSize  (theHSize) {}

  /**
   * Reset the box data.
   */
  inline void Clear ()
  {
    myCenter.SetCoord (::RealLast(), ::RealLast());
    myHSize .SetCoord (-::RealLast(), -::RealLast());
  }

  /**
   * Update the box by a point.
   */
  Standard_EXPORT void          Add     (const gp_XY& thePnt);

  /**
   * Update the box by another box.
   */
  inline void                   Add     (const LineConn2d_Box& theBox)
  {
    Add (theBox.Center() - theBox.HSize());
    Add (theBox.Center() + theBox.HSize());
  }

  /**
   * Query the box center.
   */
  inline const gp_XY&           Center  () const
  { return myCenter; }

  /**
   * Query the box half-diagonal.
   */
  inline const gp_XY&           HSize   () const
  { return myHSize; }

  /**
   * Query the square diagonal.
   */
  inline Standard_Real          SquareExtent () const
  { return 4 * myHSize.SquareModulus(); }

  /*
   * Extend the Box by the absolute value of aDiff.
   */
  Standard_EXPORT void          Enlarge (const Standard_Real theDiff);

  /**
   * Limit the curent Box with the internals of theBox.
   */
  Standard_EXPORT void          Limit   (const LineConn2d_Box& theBox);

  /**
   * Check the given point for inclusion in the box.
   * @return
   *   True if thePnt is outside the box.
   */
  Standard_EXPORT Standard_Boolean IsOut(const gp_XY& thePnt) const;

  /**
   * Check the given box for intersection with the current box.
   * @return
   *   True if there is no intersection between the boxes.
   */
  Standard_EXPORT Standard_Boolean IsOut(const LineConn2d_Box& theBox) const;

  /**
   * Check the given box for intersection with an Object
   * @return
   *   True if there is no intersection with the object theObj.
   */
  Standard_EXPORT Standard_Boolean IsOut(const LineConn2d_Object& theObj) const;

  /**
   * Check the given box for intersection with a segment
   * @return
   *   True if there is no intersection with the segment aSeg.
   */
  Standard_EXPORT Standard_Boolean IsOut(const LineConn2d_Segment& aSeg) const;

  /**
   * Find intersection points between the boundary of the box and
   * the given Segment. No intersection returned if the segment is entirely
   * inside or outside the box. Therefore this method can not be used as
   * replacement of the corresponding IsOut check. 
   * @param outInter
   *   Array of 2 output numbers. Contains the parameters ([0..1] inclusive)
   *   of intersection point(s). If 2 intersections are found, it is ensured
   *   that outInter[0] < outInter[1].
   * @param aSeg
   *   Segment for intersection.
   * @return
   *   Number of intersections (0, 1 or 2).
   */
  Standard_EXPORT Standard_Integer Intersect
                                        (Standard_Real             outInter[2],
                                         const LineConn2d_Segment& aSeg) const;

 private:
  // ---------- PRIVATE FIELDS ----------

  gp_XY         myCenter;
  gp_XY         myHSize;
};

#endif
