
.. _schemapy:

Defining a calculation scheme with the Python programming interface
============================================================================
A YACS calculation scheme can be defined from a program written in the Python language (http://www.python.org/).  
Refer to the `Python tutorial <http://docs.python.org/tut/tut.html>`_ for an introduction to the language.

The programming interface (API) is carried on three Python modules:  pilot, SALOMERuntime and loader.

The SALOMERuntime module is used to initialise YACS for SALOME.

The loader module is used to create calculation schemes by loading files in the XML format.

The pilot module is used to create calculation schemes.

These modules must be imported at the beginning of the Python program and YACS must be initialised::

    import sys
    import pilot
    import SALOMERuntime
    import loader
    SALOMERuntime.RuntimeSALOME_setRuntime()

Before YACS modules can be imported, the environment must be correctly configured, as it will be if the 
SALOME application is used.  Otherwise, the PYTHONPATH environment variable has to be set to 
<YACS_ROOT_DIR>/lib/pythonX.Y/site-packages/salome.

When you build your own Salome application and use your own modules and components (using YACSGEN for example), you may need to load
the module catalog::

    import SALOMERuntime
    SALOMERuntime.RuntimeSALOME_setRuntime()
    salome_runtime = SALOMERuntime.getSALOMERuntime()
    import salome
    salome.salome_init()
    mc = salome.naming_service.Resolve('/Kernel/ModulCatalog')
    ior = salome.orb.object_to_string(mc)
    session_catalog = salome_runtime.loadCatalog("session", ior)
    salome_runtime.addCatalog(session_catalog)


.. _loadxml:

Create a calculation scheme by loading an XML file
--------------------------------------------------------------
This is the easiest way of creating a calculation scheme.  If there is a file conforming with the YACS syntax (see :ref:`schemaxml`), 
then all that is necessary is to create an XML file loader and then to use its load method to obtain a calculation scheme object in Python.

The following shows the sufficient Python code to load an XML file::

  xmlLoader = loader.YACSLoader()
  try:
    p = xmlLoader.load("simple1.xml")
  except IOError,ex:
    print "IO exception:",ex
    sys.exit(1)

Then, if the initialisation code and the loading code are put into a file named testLoader.py, proceed as follows::
 
  python testLoader.py

to execute the program.  The IOError exception can be raised by the loading operation principally if the file does not exist 
or if it cannot be read.  If no exception has been raised, it is necessary to make sure that the file analysis took place correctly.  
This is done using the Logger object associated with the calculation scheme::

   logger=p.getLogger("parser")
   if not logger.isEmpty():
     print "The imported file has errors :"
     print logger.getStr()
     sys.exit(1)

Finally, if the file analysis took place correctly, the validity of the scheme (completeness of connections, no unconnected 
input port, etc.) has to be checked.  This is done using the isValid method of the calculation scheme object, and 
then the p.checkConsistency method of this object as below::

   if not p.isValid():
     print "The schema is not valid and can not be executed"
     print p.getErrorReport()
     sys.exit(1)

   info=pilot.LinkInfo(pilot.LinkInfo.ALL_DONT_STOP)
   p.checkConsistency(info)
   if info.areWarningsOrErrors():
     print "The schema is not consistent and can not be executed"
     print info.getGlobalRepr()
     sys.exit(1)


If all these tests took place correctly, the scheme is ready to be executed (see :ref:`execpy`).

Create a calculation scheme from scratch
-------------------------------------------
We will use the same sequence as in :ref:`schemaxml`.
The first step is to obtain the runtime object that will be used for creation of objects making up the scheme, before they are created::

  r = pilot.getRuntime()

Creating an empty scheme
''''''''''''''''''''''''''''
An empty scheme is obtained using the createProc method of the runtime object with the name of the scheme as an argument::

  p=r.createProc("pr")

The scheme object named “pr” was created.  It is represented by the Python variable p.

Definition of data types
'''''''''''''''''''''''''''''''''

.. _basictypes:

Basic types
++++++++++++++++
A basic type cannot be defined.  These types are defined by YACS.  However, it must be possible to retrieve a Python object 
equivalent to a basic type so as to be able to subsequently create ports.
 
A basic data type is recovered using the getTypeCode method in the calculation scheme with the name of the type as an argument.  
For example::

   td=p.getTypeCode("double")

will obtain a double type (Python td object).  Other basic types are obtained by::

   ti=p.getTypeCode("int")
   ts=p.getTypeCode("string")
   tb=p.getTypeCode("bool")
   tf=p.getTypeCode("file")

Object reference
+++++++++++++++++++++
The createInterfaceTc method in the calculation scheme is used to define an object reference type.  
This method accepts three arguments:  the repository id of the corresponding SALOME object, the name of the type, and a 
list of types that will be basic types of this type.  If the repository id is equal to “”, the default value will be used.

The following is a minimal example for a reference definition of an object name Obj (default repository id, no basic type)::

  tc1=p.createInterfaceTc("","Obj",[])

The same Obj type can be defined giving the repository id::

  tc1=p.createInterfaceTc("IDL:GEOM/GEOM_Object","Obj",[])

A list of basic types is also provided so as to define a reference object type derived from another type.

The following gives a definition of the MyObj type derived from the Obj type::

  tc2=p.createInterfaceTc("","MyObj",[tc1])

Sequence
+++++++++++++++++++++
The createSequenceTc method in the calculation scheme is used to define a sequence type.   
This method accepts three arguments, namely the repository id, the type name, and the type of elements in the sequence.  
There is generally no point in specifying the repository id.  The value “” will be given.

The following gives an example definition of the seqdbl double sequence type::

  tc3=p.createSequenceTc("","seqdbl",td)

td is the double type that is obtained as above in the section on :ref:`basictypes`.

A sequence type of sequence is defined as follows::

  tc4=p.createSequenceTc("","seqseqdbl",tc3)

A reference sequence type is defined as follows::

  tc5=p.createSequenceTc("","seqobj",tc1)

Structure
++++++++++++
A structure type is defined using the createStructTc method in the calculation scheme.  
This method accepts two arguments, namely the repository id and the type name.  For standard use, the repository id is 
equal to the value “”.  The structure type is the only type that is defined in two steps.  It is created empty after 
calling the createStructTc method.  Its members are then defined by adding them with the addMember method.

The following shows an example definition of an s1 type structure with 2 members (m1 and m2) of the double and double sequence types::

  ts1=p.createStructTc("","s1")
  ts1.addMember("m1",td)
  ts1.addMember("m2",tc3)

Retrieve predefined types
+++++++++++++++++++++++++++++++++
By default, YACS only defines the basic types.  If more predefined types are required, they must be requested from SALOME.  
These other predefined types are contained in module catalogs such as GEOM or SMESH.

The following code sequence is used to obtain an image of SALOME catalogs in YACS::

  try:
    cata=r.loadCatalog("session",
           "corbaname::localhost:2810/NameService#Kernel.dir/ModulCatalog.object")
  except CORBA.TRANSIENT,ex:
    print "Unable to contact server:",ex
  except CORBA.SystemException,ex:
    print ex,CORBA.id(ex)

The SALOME application must be running before the catalog is accessible.  
Predefined types are then accessible in the cata._typeMap dictionary.  
If the name of the required type is known (for example ‘GEOM_Shape’), it is obtained as follows::

  tgeom=cata._typeMap['GEOM_Shape']

.. _typedict:

Add a type into the scheme types dictionary
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Some operations require that types are defined in the scheme dictionary.  Proceed as follows if you want to add a type 
into the dictionary, for example for the seqobj type defined above::

  p.typeMap["seqobj"]=tc5

where the type name is the dictionary key and the type is the value.

Definition of elementary calculation nodes
''''''''''''''''''''''''''''''''''''''''''''''

.. _pyscript:

Python script node
+++++++++++++++++++++
Several steps are used to define a script node in a given context (for example the calculation scheme).  
The first step consists of creating the node object by calling the runtime createScriptNode method.  
This method uses 2 arguments, the first of which in standard use must be equal to “” and the second is the node name.  
The following is an example to create node node1::

  n=r.createScriptNode("","node1")
 
The second step consists of attaching the node to its definition context by calling the edAddChild method for the context object.  
This method has one argument, namely the node to be attached.  The following is an example of the attachment of the node node1 
to the calculation scheme::

  p.edAddChild(n)

Warning: the name of the method to be used depends on the type of context node.  We will see which method should be used for other 
node types later.

The third step consists of defining the Python script associated with the node.  This is done using the setScript method for the node 
with a character string argument that contains the Python code.  The following shows an example definition of the associated code::

  n.setScript("p1=p1+2.5")

The fourth step consists of defining input and output data ports.  An input port is created by calling the edAddInputPort method 
for the node.  An output port is created by calling the edAddOutputPort method for the node.  
These two methods have two arguments:  the port name and the port data type.  The following is an example creating a double 
type input port p1 and a double type output port p1::

  n.edAddInputPort("p1",td)
  n.edAddOutputPort("p1",td)

Our node is now fully defined with its name, script, ports and context.  It retrieves the double in the input port p1, adds 2.5 to it 
and puts the result into the output port p1.

If you want to execute your script node on a remote container, you have to set the execution mode of the node to **remote**
and to assign a container (see :ref:`py_container` to define a container) to the node as in the following example::

  n.setExecutionMode("remote")
  n.setContainer(cont1)

.. _pyfunc:

Python function node
++++++++++++++++++++++
The same procedure is used to define a function node.  The only differences apply to creation, in using the createFuncNode 
method and defining the function:  the setFname method must also be called to give the name of the function to be executed.  
The following is a complete example for the definition of a function node that is functionally identical to the previous script node::

  n2=r.createFuncNode("","node2")
  p.edAddChild(n2)
  n2.setScript("""
  def f(p1):
    p1=p1+2.5
    return p1
  """)
  n2.setFname("f")
  n2.edAddInputPort("p1",td)
  n2.edAddOutputPort("p1",td)

If you want to execute your function node on a remote container, you have to set the execution mode of the node to **remote**
and to assign a container (see :ref:`py_container` to define a container) to the node as in the following example::

  n2.setExecutionMode("remote")
  n2.setContainer(cont1)

.. _pyservice:

SALOME service node
++++++++++++++++++++++++++
There are two definition forms for a SALOME service node.

The first form in which the component name is given, uses the createCompoNode method to create the node.  The name of the 
component is given as an argument to the setRef method for the node.  The service name is given as an argument for the 
setMethod method for the node.  The remainder of the definition is exactly the same as for the previous Python nodes.

The following is an example of a node that calls the makeBanner service for a PYHELLO component::

  n3=r.createCompoNode("","node3")
  p.edAddChild(n3)
  n3.setRef("PYHELLO")
  n3.setMethod("makeBanner")
  n3.edAddInputPort("p1",ts)
  n3.edAddOutputPort("p1",ts)

The second form is used to define a node that uses the same component as another node uses the createNode method of this other node.  
This method only has one argument, which is the node name.  
The remainder of the definition is identical to the definition for the previous form.

The following gives an example of a service node that makes a second call to the makeBanner service for the same component 
instance as the previous node::

  n4=n3.createNode("node4")
  p.edAddChild(n4)
  n4.setMethod("makeBanner")
  n4.edAddInputPort("p1",ts)
  n4.edAddOutputPort("p1",ts)

Definition of connections
''''''''''''''''''''''''''''
Obtaining a node port 
++++++++++++++++++++++++++++
Before links can be defined, it is almost always necessary to have Python objects representing the output port to be 
connected to the input port.  There are two ways of obtaining this object.

The first way is to retrieve the port when it is created using the edAddInputPort and edAddOutputPort methods.  
For example, we can then write::

  pin=n4.edAddInputPort("p1",ts)
  pout=n4.edAddOutputPort("p1",ts)

pin and pout are then the objects necessary to define links.

The second way is to interrogate the node and ask it for one of its ports by its name.  
This is done using the getInputPort and getOutputPort methods.
pin and pout can then be obtained as follows::

  pin=n4.getInputPort("p1")
  pout=n4.getOutputPort("p1")

Control link
++++++++++++++++++++++++++++
The edAddCFLink method for the context is used to define a control link between two nodes, transferring the two nodes to be 
connected to it as arguments.  For example, a control link between nodes n3 and n4 will be written::

  p.edAddCFLink(n3,n4)

Node n3 will be executed before node n4.

Dataflow link
++++++++++++++++++++++++++++
The first step in defining a dataflow link is to obtain port objects using one of the methods described above.
Then, the edAddLink method links an output port to an input port::

  pout=n3.getOutputPort("p1")
  pin=n4.getInputPort("p1")
  p.edAddLink(pout,pin)

Most of the time, when you need a dataflow link between two ports, you also need a control link between the nodes
of the ports. In this case you can use the method edAddDFLink::

  pout=n3.getOutputPort("p1")
  pin=n4.getInputPort("p1")
  p.edAddDFLink(pout,pin)

edAddDFLink is equivalent to edAddCFLink followed by edAddLink.

Initialising an input data port
'''''''''''''''''''''''''''''''''''''''''''''''
An input data port is initialised firstly by obtaining the corresponding port object.  There are then two methods of initialising it.

The first method initialises the port with a value encoded in XML-RPC.  The edInitXML method for the port is then used.  
The following is an example that initialises the port with the integer value 5::

  pin.edInitXML("<value><int>5</int></value>")

The second method initialises the port with a Python value.  The edInitPy method is then used.  
The following is an example that initialises this port with the same value::

  pin.edInitPy(5)

Specific methods can also be used for basic types:

- ``edInitInt`` for the int type
- ``edInitDbl`` for the double type
- ``edInitBool`` for the bool type
- ``edInitString`` for the string type

First example starting from the previous elements
'''''''''''''''''''''''''''''''''''''''''''''''''''
By collecting all previous definition elements, a complete calculation scheme identical to that given in the :ref:`schemaxml` chapter 
will appear as follows::

  import sys
  import pilot
  import SALOMERuntime
  import loader
  SALOMERuntime.RuntimeSALOME_setRuntime()
  r = pilot.getRuntime()
  p=r.createProc("pr")
  ti=p.getTypeCode("int")
  td=p.getTypeCode("double")
  #node1
  n1=r.createScriptNode("","node1")
  p.edAddChild(n1)
  n1.setScript("p1=p1+10")
  n1.edAddInputPort("p1",ti)
  n1.edAddOutputPort("p1",ti)
  #node2
  n2=r.createScriptNode("","node2")
  p.edAddChild(n2)
  n2.setScript("p1=2*p1")
  n2.edAddInputPort("p1",ti)
  n2.edAddOutputPort("p1",ti)
  #node4
  n4=r.createCompoNode("","node4")
  p.edAddChild(n4)
  n4.setRef("ECHO")
  n4.setMethod("echoDouble")
  n4.edAddInputPort("p1",td)
  n4.edAddOutputPort("p1",td)
  #control links
  p.edAddCFLink(n1,n2)
  p.edAddCFLink(n1,n4)
  #dataflow links
  p.edAddLink(n1.getOutputPort("p1"),n2.getInputPort("p1"))
  p.edAddLink(n1.getOutputPort("p1"),n4.getInputPort("p1"))
  #initialisation ports
  n1.getInputPort("p1").edInitPy(5)

Definition of composite nodes
'''''''''''''''''''''''''''''''''

.. _py_block:

Block
+++++++
A block is defined using the runtime createBloc method transferring the Block name to it as an argument.  The node is then 
attached to its definition context as an elementary node.  The following is an example Block definition in a calculation scheme::

  b=r.createBloc("b1")
  p.edAddChild(b)

Once the block has been created, all nodes and links possible in its context can be added.  
Repeating a part of the example above, we will get::

  n1=r.createScriptNode("","node1")
  b.edAddChild(n1)
  n1.setScript("p1=p1+10")
  n1.edAddInputPort("p1",ti)
  n1.edAddOutputPort("p1",ti)
  n2=r.createScriptNode("","node2")
  b.edAddChild(n2)
  n2.setScript("p1=2*p1")
  n2.edAddInputPort("p1",ti)
  n2.edAddOutputPort("p1",ti)
  b.edAddDFLink(n1.getOutputPort("p1"),n2.getInputPort("p1"))

.. _py_forloop:

ForLoop
++++++++
A Forloop is defined using the runtime createForLoop method, transferring the node name to it as an argument.  
The node is then attached to its definition context.  The following is an example ForLoop definition in a calculation scheme::

  l=r.createForLoop("l1")
  p.edAddChild(l)

The number of iterations in the loop to be executed will be initialised using the “nsteps” port that is initialised 
with an integer.  For example::

  ip=l.getInputPort("nsteps") 
  ip.edInitPy(3)

There is a special method for obtaining the “nsteps” port for the loop, namely edGetNbOfTimesInputPort.  Therefore, it can also be 
written as follows::

  ip=l.edGetNbOfTimesInputPort()
  ip.edInitPy(3)

Finally, a method called edSetNode will be used in the context of a loop, instead of the edAddChild method, so as to add one (and only one) node.  
The following is a small example definition of a node inside a loop::

  n1=r.createScriptNode("","node1")
  l.edSetNode(n1)
  n1.setScript("p1=p1+10")
  n1.edAddInputPort("p1",ti)
  n1.edAddOutputPort("p1",ti)

.. _py_whileloop:

WhileLoop
++++++++++
WhileLoop node is defined in practically the same way as a ForLoop node.  The only differences apply to creation and assignment 
of the end of loop condition.  The createWhileLoop method is used for creation.  The “condition” port is used for the condition.  
If looping takes place on a node, it is important to use a data link instead of a dataflow link.  
The following is an example of WhileLoop node definition with a Python script internal node.  
The condition is initialised to True and is then changed to False by the internal node.  This results in a link loop::

  wh=r.createWhileLoop("w1")
  p.edAddChild(wh)
  n=r.createScriptNode("","node3")
  n.setScript("p1=0")
  n.edAddOutputPort("p1",ti)
  wh.edSetNode(n)
  cport=wh.getInputPort("condition")
  cport.edInitBool(True)
  p.edAddLink(n.getOutputPort("p1"),cport)

There is a special method for obtaining the loop “condition” port:  edGetConditionPort.

.. _py_foreachloop:

ForEach loop
++++++++++++++++
A ForEach node is basically defined in the same way as any other loop node.  There are several differences.  
The node is created with the createForEachLoop method that has an additional argument, namely the data type managed by the ForEach.  
The number of ForEach branches is specified with the “nbBranches” port.  The collection on which the ForEach iterates is managed by 
connection of the “evalSamples” and “SmplsCollection” ports.

The following is an example definition of the ForEach node with a Python script internal node that increments 
the element of the collection by 3::

  fe=r.createForEachLoop("fe1",td)
  p.edAddChild(fe)
  n=r.createScriptNode("","node3")
  n.setScript("p1=p1+3.")
  n.edAddInputPort("p1",td)
  n.edAddOutputPort("p1",td)
  fe.edSetNode(n)
  p.edAddLink(fe.getOutputPort("evalSamples"),n.getInputPort("p1"))
  fe.getInputPort("nbBranches").edInitPy(3)
  fe.getInputPort("SmplsCollection").edInitPy([2.,3.,4.])

Special ports for the ForEach can be obtained using the following methods instead of getInputPort and getOutputPort:

- edGetNbOfBranchesPort for the “nbBranches” port
- edGetSamplePort for the “evalSamples” port
- edGetSeqOfSamplesPort for the “SmplsCollection” port

.. _py_switch:

Switch
++++++++
A switch node is defined in several steps.  The first two steps are creation and attachment to the context node.  
The node is created by calling the runtime createSwitch method with the name of the node as an argument.  The node is attached 
to the context node by calling the edAddChild method for a scheme or a block or edSetNode for a loop node.

The following is an example of a creation followed by an attachment::

  sw=r.createSwitch("sw1")
  p.edAddChild(sw)

The next step is to create an internal elementary or composite node by case.  The node for the default case is attached to 
the switch using the edSetDefaultNode method.  Nodes for other cases are attached to the switch using the edSetNode method, in 
which the first argument is equal to the value of the case (integer) and the second argument is equal to the internal node.

The following is an example of a switch with one script node for case “1” and another script node for the “default” case 
and a script node to initialise an exchanged variable::

  #init
  n=r.createScriptNode("","node3")
  n.setScript("p1=3.5")
  n.edAddOutputPort("p1",td)
  p.edAddChild(n)
  #switch
  sw=r.createSwitch("sw1")
  p.edAddChild(sw)
  nk1=r.createScriptNode("","ncas1")
  nk1.setScript("p1=p1+3.")
  nk1.edAddInputPort("p1",td)
  nk1.edAddOutputPort("p1",td)
  sw.edSetNode(1,nk1)
  ndef=r.createScriptNode("","ndefault")
  ndef.setScript("p1=p1+5.")
  ndef.edAddInputPort("p1",td)
  ndef.edAddOutputPort("p1",td)
  sw.edSetDefaultNode(ndef)
  #initialise the select port
  sw.getInputPort("select").edInitPy(1)
  #connection of internal nodes
  p.edAddDFLink(n.getOutputPort("p1"),nk1.getInputPort("p1"))
  p.edAddDFLink(n.getOutputPort("p1"),ndef.getInputPort("p1"))

The edGetConditionPort method can be used instead of getInputPort, to obtain the special “select” port for the Switch.

.. _py_optimizerloop:

OptimizerLoop
+++++++++++++++++++

The following is an example of OptimizerLoop with one python script as internal node. The algorithm
is defined by the class async in the python module myalgo2.py::

  ol=r.createOptimizerLoop("ol1","myalgo2.py","async",True)
  p.edAddChild(ol)
  n=r.createScriptNode("","node3")
  n.setScript("p1=3")
  n.edAddInputPort("p1",td)
  n.edAddOutputPort("p1",ti)
  ol.edSetNode(n)
  ol.getInputPort("nbBranches").edInitPy(3)
  ol.getInputPort("algoInit").edInitPy("coucou")
  p.edAddLink(ol.getOutputPort("evalSamples"),n.getInputPort("p1"))
  p.edAddLink(n.getOutputPort("p1"),ol.getInputPort("evalResults"))

.. _py_container:

Definition of containers
''''''''''''''''''''''''''''
A container is defined using the runtime createContainer method and it is then given a name using its setName method.  
The next step is to assign constraints to it by adding properties.  
The following is an example creation of a container named “A”::

  c1=r.createContainer()
  c1.setName("A")

A property is added to a container using its setProperty method that uses 2 arguments (character strings).  
The first is the property name.  The second is its value.  
The following is an example of this container “A” with constraints::

  c1=r.createContainer()
  c1.setName("A")
  c1.setProperty("container_name","FactoryServer")
  c1.setProperty("hostname","localhost")
  c1.setProperty("mem_mb","1000")

Once the containers have been defined, SALOME components can be placed on this container.  The first step to place the component 
of a SALOME service node is to obtain the component instance of this service node using the getComponent method for this node.  
The previously defined container is then assigned to this component instance using the setContainer method of the component instance.

If it is required to place the SALOME service defined above (node “node3”) on container “A”, we will write::

  n3.getComponent().setContainer(c1)

Since SALOME v7.5, there is a new type of container:
*Homogeneous Pool of SALOME containers* (HP container).
It is possible to create this type of container this way::

   my_hp_cont=r.createContainer("MyHPCont","HPSalome")

- "MyHPCont" : name of the container. Same result as my_hp_cont.setName("MyHPCont").
- "HPSalome" : type of container. Possible values are "HPSalome" (for a HP container)
  or "Salome" (for a classic container).

Node properties
'''''''''''''''''''''''''''
A property is added to an elementary or composite node (or is modified) using its setProperty method that has two 
arguments (character strings).  The first is the name of the property.  The second is its value.
The following is an example for the previous node “node3”::

  n3.setProperty("VERBOSE","2")

Datastream connections
'''''''''''''''''''''''''''
Datastream connections are only possible for SALOME service nodes as we have seen in :ref:`principes`.  
We firstly need to define the datastream ports in the service node.  An input datastream port is defined using 
the edAddInputDataStreamPort method.  An output datastream port is defined using the edAddOutputDataStreamPort method.  
These methods use the port name and the datastream type as arguments.

Some datastream ports (for example CALCIUM ports) must be configured with properties.  The port setProperty method will 
be used to configure them.
The following is an example definition of the SALOME service node with datastream ports.  This is the DSCCODC component 
located in the DSCCODES module in the EXAMPLES base.  The datastream ports are of the “CALCIUM_integer” type 
with time dependency::

  calcium_int=cata._typeMap['CALCIUM_integer']
  n5=r.createCompoNode("","node5")
  p.edAddChild(n5)
  n5.setRef("DSCCODC")
  n5.setMethod("prun")
  pin=n5.edAddInputDataStreamPort("ETP_EN",calcium_int)
  pin.setProperty("DependencyType","TIME_DEPENDENCY")
  pout=n5.edAddOutputDataStreamPort("STP_EN",calcium_int)
  pout.setProperty("DependencyType","TIME_DEPENDENCY")

Once the service nodes have been provided with datastream ports, all that remains is to connect them.  
This connection is made using the edAddLink method for the context node in the same way as for data links.  
The only difference is the type of ports transferred as arguments.

To complete our example, we will define a second service node and connect the datastream ports for these services::

  n6=r.createCompoNode("","node6")
  p.edAddChild(n6)
  n6.setRef("DSCCODD")
  n6.setMethod("prun")
  pin=n6.edAddInputDataStreamPort("ETP_EN",calcium_int)
  pin.setProperty("DependencyType","TIME_DEPENDENCY")
  pout=n6.edAddOutputDataStreamPort("STP_EN",calcium_int)
  pout.setProperty("DependencyType","TIME_DEPENDENCY")
  p.edAddLink(n5.getOutputDataStreamPort("STP_EN"),n6.getInputDataStreamPort("ETP_EN"))
  p.edAddLink(n6.getOutputDataStreamPort("STP_EN"),n5.getInputDataStreamPort("ETP_EN"))

Other elementary nodes
'''''''''''''''''''''''''''''''
SalomePython node
+++++++++++++++++++
A SalomePython node is defined in practically exactly the same way as a :ref:`pyfunc`.  The runtime createSInlineNode method is used 
instead of the createFuncNode and information about placement on a container is added in the same way as for a 
SALOME service node (setContainer method).

The following is an example similar to that given in :ref:`schemaxml`::

  n2=r.createSInlineNode("","node2")
  p.edAddChild(n2)
  n2.setScript("""
  import salome
  salome.salome_init()
  import PYHELLO_ORB
  def f(p1):
    print __container__from__YACS__
    machine,container=__container__from__YACS__.split('/')
    param={'hostname':machine,'container_name':container}
    compo=salome.lcc.LoadComponent(param, "PYHELLO")
    print compo.makeBanner(p1)
    print p1
  """)
  n2.setFname("f")
  n2.edAddInputPort("p1",ts)
  n2.getComponent().setContainer(c1)

.. _py_datain:

DataIn node
+++++++++++++++
A DataIn node is defined using the runtime createInDataNode method.  It uses two arguments, the first of which must be “” and 
the second the node name.  Node data are defined by adding output data ports to it using the edAddOutputPort method 
and transferring the data name and its type to it as arguments.  
The value of the data is initialised using the port setData method thus created by transferring the value encoded in 
XML-RPC to it (see :ref:`initialisation`).

The following is an example of the DataIn node that defines 2 double type data (b and c) and one file type data (f)::

  n=r.createInDataNode("","data1")
  p.edAddChild(n)
  pout=n.edAddOutputPort('a',td)
  pout.setData("<value><double>-1.</double></value>")
  pout=n.edAddOutputPort('b',td)
  pout.setData("<value><double>5.</double></value>")
  pout=n.edAddOutputPort('f',tf)
  pout.setData("<value><objref>f.data</objref></value>")
  
A value can be directly assigned to a data with a Python object, using the setDataPy method.  Example for a sequence::

  pout.setDataPy([1.,5.])

.. _py_dataout:

DataOut node
+++++++++++++++++
A DataOut node is defined using the runtime createOutDataNode method.  It uses two arguments, the first of which 
must be “” and the second the node name .  Node results are defined by adding input data ports to it using the edAddInputPort 
method with the result name and its type as arguments.  The results are saved in a file using the node setRef method with the 
file name as an argument.  
A result file is copied into a local file using the setData method for the port corresponding to the result with the 
file name as an argument.

The following is an example of the DataOut node that defines different types (double, int, string, doubles vector, file) of 
results (a, b, c, d, f) and writes the corresponding values in the g.data file.  
The result file will be copied into the local file myfile::

  n=r.createOutDataNode("","data2")
  n.setRef("g.data")
  p.edAddChild(n)
  n.edAddInputPort('a',td)
  n.edAddInputPort('b',ti)
  n.edAddInputPort('c',ts)
  n.edAddInputPort('d',tc3)
  pin=n.edAddInputPort('f',tf)
  pin.setData("monfich")

.. _py_studyin:

StudyIn node
++++++++++++++
A StudyIn node is defined using the runtime createInDataNode method.  It uses two arguments, the first of which must be “study” 
and the second the node name.  The associated study is specified by adding the “StudyID” property to the node using 
its setProperty method.  Node data are defined by adding output data ports using the edAddOutputPOrt method, transferring 
the name of the data and its type as arguments.  The data is initialised with the reference in the study, using the setData method 
for the port thus created, transferring a character string to it containing either the SALOME Entry or the path in the study 
tree structure.

The following is an example of the StudyIn node that defines 2 GEOM_Object type data (a and b).  The study is assumed to be 
loaded into memory by SALOME as StudyID 1.  Data a is referenced by one SALOME Entry.  Data b is referenced by a path in the 
study tree structure::

  n=r.createInDataNode("study","study1")
  p.edAddChild(n)
  n.setProperty("StudyID","1")
  pout=n.edAddOutputPort('a',tgeom)
  pout.setData("0:1:1:1")
  pout=n.edAddOutputPort('b',tgeom)
  pout.setData("/Geometry/Sphere_1")

.. _py_studyout:

StudyOut node
++++++++++++++
A StudyOut node is defined using the runtime createOutDataNode method.  It uses two arguments, the first of 
which must be “study” and the second the node name.  The associated study is specified by adding 
the “StudyID” property to the node using its setProperty method.  The name of the file in which the study will be 
saved is specified using the node SetRef method with the file name as an argument.  
The node results are defined by adding input data ports to it using the edAddInputPort method, transferring the data name 
and type as arguments.  The setData method for the port is used to associate the entry into the study to the result, transferring 
a character string to it that contains either the SALOME Entry or the path in the study tree structure.

The following contains an example of the StudyOut node that defines two GEOM_Object type results (a and b).  
The studyId of the study used is 1.  Result a is referenced by a SALOME Entry.  The result b is referenced by a path.  
The complete study is saved in the study1.hdf file at the end of the calculation::

  n=r.createOutDataNode("study","study2")
  n.setRef("study1.hdf")
  p.edAddChild(n)
  n.setProperty("StudyID","1")
  pout=n.edAddInputPort('a',tgeom)
  pout.setData("0:1:2:1")
  pout=n.edAddInputPort('b',tgeom)
  pout.setData("/Save/Sphere_2")

Save a calculation scheme in an XML file
------------------------------------------------------
A calculation scheme is saved in a file in the XML format using the saveSchema method for the calculation 
scheme, transferring the file name to it as an argument.  Before a calculation scheme constructed under Python 
can be saved in a consistent form in an XML file, all types defined in Python have to be added to the scheme types 
dictionary (see :ref:`typedict`).  The save will not do this automatically.

Proceed as follows to save the scheme p constructed above in the myscheme.xml file::

  p.saveSchema("monschema.xml")

The file thus obtained can then be loaded as in :ref:`loadxml`.

Several useful operations
------------------------------

Finding a node by its name
'''''''''''''''''''''''''''''''''''
A node (Python object) can be found, when all that is available is the calculation scheme object and 
the absolute name of the node, by calling the scheme getChildByName method, transferring the absolute name to it.

To find the Python script node defined in :ref:`pyscript`::

  n=p.getChildByName("node1")

To find node “node1” node in block “b1”::

  n=p.getChildByName("b1.node1")

This operation can also be used starting from a composite node provided that the relative node name is used.  
The previous example can be rewritten::

  n=b.getChildByName("node1")

Finding a port by its name
'''''''''''''''''''''''''''''''''''
The first step to find a node port by its name is to retrieve the node by its name.  An input data port is then found 
using the getInputPort method, and an output data port is found using the getOutputPort method.

The following is an example starting from the previous node n::

  pin=n.getOutputPort("p1")
  pout=n.getInputPort("p2")

Obtaining a port value
'''''''''''''''''''''''''''''''''''
The value of a port is obtained using its getPyObj method.  For example::

  print pin.getPyObj()
  print pout.getPyObj()

Obtaining the state of a node
'''''''''''''''''''''''''''''''''''
The state of a node is obtained using its getEffectiveState method (see possible values in :ref:`etats`).

Removing a node from its context
'''''''''''''''''''''''''''''''''''
A node can be removed from its context node using a context method.  The method name will be different 
depending on the context type.

- For a block or a calculation scheme, the edRemoveChild method will be used with the node to be removed as an argument::

    p.edRemoveChild(n)

- For a loop (ForLoop, WhileLoop or ForEachLoop) the edRemoveNode method will be used without any argument::
 
    l.edRemoveNode()

- The edRemoveChild method will be used for a Switch, with the internal node concerned as an argument::

    sw.edRemoveChild(nk1)

