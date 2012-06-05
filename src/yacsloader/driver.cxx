// Copyright (C) 2006-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "yacsconfig.h"
#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "Logger.hxx"
#include "Exception.hxx"
#include "Executor.hxx"
#include "parsers.hxx"
#include "VisitorSaveState.hxx"
#include "VisitorSaveSalomeSchema.hxx"
#include "LoadState.hxx"
#include "Dispatcher.hxx"
#include "LinkInfo.hxx"

#ifdef SALOME_KERNEL
#include "SALOME_NamingService.hxx"
#include "SALOME_ModuleCatalog.hh"
#include "Basics_Utils.hxx"
#endif

#include <iostream>
#include <fstream>
#include <signal.h>

#ifdef WNT
#else
#include <argp.h>
#endif

using YACS::YACSLoader;
using namespace YACS::ENGINE;
using namespace std;


// --- use of glibc argp interface for parsing unix-style arguments

const char *argp_program_version ="driver V0.1";
const char *argp_program_bug_address ="<nepal@nepal.edf.fr>";
static char doc[] ="driver -- a SALOME YACS graph executor";
static char args_doc[] = "graph.xml";

#ifdef WNT
#else
static struct argp_option options[] =
  {
    {"display",         'd', "level", 0,                   "Display dot files: 0=never to 3=very often (default 0)"},
    {"verbose",         'v', 0,       0,                   "Produce verbose output" },
    {"stop-on-error",   's', 0,       0,                   "Stop on first error" },
    {"dump-on-error",   'e', "file",  OPTION_ARG_OPTIONAL, "Stop on first error and dump state"},
    {"dump-final",      'f', "file",  OPTION_ARG_OPTIONAL, "dump final state"},
    {"dump",            'g', "nbsec", OPTION_ARG_OPTIONAL, "dump state"},
    {"load-state",      'l', "file",  0,                   "Load State from a previous partial execution"},
    {"save-xml-schema", 'x', "file",  OPTION_ARG_OPTIONAL, "dump xml schema"},
    {"shutdown",        't', "level", 0,                   "Shutdown the schema: 0=no shutdown to 3=full shutdown (default 1)"},
    {"reset",           'r', "level", 0,                   "Reset the schema before execution: 0=nothing, 1=reset error nodes to ready state (default 0)"},
    {"kill-port",       'k', "port",  0,                   "Kill Salome application running on the specified port if the driver process is killed (with SIGINT or SIGTERM)"},
    { 0 }
  };
#endif

struct arguments
{
  char *args[1];
  int display;
  int verbose;
  int stop;
  char *dumpErrorFile;
  char *finalDump;
  int dump;
  char *xmlSchema;
  char *loadState;
  int shutdown;
  int reset;
  int killPort;
};

typedef struct {
  int nbsec;
  string dumpFile;
  string lockFile;
} thread_st;

#ifdef WNT
static int
#else
static error_t
#endif
parse_opt (int key, char *arg, struct argp_state *state)
{
#ifdef WNT
#else
  // Get the input argument from argp_parse, which we
  // know is a pointer to our arguments structure. 
  struct arguments *myArgs = (arguments*)state->input;
  
  switch (key)
    {
    case 'd':
      myArgs->display = atoi(arg);
      break;
    case 't':
      myArgs->shutdown = atoi(arg);
      break;
    case 'r':
      myArgs->reset = atoi(arg);
      break;
    case 'v':
      myArgs->verbose = 1;
      break;
    case 's':
      myArgs->stop = 1;
      break;
    case 'e':
      myArgs->stop = 1;
      if (arg)
        myArgs->dumpErrorFile = arg;
      else
        myArgs->dumpErrorFile = (char *)"dumpErrorState.xml";
      break;
    case 'f':
      if (arg)
        myArgs->finalDump = arg;
      else
        myArgs->finalDump = (char *)"finalDumpState.xml";
      break;      
    case 'g':
      if (arg)
        myArgs->dump = atoi(arg);
      else
        myArgs->dump = 60;
      break;      
    case 'l':
      myArgs->loadState = arg;
      break;
    case 'x':
      if (arg)
        myArgs->xmlSchema = arg;
      else
        myArgs->xmlSchema = (char *)"saveSchema.xml";
      break;      
    case 'k':
      myArgs->killPort = atoi(arg);
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num >=1) // Too many arguments.
        argp_usage (state);
      myArgs->args[state->arg_num] = arg;
      break;
      
    case ARGP_KEY_END:
      if (state->arg_num < 1) // Not enough arguments.
        argp_usage (state);
      break;
     
    default:
      return ARGP_ERR_UNKNOWN;
    }
