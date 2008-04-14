
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
