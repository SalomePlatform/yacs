
.. _optimizationplugin:

Definition of an algorithm for the OptimizerLoop
==========================================================================
The definition of the optimization algorithm is done by way of plugin.
The plugin can be a C++ plugin implemented in a dynamic library (.so file) or a Python plugin implemented in a Python module (.py).
It is possible to implement two kinds of algorithm : synchronous or asynchronous.

The algorithm uses a pool of samples to be evaluated.
When all the samples of the pool are evaluated, the algorithm stops.

Synchronous algorithm
--------------------------------------------------
In synchronous mode, the OptimizerLoop calls the algorithm to know what are the types of the input port (sample sent to the internal node), 
and of the output port (data returned by the internal node). Then it calls the algorithm to initialize
it. At each iteration, it calls the algorithm to produce new sample or to stop the iteration. Finally, it calls the algorithm
to finalize the optimization process.

A synchronous algorithm is implemented in a class derived from the base class OptimizerAlgSync with several methods that 
must be implemented and some optional methods (in C++ and in Python):

- **getTCForIn**, this method must return the YACS type of the input port of the internal node
- **getTCForOut**, this method must return the YACS type of the output port of the internal node
- **getTCForAlgoInit** (optional), this method returns the type of the "algoInit" port, string if undefined
- **getTCForAlgoResult** (optional), this method returns the type of the "algoResult" port, string if undefined
- **initialize** (optional), this method is called during the algorithm initialization
- **start**, this method is called at the beginning of iterations
- **takeDecision**, this method is called at each iteration
- **finish** (optional), this method is called to finish the algorithm at the end of the iteration process
- **getAlgoResult** (optional), this method returns the value of the "algoResult" port, "NULL" if undefined

In Python you need to implement another method:

- **setPool**, this method is used to set the data pool that is used to exchange data

C++ plugin example
''''''''''''''''''''
Here is a small example of a C++ synchronous algorithm:

.. literalinclude:: ../src/yacsloader/Test/OptimizerAlgSyncExample.cxx
    :language: cpp

Here, the entry point in the dynamic library is the name of the factory function : createOptimizerAlgSyncExample
that returns an instance of the OptimizerAlgSyncExample class that implements the algorithm.

Python plugin example
''''''''''''''''''''''
Here, the same example of a synchronous algorithm in Python:

.. literalinclude:: ../src/yacsloader/Test/algosyncexample.py

Here, the entry point in the Python module is directly the name of the class that implements the algorithm : myalgosync.


Asynchronous algorithm
--------------------------------------------------
In asynchronous mode, all is the same except that after the initialization phase, the OptimizerLoop calls the algorithm only one time
to start it in a separate thread.

An asynchronous algorithm is implemented in a class derived from the base class OptimizerAlgASync with several methods that 
must be implemented and some optional methods (in C++ and in Python):

- **getTCForIn**, this method must return the YACS type of the input port of the internal node
- **getTCForOut**, this method must return the YACS type of the output port of the internal node
- **getTCForAlgoInit** (optional), this method returns the type of the "algoInit" port, string if undefined
- **getTCForAlgoResult** (optional), this method returns the type of the "algoResult" port, string if undefined
- **initialize** (optional), this method is called during the algorithm initialization
- **startToTakeDecision**, this method is called to start the iteration process in a separate thread. It is the body of the algorithm.
- **finish** (optional), this method is called to finish the algorithm at the end of the iteration process
- **getAlgoResult** (optional), this method returns the value of the "algoResult" port, "NULL" if undefined

In Python you need to implement another method:

- **setPool**, this method is used to set the data pool that is used to exchange data

C++ plugin example
''''''''''''''''''''
Here is a small example of a C++ asynchronous algorithm:

.. literalinclude:: ../src/yacsloader/Test/OptimizerAlgASyncExample.cxx
    :language: cpp


Here, the entry point in the dynamic library is the name of the factory function : createOptimizerAlgASyncExample
that returns an instance of the OptimizerAlgASyncExample class that implements the algorithm.

Python plugin example
''''''''''''''''''''''''
Here is an example of an asynchronous algorithm implemented in Python:

.. literalinclude:: ../src/yacsloader/Test/algoasyncexample.py

Here, the entry point in the Python module is directly the name of the class that implements the algorithm : myalgoasync.

Managing the pool of samples
---------------------------------

Samples can be added to the pool at the initialization of the algorithm or
every time a sample is evaluated (while "taking decision").
The algorithm stops to take decisions when every sample is evaluated.

A sample has:

- an identifier - *Id*
- a priority - it is used to choose the order of evaluation
- a value - *In*
- an evaluated or computed value - *Out*

The current sample is the sample used by the latest terminated evaluation.

These are the methods needed to manage the pool of samples:

.. code-block:: cpp

  class Pool
  {
      //...
    public:
      //For algorithm use
      int getCurrentId() const ;
      Any *getCurrentInSample() const ;
      Any *getCurrentOutSample() const ;
      Any *getOutSample(int id);
      void pushInSample(int id, Any *inSample, unsigned char priority = 0);
      void destroyAll();
      //...
  }

In C++, the samples are of type ``YACS::ENGINE::Any``, in order to support any
type supported by YACS. For conversion to standard types, use:

- ``getIntValue``
- ``getBoolValue``
- ``getDoubleValue``
- ``getStringValue``

It is possible to create a pointer to a new object with:

- ``YACS::ENGINE::AtomAny::New``

For further information, see `include/salome/Any.hxx <file:../../../../../include/salome/Any.hxx>`_.

C++ algorithm calling Python code
--------------------------------------------------

In some cases, it can be necessary to implement the algorithm as a C++ class but
nevertheless to call some Python code from this class. This is also possible with the
OptimizerLoop and even quite simple. To achieve this, your C++ class should inherit from
PyOptimizerAlgSync for a synchronous algorithm or from PyOptimizerAlgASync for an
asynchronous algorithm. The guidelines for developing the algorithm are the same as in
the C++ case, but you can also call any method from the Python C API. You don't need to
take care of the Python global interpreter lock or of thread states because this is
already done in the PyOptimizerAlg classes. An example of this kind of algorithm is the
class OpenTURNSScriptLauncher that can be found in the module OPENTURNS_SRC.
