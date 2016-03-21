# -*- coding: iso-8859-1 -*-
# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

"""This module is used to parse a supervision graph Salome (XML) and convert it into
   YACS calculation schema

   This parsing is done with SalomeLoader class and its method load.
"""

import sys,os
try:
  import cElementTree as ElementTree
except ImportError:
  import ElementTree

#from sets import Set
Set=set
import graph
import pilot
import SALOMERuntime

class UnknownKind(Exception):pass

#global variables
debug=0
typeMap={}
objref=None
_containers={}
currentProc=None

def typeName(name):
  """Replace :: in type name by /"""
  return "/".join(name.split("::"))

streamTypes={
             '0':"Unknown",
             '1':"CALCIUM_integer",
             '3':"CALCIUM_real",
            }


class SalomeLoader:
  """This class parses a Salome graph (version 3.2.x) and converts it into YACS schema.

     The loadxml method parses xml file and returns a SalomeProc object

     The load method calls the loadxml method and creates a YACS object of class Proc
  """
  def loadxml(self,filename):
    """
       Parse a XML file from Salome SUPERV and return a list of SalomeProc objects.
    """
    tree = ElementTree.ElementTree(file=filename)
    root = tree.getroot()
    if debug:print "root.tag:",root.tag,root

    procs=[]
    if root.tag == "dataflow":
      #only one dataflow
      dataflow=root
      if debug:print dataflow
      proc=SalomeProc(dataflow)
      procs.append(proc)
    else:
      #one or more dataflows. The graph contains macros.
      #All macros are defined at the same level in the XML file.
      for dataflow in root.findall("dataflow"):
        if debug:print dataflow
        proc=SalomeProc(dataflow)
        if debug:print "dataflow name:",proc.name
        procs.append(proc)
    return procs

  def load(self,filename):
    """Parse a SUPERV XML file (method loadxml) and return a YACS Proc object.
    """
    global typeMap,_containers,objref,currentProc
    typeMap.clear()
    objref=None
    _containers.clear()
    currentProc=None

    procs=self.loadxml(filename)
    #Split the master proc from the possible macros.
    proc=procs.pop(0)
    #proc.display()

    #Put macros in macro_dict
    macro_dict={}
    for p in procs:
      if debug:print "proc_name:",p.name,"coupled_node:",p.coupled_node
      macro_dict[p.name]=p

    if debug:print filename
    yacsproc=ProcNode(proc,macro_dict,filename)
    return yacsproc.createNode()

class Container:
  """Class that defines a Salome Container"""
  def __init__(self,mach,name):
    self.mach=mach
    self.name=name
    self.components={}
  def getName(self):
    return self.mach+"/"+self.name

def getContainer(name):
  if not name:
    name="localhost/FactoryServer"
  elif "/" not in name:
    #no machine name: use localhost
    name="localhost/"+name
  return _containers.get(name)

def addContainer(name):
  if not name:
    mach="localhost"
    name="FactoryServer"
  elif "/" not in name:
    #no machine name: use localhost for mach
    mach="localhost"
  else:
    mach,name=name.split("/")
  c=Container(mach,name)
  _containers[mach+"/"+name]=c
  return c

class Service:
    """Class for Service properties"""
class Parameter:
    """Class for Parameter properties"""
class Link:
    """Class for Link properties"""
class Data:
    """Class for Data properties"""

class Node:
    """Base class for all nodes """
    label="Node: "
    def __init__(self):
      self.links=[]    # list to store inputs as links
      # a link has two attributes : from_node, the starting node
      # to_node, the end node
      self.datas=[]
      self.inStreamLinks=[] #list of dataStream links connected to this node (in)
      self.outStreamLinks=[] #list of dataStream links connected to this node (out)
      self.node=None
    def createNode(self):
      raise NotImplementedError
    def getInputPort(self,p):
      return self.node.getInputPort(".".join(p.split("__")))
    def getOutputPort(self,p):
      if not self.node:
        self.createNode()
      return self.node.getOutputPort(".".join(p.split("__")))
    def getInputDataStreamPort(self,p):
      return self.node.getInputDataStreamPort(p)
    def getOutputDataStreamPort(self,p):
      return self.node.getOutputDataStreamPort(p)

    def initPort(self,l):
      if l.type == 7:
        #double (CORBA::tk_double)
        try:
          self.getInputPort(l.tonodeparam).edInitDbl(l.value)
        except:
          reason="Problem in initialization, not expected type (double): %s %s" % (l.tonodeparam,l.value)
          currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
      elif l.type == 3:
        #int (CORBA::tk_long)
        try:
          self.getInputPort(l.tonodeparam).edInitInt(l.value)
        except:
          reason="Problem in initialization, not expected type (int): %s %s" % (l.tonodeparam,l.value)
          currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
      elif l.type == 14:
        #objref (CORBA::tk_objref)
        try:
          self.getInputPort(l.tonodeparam).edInitString(l.value)
        except:
          reason="Problem in initialization, not expected type (objref): %s %s" % (l.tonodeparam,l.value)
          currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
      elif l.type == 18:
        #string (CORBA::tk_string)
        try:
          self.getInputPort(l.tonodeparam).edInitString(l.value)
        except:
          reason="Problem in initialization, not expected type (string): %s %s" % (l.tonodeparam,l.value)
          currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
      else:
        reason="Problem in initialization, not expected type (%s): %s %s" % (l.type,l.tonodeparam,l.value)
        currentProc.getLogger("parser").error(reason,currentProc.filename,-1)

