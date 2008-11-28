//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef _COMMANDS_HXX_
#define _COMMANDS_HXX_

#include <list>
#include <stack>
#include <string>

namespace YACS
{
  namespace HMI
  {
    class Command
    {
    public:
      Command();
      virtual bool execute();
      virtual bool reverse();
      //virtual std::string getCommandType() = 0; // utiliser un dynamic_cast plutot
      void addSubCommand(Command* command);
    protected:
      virtual bool localExecute() = 0;
      virtual bool localReverse() = 0;
      std::list<Command*> _subCommands;
    };
    
    class Invocator
    {
    public:
      Invocator();
      void add(Command* command);
      bool undo();
      bool redo();
    protected:
      std::stack<Command*> _commandsDone;
      std::stack<Command*> _commandsUndone;
    };
  }
}
#endif
