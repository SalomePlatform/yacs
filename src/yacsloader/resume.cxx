
#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "Exception.hxx"
#include "Executor.hxx"
#include "parsers.hxx"
#include "VisitorSaveState.hxx"
#include "LoadState.hxx"


#include <iostream>
#include <fstream>

using YACS::YACSLoader;
using namespace YACS::ENGINE;
using namespace std;


int
main (int argc, char* argv[])
{
  if (argc != 3 && argc != 4)
    {
      cerr << "usage: " << argv[0] << " [--display=n] schema.xml state.xml" << endl;
      return 1;
    }
  int display=0;
  int argfile=1;
  int argState=2;
  if(argc == 4)
    {
      argfile=2;
      argState=3;
      if(std::string(argv[1]) == "--display=1") 
        display=1;
      else if(std::string(argv[1]) == "--display=2") 
        display=2;
      else if(std::string(argv[1]) == "--display=3") 
        display=3;
    }

  RuntimeSALOME::setRuntime();

  YACSLoader loader;
  Executor executor;

  try
    {
      Proc* p=loader.load(argv[argfile]);
      p->init();
      p->exUpdateState();
      stateParser* rootParser = new stateParser();
      stateLoader myStateLoader(rootParser, p);
      myStateLoader.parse(argv[argState]);

      std::ofstream f("toto");
      p->writeDot(f);
      f.close();
      cerr << "+++++++++++++++++++ start calculation +++++++++++++++++++" << endl;
      executor.RunW(p, display, false);
      cerr << "+++++++++++++++++++  end calculation  +++++++++++++++++++" << endl;
      std::ofstream g("titi");
      p->writeDot(g);
      g.close();
      YACS::ENGINE::VisitorSaveState vst(p);
      vst.openFileDump("dumpState.xml");
      p->accept(&vst);
      vst.closeFileDump();
      delete p;
      YACS::ENGINE::getRuntime()->fini();
      return 0;
    }
  catch (YACS::Exception& e)
    {
      cerr << "Caught a YACS exception" << endl;
      cerr << e.what() << endl;
      YACS::ENGINE::getRuntime()->fini();
      return 1;
    }
  catch (const std::ios_base::failure&)
    {
      cerr << "Caught an io failure exception" << endl;
      return 1;
    }
  catch(CORBA::SystemException& ex) 
    {
      cerr << "Caught a CORBA::SystemException." ;
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

