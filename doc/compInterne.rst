
.. _seccompinterne:

Defining an internal object
===============================

.. index:: single: internal object

In the remainder of this document, it is assumed that an **internal object** is a **C++ or python object** that provides a 
first interface to the initial code.  The form of this object and communication between the internal object and the 
initial code will be different depending on the form of the initial code (executable binary, static or dynamic library, 
f77, C, C++ or python source files).

Object methods and attributes
--------------------------------
In each case, the services and the internal state of the internal object will have to be defined.  In particular:

* choose the different services of this object
* for each service, define input and output data
* for each input and each output, define the data type and possibly associated pre-conditions and post-conditions (for example positive input data)
* define the internal state of the object and possibly its value before and after the call to different services.

**Services** will be implemented in the form of **public methods** and the **internal state** will be implemented in the form of **attributes**.  
If the designer of the object allows the user to access the attributes in read and write, he must provide services to access these attributes.

Fortran77 routines / C functions / C++ classes 
--------------------------------------------------
Principle
^^^^^^^^^^^
In the case of Fortran77 routines, C functions and C++ classes, the integrator will simply add a C++ envelope around 
these functions (see figure :ref:`C++ internal object<figobjetinterne>`), to obtain the internal object.  
Each method in the object:

- extracts information from the input parameters if necessary
- calls the internal routine(s) concerned,
- formats the results of these internal routines in the output parameters.

.. _figobjetinterne:


.. image:: images/objintcpp.png
   :align: center

.. centered:: C++ internal object


.. _exemple1:

Example 1
^^^^^^^^^


Consider the following f77 fortran routines performing linear algebra calculations on one dimensional floating tables:

``addvec.f``

.. include:: ./exemples/exemple1/addvec.f
   :literal:


``prdscl.f``

.. include:: ./exemples/exemple1/prdscl.f
   :literal:

and a C++ class simulating a (very) rudimentary vector type:

.. _vecteur.hxx:


``vecteur.hxx (C++ interface)``

.. include:: ./exemples/exemple1/exec2/vecteur.hxx
   :literal:


``vecteur.cxx (C++ implementation)``

.. include:: ./exemples/exemple1/exec2/vecteur.cxx
   :literal:

The internal object (i.e. the C++ class) in the example is:


``alglin.hxx``

.. include:: ./exemples/exemple1/exec2/alglin.hxx
   :literal:


``alglin.cxx``

.. include:: ./exemples/exemple1/exec2/alglin.cxx
   :literal:

**Notes**:

#. The integrator chooses methods, parameter transfers and parameter types (in accordance with the requirements of object users).  
   The correspondence between parameters of the internal object and parameters of routines in the initial code is organised 
   by the implementation (file ``alglin.cxx``, above).
#. In particular, if MED structures [MED]_ are transferred as input arguments, the C++ implementation file will be required to extract 
   and format information to be transferred to internal calculation routines (in the form of simple and scalar tables for internal 
   fortran routines).  For output arguments in the MED format, the implementation will introduce the results of internal routines 
   into the MED objects to be returned.

Note the following in the above example:

* the ``“C” extern`` declaration in front of C++ prototypes of fortran functions
* the “underscore” character added to the C++ name of fortran functions
* the mode of transferring arguments, the rule being that pointers will be transferred apart from exceptions (length of 
  character strings).  For scalar arguments, the addresses of the scalar arguments will be transferred;  for pointer 
  arguments (tables), the pointers will be transferred as is.

The internal object can now be used in a C++ code:

.. include:: ./exemples/exemple1/exec2/main_extraits.cxx
   :literal:


References
^^^^^^^^^^
The C / fortran77 encapsulation in a C++ code follows the standard procedure (formats of reals / integers, 
routine names, transfer of arguments).  For example, further information on this subject is given in [ForCpp]_ or [ForCpp2]_.

Python function/classes
------------------------
Principle
^^^^^^^^^^
The principle of encapsulation of python functions / classes in an internal object (python) is the same as in the previous case

  .. _figobjetinterne2:


  .. image:: images/objintpy.png
     :align: center

  .. centered:: Python internal object


Example 2
^^^^^^^^^

An example similar to the previous example starts from Python functions to be encapsulated:

