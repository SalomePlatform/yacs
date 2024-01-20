// Copyright (C) 2006-2024  CEA, EDF
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
#ifndef _PY2YACS_H_
#define _PY2YACS_H_

#ifdef WIN32
#  if defined py2yacslib_EXPORTS
#    define PY2YACSLIB_EXPORT __declspec( dllexport )
#  else
#    define PY2YACSLIB_EXPORT __declspec( dllimport )
#  endif
#else
#  define PY2YACSLIB_EXPORT
#endif


#include <string>
#include <list>
#include <exception>

class PY2YACSLIB_EXPORT Py2yacsException: std::exception
{
  public:
    Py2yacsException(const std::string& what);
    virtual ~Py2yacsException() noexcept;
    virtual const char *what() const noexcept;
  private:
    std::string _what;
};

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
  };
};

struct PY2YACSLIB_EXPORT FunctionProperties
{
  public:
    std::string _name;
    std::list<std::string> _input_ports;
    std::list<std::string> _output_ports;
    std::list<std::string> _errors;
    std::list<std::string> _imports;
};

/*! \brief Converter of a python script to a yacs schema.
 *  This class converts a string containing a python script to a yacs schema
 *  containing a python script node.
 */
class PY2YACSLIB_EXPORT Py2yacs
{
  public:
    Py2yacs();
    
    /*!
     * This constructor can be used if you want to define your own python parser.
     * The parser function should be a python function and return a tuple of
     * two lists.
     * The first list contains the properties of all the functions in the script
     * and the second one contains global errors.
     * \param python_parser_module: name of the parser module
     * \param python_parser_function: name of the parser function
     */
    Py2yacs(const std::string& python_parser_module,
            const std::string& python_parser_function);

    /*!
     * \param python_code: contains the python code that will be converted
     *                     to a yacs schema.
     */
    void load(const std::string& python_code);

    /*!
     * \param file_path: path to the xml file where to save the yacs schema.
     * \param python_function: function defined in the python code that will be
     *                         called in the yacs node.
     */
    void save(const std::string& file_path,
              const std::string& python_function)const;

    /*!
     * A new schema is created.
     * \param python_function: function defined in the python code that will be
     *                         called in the yacs node.
     */
    YACS::ENGINE::Proc* createProc(const std::string& python_function)const;
    
    /*!
     * Syntax errors when parsing python and py2yacs global errors.
     * \return a list of errors.
     */
    const std::list<std::string>& getGlobalErrors() const;

    const std::list<FunctionProperties>& getFunctionProperties()const;

    /*!
     * Get a string containing global errors and errors specific to py2yacs for
     * every function in python script.
     * An empty string means there is no error.
     */
    std::string getAllErrors()const;

    /*!
     * Same as getAllErrors but only for one function.
     * py2yacs errors for other functions are ignored.
     * If the function name is not found, you get an error message in the
     * returned string.
     */
    std::string getFunctionErrors(const std::string& functionName)const;

  private:
    std::string _python_parser_module;
    std::string _python_parser_function;
    std::list<FunctionProperties> _functions;
    std::list<std::string> _global_errors;
    std::string _python_code;
};

#endif //_PY2YACS_H_