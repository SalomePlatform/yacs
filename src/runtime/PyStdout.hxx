#ifndef _PYSTDOUT_HXX_
#define _PYSTDOUT_HXX_

#include <Python.h>
#include <iostream>

namespace YACS
{
  namespace ENGINE
  {
    PyObject * newPyStdOut( std::string& out );
  }
}

#endif

