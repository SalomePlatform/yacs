#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "Exception.hxx"
#include "Executor.hxx"
#include "parsers.hxx"
#include "Thread.hxx"

#include <iostream>
#include <fstream>

using YACS::YACSLoader;
using YACS::ENGINE::RuntimeSALOME;
using YACS::ENGINE::Executor;
using YACS::ENGINE::Proc;
using YACS::BASES::Thread;
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

void *executorFunc(void *arg)
{
  void **argT=(void **)arg;
  Executor *myExec = (Executor *)argT[0];
  Proc *myProc = (Proc *)argT[1];
  myExec->RunB(myProc, 2);
}

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " schema.xml" << endl;
    return 1;
  }

  RuntimeSALOME::setRuntime();

  YACSLoader loader;
  Executor *executor = new Executor();
 
  try
    {
      Proc* p=loader.load(argv[1]);
      std::ofstream f("toto");
      p->writeDot(f);
      f.close();

      executor->setExecMode(YACS::STEPBYSTEP);
      void **args=new void *[2];
      args[0]=(void *)executor;
      args[1]=(void *)p;
      Thread* execThread = new Thread(executorFunc,args);
      char com;
      cerr << "enter a char to start" << endl;
      cin >> com;
      //executor->wakeUp();
      while (executor->isNotFinished())
        {
          YACS::ExecutorState executorState = executor->getExecutorState();
          cerr << "executorState: " << executorState << endl;
          cerr << "command display=d step=s : ";
          cin >> com;
          switch (com)
            {
            case 's':
              {
                executor->setExecMode(YACS::STEPBYSTEP);
                bool res = executor->resumeCurrentBreakPoint();
                cerr << "resumeCurrentBreakPoint(): " << res << endl;
                break;
              }
            case 'd':
              {
                executor->displayDot(p);
                break;
              }
            default:
              {
                cerr << "commande inconnue" << endl;
              }
            }
        }
      execThread->join();
      std::ofstream g("titi");
      p->writeDot(g);
      g.close();
      delete executor;
      return 0;
  }
  catch (YACS::Exception& e)
    {
      DEBTRACE("exception YACS levee " << e.what());
      return 1;
    }
  catch (const std::ios_base::failure&)
    {
      DEBTRACE("io failure");
      return 1;
    }
  catch(CORBA::SystemException& ex)
    {
      DEBTRACE("...");
      CORBA::Any tmp;
      tmp <<= ex;
      CORBA::TypeCode_var tc = tmp.type();
      const char *p = tc->name();
      if ( *p != '\0' )
        {
          DEBTRACE("Caught a CORBA::SystemException. " <<p);
        }
      else
        {
          DEBTRACE("Caught a CORBA::SystemException. " << tc->id());
        }
    return 1;
    }
  catch(omniORB::fatalException& fe)
    {
      DEBTRACE("Caught omniORB::fatalException: file: "<<fe.file()<<" line: "<<fe.line()<<" msg: "<<fe.errmsg());
      return 1;
    }
  catch(...)
    {
      DEBTRACE("Caught unknown exception.");
      return 1;
    }
}