class InlineNode(Node):
    """Inline Node salome : python function in self.codes[0]"""
    def __init__(self):
      Node.__init__(self)
      self.codes=[]
    def createNode(self):
      r = pilot.getRuntime()
      if self.fnames[0] == "?":
        n=r.createScriptNode("",self.name)
      else:
        n=r.createFuncNode("",self.name)
        n.setFname(self.fnames[0])
        n.setScript(self.codes[0])
      self.node=n
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        n.edAddInputPort(para.name,typeMap[para.type])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        n.edAddOutputPort(para.name,typeMap[para.type])

      for d in self.datas:
        self.initPort(d)

      return n

class ComputeNode(Node):
    """Compute Node Salome execute a component service"""
    def createNode(self):
      if self.node:
        return self.node

      r = pilot.getRuntime()
      if self.container.components.has_key(self.sComponent):
        #a node for this component already exists
        compo_node=self.container.components[self.sComponent]
        #It's a node associated with another node of the same component instance
        #It is not sure that the yacs node has been created ????
        master_node=compo_node.createNode()
        n=master_node.createNode(self.name)
      else:
        #there is no node for this component. This node is first
        self.container.components[self.sComponent]=self
        #There is no component instance for this node
        n=r.createCompoNode("",self.name)
        n.setRef(self.sComponent)

      n.setMethod(self.service.name)
      self.node=n
      #set the container for the node
      if self.container:
        n.getComponent().setContainer(currentProc.containerMap[self.container.getName()])

      #add  dataflow ports in out 
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #Create the missing type and adds it into types table
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        n.edAddInputPort(para.name,typeMap[para.type])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #Create the missing type and adds it into types table
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        pout=n.edAddOutputPort(para.name,typeMap[para.type])

      #add datastream ports in and out
      for para in self.inStreams:
        if debug:print para.name,para.type,para.dependency,para.schema, para.interpolation,
        if debug:print para.extrapolation
        pin=n.edAddInputDataStreamPort(para.name,typeMap[streamTypes[para.type]])
      for para in self.outStreams:
        if debug:print para.name,para.type,para.dependency,para.values
        pout=n.edAddOutputDataStreamPort(para.name,typeMap[streamTypes[para.type]])

      for d in self.datas:
        self.initPort(d)

      return n

