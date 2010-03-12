
:tocdepth: 3

.. raw:: latex

  \makeatletter
  \g@addto@macro\@verbatim\small
  \makeatother


.. _yacsgen:

YACSGEN: SALOME module generator
==================================================

YACSGEN is a python package (module_generator) that automatically fabricates a SALOME module starting 
from a synthetic description of the components that it will contain.  This description is made in the python language.

The characteristics of these components are not general but they should facilitate integration of many scientific 
calculation components.

This generator does not control integration of a graphic MMI, but simply the calculation part. The main objective 
is to integrate a Fortran library in which calls to datastream ports (particularly Calcium) can be made.

How to get it
-----------------------------------------------------------------
It is a module (named YACSGEN) in the SALOME CVS TOOLS base that is distributed with main SALOME modules.

Supported versions and architectures
-----------------------------------------------------------------
YACSGEN uses python 2.4 functions but it has a compatibility mode with python 2.3.  It runs on a 32-bit and 64-bit 
architecture.

Installation
----------------------------
If you get the source archive, simply decompress and detar the archive (YACSGEN-x.y.tar.gz) 
and add the directory thus created to PYTHONPATH.

Description of a SALOME module
--------------------------------------------------------
A SALOME module is described using Python instructions and definitions contained in the Python module_generator package.

The first action to be done is to import these definitions::

     from module_generator import Generator,Module,PYComponent
     from module_generator import CPPComponent,Service,F77Component

If you want to import all definitions, you can do that::

     from module_generator import *

A SALOME module is described by giving its name <modulename> together with the list of its 
components (<components list>) and the name of the directory in which it will be installed (<prefix>).

Its description is in the following form::

  m=Module(<modulename>,components=<components list>,prefix=<prefix>)

The statement for a module named "mymodule" with a component C1 (see below for a description of components) that 
will be installed in the "Install" directory will be::

  m=Module("mymodule",components=[c1],prefix="Install")

Description of components
------------------------------------------------
Several types of components can be created:

- the C / C++ type
- the Fortran 77 type
- the Python type
- the Aster type.

All these types have a similar description.  We will begin with the C++ type and then describe the main differences for the other types.

C / C++ component
++++++++++++++++++++++++++++++++++++++++
Firstly, a C++ component has a name.  This name will be used subsequently when it is required to create instances of this component.  
Once compiled and installed, the SALOME module will contain a dynamic library named lib<compo>Engine.so, in which <compo> is the component name.  
A C++ component is implemented as a remote executable C++ object.

A C++ component has one or several services.  Each service has a name that is the name of the method of the C++ object 
that corresponds to the component.  Each service may have input and output dataflow ports and input and output datastream ports.

A first service with dataflow ports
"""""""""""""""""""""""""""""""""""""""""""""""""""""""
The only possible types for dataflow ports for the moment are:

- double:  scalar equivalent to a C double
- long:  scalar equivalent to a C long
- string:  equivalent to a C char* (character string with arbitrary length)
- file: a file object
- dblevec:  doubles vector
- stringvec:  strings vector
- intvec:  longs vector
- pyobj:  python object serialised with pickle (only works with components implemented in Python).

A port is described by a python tuple with length 2, the first value is the name of the port and the second value is the type name.  
Input ports are described by a list of these tuples as well as the output ports.

A small example is better than a long description.  A component named “mycompo” with a service named “myservice” which has 
an input dataflow port named “inputport” of the double type and an output data flow port with name “outputport” of the double 
type will have the following description::


      c1=CPPComponent("mycompo",
                      services=[
                                Service("myservice",
                                        inport=[("inputport","double"),],
                                        outport=[("outputport","double")],
                                       ),
                               ]
                     )

c1 is an intermediate Python variable that will be used to describe the list of components of a 
module: (components=[c1]) for a module with a single component.

In fact, this component is not particularly useful because during execution, it will take a double at the input to the 
execution and will provide a double at the output from the execution, but it does nothing in the meantime.  
Therefore, a content has to be added to it.  This content will be specified in two service attributes,  **defs** and **body**.  

