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

#pragma once

#include "YACSlibEngineExport.hxx"
#include "Proc.hxx"

#include <thread>
#include <memory>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT ThreadDumpState
    {
    public:
      ThreadDumpState(Proc *proc, int nbSeconds, const std::string& dumpFile, const std::string& lockFile);
      ~ThreadDumpState();
      void run();
      void start();
      void join();
    private:
      std::unique_ptr< std::thread > _my_thread;
      Proc *_proc;
      int _nb_seconds = 0;
      std::string _dump_file;
      std::string _lock_file;
    };
  }
}
