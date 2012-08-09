.. _integration:

**********************************************************
Integration of components into SALOME for YACS
**********************************************************

This part of the documentation provides information to create SALOME components that can be coupled with YACS, either 
starting from scratch or by including an existing code.

The first two chapters are tutorials to create SALOME modules with a single “Hello World” type component, implemented 
in Python (:ref:`pysalome`) or in C++ (:ref:`cppsalome`).

The next chapter (:ref:`components`) describes operations to be done to include calculation codes into the SALOME 
architecture.  
The fourth chapter (:ref:`calculator`) shows how to create a SALOME component that uses MED objects.

The next chapter presents a tool (:ref:`hxx2salome`) that automates much of the work to integrate a C++ calculation 
code, provided that only the dataflow ports are used.

The sixth chapter (:ref:`progdsc`) is aimed particularly at persons who would like to develop new datastream ports 
based on the DSC programming model.

The next chapter is a :ref:`calcium`. CALCIUM ports are datastream ports predefined in SALOME that simply implement 
datastream couplings in YACS.

Finally, the last chapter is a guide for use of a tool (:ref:`yacsgen`)  that automates the integration of calculation 
codes in Fortran, C, Python and that use CALCIUM datastream ports, that is not possible with the :ref:`hxx2salome` tool.

.. toctree::
   :maxdepth: 2

   pysalome
   cppsalome
   components
   calculator
   hxx2salome
   progdsc
   calcium
   yacsgen
