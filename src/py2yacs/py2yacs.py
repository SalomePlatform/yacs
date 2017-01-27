#!/usr/bin/env python
# -*- coding: utf-8 *-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

class v(ast.NodeVisitor):
  def visit_Module(self, node):
    #print type(node).__name__, ":"
    accepted_tokens = ["Import", "ImportFrom", "FunctionDef", "ClassDef"]
    #print "module body:"
    self.global_errors=[]
    for e in node.body:
      type_name = type(e).__name__
      if type_name not in accepted_tokens:
        error="py2yacs error at line %s: not accepted statement '%s'." % (
               e.lineno, type_name)
        self.global_errors.append(error)
      #print type_name
    #print "------------------------------------------------------------------"
    self.functions=[]
    self.lastfn=""
    self.infunc=False
    self.inargs=False
    self.generic_visit(node)
    pass
  def visit_FunctionDef(self, node):
    #print type(node).__name__, ":", node.name
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
  def visit_arguments(self, node):
    #print type(node).__name__, ":"
    self.inargs=True
    self.generic_visit(node)
    self.inargs=False
    pass
  def visit_Name(self, node):
    if self.inargs :
      #print type(node).__name__, ":", node.id
      self.lastname=node.id
      self.generic_visit(node)
    pass
  def visit_Param(self, node):
    #print type(node).__name__, ":", self.lastname
    self.lastfn.inputs.append(self.lastname)
    pass
  def visit_Return(self, node):
    #print type(node).__name__, ":", node.value
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
    #print type(node).__name__
    ast.NodeVisitor.generic_visit(self, node)

def create_yacs_schema(text, fn_name, fn_args, fn_returns, file_name):
  import pilot
  import SALOMERuntime
  #import loader
  SALOMERuntime.RuntimeSALOME_setRuntime()
  runtime = pilot.getRuntime()
  schema = runtime.createProc("schema")
  node = runtime.createFuncNode("", "default_name")
  schema.edAddChild(node)
  fncall = "\n%s=%s(%s)\n"%(",".join(fn_returns),
                            fn_name,
                            ",".join(fn_args))
  node.setScript(text+fncall)
  node.setFname(fn_name)
  td=schema.getTypeCode("double")
  for p in fn_args:
    node.edAddInputPort(p, td)
  for p in fn_returns:
    node.edAddOutputPort(p, td)
  schema.saveSchema(file_name)

def get_properties(text_file):
  bt=ast.parse(text_file)
  w=v()
  w.visit(bt)
  return w.functions, w.global_errors

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
  with open(args.file, 'r') as f:
    text_file = f.read()
  #bt=ast.parse(text_file)
  #w=vtest()
  #w=v()
  #w.visit(bt)
  #print "global errors:", w.global_errors
  #for f in w.functions:
  #  print f
  
  fn_name = args.def_name
  functions,errors = get_properties(text_file)
  print "global errors:", errors
  for f in functions:
    print f
  
  fn_properties = next((f for f in functions if f.name == fn_name), None)
  if fn_properties is not None :
    if not fn_properties.errors :
      create_yacs_schema(text_file, fn_name,
                       fn_properties.inputs, fn_properties.outputs,
                       args.output)
    else:
      print "\n".join(fn_properties.errors)
  else:
    print "Function not found:", fn_name
  