// File:      LineConn2d_Segment.cpp
// Created:   04.08.05 23:23:39
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#include <LineConn2dAfx.h>

#include <LineConn2d_Segment.h>
#include <LineConn2d.h>

//=======================================================================
//function : Limit
//purpose  : 
//=======================================================================

void LineConn2d_Segment::Limit (const LineConn2d_Box& theBox)
{
  // to be implemented ...
}

//=======================================================================
//function : IsIntersect
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_Segment::IsIntersect
                                (const LineConn2d_Segment& theSegOther,
                                 Standard_Real             outAlpha[2]) const
{
  Standard_Boolean aResult (Standard_False);
  const gp_XY aLink = theSegOther.myOrigin - myOrigin;
  const Standard_Real aProd[3] = {
    aLink       ^ theSegOther.myDelta,
    aLink       ^ myDelta,
    myDelta     ^ theSegOther.myDelta
  };
  if (LineConn2d::IsSmall(aProd[2]) == Standard_False) { // test parallel
    const Standard_Real aCoeff [2] = {
      aProd[0] / aProd[2],
      aProd[1] / aProd[2]
    };
    if (LineConn2d::IsInside (aCoeff[0], 0., 1.) &&
        LineConn2d::IsInside (aCoeff[1], 0., 1.)) {
      aResult = Standard_True;          // intersection found
      outAlpha[0] = aCoeff[0];
      outAlpha[1] = aCoeff[1];
    }
  }
  return aResult;
}
