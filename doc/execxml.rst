
.. _execxml:

Executing a calculation scheme in console mode 
========================================================

A well-configured SALOME installation is necessary to execute a calculation scheme in the XML format in console mode.  
SALOME execution mode from a SALOME application will be used in the remainder of this document.

1.	run SALOME:  ./runAppli-t, for example from the application directory (it can be run from any other directory).
2.	execute the YACS supervisor:  ./runSession driver schema.xml
3.	stop SALOME:  ./runSession shutdownSalome.py for a clean stop or ./runSession killSalome.py for an immediate stop.

Steps 2 and 3 can be done from inside a SALOME session. YACS can be executed and SALOME can be stopped within this session as follows:

1.	initialise the session:  /.runSession
2.	execute the YACS supervisor:  driver schema.xml
3.	stop SALOME:  shutdownSalome.py or killSalome.py
4.	exit from the session:  CTRL+D

The YACS supervisor in console (driver) mode accepts a few options to set parameters for its execution::

   Usage: driver [OPTION...] graph.xml
   driver -- a SALOME YACS graph executor

     -d, --display=level        Display dot files: 0=never to 3=very often
     -e, --dump-on-error[=file] Stop on first error and dump state
     -f, --dump-final[=file]    dump final state
     -l, --load-state=file      Load State from a previous partial execution
     -s, --stop-on-error        Stop on first error
     -v, --verbose              Produce verbose output
     -x, --save-xml-schema[=file]   dump xml schema
     -?, --help                 Give this help list
         --usage                Give a short usage message
     -V, --version              Print program version

The following gives a few typical uses of the driver.

Standard execution
--------------------
During a standard execution, the supervisor reads the XML file that describes the scheme, executes it and then produces 
an error report (see :ref:`errorreport`) if necessary::

  driver schema.xml

Execution displaying states of nodes during execution
----------------------------------------------------------------------
::

  driver --display=1 schema.xml
 

Execution saving the final state of the scheme
----------------------------------------------------------------------
::

  driver --dump-final=mystate.xml schema.xml

Execution loading the initial state of the scheme
----------------------------------------------------------------------
::

  driver --load-state=mystate.xml schema.xml

