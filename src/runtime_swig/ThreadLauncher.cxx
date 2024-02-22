// Copyright (C) 2024  CEA, EDF
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

#include "ThreadLauncher.hxx"
#include "VisitorSalomeSaveState.hxx"

#include "PythonCppUtils.hxx"

#include "Basics_Utils.hxx"

#include "Python.h"

using namespace YACS::ENGINE;

ThreadDumpState::~ThreadDumpState()
{
  join();
}

void ThreadDumpState::run()
{
  AutoGIL agil;
  AutoPyYielder yld;
  YACS::StatesForNode state = _proc->getEffectiveState();
  while((state != YACS::DONE) && (state != YACS::LOADFAILED) && (state != YACS::EXECFAILED) && (state != YACS::INTERNALERR) && (state != YACS::DISABLED) && (state != YACS::FAILED) && (state != YACS::ERROR))
  {
#ifdef WIN32
    Sleep(_nb_seconds);
#else 
    sleep(_nb_seconds);
#endif
    std::string cmd = "touch " + _lock_file;
    system(cmd.c_str());
    schemaSaveStateUnsafe(_proc,_dump_file);
    cmd = "rm -f " + _lock_file;
    system(cmd.c_str());
    state = _proc->getEffectiveState();
  }
}

void ThreadDumpState::start()
{
  _my_thread.reset( new std::thread([this]{
    this->run();
  }) );
}

void ThreadDumpState::join()
{
  if(_my_thread)
  {
    AutoGIL agil;
    AutoPyYielder yld;
    _my_thread->join();
    _my_thread.reset( nullptr );
  }
}
