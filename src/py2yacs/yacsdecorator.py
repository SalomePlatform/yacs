#!/usr/bin/env python3
# Copyright (C) 2006-2023  CEA/DEN, EDF R&D
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
import json

# this is a pointer to the module object instance itself.
this_module = sys.modules[__name__]

class OutputPort:
  def __init__(self, yacs_node, yacs_port):
    self.yacs_node = yacs_node
    self.yacs_port = yacs_port

  def IAmAManagedPort(self):
    """ Type check."""
    return True

  def linkTo(self, input_port, input_node, generator):
    generator.proc.edAddLink(self.yacs_port, input_port)
    generator.addCFLink(self.yacs_node, input_node)

  def getPort(self):
    return self.yacs_port

  def getNode(self):
    return self.yacs_node

class OutputPortWithCollector:
  def __init__(self, output_port):
    self.output_port = output_port
    self.connectedInputPorts = []

  def IAmAManagedPort(self):
    """ Type check."""
    return True

  def linkTo(self, input_port, input_node, generator):
    self.output_port.linkTo(input_port, input_node, generator)
    self.connectedInputPorts.append(input_port)

  def getPort(self):
    return self.output_port.getPort()

  def getNode(self):
    return self.output_port.getNode()

  def connectedPorts(self):
    return self.connectedInputPorts

class LeafNodeType:
  def __init__(self, path, fn_name, inputs, outputs, container_name):
    self.path = path
    self.fn_name = fn_name
    self.inputs = inputs
    self.outputs = outputs
    self.container_name = container_name
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

class ContainerProperties():
  def __init__(self, name, nb_cores, use_cache):
    self.name = name
    self.nb_cores = nb_cores
    self.use_cache = use_cache

def jsonContainerEncoder(obj):
  if isinstance(obj, ContainerProperties) :
    return {
            "name": obj.name,
            "nb_cores": obj.nb_cores,
            "use_cache": obj.use_cache }
  else:
    raise TypeError("Cannot serialize object "+str(obj))

def jsonContainerDecoder(dct):
  if "name" in dct and "nb_cores" in dct and "use_cache" in dct :
    return ContainerProperties(dct["name"], dct["nb_cores"], dct["use_cache"])
  return dct

