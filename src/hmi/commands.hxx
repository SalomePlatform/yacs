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

#ifndef _COMMANDS_HXX_
#define _COMMANDS_HXX_

#include "HMIExport.hxx"
#include <list>
#include <string>
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
    class Catalog;
  }

  namespace HMI
  {
    class Command
    {
    public:
      Command();
      virtual bool execute();
      virtual bool reverse(bool isNormal=true);
      virtual bool executeSubOnly();
      virtual std::string dump();
      std::string recursiveDump(int level =0);
      void addSubCommand(Command* command);
      bool isNormalReverse() { return _normalReverse; };
    protected:
      virtual bool localExecute() = 0;
      virtual bool localReverse() = 0;
      std::vector<Command*> _subCommands;
      bool _normalReverse;
    };
    
    class HMI_EXPORT Invocator
    {
      friend class Command;
    public:
      Invocator();
      void add(Command* command);
      bool undo();
      bool redo();
      std::list<std::string> getDone();
      std::list<std::string> getUndone();
      YACS::ENGINE::Proc *getUndoProc() { return _undoProc; };
      YACS::ENGINE::Catalog *getUndoCata() { return _undoCata; };
      bool isSpecialReverse() { return _specialReverse; };
      static int _ctr;

    protected:
      std::vector<Command*> _commandsDone;
      std::vector<Command*> _commandsUndone;
      std::vector<Command*> _commandsInProgress;
      YACS::ENGINE::Proc *_undoProc;
      YACS::ENGINE::Catalog *_undoCata;
      bool _isRedo;
      bool _isUndo;
      bool _specialReverse;
    };
  }
}
#endif
