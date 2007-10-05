// File:      LineConn2d.h
// Created:   04.08.05 22:26:39
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#ifndef LineConn2d_HeaderFile
#define LineConn2d_HeaderFile

#include <Precision.hxx>

class LineConn2d_Object;
class gp_XY;

#ifdef __sparc
#define LineConn2d_FABS(x) ((x) < 0.? -(x) : (x))
#else
#include <math.h>
#define LineConn2d_FABS(x) ::fabs(x)
#endif

///  Utilities
//

class LineConn2d
{
 public:

  /**
   * One of 4 possible orientations.
   */
  typedef enum {
    Indefinite  = -1,   ///< non-initialized
    E           = 0,    ///< east
    N           = 1,    ///< north
    W           = 2,    ///< west
    S           = 3,    ///< south
    Indefinite1 = 4
  } Orient;

  // ---------- PUBLIC METHODS ----------

  /**
   * Test the value if it is small (absolute value is tested).
   */
  inline static Standard_Boolean IsSmall (const Standard_Real theVal)
  { return LineConn2d_FABS (theVal) < Precision::Confusion(); }

  /**
   * Test the value for inclusion in the given interval.
   * To ensure the inclusion the interval is extended by Confusion value.
   * @param theVal
   *   the given Real value
   * @param theLow
   *   lower bound of the interval
   * @param theUp
   *   upper bound of the interval
   * @return
   *   True if theVal is inside the interval inclusive.
   */
  inline static Standard_Boolean IsInside (const Standard_Real theVal,
                                           const Standard_Real theLow,
                                           const Standard_Real theUp)
  { return (theVal > theLow - Precision::Confusion() &&
            theVal < theUp  + Precision::Confusion()); }

  /**
   * Return the orientatation of the given axis.
   * LineConn2d_Port can be used as subclass of the axis
   */
  Standard_EXPORT static Orient           Orientation (const gp_XY& aDir);

  /**
   * Test intersection of two Objects.
   */
  Standard_EXPORT static Standard_Boolean IsIntersect
                                        (const LineConn2d_Object& anObj1,
                                         const LineConn2d_Object& anObj2);


 private:
  // ---------- PRIVATE METHODS ----------
  /// Empty constructor
  LineConn2d () {}

};

#endif
