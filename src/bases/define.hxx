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
      UNDEFINED    = -1,
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
      PAUSE        = 110,
      INTERNALERR  = 666,
      DISABLED     = 777,
      FAILED       = 888,
      ERROR        = 999
    } StatesForNode;

  typedef enum
    {
      NOEVENT      = 200,
      START        = 201,
      FINISH       = 202,
      ABORT        = 203
    } Event;

  typedef enum
    {
      NOTYETINITIALIZED = 300,
      INITIALISED       = 301,
      RUNNING           = 302,
      WAITINGTASKS      = 303,
      PAUSED            = 304,
      FINISHED          = 305,
      STOPPED           = 306
    } ExecutorState;

  typedef enum
    {
      CONTINUE        = 0,
      STEPBYSTEP      = 1,
      STOPBEFORENODES = 2
    } ExecutionMode;
}
#endif
