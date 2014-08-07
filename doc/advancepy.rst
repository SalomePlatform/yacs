
.. _advancepy:

Advanced use of the Python programming interface
==========================================================================

Passing Python objects between calculation nodes
--------------------------------------------------
The YACS standard data model is used to exchange a number of data types (see :ref:`datatypes`) that are limited to 
types supported by CORBA.  The Python language allows to manipulate data types that are not managed by YACS.  
In particular, the Python dictionary with heterogeneous data types is not managed by the YACS data model.

However, Python dictionaries can be exchanged between some types of calculation nodes by using object references with 
a non-standard protocol.  The standard protocol is the IDL protocol that corresponds to serialisation of data 
managed by CORBA.  There are two other protocols (python and json) that use non-CORBA serialisation mechanisms 
that support more data types including dictionaries.  
The protocol name appears in the first part of the type Repository ID (before the first :).

The Python protocol
++++++++++++++++++++++
The Python protocol is based on serialisation done by the cPickle module (C implementation of the pickle module).  
All that is necessary is to define an object reference type with the Python protocol so that Python objects 
can be exchanged between Python inline nodes and with SALOME components implemented in Python.  
SALOME components implemented in Python that are required to support this exchange type must be designed 
to receive a character string that contains the serialised object.  The component is responsible for deserialising the object.  
In the other direction, the component is responsible for serialising the object and must return a character string 
for this object type.

Definition of the “Python object” type::

  tc1=p.createInterfaceTc("python:obj:1.0","PyObj",[])

Definition of two Python nodes that use this type::

  n2=r.createScriptNode("","node2")
  p.edAddChild(n2)
  n2.setScript("""
  p1={'a':1,'b':5.6,'c':[1,2,3]}
  """)
  n2.edAddOutputPort("p1",tc1)

  n3=r.createScriptNode("","node3")
  n3.setScript("""
  print "p1=",p1
  """)
  n3.edAddInputPort("p1",tc1)
  p.edAddChild(n3)
  p.edAddDFLink(n2.getOutputPort("p1"),n3.getInputPort("p1"))

Definition of a SALOME service node that uses this type::

  n1=r.createCompoNode("","node1")
  n1.setRef("compo1")
  n1.setMethod("run")
  n1.edAddInputPort("p1",tc1)
  n1.edAddOutputPort("p1",tc1)
  p.edAddChild(n1)
  p.edAddDFLink(n2.getOutputPort("p1"),n1.getInputPort("p1"))

The implementation of component compo1 must handle serialisation / deserialisation in the same 
way as the example in the run method described below::

  def run(self,s):
    o=cPickle.loads(s)
    ret={'a':6, 'b':[1,2,3]}
    return cPickle.dumps(ret,-1)
 
The json protocol
++++++++++++++++++++++
The json protocol is based on `JSON <http://www.json.org/>`_ (JavaScript Object Notation) 
serialisation / deserialisation instead of cPickle. JSON supports fewer data types and requires that 
the simplejson Python module should be installed, but it has the advantage that it is more interoperable.  
In particular, there are C++ libraries that serialise/deserialise the JSON.

All that is necessary to use this protocol in YACS is to replace python by json in the type definition.  For example::

  tc1=p.createInterfaceTc("json:obj:1.0","PyObj",[])

The rest is identical, except for implementation of the component that is as follows, using the same example as above::

  def run(self,s):
    o=simplejson.loads(s)
    ret={'a':6, 'b':[1,2,3]}
    return simplejson.dumps(ret)

Definition of inline Python components
--------------------------------------------------
Normally, a Python SALOME component must be developed outside YACS, either manually or using a SALOME module generator 
such as :ref:`yacsgen`.  
A SALOME component implemented in Python can be defined directly in a Python script.  This type of component 
can be useful in test phases, for example.

The first step consists of compiling the IDL Interface directly in the Python script, which has the effect of 
creating the necessary CORBA Python modules.  For example, the following shows how CORBA modules compo1 and compo1_POA 
are produced that contain the basic interface with a single run method::

  idlcompo="""
  #include "DSC_Engines.idl"
  #include "SALOME_Exception.idl"
  module compo1{
    interface base :Engines::Superv_Component {
      string run(in string s) raises (SALOME::SALOME_Exception);
    };
  };
  """
  m=omniORB.importIDLString(idlcompo,
                  ["-I/local/chris/SALOME/Install/KERNEL_V5/idl/salome"])

The second step consists of defining the body of component compo1 and therefore its run method.

The following is an example definition made in the body of the Python script::

  import compo1
  import compo1__POA

  class compo(compo1__POA.base,dsccalcium.PyDSCComponent):
    def run(self,s):
      print "+++++++++++run+++++++++++",s
      return "received "+s

  compo1.compo1=compo

The important point here is that SALOME finds the class with the same name that represents the component 
in module compo1 (which is why there is the last line).

The third step consists of defining a SALOME container local to the script because this component 
only exists in the script.  The container name “MyContainerPy” will be defined as follows::

  from omniORB import CORBA
  from SALOME_ContainerPy import SALOME_ContainerPy_i
  orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
  poa = orb.resolve_initial_references("RootPOA")
  poaManager = poa._get_the_POAManager()
  poaManager.activate()
  cpy_i = SALOME_ContainerPy_i(orb, poa, "MyContainerPy")

taking care to activate CORBA with poaManager.activate().

All that is necessary afterwards is to create a YACS container and to place a SALOME node in it, in the same 
way as for a standard component.