class ComposedNode(Node):
  """Composed Node Salome (base class)"""

  def reduceLoop(self):
    """Transform a Salome graph with loops on one level
       in a hierarchical graph.

       The initial graph is in self.G. It is transformed in place.
    """
    G=self.G
    if debug:graph.display(G)
    #invert the graph
    I=graph.invert(G)
    #graph.display(I)

    #Get all loops and their internal nodes
    loops={}
    for n in G:
      if n.kind == 4:
        #Beginning of loop
        loops[n]=graph.reachable(G,n)&graph.reachable(I,n.endloop)
        n.inner_nodes=loops[n]
        n.G=graph.InducedSubgraph(loops[n],G)

    if debug:print "all loops"
    if debug:print loops

    #Get most external loops 
    outer_loops=loops.keys()
    for l in loops:
      for ll in outer_loops:
        if loops[l] < loops[ll]:
          #internal loop
          outer_loops.remove(l)
          ll.set_inner(l)
          break

    #In the end all remaining loops in outer_loops are the most external
    if debug:print outer_loops

    #We remove all internal nodes of most external loops 
    for l in outer_loops:
      #Remove internal nodes
      for n in loops[l]:
        del G[n]
      #Remove endloop node
      suiv=G[l.endloop]
      del G[l.endloop]
      #Replace neighbours of loop by those of endloop
      G[l]= suiv

      #Try to transform incoming and outcoming links of endloop in incoming and 
      #outcoming links of internal nodes. Probably not complete.
      inputs={}
      for link in l.endloop.links:
        if debug:print link.from_node,link.to_node,link.from_param,link.to_param
        inputs[link.to_param]=link.from_node,link.from_param

      for s in suiv:
        for link in s.links:
          if link.from_node == l.endloop:
            link.from_node,link.from_param=inputs[link.from_param]
          if debug:print link.from_node,link.to_node,link.from_param,link.to_param

      if debug:graph.display(G)

      #Apply the reduction treatment to most external loops (recurse)
      for l in outer_loops:
        l.reduceLoop()

  def connect_macros(self,macro_dict):
    """This method connects the salome macros in macro_dict to the master YACS Proc.
       
    """
    if debug:print "connect_macros",self.node,macro_dict
    for node in self.G:
      if isinstance(node,MacroNode):
        #node is a macro, connect its definition to self.
        #p is the Salome macro (class SalomeProc)
        #node is the Salome MacroNode that has the subgraph p
        #node.node is the YACS Bloc equivalent to node
        p=macro_dict[node.coupled_node]
        bloc=node.node
        if debug:print "macronode:",node.name,node.coupled_node,p
        #Create a hierarchical graph from the salome graph
        G=p.create_graph()
        node.G=G
        for n in G:
          #create an equivalent YACS node from each salome node
          nod=n.createNode()
          bloc.edAddChild(nod)

        #Connect macros to node
        node.connect_macros(macro_dict)

        #add control links
        for n in G:
          for v in G[n]:
            bloc.edAddCFLink(n.node,v.node)
        #add dataflow links and initializations
        for n in G:
          #dataflow links
          for l in n.links:
            bloc.edAddLink(l.from_node.getOutputPort(l.from_param),
                        l.to_node.getInputPort(l.to_param))
          #datastream links
          for l in n.outStreamLinks:
            pout=l.from_node.getOutputDataStreamPort(l.from_param)
            pin=l.to_node.getInputDataStreamPort(l.to_param)
            bloc.edAddLink(pout,pin)
          #initializations
          for l in n.datas:
            if l.type == 7:
              #double (CORBA::tk_double)
              try:
                n.getInputPort(l.tonodeparam).edInitDbl(l.value)
              except:
                reason="Problem in initialization, not expected type (double): %s %s" % (l.tonodeparam,l.value)
                currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
            elif l.type == 3:
              #int (CORBA::tk_long)
              try:
                n.getInputPort(l.tonodeparam).edInitInt(l.value)
              except:
                reason="Problem in initialization, not expected type (int): %s %s" % (l.tonodeparam,l.value)
                currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
            elif l.type == 14:
              #objref (CORBA::tk_objref)
              try:
                n.getInputPort(l.tonodeparam).edInitString(l.value)
              except:
                reason="Problem in initialization, not expected type (objref): %s %s" % (l.tonodeparam,l.value)
                currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
            elif l.type == 18:
              #string (CORBA::tk_string)
              try:
                n.getInputPort(l.tonodeparam).edInitString(l.value)
              except:
                reason="Problem in initialization, not expected type (string): %s %s" % (l.tonodeparam,l.value)
                currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
            else:
              reason="Problem in initialization, not expected type (%s): %s %s" % (l.type,l.tonodeparam,l.value)
              currentProc.getLogger("parser").error(reason,currentProc.filename,-1)

