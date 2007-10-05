// File:      LineConn2d_Model.cpp
// Created:   03.08.05 21:05:54
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005

#include <LineConn2dAfx.h>

#include <LineConn2d_Model.h>
#include <LineConn2d_PathIterator.h>
#include <LineConn2d_IntervalBuffer.h>
#include <NCollection_UBTreeFiller.hxx>
#include <Standard_ProgramError.hxx>

#ifdef DEB
#include <stdio.h>
#endif

//=======================================================================
//function : LineConn2d_Model
//purpose  : Empty constructor
//=======================================================================

LineConn2d_Model::LineConn2d_Model ()
  : myAlloc       (new NCollection_IncAllocator()),
    myTree        (new NCollection_IncAllocator()),
    myTolerance   (10 * Precision::Confusion()),
    myPortLength  (0.),
    mySearchDepth (2.)
{
  myObjects.Append (LineConn2d_Object());
  myConnections.Append (LineConn2d_Connection());
  myPorts.Append (LineConn2d_Port());
}

//=======================================================================
//function : AddObject
//purpose  : 
//=======================================================================

Standard_Integer LineConn2d_Model::AddObject ()
{
  const Standard_Integer anInd = myObjects.Length();
  myObjects.Append (LineConn2d_Object (myAlloc));
  return anInd;
}

//=======================================================================
//function : AddPort
//purpose  : 
//=======================================================================

Standard_Integer LineConn2d_Model::AddPoort (const Standard_Integer iObj,
                                             const gp_XY&           thePnt,
                                             const gp_Dir2d&        theDir)
{
  Standard_Integer aResult (0);
  if (iObj > 0 && iObj < myObjects.Length()) {
    aResult = myPorts.Length();
    myPorts.Append (LineConn2d_Port (myObjects(iObj), thePnt, theDir));
  }
  return aResult;
}

//=======================================================================
//function : AddConnection
//purpose  : 
//=======================================================================

