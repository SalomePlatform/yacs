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

#ifndef _CHRONO_HXX_
#define _CHRONO_HXX_

#include "YACSBasesExport.hxx"

#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#ifdef WIN32
#else
#include <sys/time.h>
#endif

typedef struct acnt
{
  char*  _ctrNames;
  int    _ctrLines;
  int    _ctrOccur;
  double _ctrCumul;
} cntStruct;

class YACSBASES_EXPORT counters
{
public:
  static cntStruct *_ctrs;
  counters(int nb);
  ~counters();
  void stats();
protected:
  int _nbChrono;
};

class YACSBASES_EXPORT chrono
{
public:
  chrono(int i);
  ~chrono();
  void stop();
protected:
  bool _run;
  int _ctr;
  clock_t _start, _end;
};

#ifndef START_TIMING
static long tcount=0;
static long cumul;
#ifdef WIN32
static time_t tv;
#else
static timeval tv;
#endif
#define START_TIMING gettimeofday(&tv,0);long tt0=tv.tv_usec+tv.tv_sec*1000000;
#define END_TIMING(NUMBER) \
  tcount=tcount+1;gettimeofday(&tv,0);cumul=cumul+tv.tv_usec+tv.tv_sec*1000000 -tt0; \
  if(tcount==NUMBER){ std::cerr << __FILE__<<":"<<__LINE__<<" temps CPU(mus): "<<cumul<< std::endl; tcount=0 ;cumul=0; }
#endif

#ifdef CHRONODEF
#define CHRONO(i) counters::_ctrs[i]._ctrNames = (char *)__FILE__; \
  counters::_ctrs[i]._ctrLines = __LINE__; \
  chrono aChrono##i(i);

#define CHRONOSTOP(i) aChrono##i.stop();

#else  // CHRONODEF

#define CHRONO(i)
#define CHRONOSTOP(i)

#endif // CHRONODEF

#endif // _CHRONO_HXX_
