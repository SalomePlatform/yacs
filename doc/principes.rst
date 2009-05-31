
:tocdepth: 3

.. _principes:

YACS general principles
===============================
A calculation scheme is constructed based on the calculation node concept.  
A calculation node represents an elementary calculation that can be the local execution of a Python 
script or the remote execution of a SALOME component service.

The calculation scheme is a more or less complex assembly of calculation nodes.

This assembly is made by connecting input and output ports of these calculation nodes.

Data are exchanged between nodes through ports.  They are typed.

Composite nodes:  Block, Loop, Switch are used to modularise a calculation scheme and define 
iterative processes, parametric calculations or branches.

Finally, containers can be used to define where SALOME components will be executed (on a network or in a cluster).

.. _datatypes:

Data types
----------------------
Data exchanged between calculation nodes through ports are typed.  
There are four categories of types:  basic types, object references, sequences and structures.  
User types can be defined by combining these basic elements.  
Many types are predefined either by YACS or by the components used such as GEOM or SMESH.

Basic types
'''''''''''''''''''''
There are 5 basic types: int, double, bool, string and file. They are predefined by YACS.

================= =====================================
YACS type           Comment
================= =====================================
int                   for integers
double                for double reals
bool                  for booleans
string                for character strings
file                  for files
================= =====================================

Object references
''''''''''''''''''''''''''
Object references are used to type CORBA objects managed by SALOME components. In general, these types 
are defined by the components that use them. All that is necessary to define a YACS object reference type, is to 
give it a name and associate it with the CORBA Repository ID.  
Some examples of pre-defined types are given below.

================= ==============================
YACS type          CORBA Repository ID 
================= ==============================
Study               IDL:SALOMEDS/Study:1.0
SObject             IDL:SALOMEDS/SObject:1.0
GEOM_Shape          IDL:GEOM/GEOM_Object:1.0
MESH                IDL:SALOME_MED/MESH:1.0
FIELD               IDL:SALOME_MED/FIELD:1.0
================= ==============================

Simple or multiple inheritance relations can be defined between these types.  
The following gives an example of the MED component.

================= ============================== =====================================
YACS type          Base type                          CORBA Repository ID
================= ============================== =====================================
FIELDDOUBLE         FIELD                           IDL:SALOME_MED/FIELDDOUBLE:1.0
================= ============================== =====================================

YACS also defines types for datastream ports:

================= =======================================================
YACS type               CORBA Repository ID
================= =======================================================
CALCIUM_integer    IDL:Ports/Calcium_Ports/Calcium_Integer_Port:1.0
CALCIUM_real       IDL:Ports/Calcium_Ports/Calcium_Real_Port:1.0
CALCIUM_double     IDL:Ports/Calcium_Ports/Calcium_Double_Port:1.0
CALCIUM_string     IDL:Ports/Calcium_Ports/Calcium_String_Port:1.0
CALCIUM_logical    IDL:Ports/Calcium_Ports/Calcium_Logical_Port:1.0
CALCIUM_boolean    IDL:Ports/Calcium_Ports/Calcium_Logical_Port:1.0
CALCIUM_complex    IDL:Ports/Calcium_Ports/Calcium_Complex_Port:1.0
================= =======================================================

A list of available types can be obtained by consulting catalogs of SALOME components:  GEOMCatalog.xml, 
SMESHCatalog.xml, MEDCalatog.xml, etc. These catalogs are in the XML format that is described in :ref:`schemaxml`.  
A view is also possible in the graphic user interface by viewing the components catalog.

Sequences
'''''''''''''''
A sequence type is used to type a list of homogenous elements.  The type contained in the list is the same for 
all elements.  A sequence type is defined by giving it a name and specifying the type of elements in the sequence.

The SALOME KERNEL defines sequence types for basic types.

================= ==============================
YACS type          Type of elements 
================= ==============================
dblevec               double
intvec                int
stringvec             string
boolvec               bool
================= ==============================

A sequence type can be defined for a sequence.  In this case, the element type is a sequence type.

