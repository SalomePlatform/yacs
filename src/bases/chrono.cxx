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

#include "chrono.hxx"

//#define _DEVDEBUG_
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
        std::cerr << "Compteur[" << i << "]: "<< _ctrs[i]._ctrNames << "[" << _ctrs[i]._ctrLines << "]" << std::endl;
        std::cerr << "  " << _ctrs[i]._ctrOccur << std::endl;
        std::cerr << "  " << _ctrs[i]._ctrCumul << std::endl;
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