- defs will contain the C++ definition code (for example, #include<iostream>)
- body will contain the C++ code that will be executed between the input and the output (for example, outputport=2*inputport).  

The final description becomes::

      c1=CPPComponent("mycompo",
                       services=[
                                 Service("myservice",
                                         inport=[("inputport","double"),],
                                         outport=[("outputport","double")],
                                         defs="#include <iostream>",
                                         body="outputport=2*inputport;",
                                        ),
                                ]
                     )

Adding datastream ports to the service
""""""""""""""""""""""""""""""""""""""""""""""
Datastream ports are added to the “myservice” service by adding **instream** and **outstream** attributes to the description.  
These attributes must be lists of triplets with the following elements:

1.  the port name
2.  the port type
3.  the time (“T”) or iteration (“I”) dependency mode (refer to :ref:`calcium` for further details)

Possible types are “CALCIUM_double”, “CALCIUM_integer”, “CALCIUM_real”, “CALCIUM_string”, “CALCIUM_logical” and “CALCIUM_complex”.

The description for an input datastream port and an output port in time dependency becomes::

      c1=CPPComponent("mycompo",
                      services=[
                                Service("myservice",
                                        inport=[("inputport","double"),],
                                        outport=[("outputport","double")],
                                        instream=[("porta","CALCIUM_double","T")],
                                        outstream=[("portb","CALCIUM_double","T")],
                                        defs="#include <iostream>",
                                        body="ouputport=2*inputport;",
                                        ),
                               ]
                     )

Obviously, calls to the CALCIUM library have to be added into body to make the service genuinely functional.

Adding a second service to the component
"""""""""""""""""""""""""""""""""""""""""""""""""
If a second service has to be added for the component, simply add another service description::

      c1=CPPComponent("mycompo",
                      services=[
                                Service("myservice",
                                        inport=[("inputport","double"),],
                                        outport=[("outputport","double")],
                                        instream=[("porta","CALCIUM_double","T")],
                                        outstream=[("portb","CALCIUM_double","T")],
                                        defs="#include <iostream>",
                                        body="ouputport=2*inputport;",
                                        ),
                                Service("serv2",
                                        inport=[("a","double"),("b","long")],
                                        outport=[("c","double")],
                                        body="c=b*a",
                                       ),
                               ]
                     )

In this description, a second service name “serv2” has been added with 2 input dataflow ports (a and b) and an output dataflow port (c).  
The service is reduced to its simplest expression:  it returns the product of its 2 inputs.

Assemble with external libraries
""""""""""""""""""""""""""""""""""""""""""""""""""""
We have seen that the **defs** and **body** attributes are sufficient to define the body of the service but it is often more practical 
to use external libraries rather than put everything into these 2 attributes. This is possible provided that everything necessary 
for the component link step is indicated in the **libs** and **rlibs** attributes of the component.

For example, we can have::


      c1=CPPComponent("mycompo",
                      services=[
                                Service("myservice",
                                        inport=[("inputport","double"),],
                                        outport=[("outputport","double")],
                                        defs="extern double myfunc(double);",
                                        body="outputport=myfunc(inputport);",
                                       ),
                               ],
                      libs="-L/usr/local/mysoft -lmybib",
                      rlibs="-Wl,--rpath -Wl,/usr/local/mysoft"
                      )

The **rlibs** attribute is not compulsory but it can be used to indicate a search path for dynamic libraries in execution.  
**libs** is used during the link phase.  **rlibs** is only used during execution, it avoids the need to set the LD_LIBRARY_PATH 
environment variable to find the dynamic library.

Adding includes
""""""""""""""""""""""""""""""""""""""""""""""""""""
Includes will be added using the **defs** attribute.  For example::

   defs="""#include "myinclude.h" """

The includes path will be specified in the **includes** attribute of the component in the following form::


   defs="""#include "myinclude.h"
   extern double myfunc(double);
   """
   c1=CPPComponent("mycompo",
                   services=[
                             Service("myservice",
                                     inport=[("inputport","double"),],
                                     outport=[("outputport","double")],
                                     defs=defs,
                                     body="outputport=myfunc(inputport);",
                                    ),
                            ],
                   libs="-L/usr/local/mysoft -lmybib",
                   rlibs="-Wl,--rpath -Wl,/usr/local/mysoft",
                   includes="-I/usr/local/mysoft/include",
                  )

Adding sources
""""""""""""""""""""""""""""""""""""""""""""""""""""
It is possible to add some source files with the **sources** attribute (a list of source files will be given).

For example, instead of using an external library, we could implement the function myfunc in a file 
named myfunc.cpp. The description will be::

   defs="""#include "myinclude.h"
   extern double myfunc(double);
   """
   c1=CPPComponent("mycompo",
                   services=[
                             Service("myservice",
                                     inport=[("inputport","double"),],
                                     outport=[("outputport","double")],
                                     defs=defs,
                                     body="outputport=myfunc(inputport);",
                                    ),
                            ],
                   sources=["myfunc.cpp"],
                   includes="-I/usr/local/mysoft/include",
                  )

Fortran component
++++++++++++++++++++++++++++++++++++++++
A Fortran component is described like a C++ component, except that there are a few differences.  Firstly, the F77Component 
definition object is used instead of the CPPComponent.  Then, a special additional interface is made in Fortran.  
It is assumed that Fortran functions are implemented in a library (dynamic or static) that will be linked with the component and 
that will have several entry points with the same names as the component services.  The call to this entry point will be added 
automatically after the C++ code supplied by the user in the **body** attribute.

This makes it possible to decouple practically the entire implementation of the Fortran component that will be in 
the external library or sources, from the implementation of the SALOME component that will only be used for encapsulation.

The following example will be used to specify these final concepts::

     c3=F77Component("compo3",
                     services=[
                               Service("s1",
                                       inport=[("a","double"),("b","long"),
				               ("c","string")],
                                       outport=[("d","double"),("e","long"),
				                ("f","string")],
                                       instream=[("a","CALCIUM_double","T"),
                                                 ("b","CALCIUM_double","I")],
                                       outstream=[("ba","CALCIUM_double","T"),
                                                  ("bb","CALCIUM_double","I")],
                                       defs="#include <unistd.h>",
                                       body="chdir(c);"
                                      ),
                              ],
                     libs="-L/usr/local/fcompo -lfcompo",
                     rlibs="-Wl,--rpath -Wl,/usr/local/fcompo"
                    )

The Fortran “compo3” component has dataflow and datastream ports like the C++ component.  The Fortran dynamic library 
that contains the Fortran entry point *s1* will be linked by means of the **libs** and **rlibs** attributes of the description.  
The Fortran component also supports the **includes** and **sources** attributes.
 
A piece of C++ code can be added before the call to the Fortran entry point.  This piece of code must be put into the **body** 
attribute with any definitions in **defs**.  In this case, we use the “c” input dataflow variable to change the directory with the call to chdir.

Python component
++++++++++++++++++++++++++++++++++++++++
A Python component is also described like a C++ component.  The only differences are in the Python object to be used to 
define it:  PYComponent instead of CPPComponent and in the content of the **defs** and **body** attributes that must contain 
Python code and not C++ (warning with indentation, the indentation of the complete block of code is automatically handled
but not the internal indentation of the block).

Example Python component::

      pyc1=PYComponent("mycompo",
                       services=[
                                 Service("myservice",
                                         inport=[("inputport","double"),],
                                         outport=[("outputport","double")],
                                         defs="import sys",
                                         body="      outputport=2*inputport;",
                                        ),
                                ]
                      )

The equivalent of the assembly with external libraries is done in this case with the possibility of importing external 
Python modules.  Simply add the **python_path** attribute to the description of the component to obtain this possibility.  
The value to be given is a list of directories that might contain modules to be imported.

Example::

     pyc1=PYComponent("mycompo",
                      services=[
                                Service("myservice",
                                        inport=[("inputport","double"),],
                                        outport=[("outputport","double")],
                                       ),
                               ],
                      python_path=["/usr/local/mysoft","/home/chris/monsoft"],
                     )

.. _aster:

Aster component
++++++++++++++++++++++++++++++++++++++++
*Code_Aster* is a software package for finite element analysis and numeric simulation in structural mechanics developed by EDF.

An Aster component is a component that is a little bit special because the software functions are implemented in Fortran but
they are activated by a command supervisor written in Python.  Finally, this supervisor executes a Python script but the data 
transfer between Python and Fortran and the integration of the command supervisor into a SALOME component have to be managed.

The start point is that it is assumed that there is an Aster installation that provides an aster python module in the form of 
an importable dynamic library (astermodule.so) and not a specific Python interpreter linked with this module, as is the case 
in the existing installation.

An Aster component is described as a Python component to which several important attributes have to be added.

- the **python_path** attribute:  this indicates the path of the directory containing the aster module (astermodule.so)
- the **aster_dir** attribute:  this indicates the path of the Aster installation directory
- the **argv** attribute:  this initialises command line parameters.  For example, it will be set equal to the value 
  of memjeveux (``argv=[“-memjeveux”,”10”]``) or rep_outils.

The following shows a small example description of an Aster component with a single service provided with 3 input dataflow 
ports, one output dataflow port, 7 input datastream ports and one output datastream port::

    c1=ASTERComponent("caster",
                      services=[
                                Service("s1",
                                        inport=[("a","double"),("b","long"),
					        ("c","string")],
                                        outport=[("d","double")],
                                        instream=[("aa","CALCIUM_double","T"),
                                                  ("ab","CALCIUM_double","I"),
                                                  ("ac","CALCIUM_integer","I"),
                                                  ("ad","CALCIUM_real","I"),
                                                  ("ae","CALCIUM_string","I"),
                                                  ("af","CALCIUM_complex","I"),
                                                  ("ag","CALCIUM_logical","I"),
                                                 ],
                                       outstream=[("ba","CALCIUM_double","T"),
                                                  ("bb","CALCIUM_double","I")],
                                      ),
                               ],
                      aster_dir="/local/chris/ASTER/instals/NEW9",
                      python_path=["/local/chris/modulegen/YACSGEN/aster/bibpyt"],
                      argv=["-memjeveux","10",
                            "-rep_outils","/local/chris/ASTER/instals/outils"],
                     )

Caution, do not use the name “aster” for the component because this name is reserved for the *Code_Aster* python module.  
If the name “aster” is used, the behaviour will be completely erratic.

Although its description is very similar to the behaviour of a Python component, there is an important difference in use.  
The Aster component needs the description of a command set to run.  This command set is transferred to each service of the 
component in the form of a text in an input dataflow port named “jdc” with type “string”.  Therefore after generation, this 
Aster component will have four input dataflow ports (“jdc”, “a”, “b”, “c”) and not three as indicated in the description.  
It is important not to forget to initialise the “jdc” port in the coupling file with a command set.

The command supervisor has been integrated into a SALOME component and the variables received in the dataflow ports are available 
during execution of the command set.  Similarly, values for output dataflow ports are defined by values of variables derived 
from execution of the command set.

**Caution with the execution mode**.  The command supervisor has 2 execution modes (PAR_LOT=”OUI” or PAR_LOT=”NON” that are 
specified in the DEBUT command) (PAR_LOT = BY_BATCH).  In PAR_LOT=”OUI” mode, it is compulsory to terminate the command set 
with a FIN (END) command which has the effect of interrupting execution.  This is not the preferred method of operation with YACS.  
It is preferable to use PAR_LOT=”NON” mode without adding the FIN command, which avoids interrupting the execution prematurely.

Dynamically importable Aster module and link with YACS
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
These two points are not handled by YACSGEN.  They must be processed separately in a context similar to the context of an Aster developer.

It is assumed that there is an Aster installation, that it is required to create a dynamically importable Python Aster module, and 
that a few commands are to be added to Aster to exchange data through YACS datastream ports.

To remain simple, three commands:  YACS_INIT, ECRIRE_MAILLAGE and LECTURE_FORCE are added, for which the catalogs are::

             YACS_INIT=PROC(nom="YACS_INIT",op=181, fr="YACS initialisation",
                                  COMPO=SIMP(statut='o',typ='I'),
                           )
             ECRIRE_MAILLAGE=PROC(nom="ECRIRE_MAILLAGE",op=78, fr="write mesh")
             LECTURE_FORCE=PROC(nom="LECTURE_FORCE",op=189, fr="read force")

The first YACS_INIT command initialises Aster in the YACS context.  It has a single simple keyword COMPO (integer type) that 
will be used to transfer the SALOME component identifier to other commands.  This identifier will be stored in a Fortran COMMON. 
It is essential for calls to subprograms CPLxx and CPExx that will be used in the other two ECRIRE_MAILLAGE and LECTURE_FORCE commands.

The other two commands do not have any keyword and they retrieve the identifier from the COMMON.

The operators will be written as follows (without the declarations):

.. code-block:: fortran

          SUBROUTINE OP0189 ( IER )
    C     COMMANDE:  LECTURE_FORCE
          include 'calcium.hf'
          COMMON/YACS/ICOMPO
          CALL cpldb(ICOMPO,CP_TEMPS,t0,t1,iter,'aa',1,n,ss,info)
          CALL cpldb(ICOMPO,CP_ITERATION,t0,t1,iter,'ab',1,n,zz,info)
          CALL cplen(ICOMPO,CP_ITERATION,t0,t1,iter,'ac',1,n,zn,info)
          CALL cplre(ICOMPO,CP_ITERATION,t0,t1,iter,'ad',1,n,yr,info)
          CALL cplch(ICOMPO,CP_ITERATION,t0,t1,iter,'ae',1,n,tch,info)
          CALL cplcp(ICOMPO,CP_ITERATION,t0,t1,iter,'af',1,n,tcp,info)
          CALL cpllo(ICOMPO,CP_ITERATION,t0,t1,iter,'ag',3,n,tlo,info)
          END

          SUBROUTINE OP0078 ( IER )
    C     COMMANDE:  ECRIRE_MAILLAGE
          include 'calcium.hf'
          COMMON/YACS/ICOMPO
          CALL cpeDB(ICOMPO,CP_TEMPS,t0,1,'ba',1,tt,info)
          CALL cpeDB(ICOMPO,CP_ITERATION,t0,1,'bb',1,tp,info)
          END

Finally, an astermodule.so dynamic library must be constructed, and all necessary Python modules must be placed in a directory 
that will be indicated in the **python_path** attribute.  Different methods can be used to obtain this result.  
The following Makefile is one of them:

.. code-block:: make

     #compiler
     FC=gfortran
     #SALOME
     KERNEL_ROOT_DIR=/local/chris/SALOME/RELEASES/Install/KERNEL_V5
     KERNEL_INCLUDES=-I$(KERNEL_ROOT_DIR)/include/salome
     KERNEL_LIBS= -L$(KERNEL_ROOT_DIR)/lib/salome -lCalciumC -lSalomeDSCSuperv \
                  -lSalomeDSCContainer -lSalomeDatastream -lSalomeDSCSupervBasic \
                  -Wl,--rpath -Wl,$(KERNEL_ROOT_DIR)/lib/salome
     #ASTER
     ASTER_ROOT=/local/chris/ASTER/instals
     ASTER_INSTALL=$(ASTER_ROOT)/NEW9
     ASTER_PUB=$(ASTER_ROOT)/public
     ASTER_LIBS = -L$(ASTER_INSTALL)/lib -laster \
             -L$(ASTER_PUB)/scotch_4.0/bin -lscotch -lscotcherr \
             -lferm -llapack -lhdf5
     SOURCES=src/op0078.f src/op0189.f
     CATAPY=catalo/ecrire_maillage.capy  catalo/lecture_force.capy

     all:pyth cata astermodule
     pyth:
       cp -rf $(ASTER_INSTALL)/bibpyt .
     cata: commande/cata.py
       cp -rf commande/cata.py* bibpyt/Cata
     commande/cata.py:$(CATAPY)
       $(ASTER_ROOT)/ASTK/ASTK_SERV/bin/as_run make-cmd
     astermodule:astermodule.so pyth
       cp -rf astermodule.so bibpyt
     astermodule.so: $(SOURCES)
       $(FC) -shared -o $@ $(SOURCES) $(KERNEL_INCLUDES) $(ASTER_LIBS) $(KERNEL_LIBS)

Modify command line parameters during execution
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
The **argv** attribute gives initial values to arguments such as “memjeveux” but these values are used by the generator to 
build the component and therefore remain constant afterwards during execution.

If you want to modify these values during execution, you need to add an input port named “argv” with type “string”.  The character 
string that will be given as the value of this port will be used by the component to modify the arguments of the command 
line (see :ref:`execaster` for an example use).

Management of the elements file
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
The finite elements file is automatically copied into the working directory under the name elem.1.  
The component uses the **aster_dir** attribute to locate the origin file.

Supported Aster versions
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
YACSGEN can function with Aster 9 and higher versions.

SALOME module generator
-----------------------------------------------------------
The SALOME module is created by a generator constructed from the description of the SALOME module (m) seen previously 
and a Python dictionary (context) that provides some environment parameters::

   g=Generator(m,context)

The following parameters are mandatory for this context:

- **prerequisites**:  indicates the path of a shell script that sets the environment variables of SALOME prerequisites
- **kernel**:  indicates the installation path of the SALOME KERNEL module
- **update**:  set equal to 1 at the moment (future development)

Example creation of generator:: 

     context={'update':1,
              "prerequisites":"/local/cchris/.packages.d/envSalome",
              "kernel":"/local/chris/SALOME/RELEASES/Install/KERNEL_V5"
              }
     g=Generator(m,context)

Once this generator has been created, simply call its commands to perform the necessary operations.

- SALOME module generation:  ``g.generate()``
- initialise automake:  ``g.bootstrap()``
- execute the configure script:  ``g.configure()``
- compilation:  ``g.make()``
- installation in the directory <prefix>:  ``g.install()``
- create a SALOME application in the directory **appli_dir**::

        g.make_appli(appli_dir,restrict=<liste de modules>,
                               altmodules=<dictionnaire de modules>)

These commands do not use any parameters except for make_appli that uses 3 parameters:

- **appliname**:  the name of the directory that will contain the SALOME application
- **restrict**:  a list of SALOME module names to put into the application.  By default, make_appli puts all SALOME modules 
  that it can detect into the application (neighbour directories of KERNEL with the same suffix as KERNEL.  If the directory 
  of the KERNEL module is called KERNEL_V5, then it will use GUI_V5, GEOM_V5, etc.). If restrict is provided, make_appli will 
  only use the modules listed.
- **altmodules**:  a dictionary of other modules.  The key gives the name of the module.  The corresponding value gives the path 
  of the module installation directory.  For example ``altmodules={"mymodule":"/local/chris/amodule"}``

Fabrication of the SALOME module
-----------------------------------------------------
The module will be fabricated by executing a Python file that contains its description, by inputting data into the generator  
and generator commands.

This gives something like the following for a module with a single Fortran component::

  from module_generator import Generator,Module
  from module_generator import PYComponent,CPPComponent,Service,F77Component

  context={"update":1,
                   "prerequisites":"/local/cchris/.packages.d/envSalome",
                   "kernel":"/local/chris/SALOME/RELEASES/Install/KERNEL_V5"
                 }
  c3=F77Component("compo",
                  services=[
                            Service("s1",
                                    inport=[("a","double"),
				            ("b","long"),
					    ("c","string")],
                                    outport=[("d","double"),("e","long"),
				             ("f","string")],
                                    instream=[("a","CALCIUM_double","T"),
                                              ("b","CALCIUM_double","I")],
                                    outstream=[("ba","CALCIUM_double","T"),
                                               ("bb","CALCIUM_double","I")],
                                    defs="#include <unistd.h>",
                                    body="chdir(c);"
                                   ),
                           ],
                  libs="-L/local/chris/modulegen/YACSGEN/fcompo -lfcompo"
                  rlibs="-Wl,--rpath -Wl,/local/chris/modulegen/YACSGEN/fcompo")

  m=Module("mymodule",components=[c1],prefix="Install")
  g=Generator(m,context)
  g.generate()
  g.bootstrap()
  g.configure()
  g.make()
  g.install()
  g.make_appli("appli",restrict=["KERNEL","GUI","YACS"])

If this description is in the mymodule.py file, all that is required is to execute::

   python mymodule.py

which has the effect of creating the module source directory (mymodule_SRC), the module installation directory (Install) and a 
SALOME application directory (appli).

Obviously, it must be possible to import the **module_generator** package either while being in the current directory or in the PYTHONPATH.

It is always preferable (although not essential) to clean up the working directory before executing the generator.

Using the component in a coupling
-----------------------------------------------------------------------------------------
Create the YACS coupling file
++++++++++++++++++++++++++++++++++++++++
A YACS coupling file is an XML file that describes how SALOME components previously installed in a SALOME application are coupled and executed.

See :ref:`schemaxml` for documentation about how to write a YACS XML file.

The following is an example of a YACS file using the Fortran component defined above:

.. code-block:: xml

  <proc>
  <container name="A"> </container>
  <container name="B"> </container>

  <service name="pipo1" >
    <component>compo</component>
    <method>s1</method>
    <load container="A"/>
    <inport name="a" type="double"/>
    <inport name="b" type="int"/>
    <inport name="c" type="string"/>
    <outport name="d" type="double"/>
    <outport name="e" type="int"/>
    <outport name="f" type="string"/>
    <instream name="a" type="CALCIUM_double"/>
    <instream name="b" type="CALCIUM_double"/>
    <outstream name="ba" type="CALCIUM_double"/>
    <outstream name="bb" type="CALCIUM_double"/>
  </service>
  <service name="pipo2" >
    <component>compo</component>
    <method>s1</method>
    <load container="B"/>
    <inport name="a" type="double"/>
    <inport name="b" type="int"/>
    <inport name="c" type="string"/>
    <outport name="d" type="double"/>
    <outport name="e" type="int"/>
    <outport name="f" type="string"/>
    <instream name="a" type="CALCIUM_double"/>
    <instream name="b" type="CALCIUM_double"/>
    <outstream name="ba" type="CALCIUM_double"/>
    <outstream name="bb" type="CALCIUM_double"/>
  </service>

  <stream>
    <fromnode>pipo1</fromnode><fromport>ba</fromport>
    <tonode>pipo2</tonode><toport>a</toport>
  </stream>
  <stream>
    <fromnode>pipo1</fromnode><fromport>bb</fromport>
    <tonode>pipo2</tonode><toport>b</toport>
  </stream>
  <stream>
    <fromnode>pipo2</fromnode><fromport>ba</fromport>
    <tonode>pipo1</tonode><toport>a</toport>
  </stream>
  <stream>
    <fromnode>pipo2</fromnode><fromport>bb</fromport>
    <tonode>pipo1</tonode><toport>b</toport>
  </stream>
  <parameter>
    <tonode>pipo1</tonode> <toport>a</toport>
    <value><double>23</double> </value>
  </parameter>
  <parameter>
    <tonode>pipo1</tonode> <toport>b</toport>
    <value><int>23</int> </value>
  </parameter>
  <parameter>
    <tonode>pipo1</tonode> <toport>c</toport>
    <value><string>/local/cchris/SALOME/SUPERV/YACS/modulegen/data1</string> </value>
  </parameter>
  <parameter>
    <tonode>pipo2</tonode> <toport>a</toport>
    <value><double>23</double> </value>
  </parameter>
  <parameter>
    <tonode>pipo2</tonode> <toport>b</toport>
    <value><int>23</int> </value>
  </parameter>
  <parameter>
    <tonode>pipo2</tonode> <toport>c</toport>
    <value><string>/local/cchris/SALOME/SUPERV/YACS/modulegen/data2</string> </value>
  </parameter>

  </proc>

In general terms, coupling uses two instances of the component compo (pipo1 and pipo2) of which the service s1 is executed.  
The datastream ports of these services are connected using fromnode, fromport, tonode, toport information in the stream sections.  
The dataflow ports are initialised by the parameter sections.  In particular, the working directory of each component instance 
is initialised through input port “c” of each component instance.  Each component instance is executed in a different container (A and B).  
These names are virtual.  SALOME will decide on the effective name of the containers at the time of the startup.  The following simply 
describes constraints on containers to be used.  In fact, there is only one constraint, which is that the containers have to be different.

Executing coupling
+++++++++++++++++++++++++++++++++++++++++++++
Once the coupling file has been written using a classical editor or the YACS graphic editor, execution can be started.

It takes place in several steps:

- start SALOME:  execute the runAppli script of the SALOME application (``./appli/runAppli –t``).  The application runs 
  as a background task until it is stopped.
- start coupling:  execute the YACS coupler in the environment of the running SALOME application (``./appli/runSession driver test.xml``) 
  with test.xml as the coupling file.
- stop the application:  ``./appli/runSession killSalome.py``

There are many coupling outputs:

- the output from the coupler itself.  If no execution error is returned to the coupler, the output will only contain one useful 
  item of information:  the name of containers started by SALOME to execute the components.  If execution errors are returned to 
  the coupler, they will be listed at the end of execution.
- container outputs:  these outputs are located in the /tmp directory with a name constructed based on the container name read 
  in the coupler output.

.. warning::

   when the application is stopped, the containers are killed, and this can cause information losses in their output files.

The working directory
++++++++++++++++++++++++++++++++++++++
Each component instance is hosted in a container.  Therefore all instances hosted in a container are executed in the same 
directory, which is the container directory.  Starting from version 4.1.1 of SALOME, the working directory of a container 
can be specified in the coupling file.  All that is necessary is to add the **workingdir** property to the container.  
The following gives a few examples:

.. code-block:: xml

   <container name="A">
     <property name="workingdir" value="/home/user/w1"/>
   </container>
   <container name="B">
     <property name="workingdir" value="$TEMPDIR"/>
   </container>
   <container name="C">
     <property name="workingdir" value="a/b"/>
   </container>

The container A is executed in directory “/home/user/w1”.  This directory will be created if it does not exist.  
The container B will be executed in a new temporary directory.  
Container C will be executed in the relative directory “a/b” (starting from the directory of the application used 
for the execution).  This directory will be created if it does not already exist.

Files management
++++++++++++++++++++++++++++
Components are dynamic libraries or Python modules, and they cannot be run in shell scripts.  For components that use input and 
output files, “files” ports can be specified in the coupling file through which file transfers will be made and appropriate 
local names will be given.  For example, a service that uses an input file a and produces an output file b will be declared as follows:

.. code-block:: xml

    <service name="pipo1">
      <component>caster</component>
      <method>s1</method>
      <inport name="a" type="file"/>
      <outport name="b" type="file"/>
    </service>

These ports can be initialised or connected to other “files” ports like ordinary ports.  For example, initialisation for the input 
file will be in the following form:

.. code-block:: xml

    <parameter>
      <tonode>pipo1</tonode> <toport>a</toport>
      <value><objref>/local/chris/tmp/unfichier</objref> </value>
    </parameter>

It is impossible to initialise an output file port directly.  A special node has to be used that collects outputs.  
A “dataout” node and the link between node “pipo1” and node “dataout” will be created:

.. code-block:: xml

    <outnode name="dataout" >
      <parameter name="f1" type="file" ref="myfile"/>
    </outnode>
    <datalink>
       <fromnode>pipo1</fromnode><fromport>b</fromport>
       <tonode>dataout</tonode> <toport>f1</toport>
    </datalink>

.. warning::

   it is impossible to use the “.” character in port names.  This prevents the use of names such as fort.8 that are 
   fairly frequent.  There is a simple workaround solution, which is to replace the “.” by the “:”character (therefore fort:8 in 
   our example) to obtain the expected result.  
   Obviously, names containing the “:” characters cannot be used.  They must be very rare.

.. _execaster:

Example execution of an Aster component
+++++++++++++++++++++++++++++++++++++++++++
There are a few unusual features when executing an Aster component that are presented below:

- handling the command set
- specification of parameters in the command line
- specification of a mesh file (.mail)
- specification of environment variables (also valid for other component types).

The following is a simplified example of a YACS scheme comprising a calculation node that should execute service s1 of 
the caster component (type Aster) with an environment variable, a mail file, a comm file and command line parameters.  
A more complete example is given in the directory Examples/ast1 in the distribution:

.. code-block:: xml

    <service name="pipo1" >
      <component>caster</component>
      <property name="MYENVAR" value="25"/>
      <method>s1</method>
      <load container="A"/>
      <inport name="jdc" type="string"/>
      <inport name="argv" type="string"/>
      <inport name="a" type="double"/>
      <inport name="fort:20" type="file"/>
      <outport name="d" type="double"/>
      <instream name="aa" type="CALCIUM_double"/>
      <outstream name="ba" type="CALCIUM_double"/>
    </service>

    <inline name="ljdc" >
       <script>
       <code>f=open(comm)</code>
       <code>jdc=f.read()</code>
       <code>f.close()</code>
       </script>
       <inport name="comm" type="string"/>
       <outport name="jdc" type="string"/>
    </inline>

    <parameter>
      <tonode>ljdc</tonode> <toport>comm</toport>
      <value><string>/home/chris/jdc.comm</string> </value>
    </parameter>

    <datalink>
       <fromnode>ljdc</fromnode><fromport>jdc</fromport>
       <tonode>pipo1</tonode> <toport>jdc</toport>
    </datalink>

    <parameter>
      <tonode>pipo1</tonode> <toport>argv</toport>
      <value><string>-rep_outils /aster/outils</string> </value>
    </parameter>

    <parameter>
       <tonode>pipo1</tonode> <toport>fort:20</toport>
       <value>
         <objref>/local/chris/ASTER/instals/NEW9/astest/forma01a.mmed</objref>
       </value>
    </parameter>

Firstly, the command set has to be specified.  As mentioned above (:ref:`aster`), an additional “jdc” “string” type port 
has to be declared and it has to be initialised or connected.  In this case, the jdc port is connected to an output port 
from a python node (ljdc) that will read the .comm file, for which the path is given to it by its comm input port.  
The component identifier is transferred to the YACS_INIT command by means of the “component” variable that is 
automatically added by the generator and is available to write the .comm file.

Brief example of .comm::

   DEBUT(PAR_LOT="NON")
   YACS_INIT(COMPO=component)
   ECRIRE_MAILLAGE()
   LECTURE_FORCE()

Before values of command line parameters can be specified, a component must have been created with a “string” type port named “argv”.  
A value then has to be given to this port.  In this case, we modify the tools directory path using the **rep_outils** parameter.

A mesh file (.mail) is specified to an Aster component by adding a file port to the calculation node:

.. code-block:: xml

      <inport name="fort:20" type="file"/>

The name of this file port must be the same as the local file name as expected by Aster.  Usually, Aster uses 
the fort.20 file as an input to LIRE_MAILLAGE.  As mentioned above, the dot in fort.20 cannot be used in a port 
name, and therefore it will be given the name fort:20.  A value will then have to be given to this port that will 
correspond to the path of the file to be used.  This is done by a parameter directive:

.. code-block:: xml

    <parameter>
       <tonode>pipo1</tonode> <toport>fort:20</toport>
       <value>
         <objref>/local/chris/ASTER/instals/NEW9/astest/forma01a.mmed</objref>
       </value>
    </parameter>

Environment variables are specified by using properties of the calculation node.  In this case, we define 
the MYENVAR environment variable with value 25.

Standalone components
--------------------------------------------------
Before SALOME version 4.1, the only method for integrating a component was to produce a dynamic library (\*.so) or a python 
module (\*.py).  This component is loaded by a SALOME executable named Container, either by dlopen in the case of the 
library or by import in the case of the Python module.  This method is a little constraining for calculation codes 
like *Code_Aster* or *Code_Saturne* that are executed in a particular environment, and preferably from a shell script.
 
Starting from version 4.1.3, a component can be integrated as an executable or shell script.  This new function is 
experimental at the moment and it will have to be tested more completely.  However, it can be used and module_generator 
was adapted (starting from version 0.3) to generate standalone components.  The following describes operations to be carried out 
to change to standalone mode for each type of component (C/C++, Python, Fortran or Aster).

C/C++ component
++++++++++++++++++++++++++++++++++++++++
All that is necessary to transform a standard C/C++ component in the form of a dynamic library into a standalone component, is 
to add two attributes to its description:

- the **kind** attribute:  by setting the value “exe”
- the **exe_path** attribute:  by setting its value equal to the path of the executable or the shell script that will be used 
  when the component is started

The following is an example of a C++ component modified to make it a standalone component::

      c1=CPPComponent("compo1",services=[
                      Service("myservice",inport=[("inputport","double"),],
                               outport=[("outputport","double")],
                             ),
                            ],
         kind="exe",
         exe_path="/local/SALOME/execpp/prog",
                     )

The path given for **exe_path** corresponds to an executable with the following source:

.. code-block:: cpp

   #include "compo1.hxx"

   int main(int argc, char* argv[])
   {
     yacsinit();
     return 0;
   }

It must be compiled and linked using the compo1.hxx include and the libcompo1Exelib.so library that are given 
in the installation of the module generated in include/salome and in lib/salome respectively.  

.. note::

   the SALOME module must be generated before compiling and linking the standalone component.
 
A more complete example is given in the distribution sources in the Examples/cpp2 directory.

The executable can be replaced by an intermediary shell script, but it is good to know that the call to yacsinit 
retrieves information necessary to initialise the component in the three environment variables (*SALOME_CONTAINERNAME*, 
*SALOME_INSTANCE*, *SALOME_CONTAINER*).

Fortran component
++++++++++++++++++++++++++++++++++++++++
The method for a Fortran component is exactly the same.  The same two attributes are added:

- The **kind** attribute:  by setting the value “exe”
- The **exe_path** attribute:  by setting its value equal to the path of the executable or the shell script that will 
  be used when the component is started

The following is an example of a standalone Fortran component::

     c3=F77Component("compo3",services=[
          Service("s1",inport=[("a","double"),("b","long"),
                               ("c","string")],
                       outport=[("d","double"),("e","long"),
                                ("f","string")],
                       instream=[("a","CALCIUM_double","T"),
                                 ("b","CALCIUM_double","I")],
                       outstream=[("ba","CALCIUM_double","T"),
                                  ("bb","CALCIUM_double","I")],
                             ),
                             ],
         kind="exe",
         exe_path="/local/SALOME/fcompo/prog",
                                     )

The path given for **exe_path** corresponds to an executable with the following source:

.. code-block:: fortran

       PROGRAM P
       CALL YACSINIT()
       END

It must be compiled and linked using the libcompo3Exelib.so library that is located in the installation of the module 
generated in lib/salome, and with the Fortran source containing subroutine S1.  
Refer to a more complete example in distribution sources in the Examples/fort2 directory.

Python component
++++++++++++++++++++++++++++++++++++++++
A very rudimentary generator has been coded for a Python component.  The only possible action is to add the **kind** 
attribute (with the value "exe"). The executable is automatically generated in the module installation.  
It cannot be replaced by a script, unless the installation is modified.

Standalone Aster component
++++++++++++++++++++++++++++++++++++++++
Slightly more work is necessary for an Aster component.  Three attributes have to be specified:

- the **aster_dir** attribute:  that gives the path of the *Code_Aster* installation
- the **kind** attribute:  with the “exe” value
- the **exe_path** attribute:  that gives the path of the shell script that will be used when the component is started

The following is an example description of a standalone Aster component::

      c1=ASTERComponent("caster",services=[
                  Service("s1",inport=[("argv","string"),("a","double"),
                                       ("b","long"),("c","string")],
                               outport=[("d","double")],
                               instream=[("aa","CALCIUM_double","T"),
                                         ("ab","CALCIUM_double","I"),
                                         ("ac","CALCIUM_integer","I"),
                                         ("ad","CALCIUM_real","I"),
                                         ("ae","CALCIUM_string","I"),
                                         ("af","CALCIUM_complex","I"),
                                         ("ag","CALCIUM_logical","I"),
                                       ],
                               outstream=[("ba","CALCIUM_double","T"),
                                          ("bb","CALCIUM_double","I")],
                 ),
         ],
         aster_dir="/aster/NEW9",
         kind="exe",
         exe_path="/home/SALOME5/exeaster",
         )

The “effective” command file always has to be specified in the XML coupling file. 

Example coupling with standalone components
++++++++++++++++++++++++++++++++++++++++++++++++++++
By collecting all the above elements, coupling of a standalone Aster component with a standalone Fortran component is 
written as follows::

  from module_generator import Generator,Module
  from module_generator import ASTERComponent,Service,F77Component

  context={'update':1,"prerequisites":"/home/SALOME5/env.sh",
          "kernel":"/home/SALOME5/Install/KERNEL_V5"}

  install_prefix="./exe_install"
  appli_dir="exe_appli"

  c1=ASTERComponent("caster",services=[
          Service("s1",inport=[("a","double"),("b","long"),
                               ("c","string")],
                       outport=[("d","double")],
                   instream=[("aa","CALCIUM_double","T"),
                             ("ab","CALCIUM_double","I"),
                             ("ac","CALCIUM_integer","I"),
                             ("ad","CALCIUM_real","I"),
                             ("ae","CALCIUM_string","I"),
                             ("af","CALCIUM_complex","I"),
                             ("ag","CALCIUM_logical","I"),
                         ],
                   outstream=[("ba","CALCIUM_double","T"),
                              ("bb","CALCIUM_double","I")],
                 ),
         ],
         kind="exe",
         aster_dir="/aster/NEW9",
         exe_path="/home/SALOME5/exeaster",
         )

  c2=F77Component("cfort",services=[
          Service("s1",inport=[("a","double"),("b","long"),
                               ("c","string")],
                       outport=[("d","double"),("e","long"),
                                ("f","string")],
                  instream=[("a","CALCIUM_double","T"),
                            ("b","CALCIUM_double","I")],
                  outstream=[("ba","CALCIUM_double","T"),
                             ("bb","CALCIUM_double","I"),
                             ("bc","CALCIUM_integer","I"),
                             ("bd","CALCIUM_real","I"),
                             ("be","CALCIUM_string","I"),
                             ("bf","CALCIUM_complex","I"),
                             ("bg","CALCIUM_logical","I"),
                         ],
                       defs="",body="",
                 ),
         ],
           exe_path="/home/SALOME5/fcompo/prog",
           kind="exe")

  g=Generator(Module("astmod",components=[c1,c2],prefix=install_prefix),context)
  g.generate()
  g.bootstrap()
  g.configure()
  g.make()
  g.install()
  g.make_appli(appli_dir,restrict=["KERNEL","YACS"])

The corresponding xml coupling file and Aster command file may be viewed in the distribution (Examples/ast2 directory).  
The complementary implantation elements are located in the fcompo directory (cfort component) and in the myaster directory (caster component).