Structures
''''''''''''''''
The structure type is used to type a data structured as a C structure.  This data contains named and typed members.  
A structure type is defined by giving it a name and specifying the list of members (name, type).

For example, the GEOM component defines a structure type with name “BCError” comprising a single member with name “incriminated” 
and type “ListOfLong”.  “ListOfLong” itself is a sequence of “int”.

Ports
-------------
A port can be considered as an interface of a node with the exterior.  There are three types of port:  control ports, 
data ports and data stream ports.  Each has different semantics.
 
Control ports
''''''''''''''''''''''''
This type of port is used to apply constraints on node execution chains.  An elementary or composite node 
usually has an input control port and an output control port.  A node for which the input control port is connected 
to the output control port of another node will not be executed until the second node is terminated.

Data ports
''''''''''''''''''''''''
This type of port is used to define data that will be used by a node when it is executed, and the data that will be produced 
by the node at the end of its execution.  A data port has a name, a direction (input, output) and the data contained in it 
have a type.  The order in which ports are defined is important because this order is used as the order of the arguments 
during the call for SALOME component services.

In general, an elementary calculation node is executed as follows:
 - control is given to the node through the input control port
 - input data are acquired by the node through the input data ports
 - the node is executed
 - output data are provided to output ports
 - control is returned by the node through the output control port.

An input data port can be initialized or connected to an output data port.

Datastream ports
''''''''''''''''''''''''
This type of port is used to exchange data during execution. Not all elementary nodes support this type of port.  
For the moment, this type of port is only supported by nodes related to SALOME components.  A datastream port has a name, 
a direction (input, output) and a type.  This type is not a data type directly but is rather the type of a CORBA object 
that manages the data exchange (see :ref:`progDSC` for further information).

Elementary calculation nodes
-------------------------------------
An elementary calculation node represents a particular calculation function (for example multiplication of 2 matrices).  
Every calculation node has a type. There can be one node type that executes a service of a SALOME component and another 
node type that executes a piece of Python script.  
Elementary calculation nodes are distributed into two categories: inline nodes that are executed locally in the YACS coupler, 
and service nodes that are executed remotely and correspond to the use of SALOME components.

Every calculation node has a name used as an identifier. This identifier must be unique in its definition context. A context is 
defined by a calculation scheme or a composite node.

A calculation node has an input control port and an output control port. These control ports are connected through the control flow.

A calculation node usually has input and output data ports. The number and type of data ports is determined by the type of 
calculation node. These data ports are connected through the data flow.

A calculation node may have properties. A property is a pair (name, value), where name is the name of the property and value 
is a character string that gives its value.

Python script inline node
''''''''''''''''''''''''''''''
A Python script node executes the Python code in a context in which variables are present with a value equal to the content 
of input data ports when this execution is started. For example, if the node has an input data port named “matrix”, the 
variable “matrix” will be present in the execution context of the script and its value will be the content of the port with the 
same name. At the end of execution of the script, the output data ports will contain the values of variables with the same 
name present in the execution context. These variables must necessarily be present.

When this type of node is an internal node in a loop, the execution context is reinitialised for each iteration of the loop.

Python function inline node
''''''''''''''''''''''''''''''
A Python function node executes a Python function, for which the arguments correspond to the node input data ports.  
The name of the function to be executed is given by a parameter of the node.  If such a node has 3 input data ports 
named 'a', 'b', 'c' and the name of the function is 'f', execution of the node will correspond to calling f(a,b,c) where a, b and c 
are the values of data ports with the same name.

Node output data are expected in return from the function in the form of a Python tuple. For example, if there are three 
output data ports named 'x', 'y', 'z', the function should terminate by "return x,y,z" where x, y and z are values 
for the output ports of the same name.

When this type of node is an internal node in a loop, the execution context is kept for every iteration of the loop, so 
that variables can be reused during iterations.

SALOME service node
''''''''''''''''''''''''''''''
A SALOME service node executes a service of a SALOME component. 
A service node can be defined in two ways:

 1. by indicating the component type (GEOM, SMESH, etc.) and the service to be executed
 2. by indicating an existing service node and the service to be executed

The second form exists because in some cases, it is required to use the state of the component at the end of execution of the 
first service to execute the second service. The state of the component is kept in a component instance that is created 
every time that the first form is used. If the second form is used, the existing instance is reused and a new component 
instance will not be created.

A service node has input and output data ports and it may also have input and output datastream ports.

A service node is loaded and executed on a SALOME container. This placement is managed using the YACS container concept 
(see :ref:`containers`) that is a slight abstraction of the SALOME container. 
Placement of the SALOME service can be managed by the same name to denote the YACS container on which it is to be placed. 
This is only possible with the first node definition form. If no placement information is given, the service will be placed 
on the default container of the SALOME platform:  FactoryServer container on the local machine.

The properties of a SALOME service node are converted into environment variables when the service is executed.

SalomePython node
''''''''''''''''''''''''''''''
A SalomePython node is a Python function node to which YACS provides the information necessary to run SALOME 
components and execute their services (in the Python execution context).  It is the address of the container into 
which the component is to be loaded and executed.  This address is given in the "_container_from_YACS_" variable 
in the form <machine name>/<container name>. Therefore, parameters can be set for this node using container placement 
information like a SALOME service node.

Restriction:  this type of node cannot execute a SALOME service with datastream ports.  The node is seen by YACS 
as being a Python node.  Datastream ports are not managed.

Data nodes
''''''''''''''''''''''''''''''
A Data node is used to define data (DataIn node) or to collect results (DataOut node) of a calculation scheme.

DataIn node
++++++++++++++++++
A DataIn node has output data ports only that are used to define input data for the calculation scheme. These data have a name (the port name), a type (the port type) and an initial value.

DataOut node
++++++++++++++++++
A DataOut node only has input data ports that are used to store output results from the calculation scheme.  These results have a name (the port name) and a type (the port type).  If the result is a file, a name can be given to the file into which the result file will be copied.

All values of node results can be saved in a file at the end of the calculation.

Study nodes
''''''''''''''''''''''''''''''
A Study node is used to relate the elements of a SALOME study to the data and results of a calculation scheme.

StudyIn node
++++++++++++++++++
A StudyIn node has output data ports only.  It is used to define data in the calculation scheme originating from a SALOME study.  The associated study is given by its SALOME StudyID.

A port corresponds to data stored in the associated study.  The data has a name (the port name), a type (the port type), and a reference that gives the entry into the study.  This reference is either a SALOME Entry (for example 0:1:1:2) or a path in the SALOME study tree (for example, /Geometry/box_1).

StudyOut node
++++++++++++++++++
A StudyOut node only has input data ports.  It is used to store results in a SALOME study.  The associated study is given by its SALOME StudyID.

A port corresponds to a result to be stored in an associated study.  The result has a name (the port name), a type (the port type), and a reference that gives the entry into the study.  This reference is either a SALOME Entry (for example 0:1:1:2) or a path in the SALOME study tree (for example, /Geometry/box_1).

The associated study may be saved in a file at the end of the calculation.

Connections
-----------------
Connections between input and output ports of elementary or composite nodes are made by creating links between these ports.

Control links
''''''''''''''''''''''''''''''
Control links are used to define an order in which nodes will be executed.  They relate an output port of one node to an input port of another node.  These two nodes must be defined in the same context.  The definition of the link consists simply of giving the name of the input side node and the name of the output side node.

Dataflow links
''''''''''''''''''''''''''''''
Dataflow links are used to define a dataflow between an output data port for one node and an input data 
port for another node.  There is no need for these nodes to be defined in the same context.  A dataflow link adds a control 
link between the two nodes concerned or between the appropriate parent nodes to respect the rule for definition of the 
control links.  The dataflow link guarantees consistency between the dataflow and the execution order.   
All that is necessary to define the link is to give the names of the input side node and port and the names of the output 
side node and port.  
The port types must be compatible (see :ref:`compatibility`).

Data links
''''''''''''''''''''''''''''''
In some cases (mainly loops), it is useful to be able to define dataflows without defining the associated control link 
as in the dataflow link.  The datalink is then used.  The definition is exactly the same as for the dataflow link.  
The port types must be compatible (see :ref:`compatibility`).

Datastream links
''''''''''''''''''''''''''''''
Datastream links are used to define a data stream between an output datastream port for one node and an input datastream port 
for another node.  These two nodes must be defined in the same context and it must be possible to execute them in parallel.  
Therefore, there must not be direct or indirect control link between them.  The link is defined by giving output node and port 
names and input node and port names.  The definition of the datastream links may be complemented by properties that 
define parameters of the behaviour of the DSC port that makes the data exchange (see :ref:`progDSC`).  
The port types must be compatible (see :ref:`compatibility`).

.. _compatibility:

Compatibility of data types
'''''''''''''''''''''''''''''''''''''''''
A data, dataflow or datastream link may only be created if the data type of the output port is compatible with the data type 
of the input port.  There are three forms of compatibility:

 - identity of types (for example double -> double)
 - specialization of types (for example FIELDDOUBLE -> FIELD)
 - type conversion (for example int -> double)

