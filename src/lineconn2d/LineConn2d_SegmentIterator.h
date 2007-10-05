// File:      LineConn2d_SegmentIterator.h
// Created:   04.08.05 22:47:27
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#ifndef LineConn2d_SegmentIterator_HeaderFile
#define LineConn2d_SegmentIterator_HeaderFile

#include <LineConn2d_Segment.h>
#include <LineConn2d_Object.h>

/**
 * Iterator that is invoked on LineConn2d_Object and returns contour
 * segments starting from the one between the last point and the first point
 * of the contour.
 */

class LineConn2d_SegmentIterator
{
 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty constructor
  inline LineConn2d_SegmentIterator ()
    : myIsSeg (Standard_False) {}

  /// Constructor
  inline LineConn2d_SegmentIterator (const LineConn2d_Object& theObject)
    : myListIter (theObject.PointsIterator()),
      myIsSeg    (Standard_False)
  {
    if (theObject.NbPoints() > 1) {
      mySeg.SetOrigin    (theObject.LastPoint());
      mySeg.SetExtremity (myListIter.Value());
      myIsSeg = Standard_True;
    }
  }

  /// Method More()
  inline Standard_Boolean More() const
  { return myIsSeg; }

  /// Iterator increment
  inline void Next ()
  {
    if (myIsSeg) {
      myListIter.Next();
      if (myListIter.More()) {
        mySeg.SetOrigin    (mySeg.Extremity());
        mySeg.SetExtremity (myListIter.Value());
      } else
        myIsSeg = Standard_False;
    }
  }

  /// Query the currently iterated segment
  inline const LineConn2d_Segment& Value () const
  { return mySeg; }

 private:
  // ---------- PRIVATE FIELDS ----------

  NCollection_List <gp_XY>::Iterator    myListIter;
  LineConn2d_Segment                    mySeg;
  Standard_Boolean                      myIsSeg;
};

#endif