class LoopNode(ComposedNode):
    """Objet qui simule le comportement d'une boucle Salome."""
    def __init__(self):
      ComposedNode.__init__(self)
      self.inner_loops=[]
      #inner_nodes contains internal nodes as in Salome (on one level with endloop nodes)
      self.inner_nodes=[]

    def set_node(self,node):
      self.node=node

    def set_inner(self,loop):
      for i in self.inner_loops:
        if loop.inner_nodes < i.inner_nodes:
          #the loop is contained in i
          i.set_inner(loop)
          break
      self.inner_loops.append(loop)

    def createNode(self):
      """Create the equivalent YACS loop and store it in attribute node

         A Salome loop has n input ports and output ports with exactly same names.
         The head of loop has 3 functions : init, next, more which have almost same 
         interface. init and next have same interface : on input, input loop parameters
         on output, output loop parameters (same as input). more has one more output parameter
         in first place. This parameter says if the loop must go on or not.
         The endloop has a function with the same interface as next.

         To transform this node, create a YACS Bloc. In this bloc put a node for the init function
         and a While node. In the while put all internal nodes plus 2 nodes for the next and more 
         functions.
      """

      r = pilot.getRuntime()
      bloop=r.createBloc(self.name)

      #init node
      init=r.createFuncNode("","init")
      #print self.codes[0]
      init.setScript(self.codes[0])
      init.setFname(self.fnames[0])
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        init.edAddInputPort(para.name,typeMap[para.type])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        init.edAddOutputPort(para.name,typeMap[para.type])
      bloop.edAddChild(init)
      self.init=init

      wh=r.createWhileLoop(self.name)
      bloop.edAddChild(wh)
      blnode=r.createBloc(self.name)
      wh.edSetNode(blnode)
      cport=wh.edGetConditionPort()
      cport.edInitBool(True)

      #next node
      next=r.createFuncNode("","next")
      #print self.codes[2]
      next.setScript(self.codes[2])
      next.setFname(self.fnames[2])
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        next.edAddInputPort(para.name,typeMap[para.type])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        next.edAddOutputPort(para.name,typeMap[para.type])
      blnode.edAddChild(next)
      self.next=next

      #more node
      more=r.createFuncNode("","more")
      #print self.codes[1]
      more.setScript(self.codes[1])
      more.setFname(self.fnames[1])
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        more.edAddInputPort(para.name,typeMap[para.type])
      more.edAddOutputPort("DoLoop",typeMap["int"])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[objref])
        if not currentProc.typeMap.has_key(para.type):
          currentProc.typeMap[para.type]=typeMap[para.type]
        more.edAddOutputPort(para.name,typeMap[para.type])
      blnode.edAddChild(more)
      self.more=more

      for para in self.service.outParameters:
        bloop.edAddDFLink(init.getOutputPort(para.name),next.getInputPort(para.name))

      for para in self.service.outParameters:
        blnode.edAddDFLink(next.getOutputPort(para.name),more.getInputPort(para.name))

      wh.edAddLink(more.getOutputPort("DoLoop"),wh.getInputPort("condition"))

      for para in self.service.outParameters:
        wh.edAddLink(more.getOutputPort(para.name),next.getInputPort(para.name))

      self.node=bloop

      for n in self.G:
        node=n.createNode()
        blnode.edAddChild(node)

      for n in self.G:
        for v in self.G[n]:
          blnode.edAddCFLink(n.node,v.node)

      for n in self.G:
        for l in n.links:
          try:
            blnode.edAddDFLink(l.from_node.getOutputPort(l.from_param),
                             l.to_node.getInputPort(l.to_param))
          except:
            reason="Error while connecting output port: "+l.from_param+" from node: "+l.from_node.name
            reason=reason+" to input port: "+l.to_param+" from node: "+l.to_node.name
            currentProc.getLogger("parser").error(reason,currentProc.filename,-1)

      return bloop

    def getInputPort(self,p):
      return self.init.getInputPort(p)

    def getOutputPort(self,p):
      return self.more.getOutputPort(p)

class Bloc(ComposedNode):
    """ Composed node containing a set of connected nodes
    """
    label="Bloc: "
    def __init__(self):
      Node.__init__(self)
      self.nodes=[]

    def addLink(self,node1,node2):
      if node1 not in self.nodes:self.nodes.append(node1)
      if node2 not in self.nodes:self.nodes.append(node2)

class MacroNode(Bloc):
  """Objet that represents a Salome Macro
  """
  def createNode(self):
    """Create a YACS node (Bloc) equivalent to a Salome Macro """
    r = pilot.getRuntime()
    macro=r.createBloc(self.name)
    self.node=macro
    return macro

def is_loop(n):
  """Return true if n is a head loop node"""
  return isinstance(n,LoopNode)

