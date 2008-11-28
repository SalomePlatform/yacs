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
// File:      LineConn2d_Box.cpp
// Created:   03.08.05 20:20:53
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#include <LineConn2dAfx.h>

#include <LineConn2d_Box.h>
#include <LineConn2d_SegmentIterator.h>
#include <LineConn2d_Object.h>
#include <LineConn2d.h>

inline Standard_Boolean compareDist (const gp_XY& aHSize, const gp_XY& aDist)
{
  return (LineConn2d_FABS(aDist.X()) > aHSize.X() ||
          LineConn2d_FABS(aDist.Y()) > aHSize.Y());
}

//=======================================================================
//function : Enlarge
//purpose  : 
//=======================================================================

void LineConn2d_Box::Enlarge (const Standard_Real aDiff)
{
  const Standard_Real aD = LineConn2d_FABS(aDiff);
  myHSize = myHSize + gp_XY (aD, aD);
}

//=======================================================================
//function : Limit
//purpose  : limit the current box with the internals of theBox
//=======================================================================

void LineConn2d_Box::Limit (const LineConn2d_Box& theBox)
{
  const gp_XY diffC = theBox.Center() - myCenter;
  // check the condition IsOut
  if (::compareDist (myHSize + theBox.HSize(), diffC) == Standard_False) {
    const gp_XY diffH = theBox.HSize() - myHSize;
    if (diffC.X() - diffH.X() > 0.) {
      const Standard_Real aShift = 0.5 * (diffC.X() - diffH.X()); // positive
      myCenter.SetX (myCenter.X() + aShift);
      myHSize .SetX (myHSize.X()  - aShift);
    } else if (diffC.X() + diffH.X() < 0.) {
      const Standard_Real aShift = 0.5 * (diffC.X() + diffH.X()); // negative
      myCenter.SetX (myCenter.X() + aShift);
      myHSize .SetX (myHSize.X()  + aShift);
    }
    if (diffC.Y() - diffH.Y() > 0.) {
      const Standard_Real aShift = 0.5 * (diffC.Y() - diffH.Y()); // positive
      myCenter.SetY (myCenter.Y() + aShift);
      myHSize .SetY (myHSize.Y()  - aShift);
    } else if (diffC.Y() + diffH.Y() < 0.) {
      const Standard_Real aShift = 0.5 * (diffC.Y() + diffH.Y()); // negative
      myCenter.SetY (myCenter.Y() + aShift);
      myHSize .SetY (myHSize.Y()  + aShift);
    }
  }
}

//=======================================================================
//function : Add
//purpose  : Update the box by a point
//=======================================================================

void LineConn2d_Box::Add (const gp_XY& thePnt) {
  if (myHSize.X() < -1e-5) {
    myCenter = thePnt;
    myHSize.SetCoord (0., 0.);
  } else {
    const gp_XY aDiff = thePnt - myCenter;
    if (aDiff.X() > myHSize.X()) {
      const Standard_Real aShift = (aDiff.X() - myHSize.X()) * 0.5;
      myCenter.SetX (myCenter.X() + aShift);
      myHSize.SetX  (myHSize.X()  + aShift);
    } else if (aDiff.X() < -myHSize.X()) {
      const Standard_Real aShift = (aDiff.X() + myHSize.X()) * 0.5;
      myCenter.SetX (myCenter.X() + aShift);
      myHSize.SetX  (myHSize.X()  - aShift);
    }
    if (aDiff.Y() > myHSize.Y()) {
      const Standard_Real aShift = (aDiff.Y() - myHSize.Y()) * 0.5;
      myCenter.SetY (myCenter.Y() + aShift);
      myHSize.SetY  (myHSize.Y()  + aShift);
    } else if (aDiff.Y() < -myHSize.Y()) {
      const Standard_Real aShift = (aDiff.Y() + myHSize.Y()) * 0.5;
      myCenter.SetY (myCenter.Y() + aShift);
      myHSize.SetY  (myHSize.Y()  - aShift);
    }
  }
}

//=======================================================================
//function : IsOut
//purpose  : Intersection Box - Point
//=======================================================================

Standard_Boolean LineConn2d_Box::IsOut (const gp_XY& thePnt) const
{
  return ::compareDist (myHSize, thePnt - myCenter);
}

//=======================================================================
//function : IsOut
//purpose  : Intersection Box - Box
//=======================================================================

Standard_Boolean LineConn2d_Box::IsOut (const LineConn2d_Box& theBox) const
{
  return ::compareDist (myHSize + theBox.HSize(), theBox.Center() - myCenter);
}

//=======================================================================
//function : IsOut
//purpose  : Intersection Box - Object
//=======================================================================

Standard_Boolean LineConn2d_Box::IsOut (const LineConn2d_Object& theObj) const
{
  Standard_Boolean aResult (Standard_True);
  LineConn2d_SegmentIterator anIter (theObj);
  for (; anIter.More(); anIter.Next())
    if (IsOut (anIter.Value()) == Standard_False) {
      aResult = Standard_False;
      break;
    }
  return aResult;
}

//=======================================================================
//function : IsOut
//purpose  : Intersection Box - Segment
//=======================================================================

Standard_Boolean LineConn2d_Box::IsOut (const LineConn2d_Segment& theSeg) const
{
  Standard_Boolean aResult (Standard_True);
  // Intersect the line containing the segment.
  const Standard_Real aProd[3] = {
    theSeg.Delta() ^ (myCenter - theSeg.Origin()),
    theSeg.Delta().X() * myHSize.Y(),
    theSeg.Delta().Y() * myHSize.X()
  };
  if (LineConn2d_FABS(aProd[0]) < (LineConn2d_FABS(aProd[1]) +
                                   LineConn2d_FABS(aProd[2])))
  {
    // Intersection with line detected; check the segment as bounding box
    const gp_XY aHSeg    (0.5 * theSeg.Delta().X(),
                          0.5 * theSeg.Delta().Y());
    const gp_XY aHSegAbs (LineConn2d_FABS(aHSeg.X()),
                          LineConn2d_FABS(aHSeg.Y()));
    aResult = ::compareDist (myHSize + aHSegAbs,
                             theSeg.Origin() + aHSeg - myCenter);
  }
  return aResult;
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Integer LineConn2d_Box::Intersect
                                (Standard_Real             outInter[2],
                                 const LineConn2d_Segment& theSeg) const
{
  Standard_Integer nInter (0);
  Standard_Real    anAlpha[2];
  // Four sides of the box rectangle
  const gp_XY aSegDelta[4] = {
    gp_XY (-2 * myHSize.X(), 0.),
    gp_XY (0., -2 * myHSize.Y()),
    gp_XY (2 * myHSize.X(), 0.),
    gp_XY (0., 2 * myHSize.Y())
  };
  LineConn2d_Segment aSeg (gp_XY(0., 0.), myCenter + myHSize);

  // Intersect aSeg with all four sides of the box
  for (Standard_Integer i = 0; i < 4; i++) {
    aSeg.SetOrigin (aSeg.Extremity());
    aSeg.SetDelta  (aSegDelta[i]);
    if (theSeg.IsIntersect (aSeg, anAlpha)) {
      outInter[nInter] = anAlpha[0];
      if (nInter++ > 0) {
        // Check to exclude coincident intersection points
        if (LineConn2d::IsSmall (outInter[0] - outInter[1]))
          --nInter;
        else {
          if (outInter[0] > outInter[1]) {
            const Standard_Real aTmp = outInter[0];
            outInter[0] = outInter[1];
            outInter[1] = aTmp;
          }
          break;
        }
      }
    }
  }
  return nInter;
}