class ContainerManager():
  defaultContainerName = "default_container"
  def __init__(self):
    self._containers = []
    self._defaultContainer = ContainerProperties(
                                ContainerManager.defaultContainerName, 0, False)
    self._containers.append(self._defaultContainer)

  def setDefaultContainer(self, nb_cores, use_cache):
    self._defaultContainer.nb_cores = nb_cores
    self._defaultContainer.use_cache = use_cache

  def loadFile(self, file_path):
    with open(file_path, 'r') as json_file:
      self._containers = json.load(json_file, object_hook=jsonContainerDecoder)
    try:
      self._defaultContainer = next(cont for cont in self._containers
                          if cont.name == ContainerManager.defaultContainerName)
    except StopIteration:
      self._defaultContainer = ContainerProperties(
                                ContainerManager.defaultContainerName, 0, False)
      self._containers.append(self._defaultContainer)

  def saveFile(self, file_path):
    with open(file_path, 'w') as json_file:
      json.dump(self._containers, json_file,
                indent=2, default=jsonContainerEncoder)

  def addContainer(self, name, nb_cores, use_cache):
    try:
      # if the name already exists
      obj = next(cont for cont in self._containers if cont.name == name)
      obj.nb_cores = nb_cores
      obj.use_cache = use_cache
    except StopIteration:
      # new container
      self._containers.append(ContainerProperties(name, nb_cores, use_cache))

  def getContainer(self, name):
    ret = self._defaultContainer
    try:
      ret = next(cont for cont in self._containers if cont.name == name)
    except StopIteration:
      # not found
      pass
    return ret

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
    self.booltype = self.runtime.getTypeCode("bool")
    self.block_stack = [self.proc]
    self.name_index = 0 # used to ensure unique names
    self.container_manager = ContainerManager()

  def newName(self, name):
    new_name = name + "_" + str(self.name_index)
    self.name_index += 1
    return new_name

  def isAManagedPort(self, port):
    try:
      isManagedPort = port.IAmAManagedPort()
    except AttributeError:
      isManagedPort = False
    return isManagedPort

  def getContextName(self):
    context_name = ""
    if len(self.block_stack) > 1:
      # We are in a block
      block_path = ".".join([ b.getName() for b in self.block_stack[1:] ])
      context_name = block_path + "."
    return context_name

  def getContainer(self, container_type):
    """
    A new container may be created if it does not already exist for this type.
    """
    container_properties = self.container_manager.getContainer(container_type)
    if container_type not in self.containers:
      cont=self.proc.createContainer(container_properties.name,"Salome")
      cont.setProperty("nb_parallel_procs", str(container_properties.nb_cores))
      cont.setProperty("type","multi")
      cont.usePythonCache(container_properties.use_cache)
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
    container = self.getContainer(leaf.container_name)
    new_node = self.runtime.createScriptNode("Salome", node_name)
    new_node.setContainer(container)
    new_node.setExecutionMode("remote")
    new_node.setScript(script)
    self.block_stack[-1].edAddChild(new_node)
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
      if self.isAManagedPort(v) :
        v.linkTo(input_port, new_node, self)
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
    self.block_stack[-1].edAddChild(new_foreach)
    block_name = "block_"+foreach_name
    new_block = self.runtime.createBloc(block_name)
    new_foreach.edAddChild(new_block)
    sample_port = new_foreach.edGetSamplePort()
    input_list_port = new_foreach.edGetSeqOfSamplesPort()
    try:
      isManagedPort = input_values.IAmAManagedPort()
    except AttributeError:
      isManagedPort = False
    if self.isAManagedPort(input_values) :
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
      self.block_stack[-1].edAddChild(conversion_node)
      input_values.linkTo(input_port, conversion_node, self)
      self.proc.edAddLink(output_port, input_list_port)
      # No need to look for ancestors. Both nodes are on the same level.
      self.proc.edAddCFLink(conversion_node, new_foreach)
    else:
      input_list_port.edInitPy(list(input_values))
    self.block_stack.append(new_foreach)
    self.block_stack.append(new_block)
    return OutputPort(new_foreach, sample_port)

  def endForeach(self, outputs):
    self.block_stack.pop() # remove the block
    for_each_node = self.block_stack.pop() # remove the foreach
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
      self.block_stack[-1].edAddChild(conversion_node)
      list_ret = []
      idx_name = 0 # for unique port names
      for port in list_out :
        if self.isAManagedPort(port):
          port_name = port.getPort().getName() + "_" + str(idx_name)
          input_port = conversion_node.edAddInputPort(port_name,
                                                      self.seqpyobjtype)
          output_port = conversion_node.edAddOutputPort(port_name,
                                                        self.pyobjtype)
          self.proc.edAddLink(port.getPort(), input_port)
          list_ret.append(OutputPort(conversion_node, output_port))
          idx_name += 1
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
      while node_from.getFather().getName() != commonAncestor.getName() :
        node_from = node_from.getFather()
      while node_to.getFather().getName() != commonAncestor.getName() :
        node_to = node_to.getFather()
      self.proc.edAddCFLink(node_from, node_to)
    else:
      # from node is ancestor of to node. No CF link needed.
      pass

  def beginWhileloop(self, fn_name, context):
    whileloop_name = self.newName("whileloop_"+fn_name)
    while_node = self.runtime.createWhileLoop(whileloop_name)
    self.block_stack[-1].edAddChild(while_node)
    if not self.isAManagedPort(context):
      # create a init node in order to get a port for the context
      indata_name = "Inputdata_" + whileloop_name
      indata_node = self.runtime.createScriptNode("Salome", indata_name)
      indata_inport = indata_node.edAddInputPort("context", self.pyobjtype)
      indata_outport = indata_node.edAddOutputPort("context", self.pyobjtype)
      indata_inport.edInitPy(context)
      context = OutputPort(indata_node, indata_outport)
      self.block_stack[-1].edAddChild(indata_node)

    block_name = "block_"+whileloop_name
    new_block = self.runtime.createBloc(block_name)
    while_node.edAddChild(new_block)
    self.block_stack.append(while_node)
    self.block_stack.append(new_block)
    self.proc.edAddCFLink(context.getNode(), while_node)
    ret = OutputPortWithCollector(context)
    return ret

  def endWhileloop(self, condition, collected_context, loop_result):
    while_node = self.block_stack[-2]
    cport = while_node.edGetConditionPort()
    # need a conversion node pyobj -> bool
    conversion_node = self.runtime.createScriptNode("Salome",
                                                    "while_condition")
    conversion_node.setExecutionMode("local") # no need for container
    conversion_node.setScript("")
    port_name = "val"
    input_port = conversion_node.edAddInputPort(port_name, self.pyobjtype)
    output_port = conversion_node.edAddOutputPort(port_name, self.booltype)
    self.block_stack[-1].edAddChild(conversion_node)
    condition.linkTo(input_port, conversion_node, self)
    self.proc.edAddLink(output_port, cport)
    if not loop_result is None:
      for p in collected_context.connectedPorts():
        self.proc.edAddLink(loop_result.getPort(), p)
    self.block_stack.pop() # remove the block
    self.block_stack.pop() # remove the while node

_generator = None

