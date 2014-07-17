
:tocdepth: 3

.. _cppsalome:

===========================================================
Guide for the development of a SALOME module in C++
===========================================================

The purpose of this document is to describe the different steps in the development of a SALOME module in C++.  
It follows on from the :ref:`pysalome` document, that documents the PYHELLO module, and it uses the same 
approach:  step by step construction of a HELLO module.  Since many points are not repeated, it is recommended 
that this document should be read first.

Steps in the construction of the example module
====================================================
The component chosen to illustrate the C++ construction process is the same as the process chosen to illustrate 
construction of the Python module:  therefore, it will implement the same Corba idl interface.  It will be 
completed by a graphic GUI written in Qt.

The various steps of the development will be as follows:
 - create a module structure
 - create a SALOME component that can be loaded by a C++ container
 - configure the module so that the component is known to SALOME
 - add a graphic GUI
 - make the component usable in the YACS module.

Creating the module tree structure
=======================================
We will start by simply putting a SALOME component written in C++ and that can be loaded by a C++ container, into the 
example module.  Therefore, all that is necessary is an idl interface and a C++ implantation of the component. 
We need to reproduce the following standard file tree structure, to use it in a SALOME module::

  + HELLO1_SRC
    + CMakeLists.txt
    + adm_local
      + CMakeLists.txt
      + cmake_files
        + CMakeLists.txt
        + FindSalomeHELLO.cmake
    + bin
      + CMakeLists.txt
      + VERSION
      + runAppli.in
      + runSalome.py
    + idl
      + CMakeLists.txt
      + HELLO_Gen.idl
    + src
      + CMakeLists.txt
      + HELLO
        + CMakeLists.txt
        + HELLO.cxx 
        + HELLO.hxx 
    + doc

This is done by copying the PYHELLO tree structure, and modifying PYHELLO to HELLO if necessary::

    cp -r PYHELLO1_SRC HELLO1_SRC
    cd HELLO1_SRC
    mv idl/PYHELLO_Gen.idl idl/HELLO_Gen.idl
    mv src/PYHELLO src/HELLO


IDL interface
==================
We modify the HELLO_Gen.idl file in the idl directory:  the defined module is named HELLO_ORB, and the interface 
is named HELLO_Gen.  The service provided remains the same:  starting from a character string supplied as the 
single argument, a character string derived from the concatenation of “Hello” and the input string is returned.  
This service is specified by the hello function.

A documentation utility based on doxygen has been implemented to compile a documentation of corba services 
starting from comments located in idl files. Therefore, we will add few comments to our idl, respecting the 
doxygen formalism.  
A doxygen comment begins with "/\*" and ends with "\*/".  
They are grouped by module or subject, to provide a minimum structure for generated pages.  
We will use the following directive in our example::

  \ingroup EXAMPLES

