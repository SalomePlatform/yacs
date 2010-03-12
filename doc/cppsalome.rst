
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
 - make the component useable in the supervisor.

Creating the module tree structure
=======================================
We will start by simply putting a SALOME component written in C++ and that can be loaded by a C++ container, into the 
example module.  Therefore, all that is necessary is an idl interface and a C++ implantation of the component. 
We need to reproduce the following standard file tree structure, to use it in a SALOME module::

  + HELLO1_SRC
    + build_configure
    + configure.in.base
    + Makefile.in
    + adm_local
      + unix
        + make_commence.in
        + make_omniorb.in
        + config_files
    + bin
      + VERSION
      + runAppli.in
      + runSalome.py
    + idl
      + Makefile.in
      + HELLO_Gen.idl
    + src
      + Makefile.in
      + HELLO
        + Makefile.in
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
This service is specified by the makeBanner function.

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
the version of CORBA used.  SALOME_Component_i.hxx contains the interface of the C++ implementation class of 
the SALOME basic component (idl Engines::Component).  Finally, the CORBA_SERVER_HEADER macro makes inclusion 
file names independent of the implementation of the CORBA ORB.

The next step is to define an implementation class called HELLO, derived from POA_HELLO_ORB::HELLO_Gen (abstract class 
generated automatically by CORBA during the compilation of the idl) and Engines_Component_i (because 
the HELLO_Gen idl interface is derived from Engines::Component like every SALOME component).  
This class contains a constructor whose arguments are imposed by SALOME, a virtual destructor, and 
a makeBanner method providing the required service::

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
    char* makeBanner(const char* name);
    };

The makeBanner function uses a char* as an argument and returns a char*, which is the C++ mapping of the CORBA/IDL string type.  
OMG provides the complete documentation of the C++ mapping of the IDL on its internet site:  http://www.omg.org/cgi-bin/doc?ptc/00-01-02.

Finally, we supply the standard interface of the HELLOEngine_factory function that will be called by the “FactoryServer C++” 
to load the HELLO component::

    extern "C"
    PortableServer::ObjectId * HELLOEngine_factory(CORBA::ORB_ptr orb,
                                                   PortableServer::POA_ptr poa,
                                                   PortableServer::ObjectId * contId,
                                                   const char *instanceName,
                                                   const char *interfaceName);


The definitions of the constructor and the HELLOEngine_factory instantiation function (both normalized!) and 
makeBanner, are given in the source file (HELLO.cxx)::

    char* HELLO::makeBanner(const char* name)
    {
      string banner="Hello, ";
      banner+=name;
      return CORBA::string_dup(banner.c_str());
    }

In this function, the use of string_dup (function declared in the CORBA namespace) is not compulsory (the new operator 
could have been used), but is recommended because these functions enable ORBs to use special memory management 
mechanisms without needing to redefine new global operators.

Makefile
--------
In makefile, some targets have to be defined::

    VPATH=.:@srcdir@:@top_srcdir@/idl
    LIB = libHELLOEngine.la
    LIB_SRC = HELLO.cxx
    LIB_SERVER_IDL = HELLO_Gen.idl
    LIB_CLIENT_IDL = SALOME_Component.idl SALOME_Exception.idl Logger.idl
    CPPFLAGS += -I${KERNEL_ROOT_DIR}/include/salome
    LDFLAGS+= -lSalomeContainer -lOpUtil -L${KERNEL_ROOT_DIR}/lib/salome

Review each of these targets

- LIB contains the normalized name (lib<Nom_Module>Engine.la) the name of the library, LIB_SRC defines the name of source files, and VPATH defines the directories in which they can be found.
- LIB_SERVER_IDL contains the name of idl files implemented by the module.
- LIB_CLIENT_IDL contains the name of idls containing CORBA services used by the module.  HELLO uses Logger/idl through the “MESSAGE” and SALOME_Component.idl macros, and SALOME_Exception.idl through the inheritance of HELLO_ORB.
- The path for the include files used has to be added to CPPFLAGS (SALOME.config.h, SALOME_Component_i.hxx and utilities.h are located in ${KERNEL_ROOT_DIR}/include/salome).
- The HELLO class uses lib libraries (for Engines_Component_i) and libOptUtil (for PortableServer and Salome_Exception).  Therefore, the name of these libraries and their path in LDFLAGS will be indicated.  Other libraries are often useful, for example libsalomeDS if persistence is implemented, or libSalomeNS if the naming service is used.

Controlling the component from Python (TUI mode)
=====================================================
When the module is compiled, the lib target of the Makefile in /idl provoked generation of a Python 
stub (stub at the customer end generated from the idl and providing an interface in the client language – in this case Python).  
Specifically, a HELLO_ORB python module containing a classe_objref_HELLO_Gen is created and used to call services of our 
C++ module from Python.  To put this into application, we run SALOME in TUI mode::

    cd $HELLO_ROOT_DIR/bin/salome
    python -i runSalome.py --modules=HELLO --xterm --logger --containers=cpp,python \
                           --killall

We import the LifeCycle module from the Python window, and use its services to load our component into the FactoryServer C++ container::

    >>> import LifeCycleCORBA
    >>> lcc = LifeCycleCORBA.LifeCycleCORBA(clt.orb)
    >>> import HELLO_ORB
    >>> hello = lcc.FindOrLoadComponent("FactoryServer", "HELLO")

HELLO_ORB has to be imported before FindOrLoadComponent is called, so that a typed object can be 
returned (“narrowing” operation). Otherwise, the returned object is generic of the Engines:Component type.  
Let us check that hello object is correctly typed, and we will call the makeBanner service::

    >>> print hello
    <HELLO_ORB._objref_HELLO_Gen instance at 0x8274e94>
    >>> mybanner=hello.makeBanner("Nicolas")
    >>> print mybanner
    Hello, Nicolas

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

