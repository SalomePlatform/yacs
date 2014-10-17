
:tocdepth: 3

.. _rappels:

================================================================
Reminders about the SALOME platform
================================================================

Definitions
================================================================

   - **SALOME module**:  a SALOME module is a group of SALOME components. It is located in a directory with a standard structure.
     Its components are described in an XML file named <module>Catalog.xml.
   - **SALOME component**:  this is the SALOME equivalent of a calculation code.  It may be in the form of a dynamic 
     library or a Python module.
   - **SALOME service**:  each component has one or several services.  A service is the equivalent of a Fortran 
     subroutine, a C function, a C++ method or a Python method.  Only component services can be coupled with the YACS coupler.  
     Coupling is done connecting input-output ports with which services are provided.  There are two types of these 
     ports, dataflow and datastream.
   - **Dataflow port**:  a dataflow port is a connection port for exchanging data.  It has a name that is the name of the 
     exchanged variable, a type that indicates the data type (double, long, string, etc.) and a direction (input or output).  
     An input dataflow port can be connected with an output dataflow port of a compatible type.  The variable carried by 
     the port is exchanged at the end of the service (output port) or at the beginning of the service (input port).  
     No data exchange takes place during execution of the service.
   - **Datastream port**:  a datastream port is used to exchange data during execution.  Like a dataflow port, it has a name, 
     a type and a direction but it also has an iterative (I) or time (T) dependence mode.  An input datastream port can only 
     be connected with an output datastream port of the same type.  There may be several classes of datastream port.  
     In this description, we only deal with CALCIUM datastream ports.
   - **YACS coupler**:  this is a SALOME module that edits and executes coupling schemes that are mainly in the form of an 
     XML file.  The module provides a graphic user interface to edit a scheme, to execute it and monitor its execution.
   - **Component instance**:  there can be several copies of the same component and they can be executed by SALOME in a 
     single session.  A distinction between these copies is made by referring to a component instance.  A parallel can be 
     made with the terms calculation code (component) and execution (component instance).
   - **Container**:  SALOME process that executes components.  It loads (dynamic library or Python module) and saves 
     components.  The choice of containers may be important if it is required that one component should not be in the same 
     executable as another.
   - **SALOME application**:  set of SALOME modules integrated into the SALOME platform.  This platform is built on KERNEL 
     and GUI basic modules that provide component running and integration services in the graphic user interface.  A SALOME application 
     provides several scripts to run the application (runAppli), enter the application environment (runSession) and other 
     more internal scripts such as remote run (runRemote).

SALOME modules and components
==================================================
In principle, the SALOME platform is broken down into a basic platform (composed of a KERNEL and a GUI modules) 
and a collection of various modules.

A module is a compilable and installable product that is materialised by a source tree structure that must respect 
SALOME general rules and that includes a construction and installation procedure that also respects SALOME rules.  
Each module is managed in configuration in an independent CVS module.  Each module can deliver versions at it own rate 
respecting SALOME consistency rules.  A non-regression tests base is associated with each module.

A module has dependencies with other modules (uses, is used). The KERNEL and GUI modules makes up the base of the SALOME platform.  
All other modules depend on these modules.

===================================== ========= ======================= ==================================
Theme                                  Module     Uses                       Is used by       
===================================== ========= ======================= ==================================
Architecture                           KERNEL                            GUI, MED, GEOM, SMESH, PARAVIS,YACS
Architecture                           GUI       KERNEL                  MED, GEOM, SMESH, PARAVIS,YACS
Architecture                           MED       KERNEL, GUI             SMESH, PARAVIS
Geometry                               GEOM      KERNEL, GUI             SMESH
Mesh                                   SMESH     KERNEL, GUI, GEOM, MED
Supervision                            YACS      KERNEL, GUI
Visualization                          PARAVIS   KERNEL, GUI, MED
===================================== ========= ======================= ==================================

A module contains one or several SALOME components.  A SALOME component is a CORBA object that respects SALOME rules 
and that is declared to SALOME using a catalog.  A SALOME component may be provided with a graphic user interface (GUI) 
that must itself respect SALOME rules.

Containers
======================
In SALOME, components are dynamically loaded.  This property is obtained by using a container mechanism.

In general, a container is a CORBA server with an interface that is provided with methods necessary to load 
and unload the implementation of a SALOME component.  A component is loaded by calling the container load_impl method.

The basic mechanism for loading a component depends on the chosen implementation language.

In C++, the platform uses dynamic library loading (dlopen) and a factory function mechanism that must be 
named <Component>Engine_factory (for example GEOMEngine_factory, for GEOM component type). 
This function must return the effective CORBA object that is the SALOME component.

In Python, the platform uses the Python import mechanism (import <Component>) and instantiates the Python SALOME 
component using a class (or a factory) with the same name (<Component>).

.. _appli:

Construction and use of a SALOME application
=========================================================
This section explains how to configure, install and use your own SALOME application starting from a list of 
pre-installed modules (see more details in the on line documentation for the KERNEL module defining the 
application concept).

Principles
------------
A SALOME application is built up from a list of platform basic modules (GEOM, SMESH, ASTER, etc.) or users.  
It consists of a set of shell scripts and directories used to execute the application in different contexts.

A user can define several SALOME applications.  These applications can be used from several user accounts.  
They can use the same modules (KERNEL or other).  The SALOME application is independent of KERNEL 
and **it must not be installed in KERNEL_ROOT_DIR**.

Prerequisites for each application may be different.

A SALOME session run from a SALOME application can be executed on several computers.