class ProcNode(ComposedNode):
  """Salome proc with its macros

     The Salome proc is stored in attribute proc
     The Salome macros are stored in attribute macro_dict ({})
  """
  def __init__(self,proc,macro_dict,filename):
    ComposedNode.__init__(self)
    self.proc=proc
    self.macro_dict=macro_dict
    self.filename=filename

  def createNode(self):
    """Create the YACS node (Proc) equivalent a Salome proc"""
    global currentProc,objref
    r = pilot.getRuntime()

    #create_graph gives a hierarchical graph equivalent to the Salome proc
    G=self.proc.create_graph()
    self.G=G

    #Create the YACS proc with its elements (types, nodes, containers)
    p=r.createProc("pr")
    self.node=p
    currentProc=p
    p.filename=self.filename
    typeMap["double"]=p.typeMap["double"]
    typeMap["float"]=p.typeMap["double"]
    typeMap["int"]=p.typeMap["int"]
    typeMap["short"]=p.typeMap["int"]
    typeMap["long"]=p.typeMap["int"]
    typeMap["string"]=p.typeMap["string"]
    typeMap["char"]=p.typeMap["string"]
    typeMap["boolean"]=p.typeMap["bool"]
    typeMap["bool"]=p.typeMap["bool"]

    objref=p.createInterfaceTc("IDL:omg.org/CORBA/Object:1.0","Object",[])
    typeMap["objref"]=objref
    typeMap["Unknown"]=p.createInterfaceTc("","Unknown",[])
    typeMap["GEOM_Object"]=p.createInterfaceTc("IDL:GEOM/GEOM_Object:1.0","GEOM_Object",[objref])
    typeMap["GEOM_Shape"]=typeMap["GEOM_Object"]

    typeMap["CALCIUM_integer"]=p.createInterfaceTc("IDL:Ports/Calcium_Ports/Calcium_Integer_Port:1.0","CALCIUM_integer",[])
    typeMap["CALCIUM_real"]=p.createInterfaceTc("IDL:Ports/Calcium_Ports/Calcium_Real_Port:1.0","CALCIUM_real",[])
    typeMap["CALCIUM_double"]=p.createInterfaceTc("IDL:Ports/Calcium_Ports/Calcium_Double_Port:1.0","CALCIUM_double",[])
    typeMap["CALCIUM_string"]=p.createInterfaceTc("IDL:Ports/Calcium_Ports/Calcium_String_Port:1.0","CALCIUM_string",[])
    typeMap["CALCIUM_boolean"]=p.createInterfaceTc("IDL:Ports/Calcium_Ports/Calcium_Logical_Port:1.0","CALCIUM_boolean",[])

    typeMap["SuperVisionTest::Adder"]=p.createInterfaceTc("","SuperVisionTest/Adder",[objref])
    typeMap["Adder"]=typeMap["SuperVisionTest::Adder"]

    currentProc.typeMap["Object"]=typeMap["objref"]
    currentProc.typeMap["Unknown"]=typeMap["Unknown"]
    currentProc.typeMap["GEOM_Object"]=typeMap["GEOM_Object"]
    currentProc.typeMap["GEOM_Shape"]=typeMap["GEOM_Shape"]
    currentProc.typeMap["CALCIUM_integer"]=typeMap["CALCIUM_integer"]
    currentProc.typeMap["CALCIUM_real"]=typeMap["CALCIUM_real"]

    #create all containers
    for name,container in _containers.items():
      cont=r.createContainer()
      cont.setName(name)
      cont.setProperty("hostname",container.mach)
      cont.setProperty("container_name",container.name)
      currentProc.containerMap[name]=cont

    for n in G:
      #each node in G creates an equivalent YACS node.
      node=n.createNode()
      p.edAddChild(node)

    #Connect Salome macros to nodes of proc p.
    self.connect_macros(self.macro_dict)

    #add control links
    for n in G:
      for v in G[n]:
        p.edAddCFLink(n.node,v.node)

    #add dataflow links and initializations
    for n in G:
      #dataflow links
      for l in n.links:
        try:
          p.edAddLink(l.from_node.getOutputPort(l.from_param),
                    l.to_node.getInputPort(l.to_param))
        except:
          reason="Error while connecting output port: "+l.from_param+" from node: "+l.from_node.name
          reason=reason+" to input port: "+l.to_param+" from node: "+l.to_node.name
          currentProc.getLogger("parser").error(reason,currentProc.filename,-1)

      #datastream links
      for l in n.outStreamLinks:
        pout=l.from_node.getOutputDataStreamPort(l.from_param)
        pin=l.to_node.getInputDataStreamPort(l.to_param)
        p.edAddLink(pout,pin)
      #initializations
      for l in n.datas:
        if l.type == 7:
          #double (CORBA::tk_double)
          try:
            n.getInputPort(l.tonodeparam).edInitDbl(l.value)
          except:
            reason="Problem in initialization, not expected type (double): %s %s" % (l.tonodeparam,l.value)
            currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
        elif l.type == 3:
          #int (CORBA::tk_long)
          port=n.getInputPort(l.tonodeparam)
          try:
            port.edInitInt(l.value)
          except:
            reason="Problem in initialization, not expected type (int): %s %s" % (l.tonodeparam,l.value)
            currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
        elif l.type == 14:
          #objref (CORBA::tk_objref)
          try:
            n.getInputPort(l.tonodeparam).edInitString(l.value)
          except:
            reason="Problem in initialization, not expected type (objref): %s %s" % (l.tonodeparam,l.value)
            currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
        elif l.type == 18:
          #string (CORBA::tk_string)
          try:
            n.getInputPort(l.tonodeparam).edInitString(l.value)
          except:
            reason="Problem in initialization, not expected type (string): %s %s" % (l.tonodeparam,l.value)
            currentProc.getLogger("parser").error(reason,currentProc.filename,-1)
        else:
          reason="Problem in initialization, not expected type (%s): %s %s" % (l.type,l.tonodeparam,l.value)
          currentProc.getLogger("parser").error(reason,currentProc.filename,-1)

    return p


