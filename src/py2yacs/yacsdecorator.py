#!/usr/bin/env python3
# Copyright (C) 2006-2020  CEA/DEN, EDF R&D
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
import sys

# this is a pointer to the module object instance itself.
this_module = sys.modules[__name__]

class OutputPort:
  def __init__(self, yacs_node, yacs_port):
    self.yacs_node = yacs_node
    self.yacs_port = yacs_port

class LeafNodeType:
  def __init__(self, path, fn_name, inputs, outputs):
    self.path = path
    self.fn_name = fn_name
    self.inputs = inputs
    self.outputs = outputs
    self.number = 0

  def newName(self):
    name = self.fn_name + "_" + str(self.number)
    self.number += 1
    return name

  def createNewNode(self, inputs):
    """
    inputs : dict {input_name:value}
    """
    generator = getGenerator()
    output_ports = generator.createScriptNode(self, inputs)
    return output_ports

def leaf(f):
  """
  Decorator for python scripts.
  """
  if this_module._exec_mode == this_module._default_mode:
    return f
  co = f.__code__
  import py2yacs
  props = py2yacs.function_properties(co.co_filename, co.co_name)
  nodeType = LeafNodeType(co.co_filename, co.co_name,
                          props.inputs, props.outputs)
  def my_func(*args, **kwargs):
    if len(args) + len(kwargs) != len(nodeType.inputs):
      mes = "Wrong number of arguments when calling function '{}'.\n".format(
                                                               nodeType.fn_name)
      mes += " {} arguments expected and {} arguments found.\n".format(
                                  len(nodeType.inputs), len(args) + len(kwargs))
      raise Exception(mes)
    idx = 0
    args_dic = {}
    for a in args:
      args_dic[nodeType.inputs[idx]] = a
      idx += 1
    for k,v in kwargs.items():
      args_dic[k] = v
    if len(args_dic) != len(nodeType.inputs):
      mes="Wrong arguments when calling function {}.\n".format(nodeType.fn_name)
      raise Exception(mes)
    return nodeType.createNewNode(args_dic)
  return my_func

def bloc(f):
  """
  Decorator for blocs.
  """
  #co = f.__code__
  #print("bloc :", co.co_name)
  #print("  file:", co.co_filename)
  #print("  line:", co.co_firstlineno)
  #print("  args:", co.co_varnames)
  return f

def foreach(f):
  """
  Decorator to generate foreach blocs
  """
  if this_module._exec_mode == this_module._default_mode:
    return default_foreach(f)
  elif this_module._exec_mode == this_module._yacs_mode:
    return yacs_foreach(f)

def default_foreach(f):
  def my_func(lst):
    result = []
    for e in lst:
      result.append(f(e))
    t_result = result
    if len(result) > 0 :
      if type(result[0]) is tuple:
        # transform the list of tuples in a tuple of lists
        l_result = []
        for e in result[0]:
          l_result.append([])
        for t in result:
          idx = 0
          for e in t:
            l_result[idx].append(e)
            idx += 1
        t_result = tuple(l_result)
    return t_result
  return my_func

def yacs_foreach(f):
  #co = f.__code__
  #import yacsvisit
  #props = yacsvisit.main(co.co_filename, co.co_name)
  def my_func(input_list):
    fn_name = f.__code__.co_name
    generator = getGenerator()
    sample_port = generator.beginForeach(fn_name, input_list)
    output_list = f(sample_port)
    output_list = generator.endForeach(output_list)
    return output_list
  return my_func

