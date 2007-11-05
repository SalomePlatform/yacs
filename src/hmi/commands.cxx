
#include "commands.hxx"

#define _DEVDEBUG_
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

