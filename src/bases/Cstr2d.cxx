
#include "Cstr2d.hxx"
#include "Exception.hxx"

#include <iostream>
#include <sstream>

//! replacement for atof witch does not work everywhere
/*!
 * When using xml parser (parser.cxx) from YACSGui_XMLDriver (in GUI context),
 * instruction like double d = atof(content); where content = "0.8"
 * gives d = 0 .
 * the same binary code called from outside GUI context works fine...
 */ 
 
double Cstr2d(const char* s)
{
  std::istringstream ss(s);
  double d;
  if (!(ss >> d))
    {
      std::stringstream msg;
      msg << "problem in conversion from string to double: " << s ;
      throw YACS::Exception::Exception(msg.str());
    }
  return d;
}