Choose widgets
-----------------

Xml description
``````````````````
Items in our module are described in the XML /resources/HELLO_en.xml file.  The IAPP uses this file to load the GUI 
of the module dynamically when the it is activated.  The principle is to use markers to define the required menus 
and buttons and to associate IDs with them, that will be retrieved by functions managing GUI events.  
There are several possibilities available:

- add items to existing menus, in which case markers are reused from the previous menu and new items are added.  
  In the following example, the **Hello** menu and the **MyNewItem** item are added to the **File** menu, 
  for which the ID is equal to 1::

    <menu-item label-id="File" item-id="1" pos-id="">
      <submenu label-id="Hello" item-id="19" pos-id="8">
        <popup-item item-id="190" pos-id="" label-id="MyNewItem" 
	            icon-id="" tooltip-id="" accel-id="" toggle-id="" 
		    execute-action=""/>
      </submenu>
    </menu-item>

- Create new menus.  For the HELLO menu, we add a HELLO menu with a single item with a “Get banner” label::

    <menubar>
     <menu-item label-id="HELLO" item-id="90" pos-id="3">
      <popup-item item-id="901" label-id="Get banner" icon-id="" 
                  tooltip-id="Get HELLO banner" accel-id="" 
		  toggle-id="" execute-action=""/>
     </menu-item>
    </menubar>

- Add a button into the button bar.  In the following example, we create a second entry point for our “Get banner” 
  action in the form of a button associated with the same ID “901”.  The icon is specified by the icon-id field, 
  which must be a 20x20 pixels graphic file in the png format::

    <toolbar label-id="HELLO">
     <toolbutton-item item-id="901" label-id="Get banner" icon-id="ExecHELLO.png"
    tooltip-id="Get HELLO banner" accel-id="" toggle-id="" execute-action=""/>
    </toolbar>

Convention
``````````
There is an ID associated with each menu or item.  Numbers between 1 and 40 are reserved for the IAPP.  
ID numbers follow a certain rule, although it is not compulsory.  ID 90 is associated with the "HELLO" menu.  
The ID of its unique "Get banner" item is 901.  The ID of a second item would be 902, and the ID of the sub-item would be 9021.

Implementation of the GUI
-----------------------------
The C++ implementation of the GUI is made in the /src/HELLOGUI directory.  The HELLOGUI.h header declares 
the HELLOGUI class and contains the Qt (Q_OBJECT) directives.  Consequently, it must be processed by 
the moc compiler (Qt Meta Model Compiler).  This is why the file extension is .h and we add the target into the Makefile::

	LIB_MOC = HELLOGUI.h

The HELLO.cxx source file contains the definition of member functions, and the Makefile is used to build a 
library libHELLOGUI (the name is normalised to enable dynamic loading:  lib<NomModule>GUI.

Management of events
``````````````````````
For the HELLO GUI, we define the HELLOGUI::OnGUIEvent function, which will be called for each event.  
This function essentially contains a switch structure used to process the ID received as an argument::

  switch (theCommandID)
    {
    case 901:
      // Traitement de "Get banner"
      ...
    case 190:
      // Traitement de "MyNewItem"
      ...
    }

The standard processing consists of retrieving input data (in this case the first name through a QInputDialog::getText 
dialog window), and retrieving a handle on the interfaced CORBA component so as to call the required 
service (in this case getBanner) and to display the result obtained ().

Available classes
````````````````````
Any class supplied by Qt (http://doc.trolltech.com/) can be used for dialogs with the user.  
However it is preferable, when possible, to use QAD (Qt Application Desktop) functions defined in GUI_SRC/ 
that encapsulate the corresponding Qt functions and manage communications with IAPP better.  
Thus, the QAD_MessageBox class is used in HELLOGUI instead of the Qt QMessageBox class.

Management of multi-linguism
`````````````````````````````````
Qt provides a help tool for multi-linguism support.  This is reused in SALOME.  
The principle is simple:  all character strings that are used for menu labels and dialogs with the user 
are encapsulated in calls to the Qt tr() (translate) function that uses a label name as its argument.  
For example, the getText function will be used to ask the user to enter a first name, in which the first 
two arguments are labels encapsulated by tr()::

        myName = QInputDialog::getText( tr("QUE_HELLO_LABEL"), tr("QUE_HELLO_NAME"),
                                        QLineEdit::Normal, QString::null, &ok);

The label names are prefixed by three letters and an underscore, for information.  The following codes are used:

- MEN\_: menu label
- BUT\_: button label
- TOT\_: tooltip help
- ERR\_: error message
- WRN\_: warning message
- INF\_: information message
- QUE\_: question
- PRP\_: prompt in the status bar

Labels encapsulated by tr() are translated for different target languages (for example French and English) 
in files named “<module_name>_msg_<language>.po”.  <language> is the language code, and **en** has been chosen for English 
and **fr** for French.  This file must contain the translation for each key, for example::

    msgid "HELLOGUI::INF_HELLO_BANNER"
    msgstr "HELLO Information"

The Qt findtr utility can generate the skeleton for this file::

    findtr HELLOGUI.cxx > HELLO_msg_en.po

and then the HELLO_msg_en.po file is edited to fill in the translations.  
These files are then compiled by the **msg2qm** utility to generate *.qm* binaries.  
This is done by filling in the LIB_MOC target in the Makefile::

    PO_FILES =  HELLO_msg_en.po HELLO_msg_fr.po

The final user chooses the language in each module in the resources/config. file, using the following command::

    langage=<langage>



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

