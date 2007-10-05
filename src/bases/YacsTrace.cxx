#include <iostream>
#include <sstream>
#include "YacsTrace.hxx"

void AttachDebugger()
{
  if(getenv ("YACSDEBUGGER"))
    {
      std::stringstream exec;
      exec << "$YACSDEBUGGER " << getpid() << "&";
      std::cerr << exec.str() << std::endl;
      system(exec.str().c_str());
      while(1);
    }
}