Standard_Integer LineConn2d_Model::AddConnection (const Standard_Integer iPort1,
                                                  const Standard_Integer iPort2)
{
  Standard_Integer aResult (0);
  const Standard_Integer nPorts = myPorts.Length();
  if (iPort1 > 0 && iPort1 < nPorts &&
      iPort2 > 0 && iPort2 < nPorts) {
    aResult = myConnections.Length();
    myConnections.Append (LineConn2d_Connection (myAlloc,
                                                 iPort1,
                                                 iPort2));
  }
  return aResult;
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void LineConn2d_Model::Update ()
{
  myTree.Clear();
  NCollection_UBTreeFiller <const LineConn2d_Object *, LineConn2d_Box>
    aTreeFiller(myTree);

  // skipping the 0th member; update the objects and fill the tree.
  NCollection_Vector <LineConn2d_Object>::Iterator anIter (myObjects);
  for (anIter.Next(); anIter.More(); anIter.Next()) {
    LineConn2d_Object& anObj = anIter.ChangeValue();
    anObj.Update();
    aTreeFiller.Add (&anObj, anObj.Box());
  }
  aTreeFiller.Fill();

  _PortLen = LineConn2d::IsSmall (myPortLength)? 2 * myTolerance : myPortLength;
  myTreeBox = myTree.Root().Bnd();
  // Update the box with all ports in the model
  NCollection_Vector <LineConn2d_Port>::Iterator anIterP (myPorts);
  for (anIterP.Next(); anIterP.More(); anIterP.Next())
    myTreeBox.Add (anIterP.Value().Location().XY() +
                   anIterP.Value().Direction().XY() * _PortLen);
  myTreeBox.Enlarge (myTolerance + 10 * Precision::Confusion());
}

//=======================================================================
//function : HalfPerimeter
//purpose  : 
//=======================================================================

Standard_Real LineConn2d_Model::HalfPerimeter () const
{
  const LineConn2d_Box& aBox = myTree.Root().Bnd();
  return 2 * (aBox.HSize().X() + aBox.HSize().Y());
}

//=======================================================================
//function : TempAlloc
//purpose  : 
//=======================================================================

const Handle(NCollection_BaseAllocator)& LineConn2d_Model::TempAlloc () const
{
  if (myTempAlloc.IsNull())
    ((Handle(NCollection_IncAllocator)&) myTempAlloc) =
      new NCollection_IncAllocator();
  return myTempAlloc;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_Model::Compute ()
{
  Update();

  const Standard_Integer nConn = myConnections.Length();
  for (Standard_Integer iConn = 1; iConn < nConn; iConn++) {
    LineConn2d_Connection& aConn = myConnections(iConn);
    const LineConn2d_Port& aPortSrc = myPorts (aConn.Port(0));
    const LineConn2d_Port& aPortTgt = myPorts (aConn.Port(1));
    // Check the ports if any of them is included into other objects
    if (checkPort (aPortSrc) == Standard_False ||
	checkPort (aPortTgt) == Standard_False)
      continue;

    _PntTgt     = gp_XY (aPortTgt.Location().XY() +
                         _PortLen * aPortTgt.Direction().XY());
    _SegTgtPort = LineConn2d_Segment (aPortTgt.Location().XY(), _PntTgt);

    // Create the root path (a short segment from the first port)
    LineConn2d_Path aPathRoot;
    LineConn2d::Orient anOrient =
      LineConn2d::Orientation (aPortSrc.Direction().XY());
    if (!LineConn2d_Path::createPath (aPathRoot, * this,
                                      aPortSrc.Location().XY(), anOrient,
                                      Standard_True))
      continue;
    if (aPathRoot.Delta().Modulus() < _PortLen)
      continue;
    aPathRoot.SetDelta (aPathRoot.Direction() * _PortLen);
    aPathRoot.Evaluate (_PntTgt);

    const Standard_Integer aMaxDepthIncr =
      Standard_Integer (mySearchDepth * LineConn2d_Path::PriceOfBreak());
    const Standard_Integer anUltimateDepth = aMaxDepthIncr * 3;
    Standard_Integer aMaxDepth = aMaxDepthIncr;

    // Create three branches near the beginning of the first Path.
    const gp_XY aPntExt = aPathRoot.Extremity();
    createPath (aPathRoot, aPntExt, -1);
    createPath (aPathRoot, aPntExt,  0);
    createPath (aPathRoot, aPntExt,  1);

    // Calculation loop.
    Standard_Boolean isNewPath (Standard_True);
    Standard_Integer aMinPrice (::IntegerLast());
    Standard_Integer aCurPrice (::IntegerLast());
    const LineConn2d_Path * aMinPricedPath = 0L;

    while (isNewPath) {
      isNewPath = Standard_False;
      // Evaluate the minimal price over all the leaves
      LineConn2d_PathIterator aPathIter (aPathRoot, TempAlloc());
      while (aPathIter.More()) {
        const LineConn2d_Path& aPath = aPathIter.Value();
        Standard_Integer aPathPrice = aPath.Price();
        // Correct the price for the terminating path if it makes an angle
        // with the port direction.
        if (aPath.IsComplete()) {
          if (LineConn2d::IsSmall (aPath.Direction() ^
                                   aPortTgt.Direction().XY()) == Standard_False)
            aPathPrice += LineConn2d_Path::PriceOfBreak();
        }
        if (aPathPrice < aCurPrice)
          aCurPrice = aPathPrice;
        if (aPath.IsComplete() && aPathPrice < aMinPrice) {
          aMinPrice = aPathPrice;
          aMinPricedPath = &aPath;
        }
        aPathIter.Next();
      }

      // Create new path elements
      aPathIter = LineConn2d_PathIterator (aPathRoot, TempAlloc());
      aPathIter.SetDepthLimit (20);
      while (aPathIter.More()) {
        LineConn2d_Path& aPath = aPathIter.Value();
        aPathIter.Next();
        const Standard_Integer aPrice = aPath.Price();
        if (aPrice < aMinPrice && aPrice < aCurPrice + aMaxDepth &&
            !aPath.IsComplete ())
        {
          // Create new paths
          gp_XY aProjPnt;
          if (aPath.ProjectPoint (aProjPnt, _PntTgt)) {
            // The target is projected on this path. Create a NEW SUB-PATH
            // IN THE PROJECTION point. If this path does not reach the PntTgt,
            // it is abandoned (not included).
            if ((aProjPnt - aPath.Origin()).Modulus() > myTolerance)
              if (createPath (aPath, aProjPnt, 100))
                isNewPath = Standard_True;
          }
          if (aPath.Limitation() >= 0) {
            // limitation front or right => create the NEW PATH TO THE LEFT
            if (createPath (aPath, aPath.Extremity(), -1))
              isNewPath = Standard_True;
          }
          if (aPath.Limitation() <= 0) {
            // limitation front or left => create the NEW PATH TO THE RIGHT
            if (createPath (aPath, aPath.Extremity(), 1))
              isNewPath = Standard_True;
          }
          // Create the outline on both sides, find the extrema
          gp_XY  aPathDir = aPath.Direction();
          NCollection_List<Standard_Real> lstOut (myTempAlloc);
          for (Standard_Integer iSide = -1; iSide < 2; iSide += 2) {
            pathOutline (lstOut, aPath, iSide);
            NCollection_List<Standard_Real>::Iterator anIterOut (lstOut);
            for (; anIterOut.More(); anIterOut.Next()) {
              const gp_XY aPnt = aPath.Point (anIterOut.Value());
              // Check against the point proximity to the path start or end
              if ((aPnt - aPath.Origin())    * aPathDir > myTolerance * 1.2 &&
                  (aPath.Extremity() - aPnt) * aPathDir > myTolerance * 1.2)
              {
                if (createPath (aPath, aPnt, iSide))
                  isNewPath = Standard_True;
              }
            }
          } // end for(.. iSide ..)
        }
      } // end while (aPathIter)
      if (isNewPath == Standard_False && aMinPricedPath == 0L) {
        if (aMaxDepth < anUltimateDepth) {
          aMaxDepth += aMaxDepthIncr;
          isNewPath = Standard_True;
        }
      }
    }
    // Copy the result to the current Connection
    const LineConn2d_Path * aPath = aMinPricedPath;
    if (aPath) {
      aConn.ClearSegments();
      gp_XY aPathPoint = aPath->Extremity();
      aConn.AddSegment (LineConn2d_Segment (aPathPoint,
                                            aPortTgt.Location().XY()));
      for (; aPath; aPath = &aPath -> Parent()) {
        aConn.PrependSegment (LineConn2d_Segment (aPath->Origin(), aPathPoint));
        aPathPoint = aPath->Origin();
      }
    }
    myTempAlloc.Nullify();      // clear the temporary data storage
  }
  return Standard_True;     // OK
}

//=======================================================================
//function : pathOutline
//purpose  : Find the maximal points on the model outline on the given
//           side of the current path
//=======================================================================

void LineConn2d_Model::pathOutline (NCollection_List<Standard_Real>& outList,
                                    const LineConn2d_Path            thePath,
                                    const Standard_Integer           theSide)
{
  const gp_XY aModelSize = 2 * Box().HSize();
  const Standard_Real aConf (Precision::Confusion());
  gp_XY aPntExtr = thePath.Extremity();
  Standard_Real aRelTol (0.);

  // Create the box for the transversal band covering the whole length
  // of the path
  LineConn2d_Box anOutBox;
  const gp_XY aCenterPath = (thePath.Origin() + aPntExtr) * 0.5;
  switch (thePath.Orientation()) {
    case LineConn2d::E:
      anOutBox = LineConn2d_Box (aCenterPath
                                 + gp_XY (0., -theSide * aModelSize.Y()),
                                 gp_XY (thePath.Delta().X() * 0.5 + aConf,
                                        aModelSize.Y()));
      aRelTol = myTolerance / thePath.Delta().X();
      break;
    case LineConn2d::W:
      anOutBox = LineConn2d_Box (aCenterPath
                                 + gp_XY (0., theSide * aModelSize.Y()),
                                 gp_XY (-thePath.Delta().X() * 0.5 + aConf,
                                        aModelSize.Y()));
      aRelTol = -myTolerance / thePath.Delta().X();
      break;
    case LineConn2d::N:
      anOutBox = LineConn2d_Box (aCenterPath
                                 + gp_XY (theSide * aModelSize.X(), 0.),
                                 gp_XY (aModelSize.X(),
                                        thePath.Delta().Y() * 0.5 + aConf));
      aRelTol = myTolerance / thePath.Delta().Y();
      break;
    case LineConn2d::S:
      anOutBox = LineConn2d_Box (aCenterPath
                                 + gp_XY (-theSide * aModelSize.X(), 0.),
                                 gp_XY (aModelSize.X(),
                                        -thePath.Delta().Y() * 0.5 + aConf));
      aRelTol = -myTolerance / thePath.Delta().Y();
      break;
  }
  
  // Create the interval buffer and add there the baseline
  LineConn2d_IntervalBuffer aZBuffer (myTempAlloc);
  aZBuffer.Add (LineConn2d_ZInterval (-::RealLast(), 0., 1.));

  // Select and iterate the boxes for model objects
  LineConn2d_BoxTreeSelector aSelBox (anOutBox);
  myTree.Select (aSelBox);
  NCollection_List<const LineConn2d_Object *>::Iterator
    aSelBoxIter (aSelBox.GetObjects());
  for (; aSelBoxIter.More(); aSelBoxIter.Next()) {
    const LineConn2d_Box& aBox = aSelBoxIter.Value() -> Box();
    // X[0] - coordinate of box start (normalized and relative to the path) 
    // X[1] - coordinate of box start (normalized and relative to the path) 
    // X[2] - Z-coordinate to be stored in Z-buffer
    //        (nearest to the path is the greatest)
    Standard_Real anX[3];
    switch (thePath.Orientation()) {
    case LineConn2d::E:
      anX[0] = (aBox.Center().X() - aBox.HSize().X()
                - thePath.Origin().X()) / thePath.Delta().X();
      anX[1] = (aBox.Center().X() + aBox.HSize().X()
                - thePath.Origin().X()) / thePath.Delta().X();
      anX[2] = theSide * aBox.Center().Y() + aBox.HSize().Y();
      break;
    case LineConn2d::N:
      anX[0] = (aBox.Center().Y() - aBox.HSize().Y()
                - thePath.Origin().Y()) / thePath.Delta().Y();
      anX[1] = (aBox.Center().Y() + aBox.HSize().Y()
                - thePath.Origin().Y()) / thePath.Delta().Y();
      anX[2] = -theSide * aBox.Center().X() + aBox.HSize().X();
      break;
    case LineConn2d::W:
      anX[0] = (aBox.Center().X() + aBox.HSize().X()
                 - thePath.Origin().X()) / thePath.Delta().X();
      anX[1] = (aBox.Center().X() - aBox.HSize().X()
                 - thePath.Origin().X()) / thePath.Delta().X();
      anX[2] = -theSide * aBox.Center().Y() + aBox.HSize().Y();
      break;
    case LineConn2d::S:
      anX[0] = (aBox.Center().Y() + aBox.HSize().Y()
                 - thePath.Origin().Y()) / thePath.Delta().Y();
      anX[1] = (aBox.Center().Y() - aBox.HSize().Y()
                  - thePath.Origin().Y()) / thePath.Delta().Y();
      anX[2] = theSide * aBox.Center().X() + aBox.HSize().X();
      break;
    default:
      continue;
    }
    // Assertion check
    if (aRelTol < Precision::Confusion() || anX[1] < anX[0])
      Standard_ProgramError::Raise (__FILE__);
    if (anX[0] < 0.) anX[0] = 0.;
    if (anX[1] > 1.) anX[1] = 1.;
    if (anX[1] - anX[0] < aRelTol)
      continue;

    aZBuffer.Add (LineConn2d_ZInterval (anX[2], anX[0], anX[1]));
  } // end iteration on the model selected boxes
  aZBuffer.CheckBuffer();
  aZBuffer.GetMinima (outList, aRelTol);

//      filebuf aFileBuf;
//      if (aFileBuf.open("LineConn2d_Path.trace", ios::out|ios::app))
//        aZBuffer.Dump (ostream(&aFileBuf), aRelTol);
//      aFileBuf.close();
}

//=======================================================================
//function : createPath
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_Model::createPath
                                (LineConn2d_Path&         thePath,
                                 const gp_XY&             theStart,
                                 const Standard_Integer   iSide) const
{
  // Compute the orientation for the new path, taking into account the Side
  LineConn2d::Orient anOrient;
  Standard_Boolean isSearchTerm (Standard_False);
  if (iSide > -2 && iSide < 2)
    anOrient = thePath.Orientation (iSide);
  else {
    // the orientation is not on the side, it is directed to the target
    isSearchTerm = Standard_True;
    anOrient = LineConn2d::Orientation (_PntTgt - theStart);
  }

  LineConn2d_Path aNewPath;
  const Standard_Boolean aResult =
    LineConn2d_Path::createPath (aNewPath, * this, theStart, anOrient,
                                 Standard_False);
  if (aResult) {
    gp_XY aProjPnt;
    if (aNewPath.ProjectPoint (aProjPnt, _PntTgt))
      if ((aProjPnt - _PntTgt).SquareModulus() < Precision::Confusion())
        aNewPath.SetExtremity (aProjPnt);
      else {
        if (isSearchTerm)
          return Standard_False;
        // Check that the created path does not intersect the target port
        // segment; if so, abandon the path.
        Standard_Real bidAlpha[2];
        if (_SegTgtPort.IsIntersect (aNewPath, bidAlpha))
          return Standard_False;
      }
    thePath.AddBranch (aNewPath, _PntTgt);

#ifdef DEBB
    static FILE * ff = NULL;
    static int ccc = 0;
    ff = fopen ("LineConn2d_Path.trace",ff ? "at" : "wt");

    const gp_XY anExtr = aNewPath.Extremity();
    const LineConn2d_Path * aPath = &thePath;
    Standard_Integer aDepth (0);
    for (; aPath; aPath = &aPath->Parent())
      aDepth++;
    const gp_XY anExP = thePath.Extremity();

    fprintf (ff, "2dprofile pp%d F %.7g %.7g TT %.7g %.7g W # %c%5d\n",
             ++ccc ,aNewPath.Origin().X(),aNewPath.Origin().Y(),
             anExtr.X(),anExtr.Y(), aNewPath.IsComplete() ? 'C' : '.',
             aNewPath.Price());
//  fprintf (ff,
//           "=> (%6.1f %6.1f) - (%6.1f %6.1f), Lim %3d, Depth %3d, Price%4d\n",
//           aNewPath.Origin().X(),aNewPath.Origin().Y(),anExtr.X(),anExtr.Y(),
//           aNewPath.Limitation(), aDepth, aNewPath.Price());

//  fprintf (ff,
//           "   [%6.1f %6.1f] - [%6.1f %6.1f], Lim %3d, Price%4d\n",
//           thePath.Origin().X(), thePath.Origin().Y(), anExP.X(),anExP.Y(),
//           thePath.Limitation(), thePath.Price());

    fclose (ff);
#endif
  }
  return aResult;
}

//=======================================================================
//function : checkPort
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_Model::checkPort
                        (const LineConn2d_Port& thePort) const
{
  Standard_Boolean aResult (Standard_True);
  const gp_XY aPnt [2] = {
    thePort.Location().XY(),
    thePort.Location().XY() + _PortLen * thePort.Direction().XY()
  };

  for (Standard_Integer i = 1; i < myObjects.Length(); i++) {
    const LineConn2d_Object& anObj = myObjects(i);
    if (&anObj == &thePort.Object())    // skip checking the self
      continue;
    if (anObj.IsOut (aPnt[1], myTolerance) == Standard_False) {
      aResult = Standard_False;
      break;
    }
  }
  return aResult;
}