Compatibility by conversion
+++++++++++++++++++++++++++++++
Compatibility by conversion is applicable to basic types and to their derivatives (sequence, structure).  
The following conversions are accepted:

================= ============================== ====================================
YACS type          Conversion possible into              Comment
================= ============================== ====================================
int                 double
int                 bool                           true if int != 0 else false
================= ============================== ====================================

The conversion is also applicable to types constructed as a sequence of ints that may be converted into a 
sequence of doubles.  YACS controls the conversion.  This is also applicable to nested sequence of sequence, structure 
of structure, sequence of structure structures and types, etc.

Compatibility by specialization
+++++++++++++++++++++++++++++++
The compatibility rule is expressed differently for data (or dataflow) links and datastream links.

For data (or dataflow) links, the type of output data port must be derived from (or identical to) the type of input 
data port.  For example, an output data port with a FIELDDOUBLE type may be connected to an input data port with 
the FIELD type because the FIELDDOUBLE type is derived from the FIELD type (where FIELD is the basic type of FIELDDOUBLE).

The rule for datastream links is exactly the opposite of the rule for data links:  the type of the input datastream port 
must be derived from the type of the output port.  
At the moment there is no derived datastream type.  Therefore the only applicable rule is identity of types.

Multiple links
'''''''''''''''''''
Control ports support 1 to N and N to 1 multiple links.

