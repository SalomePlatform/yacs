
===========================
Work in progress
===========================

.. contents::

What is done ?
==============================
- XML parser with expat. No more xsd : problem with license
- support several elementary nodes : CompoNode,
  RefNode, FuncNode, ScriptNode
- support all composite nodes : Bloc, While, ForLoop, ForEachLoop, Switch
- support control links, data links and datastream links (DSC and Calcium datastream)
- support initialization from data in XML-RPC syntax
- support execution in threaded Executor : manage Python GIL
- the Runtime SALOME is connected to the SALOME LifeCycle to implement
  CompoNode. It's not yet possible to choose a computer or to express
  a resource constraint.
- dump of the schema state in graphviz form (.dot file)
- execution error management : an ElementaryNode signals an error by throwing
  an exception. This exception is catched by the executor which notifies the 
  ElementaryNode and makes propagate the error to all depending nodes and to
  the father. All depending nodes and the father are put in FAILED state. The
  ElementaryNode is put in ERROR state.
- Python interface : main C++ API wrapped with SWIG to be able to create schema from Python
  and to control execution
- a Salome loader in pyqt directory (salomeloader.py)

To do
===============================

Output a trace (PR)
--------------------------------------
It would be fine to have an execution trace (event sequence)
to be able to analyze the execution afterwards.

Make another runtime (MT)
---------------------------
To check the design

Save/load (PR)
--------------------
Be able to dump the schema state in a XML file.
Then to reload the state and restart from this state.

Two levels : with CORBA objects and without

Partial execution
-----------------------
Execute some selected nodes (directly or with backtrack)

Deployment, resource management
-------------------------------------
Not easy

Add other data types
-----------------------------------
struct, enum

Managing several schema 
------------------------------------------------------
One executor for all schemas or one executor by schema ?

Better management of outputs in ElementaryNode
-------------------------------------------------
Execution in threads : all outputs are mixed

Errors when reading the XML file
----------------------------------------------------
- Stop at first error (Exception)
- Collect all errors
- Check the overall validity

Errors when executing
---------------------------------------
- Stop as soon as possible when encountering an error
- Or execute most calculations possible

Need an execution option to specify that

To be able to restart an execution on another computer

Add macro nodes
----------------------------
Define macronodes
Be able to reference them several times

Discover base types from runtime (GUI and parser)
--------------------------------------------------------------
Runtime would have a service to give all known types (map name:type)