#endif
  return 0;
}

// Our argp parser.
#ifdef WNT
#else
static struct argp argp = { options, parse_opt, args_doc, doc };
#endif

void timer(std::string msg)
{
#ifdef WNT
#else
  struct timeval tv;
  gettimeofday(&tv,NULL);
  long t=tv.tv_sec*1000+tv.tv_usec/1000;
  static long t0=t;
  gettimeofday(&tv,NULL);
  std::cerr << msg << tv.tv_sec*1000+tv.tv_usec/1000-t0 << " ms" << std::endl;
#endif
}

Proc* p=0;
static struct arguments myArgs;

void Handler(int theSigId)
{
  if(p)
    {
      p->cleanNodes();
      //if requested save state
      bool isFinalDump = (strlen(myArgs.finalDump) != 0);
      if (isFinalDump)
        {
          YACS::ENGINE::VisitorSaveState vst(p);
          vst.openFileDump(myArgs.finalDump);
          p->accept(&vst);
          vst.closeFileDump();
        }
      //if requested shutdown schema
      if(myArgs.shutdown < 999)
        {
          p->shutdown(myArgs.shutdown);
        }
    }
  if (myArgs.killPort)
    {
      ostringstream command;
      command << "killSalomeWithPort.py " << myArgs.killPort;
      int status = system(command.str().c_str());
      if (status == 0)
        cerr << "Salome application on port " << myArgs.killPort << " is killed" << endl;
      else
        cerr << "Error: Can't kill Salome application on port " << myArgs.killPort << endl;
    }
  _exit(1);
}

void * dumpState(void *arg)
{
  thread_st *st = (thread_st*)arg;
  YACS::StatesForNode state = p->getEffectiveState();
  while((state != YACS::DONE) && (state != YACS::LOADFAILED) && (state != YACS::EXECFAILED) && (state != YACS::INTERNALERR) && (state != YACS::DISABLED) && (state != YACS::FAILED) && (state != YACS::ERROR)){
#ifdef WIN32
    Sleep(st->nbsec);
#else 
    sleep(st->nbsec);
#endif
    string cmd = "touch " + st->lockFile;
    system(cmd.c_str());
    YACS::ENGINE::VisitorSaveState vst(p);
    vst.openFileDump(st->dumpFile);
    p->accept(&vst);
    vst.closeFileDump();
    cmd = "rm -f " + st->lockFile;
    system(cmd.c_str());
    state = p->getEffectiveState();
  }
  delete st;
  return NULL;
}

#ifndef WIN32
typedef void (*sighandler_t)(int);
sighandler_t setsig(int sig, sighandler_t handler)
{
  struct sigaction context, ocontext;
  context.sa_handler = handler;
  sigemptyset(&context.sa_mask);
  context.sa_flags = 0;
  if (sigaction(sig, &context, &ocontext) == -1)
    return SIG_ERR;
  return ocontext.sa_handler;
}
#endif