For the installation of a multi-machine application, the modules and prerequisites must be installed on 
all machines involved.  There is no need to install all modules on all machines except for KERNEL.  
The SALOME user must have an account on each machine.  Remote machines are accessed by rsh or ssh.  
These accesses must be configured for use without a password.  The accounts may be different on the different machines.

Creating a SALOME application
------------------------------
A SALOME application is created using the appli_gen.py tool located in the installation of the KERNEL module.  
This tool builds the application starting from a configuration file in the XML format that describes the list 
of modules to be used (name, installation path), the file that sets the environment for SALOME pre-requisites 
and optionally the SALOME examples directory (SAMPLES_SRC) and a catalog of resources.

The following command is used:

.. code-block:: sh

   python <KERNEL_ROOT_DIR>/bin/salome/appli_gen.py --prefix=<install directory> \
                               --config=<configuration file>

where <configuration file> is the name of the configuration file and <install directory> is the name of the 
directory in which the application is to be created.  <KERNEL_ROOT_DIR> indicates the directory in which 
the KERNEL module is installed.

The configuration file can be created by modifying a copy of the ${KERNEL_ROOT_DIR}/bin/salome/config_appli.xml file.

For example:

.. code-block:: xml

  <application>
  <prerequisites path="/data/SALOME_V5/env_products.sh"/>
  <resources path="myCata.xml"/>
  <modules>
     <!-- variable name <MODULE>_ROOT_DIR is built 
          with <MODULE> == name attribute value -->
     <!-- <MODULE>_ROOT_DIR values is set with path attribute value -->
     <!-- attribute gui (defaults = yes) indicates if the module 
                                          has a gui interface -->
     <module name="KERNEL"       gui="no"  path="/data/SALOME_V5/KERNEL_INSTALL"/>
     <module name="GUI"          gui="no"  path="/data/SALOME_V5/GUI_5"/>
     <module name="MED"                    path="/data/SALOME_V5/MED_5"/>
     <module name="GEOM"                   path="/data/SALOME_V5/GEOM_5"/>
     <module name="SMESH"                  path="/data/SALOME_V5/SMESH_5"/>
     <module name="YACS"                   path="/data/SALOME_V5/YACS_5"/>
     <module name="VISU"                   path="/data/SALOME_V5/VISU_5"/>
     <module name="HELLO"                  path="/data/SALOME_V5/HELLO1_5"/>
     <module name="PYHELLO"                path="/data/SALOME_V5/PYHELLO1_5"/>
     <module name="NETGENPLUGIN"           path="/data/SALOME_V5/NETGENPLUGIN_5"/>
  </modules>
  <samples path="/data/SALOME_V5/SAMPLES/SAMPLES_SRC"/>
  </application>

Some rules to be followed
------------------------------
The application directory must be created on all computers on which components of this application are to be executed.  
The simplest method is to create the application directory using the same relative path from the HOME directory on each machine.  
If this is not wanted, then different paths can be used on different computers, but these paths will have to be specified 
in the CatalogRessources.xml configuration file.

The application directory contains scripts to initialize environment variables and to make executions.

The environment is initialized by scripts placed in the env.d sub-directory.  Scripts for SALOME are created at 
the time that the application is created but the user can add his own scripts.  All that is necessary is that 
they have the .sh suffix.  These scripts must be installed on all machines used by the application.

The SALOME application provides the user with 3 execution scripts:
 - **runAppli** runs a SALOME session (in the same way as ${KERNEL_ROOT_DIR}/bin/Salome/runSalome).
 - **runSession** connects to a running SALOME session, in a shell with a conforming environment.  If there is no argument, the 
   script opens an interactive shell.  If there are arguments, it executes the command supplied in the environment of the SALOME application.
 - **runConsole** opens a python console connected to the current SALOME session.  Another option is to use **runSession** and then to run Python.

The application configuration files are:
 - **SALOMEApp.xml**:  this file is similar to the default file located in ${GUI_ROOT_DIR}/share/SALOME/resources/gui.  
   It can be adapted to the user’s needs.
 - **CatalogResources.xml**:  this file describes all computers that the application might use.  The initial file only 
   contains the local machine.  The user must add the machines to be used.  If it is required to use arbitrary 
   application directories on the different computers, their location must be specified in this file using the appliPath attribute::

        appliPath="/my/specific/path/on/this/computer"

Configuring a SALOME application for remote components
----------------------------------------------------------
If you have a multi-machine application, it is possible that some modules (and components) are only available on remote
computers. In this case, you need to configure your application for this situation by using the **update_catalogs.py** script provided by the
SALOME application.

The first thing to do is to create a file named **CatalogResources.base.xml** that will contain all information about your multi-machine
configuration.

Example of **CatalogResources.base.xml** file:

.. code-block:: xml

  <!DOCTYPE ResourcesCatalog>
  <resources>
     <machine name="res1" hostname="localhost" >
        <component name="GEOM_Superv" moduleName="GEOM"/>
	<modules moduleName="YACS"/>
     </machine>

     <machine name="res2" hostname="computer1" userName="user" protocol="ssh" appliPath="/home/user/SALOME514/appli_V5_1_4" >
        <modules moduleName="AddComponent"/>
     </machine>
  </resources>

In this file, we say that we have 2 resources **res1** and **res2** on localhost and computer1. On the remote machine, we give
the SALOME application path and we give, for each resource, the list of available components or modules : GEOM_Superv and YACS on localhost
and AddComponent on computer1.

Starting from this file, the **update_catalogs.py** script gets all remote catalogs, puts them in local directories (remote_catalogs/<resource name>),
builds an updated **CatalogResource.xml** file and adds a new environment variable (**SALOME_CATALOGS_PATH** in env.d/configRemote.sh) to the
SALOME application. With these 3 elements the application is now correctly configured for a multi-machine use.





