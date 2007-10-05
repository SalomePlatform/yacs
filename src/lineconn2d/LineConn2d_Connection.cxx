// File:      LineConn2d_Connection.cpp
// Created:   03.08.05 21:55:54
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#include <LineConn2dAfx.h>

#include <LineConn2d_Connection.h>

//=======================================================================
//function : LineConn2d_Connection()
//purpose  : Constructor
//=======================================================================

LineConn2d_Connection::LineConn2d_Connection
                        (const Handle(NCollection_BaseAllocator)& theAlloc,
                         const Standard_Integer                   thePort1,
                         const Standard_Integer                   thePort2)
   : mySegments (theAlloc)
{
  myPort[0] = thePort1;
  myPort[1] = thePort2;
}
