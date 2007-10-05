// File:      LineConn2d_PathIterator.h
// Created:   16.08.05 10:31:29
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#ifndef LineConn2d_PathIterator_HeaderFile
#define LineConn2d_PathIterator_HeaderFile

#include <LineConn2d_Path.h>
#include <NCollection_Stack.hxx>

///  Tree iterator of Path structure 

class LineConn2d_PathIterator
{
 protected:
  typedef NCollection_List <LineConn2d_Path>::Iterator PathListIterator;

 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty constructor
  inline LineConn2d_PathIterator () {}

  /**
   * Constructor.
   * @param theRoot
   *   The top-level Path object, the iterator would explore its branches.
   * @param theAlloc
   *   Allocator for the internal Stack data structure.
   */
  Standard_EXPORT LineConn2d_PathIterator
                        (const LineConn2d_Path&                   theRoot,
                         const Handle(NCollection_BaseAllocator)& theAlloc);

  /**
   * Check if there is a Path object available.
   */
  inline Standard_Boolean       More            () const
  { return (myStack.IsEmpty() == Standard_False); }

  /**
   * Advance the iterator to the next item - the leaf of the next tree branch.
   */ 
  Standard_EXPORT void          Next            ();

  /**
   * Query the iterated value.
   */
  inline  LineConn2d_Path&      Value           () const
  { return myStack.Top().ChangeValue(); }

  /**
   * Set the maximal depth for the iteration stack. After that, any Path that
   * is more that theLimit level from the root, would be ignored.
   */
  inline void                   SetDepthLimit   (const Standard_Integer theLim)
  { myLimit = theLim; }

 private:
  // ---------- PRIVATE METHODS ----------
  Standard_Boolean              findNext        (PathListIterator&);

 private:
  // ---------- PRIVATE FIELDS ----------

  NCollection_Stack <PathListIterator>  myStack;
  Standard_Integer                      myLimit;  ///< maximal stack depth
};

#endif
