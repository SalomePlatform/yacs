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

#include "commands.hxx"
#include "guiContext.hxx"
#include "Proc.hxx"
#include "Catalog.hxx"
#include "RuntimeSALOME.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"
#include <sstream>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

int Invocator::_ctr = 0;

Command::Command()
{
  _subCommands.clear();
  _normalReverse = true;
}

//! the command is executed a first time after its registration for undo redo, then on redo
/*!
 *  Execute may induce subcommands to be registered during its execution.
 *  Subcommands must be deleted before redo, to be recreated by redo.
 *  CommandDestroy is a special case, where subcommands are registered before Execute, and
 *  undo action calls executeSubOnly instead of reverse. Subcommands pf CommandDestroy must 
 *  not be deleted.
 */
bool Command::execute()
{
  if ( !GuiContext::getCurrent() || !GuiContext::getCurrent()->getInvoc() )
    return false;

  bool commandInProgress = !GuiContext::getCurrent()->getInvoc()->isSpecialReverse();
  if (commandInProgress)
    GuiContext::getCurrent()->getInvoc()->_commandsInProgress.push_back(this);

  bool ret=true;
  if (GuiContext::getCurrent()->getInvoc()->_isRedo && _normalReverse) 
    {
      for (int i=0; i<_subCommands.size(); i++)
        delete _subCommands[i];
      _subCommands.clear();
    }
  ret = localExecute();
  if (ret) GuiContext::getCurrent()->setNotSaved(true);

  if (commandInProgress)
    GuiContext::getCurrent()->getInvoc()->_commandsInProgress.pop_back();
  return ret;
}

//! Reverse action: undo
/*! Recursive reverse of sub commands then local action
 *  @param isNormal: true for ordinary command, false for command Destroy
 */
bool Command::reverse(bool isNormal)
{
  bool ret=true;
  if (! _subCommands.empty())
    {
      for (int i=0; i<_subCommands.size(); i++)
        {
          ret =_subCommands[i]->reverse(isNormal);
          if (!ret) break;
        }
    }
  if (ret)
    {
      if (isNormal) // true for all commands but commandDestroy
        ret = localReverse();
      else
        ret = localExecute(); // subCommand of command Destroy 
    }
  return ret;
}

//! execute only sub commands in a reverse action
/*!
 * for undo of commandDestroy, only sub commands of commandDestroy must be executed,
 * via subcommand->reverse(false)
 */
bool Command::executeSubOnly()
{
  bool ret=true;
  if (! _subCommands.empty())
    {
      for (int i=0; i<_subCommands.size(); i++)
        {
          ret =_subCommands[i]->reverse(false);
          if (!ret) break;
        }
    }
  return ret;
}

std::string Command::dump()
{
  return "=== generic command dump ===";
}

std::string Command::recursiveDump(int level)
{
  string prefix = "";
  for (int i=0; i<level; i++) prefix += "  ";
  string ret = prefix + dump() + "\n";
  for (int i=0; i<_subCommands.size(); i++)
    ret += _subCommands[i]->recursiveDump(level+1);
  return ret;
}

void Command::addSubCommand(Command* command)
{
  _subCommands.push_back(command); 
}





Invocator::Invocator()
{
  _commandsDone.clear();
  _commandsUndone.clear();
  _commandsInProgress.clear();
  _isRedo = false;
  _isUndo = false;
  _specialReverse = false;
  _undoCata = new YACS::ENGINE::Catalog("undoCata");
  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  _undoProc= runTime->createProc("undoProc");
}

void Invocator::add(Command* command)
{
  DEBTRACE("Invocator::add");
  if(_isUndo)
    return;
  if (GuiContext::getCurrent()->getInvoc()->isSpecialReverse())
    {
      delete command;
      return;
    }
  if (_commandsInProgress.empty())
    {
      _commandsDone.push_back(command); // --- to do after execute ok
      _commandsUndone.clear();           // --- undone stack is no more relevant
    }
  else
    {
      DEBTRACE("addSubCommand");
      _commandsInProgress.back()->addSubCommand(command);
    }
}

bool Invocator::undo()
{
  DEBTRACE("Invocator::undo");
  bool ret = true;

  {
    stringstream ret1;
    ret1 << "_commandsDone" << endl;
    for (int i=0; i<_commandsDone.size(); i++)
      ret1 << i << _commandsDone[i]->recursiveDump(2);
    DEBTRACE(ret1.str());
    stringstream ret2;
    ret2 << "_commandsUndone" << endl;
    for (int i=0; i<_commandsUndone.size(); i++)
      ret2 << i << _commandsUndone[i]->recursiveDump(2);
    DEBTRACE(ret2.str());
  }

  if (! _commandsDone.empty())
    {
      bool isNormal = _commandsDone.back()->isNormalReverse();
      _specialReverse = !isNormal;
      _isUndo=true;
      if (isNormal)
        ret = _commandsDone.back()->reverse(isNormal);
      else
        ret = _commandsDone.back()->executeSubOnly();
      _isUndo=false;
      _commandsUndone.push_back(_commandsDone.back());
      _commandsDone.pop_back();
      _specialReverse = false;
    }

  {
    stringstream ret1;
    ret1 << "_commandsDone" << endl;
    for (int i=0; i<_commandsDone.size(); i++)
      ret1 << i << _commandsDone[i]->recursiveDump(2);
    DEBTRACE(ret1.str());
    stringstream ret2;
    ret2 << "_commandsUndone" << endl;
    for (int i=0; i<_commandsUndone.size(); i++)
      ret2 << i << _commandsUndone[i]->recursiveDump(2);
    DEBTRACE(ret2.str());
  }

  return ret;
}

bool Invocator::redo()
{
  DEBTRACE("Invocator::redo");
  bool ret = true;

  {
    stringstream ret1;
    ret1 << "_commandsDone" << endl;
    for (int i=0; i<_commandsDone.size(); i++)
      ret1 << i << _commandsDone[i]->recursiveDump(2);
    DEBTRACE(ret1.str());
    stringstream ret2;
    ret2 << "_commandsUndone" << endl;
    for (int i=0; i<_commandsUndone.size(); i++)
      ret2 << i << _commandsUndone[i]->recursiveDump(2);
    DEBTRACE(ret2.str());
  }

  if (! _commandsUndone.empty())
    {
      _isRedo=true;
      ret = _commandsUndone.back()->execute();
      _isRedo=false;
      _commandsDone.push_back(_commandsUndone.back());
      _commandsUndone.pop_back();
    }

  {
    stringstream ret1;
    ret1 << "_commandsDone" << endl;
    for (int i=0; i<_commandsDone.size(); i++)
      ret1 << i << _commandsDone[i]->recursiveDump(2);
    DEBTRACE(ret1.str());
    stringstream ret2;
    ret2 << "_commandsUndone" << endl;
    for (int i=0; i<_commandsUndone.size(); i++)
      ret2 << i << _commandsUndone[i]->recursiveDump(2);
    DEBTRACE(ret2.str());
  }

  return ret;
}

std::list<std::string> Invocator::getDone()
{
  list<string> listDone;
  listDone.clear();
  for (int i=0; i<_commandsDone.size(); i++)
    listDone.push_back(_commandsDone[i]->recursiveDump());
  return listDone;
}

std::list<std::string> Invocator::getUndone()
{
  list<string> listUndone;
  listUndone.clear();
  for (int i=0; i<_commandsUndone.size(); i++)
    listUndone.push_back(_commandsUndone[i]->recursiveDump());
  return listUndone;
}