Data ports support 1 to N and N to 1 multiple links.  1 to N links do not create any problem.  N to 1 links should be used with 
caution, because the final result depends on the order in which the exchanges are made.  This type of link will be reserved 
for looping back in iterative loops.  In this case, the order in which exchanges are made is perfectly reproducible. 

Datastream ports also support 1 to N and N to 1 multiple links.  1 to N datastream links do not create any particular problems:  data 
exchanges are simply duplicated for all connected input ports.  However, data exchanges for N to 1 datastream links will be 
overlapped in the single input port.  The final result may depend on the order in which exchanges are made.

Composite nodes
--------------------------------
There are several types of composite nodes, namely block, loop and switch nodes.  
A composite node may contain one or several nodes of an arbitrary type (elementary or composite).  
By default, the set of node inputs and outputs making up the composite node are accessible from the outside.  
It can be said that composite node inputs are composed of the set of internal node inputs.  The same is applicable for outputs.  
This is the white box concept.

The Bloc node
''''''''''''''
This is a group of nodes with dependency links between internal nodes.  
The Bloc is a white box (internal nodes are visible).  
A calculation scheme is a Bloc.  The Bloc is manipulated in a manner similar to an elementary node.  
It is provided with a single input control port and a single output control port.  
Consequently, two blocks connected through a dataflow data link will be executed in sequence, all nodes in the 
first block will be executed before starting the second block.