int main (int argc, char* argv[])
{
     
  // Default values.
  myArgs.display = 0;
  myArgs.verbose = 0;
  myArgs.stop = 0;
  myArgs.dumpErrorFile= (char *)"";
  myArgs.finalDump = (char *)"";
  myArgs.dump = 0;
  myArgs.loadState = (char *)"";
  myArgs.xmlSchema = (char *)"";
  myArgs.shutdown = 1;
  myArgs.reset = 0;
  myArgs.killPort = 0;

  // Parse our arguments; every option seen by parse_opt will be reflected in arguments.
#ifdef WNT
#else
  argp_parse (&argp, argc, argv, 0, 0, &myArgs);
  std::cerr << "graph = " << myArgs.args[0];
  std::cerr << " options: display=" << myArgs.display;
  std::cerr << " verbose="<<myArgs.verbose;
  std::cerr << " stop-on-error=" << myArgs.stop;
  std::cerr << " shutdown=" << myArgs.shutdown;
  std::cerr << " reset=" << myArgs.reset;
  if (myArgs.killPort)
    std::cerr << " kill-port=" << myArgs.killPort;
  if (myArgs.stop)
    std::cerr << " dumpErrorFile=" << myArgs.dumpErrorFile << std::endl;
  else
    std::cerr << std::endl;
#endif

#ifndef WNT
  setsig(SIGINT,&Handler);
  setsig(SIGTERM,&Handler);
#endif

  timer("Starting ");
  RuntimeSALOME::setRuntime();

  // Try to load the session catalog if it exists
  try
    {
      YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
      CORBA::ORB_ptr orb = runTime->getOrb();
      if (orb)
        {
          SALOME_NamingService namingService(orb);
          CORBA::Object_var obj = namingService.Resolve("/Kernel/ModulCatalog");
          SALOME_ModuleCatalog::ModuleCatalog_var aModuleCatalog = SALOME_ModuleCatalog::ModuleCatalog::_narrow(obj);
          if (! CORBA::is_nil(aModuleCatalog))
            {
              CORBA::String_var anIOR = orb->object_to_string( aModuleCatalog );
              YACS::ENGINE::Catalog* aCatalog = runTime->loadCatalog( "session", anIOR.in() );
              runTime->addCatalog(aCatalog);
            }
        }
    }
  catch(ServiceUnreachable& e)
    {
      //Naming service unreachable don't add catalog
    }

  YACSLoader loader;
  Executor executor;

  try
    {
      timer("Elapsed time before load: ");
      p=loader.load(myArgs.args[0]);
      if(p==0)
        {
          std::cerr << "The imported file is probably not a YACS schema file" << std::endl;
          return 1;
        }
      //Get the parser logger
      Logger* logger=p->getLogger("parser");
      //Print errors logged if any
      if(!logger->isEmpty())
        {
          std::cerr << "The imported file has errors" << std::endl;
          std::cerr << logger->getStr() << std::endl;
        }
      //Don't execute if there are errors
      if(logger->hasErrors())
        {
          if(!p->isValid())
            {
              std::string report=p->getErrorReport();
              std::cerr << "The schema is not valid and can not be executed" << std::endl;
              std::cerr << report << std::endl;
            }
          delete p;
          Runtime* r=YACS::ENGINE::getRuntime();
          Dispatcher* disp=Dispatcher::getDispatcher();
          r->fini();
          delete r;
          delete disp;
          return 1;
        }
      timer("Elapsed time after load: ");

      if(!p->isValid())
        {
          std::string report=p->getErrorReport();
          std::cerr << "The schema is not valid and can not be executed" << std::endl;
          std::cerr << report << std::endl;
          Runtime* r=YACS::ENGINE::getRuntime();
          Dispatcher* disp=Dispatcher::getDispatcher();
          r->fini();
          delete r;
          delete disp;
          return 1;
        }
      timer("Elapsed time after validation: ");

      // Check consistency
      LinkInfo info(LinkInfo::ALL_DONT_STOP);
      p->checkConsistency(info);
      if(info.areWarningsOrErrors())
        {
          std::cerr << "The schema is not consistent and can not be executed" << std::endl;
          std::cerr << info.getGlobalRepr() << std::endl;
          Runtime* r=YACS::ENGINE::getRuntime();
          Dispatcher* disp=Dispatcher::getDispatcher();
          r->fini();
          delete r;
          delete disp;
          return 1;
        }
      timer("Elapsed time after check consistency: ");

      //execution
      bool isXmlSchema = (strlen(myArgs.xmlSchema) != 0);
      if (isXmlSchema)
      {
        YACS::ENGINE::VisitorSaveSalomeSchema vss(p);
        vss.openFileSchema(myArgs.xmlSchema);
        p->accept(&vss);
        vss.closeFileSchema();
      }

      bool fromScratch = (strlen(myArgs.loadState) == 0);
      if (!fromScratch)
        {
          p->init();
          p->exUpdateState();
          stateParser* rootParser = new stateParser();
          stateLoader myStateLoader(rootParser, p);
          myStateLoader.parse(myArgs.loadState);
          if(myArgs.reset>0)
            {
              p->resetState(myArgs.reset);
              p->exUpdateState();
            }
        }

      if (myArgs.stop)
        if (strlen(myArgs.dumpErrorFile) >0)
          executor.setStopOnError(true, myArgs.dumpErrorFile);
        else
          executor.setStopOnError(false, myArgs.dumpErrorFile);

      if(myArgs.display>0)
        {
          std::ofstream f("toto");
          p->writeDot(f);
          f.close();
        }

      bool isDump = (myArgs.dump != 0);
      pthread_t th;
      if (isDump)
        {
          thread_st *st = new thread_st;
          st->nbsec = myArgs.dump;
          st->dumpFile = string("dumpState_") + myArgs.args[0];
          string rootFile = st->dumpFile.substr(0,st->dumpFile.find("."));
          st->lockFile = rootFile + ".lock";
          pthread_create(&th,NULL,&dumpState,(void*)st);
        }

      cerr << "+++++++++++++++++++ start calculation +++++++++++++++++++" << endl;
      executor.RunW(p,myArgs.display, fromScratch);
      cerr << "+++++++++++++++++++  end calculation  +++++++++++++++++++" << endl;
      cerr << "Proc state : " << Node::getStateName(p->getEffectiveState()) << endl;
      timer("Elapsed time after execution: ");

      // Return 0 if SCHEMA OK
      // Return 1 for YACS ERROR (EXCEPTION NOT CATCHED)
      // Return 2 for YACS SCHEMA ERROR/FAILED
      int return_value = 0;

      if(p->getEffectiveState() != YACS::DONE)
        {
          std::string report=p->getErrorReport();
          std::cerr << "Execution has ended in error" << std::endl;
          std::cerr << report << std::endl;
          return_value = 2;
        }

      if(myArgs.display>0)
        {
          std::ofstream g("titi");
          p->writeDot(g);
          g.close();
        }

      if (isDump)
        {
          pthread_join(th,NULL);
        }

      bool isFinalDump = (strlen(myArgs.finalDump) != 0);
      if (isFinalDump)
        {
          YACS::ENGINE::VisitorSaveState vst(p);
          vst.openFileDump(myArgs.finalDump);
          p->accept(&vst);
          vst.closeFileDump();
        }
      if(myArgs.shutdown < 999)
        {
          p->shutdown(myArgs.shutdown);
        }
      delete p;
      Runtime* r=YACS::ENGINE::getRuntime();
      Dispatcher* disp=Dispatcher::getDispatcher();
      r->fini();
      delete r;
      delete disp;
      return return_value;
    }
  catch (YACS::Exception& e)
    {
      cerr << "Caught a YACS exception" << endl;
      cerr << e.what() << endl;
      Runtime* r=YACS::ENGINE::getRuntime();
      Dispatcher* disp=Dispatcher::getDispatcher();
      r->fini();
      delete r;
      delete disp;
      return 1;
    }
  catch (const std::ios_base::failure&)
    {
      cerr << "Caught an io failure exception" << endl;
      return 1;
    }
  catch(CORBA::SystemException& ex) 
    {
      cerr << "Caught a CORBA::SystemException.:" << __FILE__ << ":" << __LINE__ << ":" ;
      CORBA::Any tmp;
      tmp <<= ex;
      CORBA::TypeCode_var tc = tmp.type();
      const char *p = tc->name();
      if ( *p != '\0' ) 
        cerr <<p;
      else  
        cerr  << tc->id();
      cerr << endl;
      return 1;
    }
  catch(omniORB::fatalException& fe) 
    {
      cerr << "Caught omniORB::fatalException:" << endl;
      cerr << "  file: " << fe.file() << endl;
      cerr << "  line: " << fe.line() << endl;
      cerr << "  mesg: " << fe.errmsg() << endl;
      return 1;
    }
  catch(...) 
    {
      cerr << "Caught unknown exception." << endl;
      return 1;
    }
}

