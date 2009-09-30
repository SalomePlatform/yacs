#ifndef _CHRONO_HXX_
#define _CHRONO_HXX_

#include "YACSBasesExport.hxx"

#include <vector>
#include <string>
#include <iostream>
#include <ctime>

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