The ForLoop node
'''''''''''''''''''''
A loop is used to make iterations on an internal node.  
This internal node may be a composite node or an elementary node.  
Some internal node outputs may be explicitly looped back onto inputs of this internal node.  
A ForLoop loop executes the internal node a fixed number of times.  This number is given by a data port in the loop 
named “nsteps” or by a parameter of the loop of the same name. The current step number is accessible through
an output port of the loop named "index".

The While node
''''''''''''''''''''
A While loop executes the internal node as long as a condition is true.  
The value of the condition is given by a data port of the loop named “condition”.

The ForEach node
''''''''''''''''''''''
The ForEach node is also a loop, but it executes a loop body in parallel by iterating on one and only one data collection.  
A data collection is of the sequence type.  
An input data port of the ForEach node named “SmplsCollection” receives the data collection on which the loop iterates.
This data collection is typed.  The data type on which the loop iterates is unique.  The number of parallel branches managed 
by the loop is fixed by a parameter of the loop (input port named "nbBranches").  
If the collection size is 100 and this parameter is fixed at 25, the loop will execute 4 packets of 25 calculations in parallel.  
The internal node can access the current iteration of the data collection through the output data port from the loop named “SmplPrt”.

Typed data collections can be constructed at the output from the loop.  All that is necessary is to connect an output data 
port of the internal node to an input data port of a node outside the loop. The loop automatically constructs the data collection.

