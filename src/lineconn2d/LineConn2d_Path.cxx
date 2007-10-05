// File:      LineConn2d_Path.cpp
// Created:   11.08.05 15:16:13
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#include <LineConn2dAfx.h>

#include <LineConn2d_Path.h>
#include <LineConn2d_Model.h>
#include <LineConn2d_SegmentIterator.h>

//=======================================================================
//function : LineConn2d_Path
//purpose  : Empty constructor
//=======================================================================

LineConn2d_Path::LineConn2d_Path ()
   : myModel            (0L),
     myParent           (0L),
     myPriceLen         (0),
     myPriceDist        (0),
     myLimitation       (0),
     myOrient           (LineConn2d::Indefinite),
     myIsComplete       (Standard_False) {}

//=======================================================================
//function : LineConn2d_Path()
//purpose  : Constructor
//=======================================================================

LineConn2d_Path::LineConn2d_Path (const gp_XY&             theStart,
                                  const gp_XY&             theEnd,
                                  const LineConn2d_Model&  theModel,
                                  const LineConn2d::Orient theOri)
  : LineConn2d_Segment  (theStart, theEnd),
    myModel             (&theModel),
    myParent            (0L),
    myBranches          (theModel.TempAlloc()),
    myPriceLen          (0),
    myPriceDist         (0),
    myLimitation        (0),
    myOrient            (theOri),
    myIsComplete        (Standard_False)
{
  if (theOri == LineConn2d::Indefinite)
    myOrient = LineConn2d::Orientation (Delta());
}

//=======================================================================
//function : Width
//purpose  : 
//=======================================================================

Standard_Real LineConn2d_Path::Width () const
{
  return (myModel ? myModel -> Tolerance() : 0.);
}

//=======================================================================
//function : AddBranch
//purpose  : 
//=======================================================================

void LineConn2d_Path::AddBranch (LineConn2d_Path& theBranch,
                                 const gp_XY&     theTgt)
{
  theBranch.myParent = this;
  theBranch.Evaluate (theTgt);
  myBranches.Append (theBranch);
}

//=======================================================================
//function : Evaluate
//purpose  : 
//=======================================================================

Standard_Integer LineConn2d_Path::Evaluate (const gp_XY& theTarget)
{
  const Standard_Real aHalfPerimeterNorm  (myModel->HalfPerimeter() / 1000.);
  const Standard_Real aHalfPerimeterNorm1 (aHalfPerimeterNorm * 10.);

  // Calculate the price due to the distance
  const gp_XY aDistVector (theTarget - Extremity());
  // ... distance from the extremity to the target
  const Standard_Real aDist =
    LineConn2d_FABS(aDistVector.X()) + LineConn2d_FABS(aDistVector.Y());
  // ... deflection from the shortest way
  gp_XY aV = theTarget - Origin();
  Standard_Real aDist1 = (Delta() ^ aV);
  aDist1 = ::sqrt((aDist1 * aDist1) / (aV * aV));

  if (aDist < Precision::Confusion())
    myIsComplete = Standard_True;

  myPriceDist = Standard_Integer ((aDist + 0.2 * aDist1) / aHalfPerimeterNorm);

  // Calculate the price due to length.
  myPriceLen = Standard_Integer (Delta().Modulus() / aHalfPerimeterNorm1);

  // Take into account the additive from the parent
  if (myParent != 0L) {
    if (myParent -> Orientation() != myOrient)
      myPriceLen += PriceOfBreak();
    myPriceLen += (myParent -> myPriceLen -
                   (Origin() - myParent->Extremity()).Modulus()
                      / aHalfPerimeterNorm1);
  }
  return myPriceLen + myPriceDist;
}