specifying that the generated documentation forms part of the EXAMPLES group (please refer to 
the http://www.doxygen.org site for further information about doxygen).

Finally, we will update the Makefile with the new component name::

    IDL_FILES = HELLO_Gen.idl


C++ implementation
==================
Sources
-----------
The C++ implementation of our CORBA HELLO module (HELLO_Gen idl interface) is made in the src/HELLO directory::

    HELLO.hxx
    HELLO.cxx

The following inclusions are necessary at the beginning of the header of our module (HELLO.hxx)::

    #include <SALOMEconfig.h>
    #include CORBA_SERVER_HEADER(HELLO_Gen)
    #include "SALOME_Component_i.hxx"

The SALOMEconfig.h file contains a number of definitions useful for making the code independent from 
the version of CORBA used. SALOME_Component_i.hxx contains the interface of the C++ implementation class 
of the SALOME basic component (idl Engines::EngineComponent). Finally, the CORBA_SERVER_HEADER macro 
makes inclusion file names independent of the implementation of the CORBA ORB.

The next step is to define an implementation class called HELLO, derived from POA_HELLO_ORB::HELLO_Gen (abstract class 
generated automatically by CORBA during the compilation of the idl) and Engines_Component_i (because 
the HELLO_Gen idl interface is derived from Engines::EngineComponent like every SALOME component).  
This class contains a constructor whose arguments are imposed by SALOME, a virtual destructor, hello, goodbye and copyOrMove methods providing the required service::

    class HELLO:
      public POA_HELLO_ORB::HELLO_Gen,
      public Engines_Component_i
    {
    public:
    HELLO(CORBA::ORB_ptr orb,
      PortableServer::POA_ptr poa,
      PortableServer::ObjectId * contId,
      const char *instanceName,
      const char *interfaceName);
    virtual ~HELLO();
    HELLO_ORB::status hello  ( SALOMEDS::Study_ptr study, const char* name );
    HELLO_ORB::status goodbye( SALOMEDS::Study_ptr study, const char* name );
    void              copyOrMove( const HELLO_ORB::object_list& what,
				  SALOMEDS::SObject_ptr where,
				  CORBA::Long row, CORBA::Boolean isCopy );

    };

The hello and goodbye functions use a char* as an argument and return status of the operation.
The list of the statuses is defined in the HELLO_Gen.idl, see status enumeration for details.

Finally, we supply the standard interface of the HELLOEngine_factory function that will be called by the “FactoryServer C++” 
to load the HELLO component::

    extern "C"
    PortableServer::ObjectId * HELLOEngine_factory(CORBA::ORB_ptr orb,
                                                   PortableServer::POA_ptr poa,
                                                   PortableServer::ObjectId * contId,
                                                   const char *instanceName,
                                                   const char *interfaceName);


The definitions of the constructor and the HELLOEngine_factory instantiation function (both normalized!),
hello, goodbye and copyOrMove are given in the source file (HELLO.cxx)::	

	HELLO_ORB::status HELLO::hello( SALOMEDS::Study_ptr study, const char* name )
	{
	...
	}

	HELLO_ORB::status HELLO::goodbye( SALOMEDS::Study_ptr study, const char* name )
	{
	...
	}

	void HELLO::copyOrMove( const HELLO_ORB::object_list& what,
        	                SALOMEDS::SObject_ptr where,
                	        CORBA::Long row, CORBA::Boolean isCopy ) 
	{
	...
	}

CMakeLists.txt
--------------
Create and add library to the project::	

    # --- options ---
    # additional include directories
    INCLUDE_DIRECTORIES(
      ${KERNEL_INCLUDE_DIRS}
      ${OMNIORB_INCLUDE_DIR}
      ${PROJECT_BINARY_DIR}
      ${PROJECT_BINARY_DIR}/idl
    )
    # libraries to link to
    SET(_link_LIBRARIES
      ${OMNIORB_LIBRARIES}
      ${KERNEL_SalomeIDLKernel}
      ${KERNEL_OpUtil}
      ${KERNEL_SalomeContainer}
      SalomeIDLHELLO
    )
    # --- headers ---
    # header files / no moc processing
    SET(HELLO_HEADERS
      HELLO.hxx
    )
    # --- sources ---
    # sources / static
    SET(HELLO_SOURCES
      HELLO.cxx
    )
    # --- rules ---
    ADD_LIBRARY(HELLOEngine ${HELLO_SOURCES})
    TARGET_LINK_LIBRARIES(HELLOEngine ${_link_LIBRARIES} )
    INSTALL(TARGETS HELLOEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
    INSTALL(FILES ${HELLO_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
	
Review some of components:

- HELLO_HEADERS contains the header files.
- HELLO_SOURCES defines the name of source files
- HELLOEngine - library name.
- SALOME_INSTALL_LIBS path defines the directories in which library will be install.
- Command INCLUDE_DIRECTORIES used to add /bin directories of some modules and packages.
- Variable _link_LIBRARIES contains path to libraries of dependent modules and packages.

Controlling the component from Python (TUI mode)
=====================================================
When the module is compiled, the lib target of the Makefile in /idl provoked generation of a Python 
stub (stub at the customer end generated from the idl and providing an interface in the client language – in this case Python).  
Specifically, a HELLO_ORB python module containing a classe_objref_HELLO_Gen is created and used to call services of our 
C++ module from Python.  To put this into application, we run SALOME in TUI mode::

    runSalome --modules=HELLO -t --pinter --logger --killall

We import the LifeCycle module from the Python window, and use its services to load our component into the FactoryServer C++ container::

    >>> import LifeCycleCORBA
    >>> lcc = LifeCycleCORBA.LifeCycleCORBA()
    >>> import salome
    >>> salome.salome_init()
    createNewStudy
    []
    extStudy_1 1
    >>> import HELLO_ORB
    >>> hello = lcc.FindOrLoadComponent("FactoryServer", "HELLO")

HELLO_ORB has to be imported before FindOrLoadComponent is called, so that a typed object can be 
returned (“narrowing” operation). Otherwise, the returned object is generic of the 
Engines::EngineComponent type.  
Let us check that hello object is correctly typed, and we will call the hello service::

    >>> print hello
    <HELLO_ORB._objref_HELLO_Gen instance at 0x8274e94>
    >>> status=hello.hello(salome.myStudy, "Nicolas")
    >>> print status
    OP_OK

The previous commands were grouped in the test function of the /bin/runSalome.py script.

Graphic interface
===================
Introduction
----------------
To go further with the integration of our module, we will add a graphic interface (developed in Qt) that is 
integrated into the SALOME application interface (IAPP).  We will not describe operation of the SALOME IAPP herein, 
but in summary, the IAPP manages an event loop (mouse click, keyboard, etc.) and after processing these events 
redirects them towards the active module (the principle is that **a single** module is active at a given moment.  
When a module is activated, its Graphic User Interface is dynamically loaded).  
Therefore the programmer of a module GUI defines methods to process transmitted events correctly.  
The most important of these events are OnGUIEvent(), OnMousePress(), OnMouseMove(), OnKeyPress(), DefinePopup(), CustomPopup().

Strictly speaking, the GUI library is optional for each SALOME module.
In some cases it's enough to implement CORBA engine only. Then,
the services of the module will be avaiable in a CORBA environment.
The module can be loaded to the SALOME container and its services
can be used in the SALOME supervision computation schemas, in Python
scripts or/and in C++ implementation of other modules.

A GUI library is necessary only if it is planned to access the module
functionality from the SALOME GUI session via menu actions, dialog boxes
and so on. 

- src/HELLOGUI/HELLOGUI.h
- src/HELLOGUI/HELLOGUI.cxx

These files provide the implementation of a GUI library of
the HELLO module. In particular, these files specify menus, toolbars,
dialog boxes and other such staff.

- src/HELLOGUI/HELLO_msg_en.ts
- src/HELLOGUI/HELLO_msg_fr.ts
- src/HELLOGUI/HELLO_icons.ts

These files provide a description (internationalization) of GUI
resources of the HELLO module. HELLO_msg_en.ts provides an English
translation of the string resources used in a module (there can be also
translation files for other languages, for instance French; these files
are distinguished by the language suffix). HELLO_icons.ts
defines images and icons resources used within the GUI library of
HELLO module. Please refer to Qt linguist documentation for more
details.

- resources

This optional directory usually contains different resources files
required for the correct operation of SALOME module.

- resources/HELLO.png
- resources/handshake.png
- resources/goodbye.png
- resources/testme.png

These are different module icon files. HELLO.png file provides main icon
of HELLO module to be shown in the SALOME GUI desktop. Other files are
the icons for the functions implemented by the module; they are used
in the menus and toolbars.


- resources/HELLOCatalog.xml.in

The XML description of the CORBA services provided by the HELLO
module. This file is parsed by SALOME supervision module (YACS) to generate
the list of service nodes to be used in the calculation schemas. The
simplest way to create this file is to use Catalog Generator utility
provided by the SALOME KERNEL module, that can automatically generate
XML description file from the IDL file. In GUI, this utility is available
via the Tools main menu.




Syntax naming rules
=============================
A number of naming rules were used in the above description.  This chapter gives more details about these rules.  
They are not all compulsory, but they make it easy to understand the program if they are respected!

======================== ======================== ===================== =============================================================================
  Rules                    Formalism                HELLO example              Comment                               
======================== ======================== ===================== =============================================================================
 Module name              <Module>                HELLO                 This is the name that appears in the modules catalog
 CVS base                 <Module>                EXAMPLES              If the cvs base contains several modules, another name will be used.
 Source directory         <Module>_SRC            HELLO1_SRC            Index 1 is used because several versions of the module are provided.
 Idl file                 <Module>_Gen.idl        HELLO_Gen.idl 
 CORBA module name        <Module>_ORB            HELLO_ORB             Avoid the use of the module name (conflicts)
 CORBA interface name     <Module>_Gen            HELLO_Gen             The idl compilation generates an abstract class POA_<Module>_ORB::<Module>_Gen
 Source file              <Module>.cxx            HELLO.cxx             In the /src/<Module> directory
 Implementation class     <Module>                HELLO                 This class inherits from POA_HELLO_ORB::HELLO_Gen
 Instantiation function   <Module>_Engine_factory HELLO_Engine_factory  This function is called by the SALOME Container
 Modules catalog          <Module>Catalog.xml     HELLOCatalog.xml      In /resources
 C++ library name         lib<Module>Engine       HELLO-Engine          In the /src/<Module> directory
 GUI C++ name             lib<Module>GUI          libHELLOGUI           In the /src/<Module>GUI directory
 Environment variable     <Module>_ROOT_DIR…      HELLO_ROOT_DIR  
 ...                      ...                      ...                   ...                                   
======================== ======================== ===================== =============================================================================

