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
#include "commands.hxx"
#include "guiContext.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

Command::Command()
{
  _subCommands.clear();
}

bool Command::execute()
{
  bool ret=true;
  ret = localExecute();
  if (! _subCommands.empty())
    {
      list<Command*>::iterator it;
      for (it=_subCommands.begin(); it!=_subCommands.end(); it++)
        {
          if (!ret) break;
          ret = (*it)->execute();
        }
    }
  if (ret) GuiContext::getCurrent()->setNotSaved(true);
  return ret;
}

bool Command::reverse()
{
  bool ret=true;
  if (! _subCommands.empty())
    {
      list<Command*>::reverse_iterator it;
      for (it=_subCommands.rbegin(); it!=_subCommands.rend(); it++)
        {
          ret =(*it)->execute();
          if (!ret) break;
        }
    }
  if (ret) ret = localReverse();
  return ret;
}

void Command::addSubCommand(Command* command)
{
  _subCommands.push_back(command); 
}

Invocator::Invocator()
{
}

void Invocator::add(Command* command)
{
  _commandsDone.push(command); // to do after execute ok
}

bool Invocator::undo()
{
  bool ret = true;
  if (! _commandsDone.empty())
    {
      _commandsDone.pop();
      ret = _commandsDone.top()->reverse();
      _commandsUndone.push(_commandsDone.top());
    }
  return ret;
}

bool Invocator::redo()
{
  bool ret = true;
  if (! _commandsUndone.empty())
    {
      _commandsUndone.pop();
      ret = _commandsUndone.top()->execute();
      _commandsDone.push(_commandsUndone.top());
    }
  return ret;
}

