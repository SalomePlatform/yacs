// File:      LineConn2d_Object.h
// Created:   03.08.05 20:34:45
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#ifndef LineConn2d_Object_HeaderFile
#define LineConn2d_Object_HeaderFile

#include <LineConn2d_Box.h>
#include <NCollection_List.hxx>

/**
 * Data Object is represented by:
 *  - Polyline (a list of points) for one closed contour defioning the shape
 *  - Bounding box of the polyline.
 * The Objects should be allocated/destroyed exclusively from the Model class.
 */

class LineConn2d_Object 
{
 public:

  /// Iterator type, to retrieve the contour points
  typedef NCollection_List<gp_XY>::Iterator PointIterator;

  // ---------- PUBLIC METHODS ----------

  /// Empty constructor.
  inline LineConn2d_Object () {}

  /**
   * Add a next point to the polyline of the given Object.
   * You should be careful to provide a correct index value.
   */
  inline void                       AddPoint  (const gp_XY& thePnt)
  {
    myPoints.Append (thePnt);
    myIsModified = 1;
  }

  /**
   * Update the Object internal data after its initialisation. This method
   * builds the bounding box and performs other preparations.
   */
  Standard_EXPORT void              Update    ();

  /**
   * Query the bounding box.
   */
  inline const LineConn2d_Box&      Box       () const
  { return myBox; }

  /**
   * Query the number of points in the contour.
   */
  inline Standard_Integer           NbPoints  () const
  { return myPoints.Extent(); }

  /**
   * Get Iterator on points.
   */
  inline PointIterator              PointsIterator () const
  { return myPoints; }

  /**
   * Query the last point in the contour.
   */
  inline const gp_XY&               LastPoint () const
  { return myPoints.Last(); }

  /**
   * Classify a point towards the polygon defined by the object
   * @param thePoint
   *   The given point to be classified
   * @param theTol
   *   Tolerance for ON condition: if thePoint is nearer to any contour segment
   *   than the value theTol, the method returns False.
   * @return
   *   True if the given point is outside the Object contour.
   */
  Standard_EXPORT Standard_Boolean  IsOut     (const gp_XY&        thePoint,
                                               const Standard_Real theTol)const;

 protected:
  // ---------- PROTECTED METHODS ----------

  /// Constructor.
  inline LineConn2d_Object (const Handle(NCollection_BaseAllocator)& theAlloc)
    : myPoints (theAlloc)  {}

 private:
  // ---------- PRIVATE FIELDS ----------

  NCollection_List<gp_XY>       myPoints;
  LineConn2d_Box                myBox;
  Standard_Boolean              myIsModified : 1;

  friend class LineConn2d_Model;
};

#endif
