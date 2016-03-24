// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
      INVALID      =  99,
      READY        = 100,
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
      TORECONNECT  = 111,
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