class SalomeProc(ComposedNode):
    """Salome proc with all its dataflow, datastream and control links
       The object is built by parsing an XML file.
    """
    def __init__(self,dataflow):
        self.name="name"
        self.parse(dataflow)
        #self.links : list of dataflow links (Link objects)
        #self.nodes : list of graph nodes
        #self.node_dict : nodes dict ({name:node})
        #self.datas : list of graph datas 
        #each node has 2 lists of datastream links (inStreams, outStreams)

    def parse(self,dataflow):
        if debug:print "All XML nodes"
        for node in dataflow:
            if debug:print node.tag,node

        #Parse dataflow info-list
        self.dataflow_info=self.parseService(dataflow.find("info-list/node/service"))
        if debug:print self.dataflow_info
        if debug:print self.dataflow_info.inParameters
        if debug:print self.dataflow_info.outParameters
        if debug:
            for para in self.dataflow_info.inParameters:
                print "inParam:",para.name,para.name.split("__",1)

        self.name=dataflow.findtext("info-list/node/node-name")
        self.coupled_node=dataflow.findtext("info-list/node/coupled-node")

        if debug:print "All XML nodes dataflow/node-list"
        nodes=[]
        node_dict={}
        #Parse all nodes
        for n in dataflow.findall('node-list/node'):
            #n is a node-list node
            kind=n.findtext("kind")
            comp=n.findtext("component-name")
            name=n.findtext("node-name")
            coupled_node=n.findtext("coupled-node")
            interface=n.findtext("interface-name")
            container=n.findtext("container")

            #kind=1 : dataflow ?
            #kind=2 : ?
            #kind=9 : datastream graph ?
            #kind=6 : ??
            #kind=8 : ??

            if kind == "0":
              #It's a service
              node=ComputeNode()
              node.kind=0
              node.sComponent = comp
              node.interface=interface
              node.container= getContainer(container)
              if not node.container:
                node.container=addContainer(container)
              if debug:print "\tcontainer",node.container

            elif kind == "3":
              #It's a python function
              node=InlineNode()
              node.kind=3
              codes=[]
              fnames=[]
              for pyfunc in n.findall("PyFunction-list/PyFunction"):
                fnames.append(pyfunc.findtext("FuncName"))
                codes.append(self.parsePyFunction(pyfunc))
              node.fnames=fnames
              node.codes=codes

            elif kind == "4":
              #It's a loop : make a LoopNode
              #python functions (next, more, init) are found in codes
              node=LoopNode()
              node.kind=4
              codes=[]
              fnames=[]
              for pyfunc in n.findall("PyFunction-list/PyFunction"):
                fnames.append(pyfunc.findtext("FuncName"))
                codes.append(self.parsePyFunction(pyfunc))
              node.fnames=fnames
              node.codes=codes

            elif kind == "5":
              #End of loop : make an InlineNode
              node=InlineNode()
              node.kind=5
              codes=[]
              fnames=[]
              for pyfunc in n.findall("PyFunction-list/PyFunction"):
                fnames.append(pyfunc.findtext("FuncName"))
                codes.append(self.parsePyFunction(pyfunc))
              node.fnames=fnames
              node.codes=codes

            elif kind == "10":
              # It's a Macro node : make a MacroNode
              node=MacroNode()
              node.kind=10
            else:
              raise UnknownKind,kind

            node.name=name
            node.service=None
            node.coupled_node=coupled_node
            #Put nodes in a dict to ease search
            node_dict[node.name]=node
            if debug:print "\tnode-name",node.name
            if debug:print "\tkind",node.kind,node.__class__.__name__

            s=n.find("service")
            if s:
                node.service=self.parseService(s)


            #Parse datastream ports
            if debug:print "DataStream ports"
            inStreams=[]
            for indata in n.findall("DataStream-list/inParameter"):
                inStreams.append(self.parseInData(indata))
            node.inStreams=inStreams
            outStreams=[]
            outStreams_dict={}
            for outdata in n.findall("DataStream-list/outParameter"):
                p=self.parseOutData(outdata)
                outStreams.append(p)
                outStreams_dict[p.name]=p
            node.outStreams=outStreams
            node.outStreams_dict=outStreams_dict
            if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"
            nodes.append(node)

        self.nodes=nodes
        self.node_dict=node_dict
        #Nodes parsing is finished.
        #Parse dataflow and datastream links.
        """
        <link>
        <fromnode-name>Node_A_1</fromnode-name>
        <fromserviceparameter-name>a_1</fromserviceparameter-name>
        <tonode-name>Node_B_1</tonode-name>
        <toserviceparameter-name>b_1</toserviceparameter-name>
        <coord-list/>
        </link>
        """
        if debug:print "All XML nodes dataflow/link-list"
        links=[]
        if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"
        for link in dataflow.findall('link-list/link'):
            l=Link()
            l.from_name=link.findtext("fromnode-name")
            l.to_name=link.findtext("tonode-name")
            l.from_param=link.findtext("fromserviceparameter-name")
            l.to_param=link.findtext("toserviceparameter-name")
            links.append(l)
            if debug:print "\tfromnode-name",l.from_name
            if debug:print "\tfromserviceparameter-name",l.from_param
            if debug:print "\ttonode-name",l.to_name
            if debug:print "\ttoserviceparameter-name",l.to_param
            if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"

        self.links=links
        if debug:print "All XML nodes dataflow/data-list"
        datas=[]
        for data in dataflow.findall('data-list/data'):
            d=self.parseData(data)
            datas.append(d)
            if debug:print "\ttonode-name",d.tonode
            if debug:print "\ttoserviceparameter-name",d.tonodeparam
            if debug:print "\tparameter-value",d.value
            if debug:print "\tparameter-type",d.type
            if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"

        self.datas=datas

    def parseService(self,s):
        service=Service()
        service.name=s.findtext("service-name")
        if debug:print "\tservice-name",service.name

        inParameters=[]
        for inParam in s.findall("inParameter-list/inParameter"):
            p=Parameter()
            p.name=inParam.findtext("inParameter-name")
            p.type=typeName(inParam.findtext("inParameter-type"))
            if debug:print "\tinParameter-name",p.name
            if debug:print "\tinParameter-type",p.type
            inParameters.append(p)
        service.inParameters=inParameters
        if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"

        outParameters=[]
        for outParam in s.findall("outParameter-list/outParameter"):
            p=Parameter()
            p.name=outParam.findtext("outParameter-name")
            p.type=typeName(outParam.findtext("outParameter-type"))
            if debug:print "\toutParameter-name",p.name
            if debug:print "\toutParameter-type",p.type
            outParameters.append(p)
        service.outParameters=outParameters
        if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"
        return service

    def parseData(self,d):
        da=Data()
        da.tonode=d.findtext("tonode-name")
        da.tonodeparam=d.findtext("toserviceparameter-name")
        da.value=d.findtext("data-value/value")
        da.type=eval(d.findtext("data-value/value-type"))
        if da.type < 9:
            da.value=eval(da.value)
        return da

    def parsePyFunction(self,pyfunc):
        if debug:print pyfunc.tag,":",pyfunc
        if debug:print "\tFuncName",pyfunc.findtext("FuncName")
        text=""
        for cdata in pyfunc.findall("PyFunc"):
            if text:text=text+'\n'
            if cdata.text != '?':
              text=text+ cdata.text
        return text

    """<inParameter-type>1</inParameter-type>
    <inParameter-name>istream</inParameter-name>
    <inParameter-dependency>2</inParameter-dependency>
    <inParameter-schema>0</inParameter-schema>
    <inParameter-interpolation>0</inParameter-interpolation>
    <inParameter-extrapolation>0</inParameter-extrapolation>
    </inParameter>
    <outParameter>
    <outParameter-type>1</outParameter-type>
    <outParameter-name>ostream</outParameter-name>
    <outParameter-dependency>2</outParameter-dependency>
    <outParameter-values>0</outParameter-values>
    </outParameter>
    """

    def parseInData(self,d):
        if debug:print d.tag,":",d
        p=Parameter()
        p.name=d.findtext("inParameter-name")
        p.type=typeName(d.findtext("inParameter-type"))
        p.dependency=d.findtext("inParameter-dependency")
        p.schema=d.findtext("inParameter-schema")
        p.interpolation=d.findtext("inParameter-interpolation")
        p.extrapolation=d.findtext("inParameter-extrapolation")
        if debug:print "\tinParameter-name",p.name
        return p

    def parseOutData(self,d):
        if debug:print d.tag,":",d
        p=Parameter()
        p.name=d.findtext("outParameter-name")
        p.type=typeName(d.findtext("outParameter-type"))
        p.dependency=d.findtext("outParameter-dependency")
        p.values=d.findtext("outParameter-values")
        if debug:print "\toutParameter-name",p.name
        return p

    def create_graph(self):
      #a graph is a dict {node:neighbours}
      #neighbours is a Set of neighbour nodes (of course)
      #for v in graph (python >= 2.3): iterate through graph nodes 
      #for v in graph[node] iterate through node neighbours 
      G={}
      #create all nodes without neighbours
      for n in self.nodes:
        G[n]=Set()

      #calculate neighbours with links
      for link in self.links:
        from_node=self.node_dict[link.from_name]
        if link.from_param == "Gate" or link.to_param == "Gate":
          #control link salome : add to_name node to neighbours
          if debug:print "add control link",link.from_name,link.to_name
          G[self.node_dict[link.from_name]].add(self.node_dict[link.to_name])

        elif from_node.outStreams_dict.has_key(link.from_param):
          # datastream link : 
          # 1- add link in link list
          # 2- add in link references on from_node and to_node
          if debug:print "add stream link",link.from_name,link.to_name
          self.node_dict[link.to_name].inStreamLinks.append(link)
          self.node_dict[link.from_name].outStreamLinks.append(link)
          link.from_node=self.node_dict[link.from_name]
          link.to_node=self.node_dict[link.to_name]

        else:
          # other salome link
          # if link from Loop node to EndOfLoop node, we ignore it
          # all others are kept
          from_node=self.node_dict[link.from_name]
          to_node=self.node_dict[link.to_name]
          if isinstance(to_node,LoopNode):
            # If it's the link from EndOfLoop to Loop , we ignore it
            if to_node.coupled_node == from_node.name:
              if debug:print "backlink loop:",from_node,to_node
              #ignored
              continue
          if debug:print "add dataflow link",link.from_name,link.to_name
          G[self.node_dict[link.from_name]].add(self.node_dict[link.to_name])

          if link.from_param != "DoLoop" and link.to_param != "DoLoop":
            #Links on DoLoop are used by Salome supervisor. We ignore them.
            #Add in the link references on nodes (from_node and to_node)
            #Add this link into the list of links of to_node node.
            self.node_dict[link.to_name].links.append(link)
            link.from_node=self.node_dict[link.from_name]
            link.to_node=self.node_dict[link.to_name]

          #In a Salome graph with loops, head node and end node are connected 
          #with 2 opposite links 
          #Store the endloop in attribute endloop of head node.
          if link.from_param == "DoLoop" and link.to_param == "DoLoop" \
             and is_loop(self.node_dict[link.from_name]) \
             and isinstance(self.node_dict[link.to_name],InlineNode):
            #Store the end loop inline node in attribute endloop
            #self.node_dict[link.to_name] is the end node of the head loop node self.node_dict[link.from_name]
            if debug:print "add loop",link.from_name,link.to_name
            self.node_dict[link.from_name].endloop=self.node_dict[link.to_name]
            self.node_dict[link.to_name].loop=self.node_dict[link.from_name]

      for data in self.datas:
        if debug:print "datas",data
        self.node_dict[data.tonode].datas.append(data)

      self.G=G

      #Transform the graph in place
      # Transform one level loops in hierarchical graph
      self.reduceLoop()

      #Return the hierarchical graph that can be transform into YACS objects.
      return G

    def display(self,suivi="sync"):
        """Display Salome proc with graphviz (dot file)"""
        #to display : dot -Tpng salome.dot |display
        f=file("salome.dot", 'w')
        self.write_dot(f)
        f.close()
        cmd="dot -Tpng salome.dot |display" + (suivi == "async" and "&" or "")
        os.system(cmd)

    def write_dot(self,stream):
        """Dump Salome proc into stream with dot format"""
        stream.write('digraph %s {\nnode [ style="filled" ]\n' % self.name)
        for node in self.nodes:
            label = "%s:%s"% (node.name,node.__class__.__name__)
            color='green'
            stream.write('   %s [fillcolor="%s" label=< %s >];\n' % (
                    id(node), color, label
                ))
        for link in self.links:
            from_node=self.node_dict[link.from_name]
            to_node=self.node_dict[link.to_name]
            stream.write('   %s -> %s;\n' % (id(from_node), id(to_node)))
        stream.write("}\n")


def main():
  import traceback
  usage ="""Usage: %s salomeFile convertedFile
    where salomeFile is the name of the input schema file (old Salome syntax)
    and convertedFile is the name of the output schema file (new YACS syntax)
    """
  try:
    salomeFile=sys.argv[1]
    convertedFile=sys.argv[2]
  except :
    print usage%(sys.argv[0])
    sys.exit(3)

  SALOMERuntime.RuntimeSALOME_setRuntime()
  loader=SalomeLoader()

  try:
    p= loader.load(salomeFile)
    s= pilot.SchemaSave(p)
    s.save(convertedFile)
  except:
    traceback.print_exc(file=sys.stdout)
    f=open(convertedFile,'w')
    f.write("<proc></proc>\n")
    sys.exit(2)

  logger=p.getLogger("parser")
  if not logger.isEmpty():
    print logger.getStr()
    sys.exit(1)

if __name__ == "__main__":
  main()