class SchemaGenerator():
  """
  Link to Salome for YACS schema generation.
  """
  def __init__(self):
    import SALOMERuntime
    SALOMERuntime.RuntimeSALOME.setRuntime()
    self.runtime = SALOMERuntime.getSALOMERuntime()
    self.proc = self.runtime.createProc("GeneratedSchema")
    self.proc.setProperty("executor","workloadmanager")
    self.containers = {}
    self.pyobjtype = self.runtime.getTypeCode("pyobj")
    self.seqpyobjtype = self.runtime.getTypeCode("seqpyobj")
    self.bloc_stack = [self.proc]
    self.name_index = 0 # used to ensure unique names

  def newName(self, name):
    new_name = name + "_" + str(self.name_index)
    self.name_index += 1
    return new_name

  def getContextName(self):
    context_name = ""
    if len(self.bloc_stack) > 1:
      # We are in a block
      block_path = ".".join([ b.getName() for b in self.bloc_stack[1:] ])
      context_name = block_path + "."
    return context_name

  def getContainer(self, container_type):
    """
    A new container may be created if it does not already exist for this type.
    """
    if container_type not in self.containers:
      cont=self.proc.createContainer(container_type,"Salome")
      #cont.setProperty("nb_proc_per_node","0")
      cont.setProperty("type","multi")
      cont.usePythonCache(False)
      cont.attachOnCloning()
      self.containers[container_type] = cont
    return self.containers[container_type]

  def createScript(self, file_path, function_name, inputs, outputs):
    import inspect
    stack = inspect.stack()
    stack_info = "Call stack\n"
    # skip the first 4 levels in the stack
    for level in stack[4:-1] :
      info = inspect.getframeinfo(level[0])
      stack_info += "file: {}, line: {}, function: {}, context: {}\n".format(
        info.filename, info.lineno, info.function, info.code_context)
     
    if len(outputs) == 0:
      result = ""
    elif len(outputs) == 1:
      result = "{} = ".format(outputs[0])
    else:
      result = ",".join(outputs)
      result += " = "

    if len(inputs) == 0:
      params = ""
    elif len(inputs) == 1:
      params = "{} ".format(inputs[0])
    else:
      params = ",".join(inputs)
    
    script = """'''
{call_stack}
'''
import yacstools
study_function = yacstools.getFunction("{file_path}", "{function_name}")
{result}study_function({parameters})
""".format(call_stack=stack_info,
           file_path=file_path,
           function_name=function_name,
           result=result,
           parameters=params)
    return script

  def createScriptNode(self, leaf, input_values):
    node_name = leaf.newName()
    file_path = leaf.path
    function_name = leaf.fn_name
    inputs = leaf.inputs # names
    outputs = leaf.outputs # names
    script = self.createScript(file_path, function_name, inputs, outputs)
    container = self.getContainer("generic_cont")
    new_node = self.runtime.createScriptNode("Salome", node_name)
    new_node.setContainer(container)
    new_node.setExecutionMode("remote")
    new_node.setScript(script)
    self.bloc_stack[-1].edAddChild(new_node)
    # create ports
    for p in inputs:
      new_node.edAddInputPort(p, self.pyobjtype)
    output_obj_list = []
    for p in outputs:
      port = new_node.edAddOutputPort(p, self.pyobjtype)
      output_obj_list.append(OutputPort(new_node, port))
    # create links
    for k,v in input_values.items():
      input_port = new_node.getInputPort(k)
      if isinstance(v, OutputPort):
        self.proc.edAddLink(v.yacs_port, input_port)
        self.addCFLink(v.yacs_node, new_node)
        #self.proc.edAddCFLink(v.yacs_node, new_node)
      else:
        input_port.edInitPy(v)
    # return output ports
    result = None
    if len(output_obj_list) == 1 :
      result = output_obj_list[0]
    elif len(output_obj_list) > 1 :
      result = tuple(output_obj_list)
    return result

  def beginForeach(self, fn_name, input_values):
    foreach_name = self.newName(fn_name)
    new_foreach = self.runtime.createForEachLoopDyn(foreach_name,
                                                    self.pyobjtype)
    #new_foreach = self.runtime.createForEachLoop(foreach_name, self.pyobjtype)
    #new_foreach.edGetNbOfBranchesPort().edInitInt(1)
    self.bloc_stack[-1].edAddChild(new_foreach)
    bloc_name = "bloc_"+foreach_name
    new_block = self.runtime.createBloc(bloc_name)
    new_foreach.edAddChild(new_block)
    sample_port = new_foreach.edGetSamplePort()
    input_list_port = new_foreach.edGetSeqOfSamplesPort()
    if isinstance(input_values, OutputPort):
      # we need a conversion node pyobj -> seqpyobj
      conversion_node = self.runtime.createScriptNode("Salome",
                                                      "input_"+foreach_name)
      port_name = "val"
      input_port = conversion_node.edAddInputPort(port_name, self.pyobjtype)
      output_port = conversion_node.edAddOutputPort(port_name,
                                                    self.seqpyobjtype)
      conversion_node.setExecutionMode("local") # no need for container
      # no script, the same variable for input and output
      conversion_node.setScript("")
      self.bloc_stack[-1].edAddChild(conversion_node)
      self.proc.edAddLink(input_values.yacs_port, input_port)
      self.addCFLink(input_values.yacs_node, conversion_node)
      self.proc.edAddLink(output_port, input_list_port)
      # No need to look for ancestors. Both nodes are on the same level.
      self.proc.edAddCFLink(conversion_node, new_foreach)
    else:
      input_list_port.edInitPy(list(input_values))
    self.bloc_stack.append(new_foreach)
    self.bloc_stack.append(new_block)
    return OutputPort(new_foreach, sample_port)

  def endForeach(self, outputs):
    self.bloc_stack.pop() # remove the block
    for_each_node = self.bloc_stack.pop() # remove the foreach
    converted_ret = None
    if outputs is not None:
      # We need a conversion node seqpyobj -> pyobj
      if type(outputs) is tuple:
        list_out = list(outputs)
      else:
        list_out = [outputs]
      conversion_node_name = "output_" + for_each_node.getName()
      conversion_node = self.runtime.createScriptNode("Salome",
                                                      conversion_node_name)
      conversion_node.setExecutionMode("local") # no need for container
      conversion_node.setScript("")
      self.bloc_stack[-1].edAddChild(conversion_node)
      list_ret = []
      idx_name = 0 # for unique port names
      for port in list_out :
        if isinstance(port, OutputPort):
          port_name = port.yacs_port.getName() + "_" + str(idx_name)
          idx_name += 1
          input_port = conversion_node.edAddInputPort(port_name,
                                                      self.seqpyobjtype)
          output_port = conversion_node.edAddOutputPort(port_name,
                                                        self.pyobjtype)
          self.proc.edAddLink(port.yacs_port, input_port)
          list_ret.append(OutputPort(conversion_node, output_port))
        else:
          list_ret.append(port)
      self.proc.edAddCFLink(for_each_node, conversion_node)
      if len(list_ret) > 1 :
        converted_ret = tuple(list_ret)
      else:
        converted_ret = list_ret[0]
    return converted_ret

  def dump(self, file_path):
    self.proc.saveSchema(file_path)

  def addCFLink(self, node_from, node_to):
    commonAncestor = self.proc.getLowestCommonAncestor(node_from, node_to)
    if node_from.getName() != commonAncestor.getName() :
      link_from = node_from
      while link_from.getFather().getName() != commonAncestor.getName() :
        link_from = link_from.getFather()
      link_to = node_to
      while link_to.getFather().getName() != commonAncestor.getName() :
        link_to = link_to.getFather()
      self.proc.edAddCFLink(link_from, link_to)
    else:
      # from node is ancestor of to node. No CF link needed.
      pass

_generator = None

_default_mode = "Default"
_yacs_mode = "YACS"
_exec_mode = _default_mode

def getGenerator():
  """
  Get the singleton object.
  """
  if this_module._generator is None:
    if this_module._exec_mode == this_module._yacs_mode:
      this_module._generator = SchemaGenerator()
  return this_module._generator

def activateYacsMode():
  this_module._exec_mode = this_module._yacs_mode

def activateDefaultMode():
  this_module._exec_mode = this_module._default_mode

def finalize(path):
  if this_module._exec_mode == this_module._yacs_mode :
    getGenerator().dump(path)

  