The Switch node
''''''''''''''''''''''
The Switch node performs the conditional execution (among N) of a node (composite, elementary).  
These nodes must have a minimum number of compatible inputs and outputs.  
The switch condition (integer, real) is used to switch execution of one node among N.  
The switch condition is given by an input data port of the Switch node named “select” or by a parameter of this node with the same name.

If the nodes are terminal (nothing is executed from their outputs), they do not need to have compatible outputs.  
Output ports used at the node output must be compatible with each other (i.e. they must be derived from a common generic 
type that can be used by another input node).

.. _containers:

Containers
---------------------
The SALOME platform executes its components after loading them in containers.  A SALOME container is a process managed 
by the platform that may be executed on any known machine.
A YACS container is used to define component placement constraints without necessarily precisely defining the machine 
to be used or the container name.
The YACS container has a name.  Constraints are given in the form of container properties.  
The current list of properties is as follows:

=================== ============= =============================================
Name                  Type            Type of constraint
=================== ============= =============================================
policy               "best",       Choose the best or the first or the next in 
                     "first" or    the list of machines, once other criteria  
                     "cycl"        have been applied. By default, YACS uses the “cycl” policy
                                   that selects the next machine in the list of known machines
container_name        string       if given imposes the SALOME container name
hostname              string       if given imposes the machine
OS                    string       if given restricts the choice of the OS
parallelLib           string       ??
workingdir            string      if given specifies the execution directory.  
                                  By default, the YACS run directory will be used 
                                  on the local machine and the $HOME directory will be used on remote machines.
isMPI                 bool         indicates if the container has to support MPI
mem_mb                int          minimum requested memory size
cpu_clock             int          minimum requested CPU speed
nb_proc_per_node      int          ??
nb_node               int          ??
nb_component_nodes    int          ??
=================== ============= =============================================

The hardware resources catalog
''''''''''''''''''''''''''''''''''''''''''
The list of hardware resources (machines) known to SALOME is given in the resources catalog, the CatalogResources.xml file 
that must be located in the directory of the SALOME application used.  
This file is in the XML format.  Each machine is described with the machine tag that has several attributes that characterize it.

================================== =========================== ==============================================
Characteristic                         XML attribute               Description
================================== =========================== ==============================================
nom                                 hostname                   the complete name:  this is the key that uniquely determines the machine
                                                               (for example : "nickel.ccc.cea.fr") 
alias                               alias                      character string to identify the machine (for example,  “pluton”)
access protocol                     protocol                   "rsh" (default) or "ssh"
access type                         mode                       interactive "i" or batch "b". By default "i"
user name                           userName                   user to be used to connect to the machine 
operating system                    OS
central memory size                 memInMB
clock frequency                     CPUFreqMHz
Number of nodes                     nbOfNodes
Number of processors per node       nbOfProcPerNode
SALOME application                  appliPath                  directory of the SALOME application to be used on this machine
mpi implementation                  mpi                        indicates which MPI implementation is used on this machine
                                                               ("lam", "mpich1",
                                                               "mpich2", "openmpi")
batch manager                       batch                      if the machine has to be used through a batch system, gives the 
                                                               name of the batch manager
                                                               ("pbs", "lsf", "slurm").
                                                               No default.
================================== =========================== ==============================================

The list of SALOME modules present on the machine can also be indicated.  By default, SALOME assumes that all components 
requested by YACS are present.  Each module present is given by the module sub-tag and its moduleName attribute.

The following is an example of a resource catalog::

  <!DOCTYPE ResourcesCatalog>
  <resources>
    <machine hostname="is111790" alias="is111790" OS="LINUX" CPUFreqMHz="2992" memInMB="1024" 
             protocol="rsh" mode="interactif" nbOfNodes="1" nbOfProcPerNode="1" >
    </machine>
    <machine hostname="is111915" alias="is111915" OS="LINUX" CPUFreqMHz="2992" memInMB="1024" 
             protocol="ssh" mode="interactif" nbOfNodes="1" nbOfProcPerNode="1" 
             appliPath="SALOME43/Run">
             <modules moduleName="GEOM"/>
    </machine>
  </resources>

.. _etats:

States of a node
-----------------------------
The possible states of a node when a calculation scheme is being edited are as follows:

=================== =============================================
State                 Comment
=================== =============================================
READY                The node is valid, ready to be executed   
INVALID              The node is invalid, the scheme cannot be executed
=================== =============================================

A node may be in the following states during execution of a calculation scheme:

=================== =============================================================
State                 Comment
=================== =============================================================
READY                the node is valid, ready to be executed
TOLOAD               the component associated with the node can be loaded
LOADED               the component associated with the node is loaded
TOACTIVATE           the node can be executed
ACTIVATED            the node is being executed
DONE                 execution of the node is finished with no error
ERROR                execution of the node is finished with error
FAILED               node in error because previous nodes were in error
DISABLED             execution of the node is disabled
PAUSE                execution of the node is paused
=================== =============================================================

.. _nommage:

Context sensitive naming of nodes
-------------------------------------
We have seen that elementary and composite nodes have a unique name in the definition context that corresponds 
to the parent node (calculation scheme or composite node).  Several sorts of naming are used to denote nodes in all 
possible situations:

 - local naming:  this is the name of the node in its definition context
 - absolute naming:  this is the name of the node seen from the highest level of the calculation scheme
 - relative naming:  this is the name of a node seen from a parent composite node.

The general rule is that absolute and relative names are constructed by concatenating local names of the node and 
its parents, and separating them with dots.

Consider the example of an elementary node with name “n” defined in a block name “b”, that is itself defined in a block name “c” 
itself defined at the highest level of the scheme. The local name of the node is “n”.  The absolute name is “c.b.n”.  
The relative name in block “c” is “b.n”.  

The same rule is applied for naming ports.  If node “n” has a port name “p”, then all that is necessary to obtain the port 
name is to add “.p” to the node name.

There is an exception to this rule that concerns the Switch node.  In this case, it is necessary to take account of case 
that is not a genuine node.  If it is said that block “b” in the previous example is a switch that has a case with a 
value of 1 and a default case, then the absolute name of node “n” in the case 1 will be “c.b.p1_n” and the absolute name of the node in 
the default case will be “c.b.default_n”.

.. _errorreport:

Error report
-------------------
Every node has an associated error report if its state is INVALID, ERROR or FAILED.  This report is in the XML format.

Elementary nodes produce a simple report that contains a single (error) tag with 2 attributes:

- node:  that gives the node name
- state:  that indicates its state.

The tag content is the text of the error.  For a Python script node, this will usually be the traceback of the exception 
encountered.  For a service node, it will be either the content of a SALOME exception or the content of a CORBA exception.

Composite nodes produce a composite report contained in a tag with the same name (error) with the same two node and state 
attributes.  The tag contains all error reports for erroneous child  nodes.

The following shows an error report for a division by zero in a Python node contained in a loop::

  <error node= proc state= FAILED>
  <error node= l1 state= FAILED>
  <error node= node2 state= ERROR>
  Traceback (most recent call last):
    File "<string>", line 1, in ?
  ZeroDivisionError: integer division or modulo by zero
  
  </error>
  </error>
  </error>

Execution trace
------------------
An execution trace is produced for each execution in a file named traceExec_<scheme name>, in which <scheme name> 
is the name given to the scheme.

Each line of the file represents an event related to a node.  It contains two character strings.  
The first is the node name.  The second describes the event.

The following shows a trace for the same example as above::

  n load
  n initService
  n connectService
  n launch
  n start execution
  n end execution OK
  n disconnectService
  l1.node2 load
  l1.node2 initService
  l1.node2 connectService
  l1.node2 launch
  l1.node2 start execution
  l1.node2 end execution ABORT, Error during execution
  l1.node2 disconnectService

Execution of concurrent branches
-------------------------------------
YACS can execute calculation nodes of a scheme simultaneously.  
However, simultaneous execution of a large number of nodes can saturate the system.  
The maximum number of simultaneous executions can be controlled by fixing the maximum number of threads used with the 
YACS_MAX_THREADS environment variable. By default, this value is equal to 50.

.. _archi:

YACS general architecture
------------------------------

YACS module implements API of a full SALOME module only for the schema execution.  The schema edition is done in the GUI process alone.  
For execution, YACS has a CORBA servant that implements Engines::Component CORBA interface (see SALOME KERNEL IDL interfaces).  
YACS GUI and YACS CORBA engine share YACS core libraries (engine and runtime): GUI uses them at schema design time, then a schema XML 
file is saved and passed to YACS CORBA API, and finally YACS core libraries execute the schema at YACS CORBA server side.

YACS GUI differs from standard full SALOME modules (such as Geometry or Mesh) in that it does not use SALOMEDS objects to create 
Object Browser representation of its data, and creates this representation in a way light SALOME modules do.  
This is done in order to avoid publishing lots of objects in SALOMEDS study just to create visual representation of data and 
thus to improve GUI performance.

YACS architecture scheme is shown on the picture below.

.. image:: images/general_architecture_0.jpg
     :align: center

The YACS module is a SALOME module with one document (study) per desktop.

YACS is composed of several packages. The main things are mentioned in the next sections.

Bases package
'''''''''''''''''''''''
Bases package contains common base classes (exception, threads, etc.) and constants.

