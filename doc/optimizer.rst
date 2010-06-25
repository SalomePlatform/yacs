
.. _optimizationplugin:

Definition of an algorithm for the OptimizerLoop
==========================================================================
The definition of the optimization algorithm is done by way of plugin.
The plugin can be a C++ plugin implemented in a dynamic library (.so file) or a Python plugin implemented in a Python module (.py).
It is possible to implement two kinds of algorithm : synchronous or asynchronous.

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
- **initialize** (optional), this method is called during the algorithm initialization
- **parseFileToInit** (optional), this method is called during the algorithm initialization with a file name as argument. This file can be used by the algorithm.
- **start**, this method is called at the beginning of iterations
- **takeDecision**, this method is called at each iteration
- **finish** (optional), this method is called to finish the algorithm at the end of the iteration process

In Python you need to implement another method:

- **setPool**, this method is used to set the data pool that is used to exchange data

C++ plugin example
''''''''''''''''''''
Here is a small example of a C++ synchronous algorithm:

.. code-block:: cpp

  #include <cmath>
  
  #include "OptimizerAlg.hxx"
  
  using namespace YACS::ENGINE;
  
  extern "C"
  {
    OptimizerAlgBase * createOptimizerAlgSyncExample(Pool * pool);
  }
  
  class OptimizerAlgSyncExample : public OptimizerAlgSync
    {
    private:
      int _idTest;
      TypeCode *_tcIn;
      TypeCode *_tcOut;
    public:
      OptimizerAlgSyncExample(Pool *pool);
      virtual ~OptimizerAlgSyncExample();
      TypeCode *getTCForIn() const;
      TypeCode *getTCForOut() const;
      void parseFileToInit(const std::string& fileName);
      void start();
      void takeDecision();
      void initialize(const Any *input) throw(YACS::Exception);
      void finish();
    };
  
  OptimizerAlgSyncExample::OptimizerAlgSyncExample(Pool *pool)
    : OptimizerAlgSync(pool), _tcIn(0), _tcOut(0), _idTest(0)
  {
    _tcIn=new TypeCode(Double);
    _tcOut=new TypeCode(Int);
  }
  
  OptimizerAlgSyncExample::~OptimizerAlgSyncExample()
  {
    _tcIn->decrRef();
    _tcOut->decrRef();
  }
  
  //! Return the typecode of the expected input type
  TypeCode * OptimizerAlgSyncExample::getTCForIn() const
  {
    return _tcIn;
  }
  
  //! Return the typecode of the expected output type
  TypeCode * OptimizerAlgSyncExample::getTCForOut() const
  {
    return _tcOut;
  }
  
  //! Optional method to initialize the algorithm from a file
  void OptimizerAlgSyncExample::parseFileToInit(const std::string& fileName)
  {
  }
  
  //! Start to fill the pool with samples to evaluate
  void OptimizerAlgSyncExample::start()
  {
    _idTest=0;
    Any *val=AtomAny::New(1.2);
    _pool->pushInSample(4,val);
    val=AtomAny::New(3.4);
    _pool->pushInSample(9,val);
  }
  
  //! This method is called each time a sample has been evaluated.
  /*!
   *  It can either add new samples to evaluate in the pool, do nothing (wait
   *  for more samples), or empty the pool to finish the evaluation.
   */
  void OptimizerAlgSyncExample::takeDecision()
  {
    if(_idTest==1)
      {
        Any *val=AtomAny::New(5.6);
        _pool->pushInSample(16,val);
        val=AtomAny::New(7.8);
        _pool->pushInSample(25,val);
        val=AtomAny::New(9. );
        _pool->pushInSample(36,val);
        val=AtomAny::New(12.3);
        _pool->pushInSample(49,val);
      }
    else if(_idTest==4)
      {
        Any *val=AtomAny::New(45.6);
        _pool->pushInSample(64,val);
        val=AtomAny::New(78.9);
        _pool->pushInSample(81,val);
      }
    else
      {
        Any *tmp= _pool->getCurrentInSample();
        if(fabs(tmp->getDoubleValue()-45.6)<1.e-12)
          _pool->destroyAll();
      }
    _idTest++;
  }
  
  //! Optional method to initialize the algorithm.
  /*!
   *  For now, the parameter input is always NULL. It might be used in the
   *  future to initialize an algorithm with custom data.
   */
  void OptimizerAlgSyncExample::initialize(const Any *input)
    throw (YACS::Exception)
  {
  }
  
  /*!
   *  Optional method called when the algorithm has finished, successfully or
   *  not, to perform any necessary clean up.
   */
  void OptimizerAlgSyncExample::finish()
  {
  }
  
  //! Factory method to create the algorithm.
  OptimizerAlgBase * createOptimizerAlgSyncExample(Pool *pool)
  {
    return new OptimizerAlgSyncExample(pool);
  }


