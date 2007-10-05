// File:      LineConn2d_PathIterator.cpp
// Created:   16.08.05 10:38:26
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#include <LineConn2dAfx.h>

#include <LineConn2d_PathIterator.h>

//=======================================================================
//function : LineConn2d_PathIterator()
//purpose  : Constructor
//=======================================================================

LineConn2d_PathIterator::LineConn2d_PathIterator
                        (const LineConn2d_Path&                   theRoot,
                         const Handle(NCollection_BaseAllocator)& theAlloc)
   : myStack (theAlloc),
     myLimit (1024)
{
  PathListIterator anIter (theRoot.myBranches);
  findNext (anIter);
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void LineConn2d_PathIterator::Next ()
{
  while (myStack.IsEmpty() == Standard_False) {
    PathListIterator& anIter = myStack.ChangeTop();
    anIter.Next();
    if (anIter.More()) {
      PathListIterator aNewIter (anIter.Value().myBranches);
      if (findNext (aNewIter))
        break;
    }
    myStack.Pop();
  }
}

//=======================================================================
//function : findNext
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_PathIterator::findNext
                        (LineConn2d_PathIterator::PathListIterator& theIter)
{
  Standard_Boolean aResult (Standard_False);
  if (myStack.Depth() < myLimit) {
    while (theIter.More()) {
      myStack.Push (theIter);
      theIter = PathListIterator (theIter.Value().myBranches);
    }
    aResult = Standard_True;
  }
  return aResult;
}