Engine package
'''''''''''''''''''''''
Engine package consists of calculation schema generic classes (calculation nodes, control nodes, control and data 
flow links, etc.). 

Engine is in charge to:

    * edit,
    * check consistency,
    * schedule,
    * execute

graphs independently from the context (i.e. Runtime) the graph is destined to run.

SALOME Runtime package
'''''''''''''''''''''''
Runtime package provides implementation of YACS generic calculation nodes for SALOME platform. 
Runtime exists in a given Context.  

Runtime is in charge to:

    * treat physically the basic execution of elementary tasks in a given context,
    * transfer data in this context,
    * perform the physical deployment of the execution.

Runtime simply appears in Engine as an interface that a concrete Runtime must implement to be piloted by Engine.

The SALOME Runtime implements following nodes:

    * Inline function node.  A function inline node is implemented by a Python function.
    * Inline script node.  A script inline node is implemented by a Python script.
    * Component service node.  This is a calculation node associated with a SALOME component service.
    * CORBA reference service node.  Reference service node for CORBA objects.  This is a node that executes a CORBA service.
    * CPP node.  This is a C++ node (in process component), i.e. local C++ implementation - single process.

XML file loader package
''''''''''''''''''''''''''''''''
This is XML reader for generic calculation schema.

XML file loader provides

    * a possibility to load a calculation schema in memory by reading and parsing a XML file describing it,
    * an executable named driver that can be used to load and execute (see :ref:`execxml`) a calculation 
      schema given as a XML file (see :ref:`schemaxml`).

GUI design
''''''''''''''''''''''''''''''''
Goals of Graphic User Interface design are the following:

    * Provide a general mechanism for the synchronisation of several views (tree views, 2D canvas views, edition dialogs).  
      For this goal, a subject/observers design pattern is used: several observers can attach or detach themselves to/from the subject.  
      The subject send update events to the lists of observers and does not know the implementation of the observers.  The observers 
      correspond to the different views in case of YACS.
    * Provide an interface of Engine for edition with a general mechanism for undo-redo (in a future version!).
    * Be as independent as possible of Qt (and SALOME) to allow a potential re-use of YACS GUI outside SALOME.


