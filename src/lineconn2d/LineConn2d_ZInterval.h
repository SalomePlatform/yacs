// File:      LineConn2d_ZInterval.hxx
// Created:   21.02.05 14:48:01
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#ifndef LineConn2d_ZInterval_HeaderFile
#define LineConn2d_ZInterval_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_Real.hxx>

///  Elementary interval used in LineConn2d_IntervalBuffer.

class LineConn2d_ZInterval 
{
 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty constructor
  inline LineConn2d_ZInterval ()
    : myZ (-::RealLast())
  { myX[0] = 0.; myX[1] = 0.; }

  /// Constructor
  inline LineConn2d_ZInterval (const Standard_Real theZ,
                           const Standard_Real theX0,
                           const Standard_Real theX1)
    : myZ (theZ)
  { myX[0] = theX0; myX[1] = theX1; }

  /// Query the Z value.
  inline Standard_Real  Z () const                         { return myZ; }

  /// Query the X extremity value.
  inline Standard_Real  X (const Standard_Integer i) const { return myX[i&0x1];}

  /// Comparison operator. Checks only coincidence of X extremities.
  inline Standard_Boolean operator == (const LineConn2d_ZInterval& theOther) const
  { return ((myX[0] - theOther.myX[0]) * (myX[0] - theOther.myX[0]) +
            (myX[1] - theOther.myX[1]) * (myX[1] - theOther.myX[1])) < 1e-6; }

  /// Set the attribute value
  inline void           SetAttribute (const long theAttrib)
  { myAttrib = theAttrib; }

  /// Query the attribute value
  inline long           Attribute    () const              { return myAttrib; }

 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Real         myZ;
  Standard_Real         myX[2];
  long                  myAttrib;

  friend class LineConn2d_IntervalBuffer;
};

#endif