``func.py``

.. include:: ./exemples/exemple2/func.py
   :literal:

It is easy to integrate these functions into a python class:


``compo.py``

.. include:: ./exemples/exemple2/compo.py
   :literal:

**Note**:  
  In fact, it is not even necessary to embed python functions of ``func.py``, but it is “cleaner” (particularly if 
  the object has an internal state).  The following script uses the Python internal object from a python interpreter:

.. include:: ./exemples/exemple2/exmpl.py
   :literal:


Initial code in the form of executables
--------------------------------------------
Principle
^^^^^^^^^^
This case occurs when there are no internal code sources (or when it is not required to integrate these sources 
into the internal architecture).  It will be assumed that the code is in the form of a binary that can be executed 
by the operating system.  Communications can be made with the code.

1. In  input, either:

  - by one or several files,
  - by the command line,
  - using the keyboard to answer questions from the code 

2. In output, either:

  - by one or several files,
  - on-screen display.

Communication with executables is made using commands (available in C++ and in python):

- ``system``:  start an executable with which input communications are made through files or the command line, and 
  output communications are made through files,
- ``popen``:  same functions as the previous case, also with the possibility of retrieving the standard output (screen) 
  for the executable.

The above commands are stored in order of increasing complexity (it is recommended that ``system`` should be used as much as possible).

Example 3:  Python internal object connected to external executables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
It is required to use a “System” object that has 5 services:

- ``cd``, that starts from a path (character string) and sets a current directory,
- ``cp``, that starts from 2 file names, and copies the first file onto the second in the current directory,
- ``touch``, that starts from a file name and updates the date of the file if there is one, and otherwise creates it,
- ``rm``, that starts from a file name and destroys the file in the current directory,
- ``dir``, that lists the files contained in the current directory.

The internal state of the object will be composed of the name of the current directory in which the services of the 
object (that is set by the ``cd`` service) will work.

In Python, the object class could be written:

``systeme.py``

.. include:: ./exemples/exemple3/systeme.py
   :literal:

and its use from the python interpreter:


.. include:: ./exemples/exemple3/use.py
   :literal:

**Notes**

#. This is given as an example, Python has everything necessary in the standard version to perform these services, without 
   the use of system commands (``system`` and ``popen``).
#. The example illustrates transfer of input arguments through the command line (names transferred as arguments) and the “capture” of 
   screen outputs from external executables (``system`` that cannot simply recover the standard output from the unix command ``ls``, and 
   in this case ``popen`` is used).

.. _exemple4:

Example 4:  Internal object connected to an external executable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This example shows a (very) partial interface of a binary executable *FreeFem* [FreeFem]_ in the form of a C++ object.  
The interface provides access to the definition of a 2D geometry through its boundary, and the approximate resolution of a 
simple equation (forced convection) on this geometry.  The different methods of the internal object are:

- a method that records the geometry of the domain,
- a method that records the convecting velocity fields,
- the calculation method that receives the initial condition (in analytic form – character string), the time step and the number of time steps.

The internal state of the object is composed of the geometry and the velocity field.  
The calculation method creates a file starting from its parameters and the internal state, and then starts a calculation 
loop (by a system call).  The object does not recover the calculation results.

**Comments**

#. A complete encapsulation of FreeFem would require far too much effort, this is simply an example.
#. We do not retrieve a result in the C++ object in this example (the change is only displayed by the FreeFem internal graphic 
   engine).  If it was required to do so, it would be necessary to read the file produced by the external code after the system 
   call, and transfer the results in a form that could be understood by the User of the internal object.

Two versions (C++ and python) are listed below.

.. _freefem.hxx:


``FreeFem.hxx``

.. include:: ./exemples/exemple4/FreeFem.hxx
   :literal:


``FreeFem.cxx``

.. include:: ./exemples/exemple4/FreeFem.cxx
   :literal:


``FreeFem.py``

.. include:: ./exemples/exemple4/FreeFem.py
   :literal:

Use from a C++ code or a python interpreter is similar in the 2 versions:

``version C++``

.. include:: ./exemples/exemple4/main.cxx
   :literal:


``version python``

.. include:: ./exemples/exemple4/useFreeFem.py
   :literal:

