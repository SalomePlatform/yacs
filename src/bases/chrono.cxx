
#include "chrono.hxx"

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

cntStruct* counters::_ctrs = 0;
  
counters::counters(int nb)
{
  DEBTRACE("counters::counters(int nb)");
  _nbChrono = nb;
  _ctrs = new cntStruct[_nbChrono];

  for (int i=0; i< _nbChrono; i++)
    {
      _ctrs[i]._ctrNames = 0;
      _ctrs[i]._ctrLines = 0;
      _ctrs[i]._ctrOccur = 0;
      _ctrs[i]._ctrCumul = 0;
    }

  DEBTRACE("counters::counters()");
}

counters::~counters()
{
  stats();
}

void counters::stats()
{
  DEBTRACE("counters::stats()");
  for (int i=0; i < _nbChrono; i++)
    if (_ctrs[i]._ctrOccur)
      {
        DEBTRACE("Compteur[" << i << "]: "<< _ctrs[i]._ctrNames << "[" << _ctrs[i]._ctrLines << "]");
        DEBTRACE("  " << _ctrs[i]._ctrOccur);
        DEBTRACE("  " << _ctrs[i]._ctrCumul);
      }
}



chrono::chrono(int i) : _ctr(i), _run(true)
{
  //DEBTRACE("chrono::chrono " << _ctr << " " << _run);
  _start = clock();    
}

chrono::~chrono()
{
  if (_run) stop();
}

void chrono::stop()
{
  //DEBTRACE("chrono::stop " << _ctr << " " << _run);
  if (_run)
    {
      _run = false;
      _end = clock();
      double elapse = double(_end - _start)/double(CLOCKS_PER_SEC);
      counters::_ctrs[_ctr]._ctrOccur++;
      counters::_ctrs[_ctr]._ctrCumul += elapse;
    }
}
