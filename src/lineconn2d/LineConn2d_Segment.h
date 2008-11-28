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
// File:      LineConn2d_Segment.h
// Created:   04.08.05 22:37:59
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#ifndef LineConn2d_Segment_HeaderFile
#define LineConn2d_Segment_HeaderFile

#include <gp_XY.hxx>

class LineConn2d_Box;

/**
 *  2D segment for which the data are defined:
 *  @li Origin point;
 *  @li Extremity point;
 *  @li Delta (vector from Origin to Extremity).
 */
class LineConn2d_Segment
{
 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty constructor
  inline LineConn2d_Segment ()
    : myOrigin (0., 0.), myDelta (0., 0.) {}

  /**
   * Constructor
   */
  inline LineConn2d_Segment (const gp_XY& theOrigin, const gp_XY& theExtremity)
    : myOrigin (theOrigin),
      myDelta  (theExtremity - theOrigin) {}

  /**
   * Set the Origin point
   */
  inline void SetOrigin (const gp_XY& theOrigin)
  { myOrigin = theOrigin; }

  /**
   * Set the Delta
   */
  inline void SetDelta (const gp_XY& theDelta)
  { myDelta = theDelta; }

  /**
   * Set the Extremity
   */
  inline void SetExtremity (const gp_XY& theExtremity)
  { myDelta = theExtremity - myOrigin; }

  /**
   * Query the Origin
   */
  inline const gp_XY& Origin () const
  { return myOrigin; }

  /**
   * Query the Delta
   */
  inline const gp_XY& Delta () const
  { return myDelta; }

  /**
   * Query the point interpolated between Origin and Extremity
   * in interval [0 .. 1].
   * @param theParam
   *   interpolation parameter; if negative or more than 1, the result is
   *   outside the Segment
   */
  inline gp_XY Point (const Standard_Real theParam) const
  { return myOrigin + myDelta * theParam; }

  /**
   * Query the Extremity
   */
  inline gp_XY Extremity () const
  { return myOrigin + myDelta; }

  /**
   * Limit the Segment by the internals of theBox.
   */
  Standard_EXPORT void Limit (const LineConn2d_Box& theBox);

  /**
   * Test two segments for intersection.
   * @param theSegOther
   *   Other segment (to be intersected with)
   * @param outAlpha
   *   2 Parameters of intersecton (this and other segment). The parameter
   *   defines the intersection point on the given segment, as interpolation
   *   value [0 .. 1] between the Origin and the Extremity.
   *   Indefinite if there is no intersection.
   * @return
   *   True if there is intersection.
   */ 
  Standard_EXPORT Standard_Boolean IsIntersect
                                (const LineConn2d_Segment& theSegOther,
                                 Standard_Real             outAlpha[2]) const;


 private:
  // ---------- PRIVATE FIELDS ----------

  gp_XY                 myOrigin;
  gp_XY                 myDelta;
};

#endif
