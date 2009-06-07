
:tocdepth: 3

.. _components:

Integration of components into the SALOME environment
=============================================================

This document describes operations to be done to integrate codes (for example calculation codes) in the 
SALOME architecture.

Several integration scenarios could be envisaged:

- The simplest is local control of the code from a Python interpretor.  The code will only operate on one machine.
- The next stage consists of being able to control the code locally or remotely.  The selected architecture 
  consists of presenting the code as a CORBA server [CORBA]_, capable of receiving and executing requests 
  from CORBA clients.  SALOME offers a number of services that simplify the use of CORBA.
- Finally, actual integration into SALOME consists of providing SALOME with all information necessary to control 
  the code, for example from the man-machine interface or for example through supervision.

Several types of situations may be envisaged, knowing that there will always be special cases that should be 
dealt with by adapting procedures described in this document.  
The typical situations envisaged depend on the form of the available code:

- binary executable
- binary library,
- f77/C/C++ source code 
- python source code

We will give examples of the different situations encountered wherever possible.

.. toctree::
   :maxdepth: 2

   etapes.rst
   compInterne.rst
   accesLocal.rst
   accesCorba.rst
   salome.rst
   annexe_common.rst
   references.rst