Here, the entry point in the dynamic library is the name of the factory function : createOptimizerAlgSyncExample
that returns an instance of the OptimizerAlgSyncExample class that implements the algorithm.

Python plugin example
''''''''''''''''''''''
Here, the same example of a synchronous algorithm in Python::

  import SALOMERuntime
  
  class myalgosync(SALOMERuntime.OptimizerAlgSync):
    def __init__(self):
      SALOMERuntime.OptimizerAlgSync.__init__(self, None)
      r=SALOMERuntime.getSALOMERuntime()
      self.tin=r.getTypeCode("double")
      self.tout=r.getTypeCode("int")
  
    def setPool(self,pool):
      """Must be implemented to set the pool"""
      self.pool=pool
  
    def getTCForIn(self):
      """returns typecode of type expected as Input"""
      return self.tin
  
    def getTCForOut(self):
      """returns typecode of type expected as Output"""
      return self.tout
  
    def initialize(self,input):
      """Optional method called on initialization. Do nothing here"""
  
    def parseFileToInit(self,fileName):
      """Optional method to read and parse an init file given by the
         OptimizerLoop. Do nothing here"""
  
    def start(self):
      """Start to fill the pool with samples to evaluate."""
      self.iter=0
      self.pool.pushInSample(4,1.2)
      self.pool.pushInSample(9,3.4)
  
    def takeDecision(self):
      """ This method is called each time a sample has been evaluated. It can
          either add new samples to evaluate in the pool, do nothing (wait for
          more samples), or empty the pool to finish the evaluation.
      """
      currentId=self.pool.getCurrentId()
  
      if self.iter==1:
        self.pool.pushInSample(16,5.6)
        self.pool.pushInSample(25,7.8)
        self.pool.pushInSample(36,9.)
        self.pool.pushInSample(49,12.3)
      elif self.iter==4:
        self.pool.pushInSample(64,45.6)
        self.pool.pushInSample(81,78.9)
      else:
        val=self.pool.getCurrentInSample()
        if abs(val.getDoubleValue()-45.6) < 1.e-12:
          self.pool.destroyAll()
      self.iter=self.iter+1
  
    def finish(self):
      """Optional method called when the algorithm has finished, successfully
         or not, to perform any necessary clean up. Do nothing here"""

Here, the entry point in the Python module is directly the name of the class that implements the algorithm : myalgosync.


Asynchronous algorithm
--------------------------------------------------
In asynchronous mode, all is the same except that after the initialization phase, the OptimizerLoop calls the algorithm only one time
to start it in a separate thread.

An asynchronous algorithm is implemented in a class derived from the base class OptimizerAlgASync with several methods that 
must be implemented and some optional methods (in C++ and in Python):

- **getTCForIn**, this method must return the YACS type of the input port of the internal node
- **getTCForOut**, this method must return the YACS type of the output port of the internal node
- **initialize** (optional), this method is called during the algorithm initialization
- **parseFileToInit** (optional), this method is called during the algorithm initialization with a file name as argument. This file can be used by the algorithm.
- **startToTakeDecision**, this method is called to start the iteration process in a separate thread. It is the body of the algorithm.
- **finish** (optional), this method is called to finish the algorithm at the end of the iteration process

In Python you need to implement another method:

- **setPool**, this method is used to set the data pool that is used to exchange data

C++ plugin example
''''''''''''''''''''
Here is a small example of a C++ asynchronous algorithm:

.. code-block:: cpp

  #include "OptimizerAlg.hxx"
  
  using namespace YACS::ENGINE;
  
  extern "C"
  {
    OptimizerAlgBase * createOptimizerAlgASyncExample(Pool * pool);
  }
  
  class OptimizerAlgASyncExample : public OptimizerAlgASync
    {
    private:
      TypeCode * _tcIn;
      TypeCode * _tcOut;
    public:
      OptimizerAlgASyncExample(Pool * pool);
      virtual ~OptimizerAlgASyncExample();
      TypeCode * getTCForIn() const;
      TypeCode * getTCForOut() const;
      void startToTakeDecision();
    };
  
  OptimizerAlgASyncExample::OptimizerAlgASyncExample(Pool * pool)
    : OptimizerAlgASync(pool), _tcIn(0), _tcOut(0)
  {
    _tcIn = new TypeCode(Double);
    _tcOut = new TypeCode(Int);
  }
  
  OptimizerAlgASyncExample::~OptimizerAlgASyncExample()
  {
    _tcIn->decrRef();
    _tcOut->decrRef();
  }
  
  //! Return the typecode of the expected input type
  TypeCode *OptimizerAlgASyncExample::getTCForIn() const
  {
    return _tcIn;
  }
  
  //! Return the typecode of the expected output type
  TypeCode *OptimizerAlgASyncExample::getTCForOut() const
  {
    return _tcOut;
  }
  
  //! This method is called only once to launch the algorithm.
  /*!
   *  It must first fill the pool with samples to evaluate and call
   *  signalMasterAndWait() to block until a sample has been evaluated. When
   *  returning from this method, it MUST check for an eventual termination
   *  request (with the method isTerminationRequested()). If the termination
   *  is requested, the method must perform any necessary cleanup and return
   *  as soon as possible. Otherwise it can either add new samples to evaluate
   *  in the pool, do nothing (wait for more samples), or empty the pool and
   *  return to finish the evaluation.
   */
  void OptimizerAlgASyncExample::startToTakeDecision()
  {
    double val = 1.2;
    for (int i=0 ; i<5 ; i++) {
      // push a sample in the input of the slave node
      _pool->pushInSample(i, AtomAny::New(val));
      // wait until next sample is ready
      signalMasterAndWait();
      // check error notification
      if (isTerminationRequested()) {
        _pool->destroyAll();
        return;
      }
  
      // get a sample from the output of the slave node
      Any * v = _pool->getCurrentOutSample();
      val += v->getIntValue();
    }
  
    // in the end destroy the pool content
    _pool->destroyAll();
  }
  
  //! Factory method to create the algorithm.
  OptimizerAlgBase * createOptimizerAlgASyncExample(Pool * pool)
  {
    return new OptimizerAlgASyncExample(pool);
  }


Here, the entry point in the dynamic library is the name of the factory function : createOptimizerAlgASyncExample
that returns an instance of the OptimizerAlgASyncExample class that implements the algorithm.

Python plugin example
''''''''''''''''''''''''
Here is an example of an asynchronous algorithm implemented in Python::

  import SALOMERuntime
  
  class myalgoasync(SALOMERuntime.OptimizerAlgASync):
    def __init__(self):
      SALOMERuntime.OptimizerAlgASync.__init__(self, None)
      r=SALOMERuntime.getSALOMERuntime()
      self.tin=r.getTypeCode("double")
      self.tout=r.getTypeCode("int")
  
    def setPool(self,pool):
      """Must be implemented to set the pool"""
      self.pool=pool
  
    def getTCForIn(self):
      """returns typecode of type expected as Input"""
      return self.tin
  
    def getTCForOut(self):
      """returns typecode of type expected as Output"""
      return self.tout
  
    def startToTakeDecision(self):
      """This method is called only once to launch the algorithm. It must
         first fill the pool with samples to evaluate and call
         self.signalMasterAndWait() to block until a sample has been
         evaluated. When returning from this method, it MUST check for an
         eventual termination request (with the method
         self.isTerminationRequested()). If the termination is requested, the
         method must perform any necessary cleanup and return as soon as
         possible. Otherwise it can either add new samples to evaluate in the
         pool, do nothing (wait for more samples), or empty the pool and
         return to finish the evaluation.
      """
      val=1.2
      for iter in xrange(5):
        #push a sample in the input of the slave node
        self.pool.pushInSample(iter,val)
        #wait until next sample is ready
        self.signalMasterAndWait()
        #check error notification
        if self.isTerminationRequested():
          self.pool.destroyAll()
          return
  
        #get a sample from the output of the slave node
        currentId=self.pool.getCurrentId()
        v=self.pool.getCurrentOutSample()
        val=val+v.getIntValue()
  
      #in the end destroy the pool content
      self.pool.destroyAll()

Here, the entry point in the Python module is directly the name of the class that implements the algorithm : myalgoasync.


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
