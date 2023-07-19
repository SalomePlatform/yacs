#!/usr/bin/env python3
# -*- coding: utf-8 *-
# Copyright (C) 2007-2023  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
import ast

class FunctionProperties:
  def __init__(self, function_name):
    self.name = function_name
    self.inputs=[]
    self.outputs=None
    self.errors=[]
    self.imports=[]
    pass
  def __str__(self):
    result = "Function:" + self.name + "\n"
    result+= "  Inputs:" + str(self.inputs) + "\n"
    result+= "  Outputs:"+ str(self.outputs) + "\n"
    result+= "  Errors:" + str(self.errors) + "\n"
    result+= "  Imports:"+ str(self.imports) + "\n"
    return result

class VisitAST(ast.NodeVisitor):
  def visit_Module(self, node):
    accepted_tokens = ["Import", "ImportFrom", "FunctionDef", "ClassDef", "If"]
    self.global_errors=[]
    for e in node.body:
      type_name = type(e).__name__
      if type_name not in accepted_tokens:
        error="py2yacs error at line %s: not accepted statement '%s'." % (
               e.lineno, type_name)
        self.global_errors.append(error)
    self.functions=[]
    self.lastfn=""
    self.infunc=False
    self.inargs=False
    self.generic_visit(node)
    pass
  def visit_FunctionDef(self, node):
    if not self.infunc:
      self.lastfn = FunctionProperties(node.name)
      self.functions.append(self.lastfn)
      self.infunc=True
      #
      self.generic_visit(node)
      #
      self.lastfn = None
      self.infunc=False
    pass
  def visit_arg(self, node):
    self.lastfn.inputs.append(node.arg)
    pass
  def visit_Return(self, node):
    if self.lastfn.outputs is not None :
      error="py2yacs error at line %s: multiple returns." % node.lineno
      self.lastfn.errors.append(error)
      return
    self.lastfn.outputs = []
    if node.value is None :
      pass
    elif 'Tuple' == type(node.value).__name__ :
      for e in node.value.elts:
        if 'Name' == type(e).__name__ :
          self.lastfn.outputs.append(e.id)
        else :
          error="py2yacs error at line %s: invalid type returned '%s'." % (
                  node.lineno, type(e).__name__)
          self.lastfn.errors.append(error)
    else:
      if 'Name' == type(node.value).__name__ :
        self.lastfn.outputs.append(node.value.id)
      else :
        error="py2yacs error at line %s: invalid type returned '%s'." %(
                  node.lineno, type(node.value).__name__)
        self.lastfn.errors.append(error)
        pass
      pass
    pass

  def visit_ClassDef(self, node):
    # just ignore classes
    pass

  def visit_Import(self, node):
    if self.infunc:
      for n in node.names:
        self.lastfn.imports.append(n.name)
  def visit_ImportFrom(self, node):
    if self.infunc:
      m=node.module
      for n in node.names:
        self.lastfn.imports.append(m+"."+n.name)

class vtest(ast.NodeVisitor):
  def generic_visit(self, node):
    ast.NodeVisitor.generic_visit(self, node)

def create_yacs_schema(text, fn_name, fn_args, fn_returns, file_name):
  import pilot
  import SALOMERuntime
  SALOMERuntime.RuntimeSALOME_setRuntime()
  runtime = pilot.getRuntime()
  schema = runtime.createProc("schema")
  node = runtime.createScriptNode("", "default_name")
  schema.edAddChild(node)
  fncall = "\n%s=%s(%s)\n"%(",".join(fn_returns),
                            fn_name,
                            ",".join(fn_args))
  node.setScript(text+fncall)
  td=schema.getTypeCode("double")
  for p in fn_args:
    newport = node.edAddInputPort(p, td)
    newport.edInit(0.0)
  for p in fn_returns:
    node.edAddOutputPort(p, td)
  myContainer=schema.createContainer("Py2YacsContainer")
  node.setExecutionMode(pilot.InlineNode.REMOTE_STR)
  node.setContainer(myContainer)
  schema.saveSchema(file_name)

def get_properties(text_file):
  try:
    bt=ast.parse(text_file)
  except SyntaxError as err:
    import traceback
    return [], ["".join(traceback.format_exception_only(SyntaxError,err))]
  w=VisitAST()
  w.visit(bt)
  return w.functions, w.global_errors

def function_properties(python_path, fn_name):
  """
  python_path : path to a python file
  fn_name : name of a function in the file
  return : properties of the function. see class FunctionProperties
  """
  with open(python_path, 'r') as f:
    text_file = f.read()
  functions,errors = get_properties(text_file)
  result = [fn for fn in functions if fn.name == fn_name]
  if len(result) < 1:
    raise Exception("Function not found: {}".format(fn_name))
  result = result[0]
  error_string = ""
  if len(errors) > 0:
    error_string += "Global errors in file {}\n".format(python_path)
    error_string += '\n'.join(errors)
    raise Exception(error_string)
  if len(result.errors) > 0:
    error_string += "Errors when parsing function {}\n".format(fn_name)
    error_string += '\n'.join(result.errors)
    raise Exception(error_string)
  return result


def main(python_path, yacs_path, function_name="_exec"):
  with open(python_path, 'r') as f:
    text_file = f.read()
  fn_name = function_name
  functions,errors = get_properties(text_file)
  error_string = ""
  if len(errors) > 0:
    error_string += "global errors:\n"
    error_string += '\n'.join(errors)
    return error_string
  fn_properties = next((f for f in functions if f.name == fn_name), None)
  if fn_properties is not None :
    if not fn_properties.errors :
      create_yacs_schema(text_file, fn_name,
                         fn_properties.inputs, fn_properties.outputs,
                         yacs_path)
    else:
      error_string += '\n'.join(fn_properties.errors)
  else:
    error_string += "Function not found:"
    error_string += fn_name
  return error_string

if __name__ == '__main__':
  import argparse
  parser = argparse.ArgumentParser(description="Generate a YACS schema from a python file containing a function to run.")
  parser.add_argument("file", help='Path to the python file')
  parser.add_argument("-o","--output",
        help='Path to the output file (yacs_schema.xml by default)',
        default='yacs_schema.xml')
  parser.add_argument("-d","--def_name",
        help='Name of the function to call in the yacs node (_exec by default)',
        default='_exec')
  args = parser.parse_args()
  erreurs = main(args.file, args.output, args.def_name)
  import sys
  if len(erreurs) > 0:
    print(erreurs)
    sys.exit(1)
  else:
    sys.exit(0)
