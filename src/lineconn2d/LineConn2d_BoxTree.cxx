// File:      LineConn2d_BoxTree.cpp
// Created:   12.08.05 15:19:55
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#include <LineConn2dAfx.h>

#include <LineConn2d_BoxTree.h>
#include <LineConn2d_Segment.h>

//=======================================================================
//function : LineConn2d_BoxTreeSelector
//purpose  : Constructor
//=======================================================================

LineConn2d_BoxTreeSelector::LineConn2d_BoxTreeSelector
                (const LineConn2d_Segment& theSeg)
{
  myBox.Add (theSeg.Origin());
  myBox.Add (theSeg.Extremity());
}


//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_BoxTreeSelector::Accept
                                (const LineConn2d_Object * const& theObj)
{
  myList.Append (theObj);
  return Standard_True;
}

//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_BoxTreeSelector::Reject
                                (const LineConn2d_Box& theBox) const
{
  return theBox.IsOut (myBox);
}