//=======================================================================
//function : createPath
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_Path::createPath
                                        (LineConn2d_Path&          thePath,
                                         const LineConn2d_Model&   theModel,
                                         const gp_XY&              theStart,
                                         const LineConn2d::Orient  theOrient,
                                         const Standard_Boolean    isRoot)
{
  Standard_Boolean aResult (Standard_False);
  const Standard_Real aTol = theModel.Tolerance();
  // Create a long box extending in the direction theOrient (till the boundary)
  LineConn2d_Box        aPathBox;
  const LineConn2d_Box& aModelBox = theModel.Box();
  Standard_Real         aPathBoxLen (0.);
  gp_XY                 aPathDir (0., 0.);
  // Four segments forming the PathBox:
  // 0 - the transversal segment containing theStart  
  // 1 - left side
  // 2 - the front
  // 3 - the right side
  LineConn2d_Segment aSeg[4];
  switch (theOrient) {
  case LineConn2d::E:
    aPathBoxLen = aModelBox.Center().X() + aModelBox.HSize().X() - theStart.X();
    aPathBox = LineConn2d_Box (theStart + gp_XY (0.5 * aPathBoxLen, 0.),
                               gp_XY (0.5 * aPathBoxLen, aTol));
    aPathDir.SetX (1.);
    aSeg[0] = LineConn2d_Segment (theStart + gp_XY (0., -aTol),
                                  theStart + gp_XY (0., aTol));
    aSeg[1] = LineConn2d_Segment (theStart + gp_XY (0., aTol),
                                  theStart + gp_XY (aPathBoxLen, aTol));
    aSeg[2] = LineConn2d_Segment (theStart + gp_XY (aPathBoxLen, aTol),
                                  theStart + gp_XY (aPathBoxLen, -aTol));
    aSeg[3] = LineConn2d_Segment (theStart + gp_XY (0., -aTol),
                                  theStart + gp_XY (aPathBoxLen, -aTol));
    break;
  case LineConn2d::N:
    aPathBoxLen = aModelBox.Center().Y() + aModelBox.HSize().Y() - theStart.Y();
    aPathBox = LineConn2d_Box (theStart + gp_XY (0., 0.5 * aPathBoxLen),
                               gp_XY (aTol, 0.5 * aPathBoxLen));
    aPathDir.SetY (1.);
    aSeg[0] = LineConn2d_Segment (theStart + gp_XY (aTol, 0.),
                                  theStart + gp_XY (-aTol, 0.));
    aSeg[1] = LineConn2d_Segment (theStart + gp_XY (-aTol, 0),
                                  theStart + gp_XY (-aTol, aPathBoxLen));
    aSeg[2] = LineConn2d_Segment (theStart + gp_XY (-aTol, aPathBoxLen),
                                  theStart + gp_XY (aTol, aPathBoxLen));
    aSeg[3] = LineConn2d_Segment (theStart + gp_XY (aTol, 0.),
                                  theStart + gp_XY (aTol, aPathBoxLen));
    break;
  case LineConn2d::W:
    aPathBoxLen= -aModelBox.Center().X() + aModelBox.HSize().X() + theStart.X();
    aPathBox = LineConn2d_Box (theStart - gp_XY (0.5 * aPathBoxLen, 0.),
                               gp_XY (0.5 * aPathBoxLen, aTol));
    aPathDir.SetX (-1.);
    aSeg[0] = LineConn2d_Segment (theStart - gp_XY (0., -aTol),
                                  theStart - gp_XY (0., aTol));
    aSeg[1] = LineConn2d_Segment (theStart - gp_XY (0., aTol),
                                  theStart - gp_XY (aPathBoxLen, aTol));
    aSeg[2] = LineConn2d_Segment (theStart - gp_XY (aPathBoxLen, aTol),
                                  theStart - gp_XY (aPathBoxLen, -aTol));
    aSeg[3] = LineConn2d_Segment (theStart - gp_XY (0., -aTol),
                                  theStart - gp_XY (aPathBoxLen, -aTol));
    break;
  case LineConn2d::S:
    aPathBoxLen= -aModelBox.Center().Y() + aModelBox.HSize().Y() + theStart.Y();
    aPathBox = LineConn2d_Box (theStart - gp_XY (0., 0.5 * aPathBoxLen),
                               gp_XY (aTol, 0.5 * aPathBoxLen));
    aPathDir.SetY (-1.);
    aSeg[0] = LineConn2d_Segment (theStart - gp_XY (aTol, 0.),
                                  theStart - gp_XY (-aTol, 0.));
    aSeg[1] = LineConn2d_Segment (theStart - gp_XY (-aTol, 0),
                                  theStart - gp_XY (-aTol, aPathBoxLen));
    aSeg[2] = LineConn2d_Segment (theStart - gp_XY (-aTol, aPathBoxLen),
                                  theStart - gp_XY (aTol, aPathBoxLen));
    aSeg[3] = LineConn2d_Segment (theStart - gp_XY (aTol, 0.),
                                  theStart - gp_XY (aTol, aPathBoxLen));
    break;
  }

  // Find intersections between aPathBox and the model Objects
  Standard_Real aMinInterDistance (aPathBoxLen);
  Standard_Integer anInterSide (0);

  aPathBox.Enlarge (4 * Precision::Confusion());
  LineConn2d_BoxTreeSelector aSelBox (aPathBox);
  theModel.GetTree().Select (aSelBox);
  NCollection_List<const LineConn2d_Object *>::Iterator
    aSelBoxIter (aSelBox.GetObjects());
  for (; aSelBoxIter.More(); aSelBoxIter.Next()) {
    LineConn2d_SegmentIterator anIter (* aSelBoxIter.Value());
    Standard_Real aMinDistance (aMinInterDistance);
    // Loop on the segments of the Object
    for (; anIter.More(); anIter.Next()) {
      const LineConn2d_Segment& aSegment = anIter.Value();
      Standard_Real anAlpha[2]; 
      // intersection box - segment
      if (aPathBox.IsOut (aSegment) == Standard_False) {
        if (aSeg[0].IsIntersect (aSegment, anAlpha)) // intersect with the rear
        {
          // cancel intersection test, the rear intersection means the
          // intersection with the own object
          aMinDistance = (isRoot ? ::RealLast() : 0.);
          break;
        }
        if (aSeg[1].IsIntersect (aSegment, anAlpha)) // intersect with the Left
        {
          const Standard_Real aDist = anAlpha[0] * aPathBoxLen;
          if (aDist < aMinDistance) {
            aMinDistance = aDist;
            anInterSide = -1;
          }
        }
        if (aSeg[3].IsIntersect (aSegment, anAlpha)) // intersect with the Right
        {
          const Standard_Real aDist = anAlpha[0] * aPathBoxLen;
          if (aDist < aMinDistance) {
            aMinDistance = aDist;
            anInterSide = 1;
          }
        }
        if (aPathBox.IsOut (aSegment.Origin()) == Standard_False) {
          const Standard_Real aDist = (aSegment.Origin() - theStart) * aPathDir;
          if (aDist < aMinDistance) {
            aMinDistance = aDist;
            anInterSide = 0;
          }
        }
      }
    }
    if (aMinDistance < aMinInterDistance)
      aMinInterDistance = aMinDistance;
  }

  // The PathLine segment is the medial segment of the PathBox prolongated
  // by the Tolerance (i.e., by the half-width of the PathBox). Find the
  // intersection with this PathLine. This intersection also delimits the Path.
  if (aMinInterDistance > aTol) {
    Standard_Real aPathLineDist = aTol + aMinInterDistance + 2 * Precision::Confusion();
    if (aPathLineDist > aPathBoxLen)
      aPathLineDist = aPathBoxLen;
    const LineConn2d_Segment aPathLine
      (theStart, theStart + aPathDir * aPathLineDist);

    LineConn2d_BoxTreeSelector aSelSeg (aPathLine);
    theModel.GetTree().Select (aSelSeg);
    NCollection_List<const LineConn2d_Object *>::Iterator
      aSelSegIter (aSelSeg.GetObjects());
    for (; aSelSegIter.More(); aSelSegIter.Next()) {
      LineConn2d_SegmentIterator anIter (* aSelSegIter.Value());
      for (; anIter.More(); anIter.Next()) {
        const LineConn2d_Segment& aSegment = anIter.Value();
        Standard_Real anAlpha[2]; 
        // intersection pathline - segment
        if (aPathLine.IsIntersect (aSegment, anAlpha)) {
          const Standard_Real aDist = anAlpha[0] * aPathLineDist - aTol;
          if (aDist < aMinInterDistance && aDist > 0)
            aMinInterDistance = aDist;
        }
      }
    }
  }

  if (aMinInterDistance > aTol) {
    thePath = LineConn2d_Path (theStart, theStart + aMinInterDistance*aPathDir,
                               theModel, theOrient);
    thePath.myLimitation = anInterSide;
    aResult = Standard_True;
  }
  return aResult;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

LineConn2d::Orient LineConn2d_Path::Orientation
                                        (const Standard_Integer theSide) const
{
  LineConn2d::Orient aResult (myOrient);
  switch (theSide) {
  case 1:
    ((Standard_Integer&)aResult) --;
    if (aResult == LineConn2d::Indefinite)
      aResult = LineConn2d::S;
    break;
  case -1:
    ((Standard_Integer&)aResult) ++;
    if (aResult == LineConn2d::Indefinite1)
      aResult = LineConn2d::E;
    break;
  }
  return aResult;
}


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_XY LineConn2d_Path::Direction (const Standard_Integer theSide) const
{
  gp_XY aResult (0., 0.);
  switch (theSide) {
  case -1:
    switch (myOrient) {
    case LineConn2d::E : aResult.SetY (1.); break;
    case LineConn2d::N : aResult.SetX (-1.); break;
    case LineConn2d::W : aResult.SetY (-1.); break;
    case LineConn2d::S : aResult.SetX (1.); break;
    }
    break;
  case 0:
    switch (myOrient) {
    case LineConn2d::E : aResult.SetX (1.); break;
    case LineConn2d::N : aResult.SetY (1.); break;
    case LineConn2d::W : aResult.SetX (-1.); break;
    case LineConn2d::S : aResult.SetY (-1.); break;
    }
    break;
  case 1:
    switch (myOrient) {
    case LineConn2d::E : aResult.SetY (-1.); break;
    case LineConn2d::N : aResult.SetX (1.); break;
    case LineConn2d::W : aResult.SetY (1.); break;
    case LineConn2d::S : aResult.SetX (-1.); break;
    }
    break;
  }
  return aResult;
}

//=======================================================================
//function : ProjectPoint
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_Path::ProjectPoint (gp_XY&         theProj,
                                                const gp_XY&   thePoint) const
{
  Standard_Boolean aResult (Standard_False);
  switch (myOrient) {
  case LineConn2d::E :
    if (LineConn2d::IsInside (thePoint.X(),
                              Origin().X(),
                              Extremity().X()))
    {
      aResult = Standard_True;
      theProj.SetCoord (thePoint.X(), Origin().Y());
    }
    break;
  case LineConn2d::W :
    if (LineConn2d::IsInside (thePoint.X(),
                              Extremity().X(),
                              Origin().X()))
    {
      aResult = Standard_True;
      theProj.SetCoord (thePoint.X(), Origin().Y());
    }
    break;
  case LineConn2d::N :
    if (LineConn2d::IsInside (thePoint.Y(),
                              Origin().Y(),
                              Extremity().Y()))
    {
      aResult = Standard_True;
      theProj.SetCoord (Origin().X(), thePoint.Y());
    }
    break;
  case LineConn2d::S :
    if (LineConn2d::IsInside (thePoint.Y(),
                              Extremity().Y(),
                              Origin().Y()))
    {
      aResult = Standard_True;
      theProj.SetCoord (Origin().X(), thePoint.Y());
    }
    break;
  }
  return aResult;
}