_default_mode = "Default"
_yacs_mode = "YACS"
_exec_mode = _default_mode

# Public functions

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

def loadContainers(file_path):
  getGenerator().container_manager.loadFile(file_path)

def export(path):
  if this_module._exec_mode == this_module._yacs_mode :
    getGenerator().dump(path)

# Decorators
class LeafDecorator():
  def __init__(self, container_name):
    self.container_name = container_name

  def __call__(self, f):
    if this_module._exec_mode == this_module._default_mode:
      return f
    co = f.__code__
    import py2yacs
    props = py2yacs.function_properties(co.co_filename, co.co_name)
    nodeType = LeafNodeType(co.co_filename, co.co_name,
                            props.inputs, props.outputs, self.container_name)
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

def leaf(arg):
  """
  Decorator for python scripts.
  """
  if callable(arg):
    # decorator used without parameters. arg is the function
    container = ContainerManager.defaultContainerName
    ret = (LeafDecorator(container))(arg)
  else:
    # decorator used with parameter. arg is the container name
    ret = LeafDecorator(arg)
  return ret

def block(f):
  """
  Decorator for blocks.
  """
  #co = f.__code__
  #print("block :", co.co_name)
  #print("  file:", co.co_filename)
  #print("  line:", co.co_firstlineno)
  #print("  args:", co.co_varnames)
  return f

def seqblock(f):
  """
  Decorator for sequential blocks.
  """
  if this_module._exec_mode == this_module._yacs_mode:
  # TODO create a new block and set a flag to add dependencies between
  # nodes in the block
    pass
  return f

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

def foreach(f):
  """
  Decorator to generate foreach blocks
  """
  if this_module._exec_mode == this_module._default_mode:
    return default_foreach(f)
  elif this_module._exec_mode == this_module._yacs_mode:
    return yacs_foreach(f)

def default_forloop(l, f, context):
  for e in l:
    context = f(e, context)
  return context

def yacs_forloop(l, f, context):
    # TODO
    pass

def forloop(l, f, context):
  """
  Forloop structure for distributed computations.
  This shall be used as a regular function, not as a decorator.
  Parameters:
  l : list of values to iterate on
  f : a function which is the body of the loop
  context : the value of the context for the first iteration.
  Return: context of the last iteration.

  The f function shall take two parameters. The first is an element of the list
  and the second is the context returned by the previous iteration.
  The f function shall return one value, which is the context needed by the next
  iteration.
  """
  if this_module._exec_mode == this_module._default_mode:
    return default_forloop(l, f, context)
  elif this_module._exec_mode == this_module._yacs_mode:
    return yacs_forloop(l, f, context)

def default_whileloop(f, context):
  cond = True
  while cond :
    cond, context = f(context)
  return context

def yacs_whileloop(f, context):
  fn_name = f.__code__.co_name
  generator = getGenerator()
  managed_context = generator.beginWhileloop(fn_name, context)
  # managed context extends the context with the list of all input ports
  # the context is linked to
  cond, ret = f(managed_context)
  generator.endWhileloop(cond, managed_context, ret)
  return ret

def whileloop( f, context):
  """
  Whileloop structure for distributed computations.
  This shall be used as a regular function, not as a decorator.
  Parameters:
  f : a function which is the body of the loop
  context : the value of the context for the first iteration.
  Return: context of the last iteration.

  The f function shall take one parameter which is the context returned by the
  previous iteration. It shall return a tuple of two values. The first value
  should be True or False, to say if the loop shall continue or not. The second
  is the context used by the next iteration.
  """
  if this_module._exec_mode == this_module._default_mode:
    return default_whileloop(f, context)
  elif this_module._exec_mode == this_module._yacs_mode:
    return yacs_whileloop(f, context)

DEFAULT_SWITCH_ID = -1973012217

def default_switch(t, cases, *args, **kwargs):
  ret = None
  if t in cases.keys():
    ret = cases[t](*args, **kwargs)
  elif DEFAULT_SWITCH_ID in cases.keys():
    ret = cases[DEFAULT_SWITCH_ID](*args, **kwargs)
  return ret

def yacs_switch(t, cases, *args, **kwargs):
  # TODO
  pass

def switch( t,       # integer value to test
            cases,   # dic { value: function}
            *args,   # args to call the function
            **kwargs # kwargs to call the function
           ):
  if this_module._exec_mode == this_module._default_mode:
    return default_switch(t, cases, *args, **kwargs)
  elif this_module._exec_mode == this_module._yacs_mode:
    return yacs_switch(t, cases, *args, **kwargs)

def begin_sequential_block():
  if this_module._exec_mode == this_module._default_mode:
    return
  # TODO yacs mode

def end_sequential_block():
  if this_module._exec_mode == this_module._default_mode:
    return
  # TODO yacs mode
