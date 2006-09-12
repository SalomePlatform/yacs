#ifndef __DEFINE_HXX__
#define __DEFINE_HXX__

namespace YACS
{
  //for algs of graphs and trees
  typedef enum
    {
      White = 10,
      Grey  = 11,
      Black = 12
    } Colour;

  typedef enum
    {
      Double = 41,
      Int = 42,
      String = 44,
      Bool = 45,
      //CorbaRef = 46,
      None = 49
    } DynType;

  typedef enum
    {
      SDouble = 71
    } StreamType;

  typedef enum
    {
      INITED       = 100,
      TOLOAD       = 101,
      LOADED       = 102,
      TOACTIVATE   = 103,
      ACTIVATED    = 104,
      DESACTIVATED = 105,
      DONE         = 106,
      SUSPENDED    = 107,
      LOADFAILED   = 108,
      EXECFAILED   = 109,
      PAUSE        = 110
    } StatesForNode;

  typedef enum
    {
      NOEVENT      = 200,
      START        = 201,
      FINISH       = 202,
      ABORT        = 203
    } Event;
}

#endif
