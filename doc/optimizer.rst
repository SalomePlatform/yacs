
.. _optimizationplugin:

Definition of an algorithm for the OptimizerLoop
==========================================================================
The definition of the optimization algorithm is done by way of plugin.
The plugin can be a C++ plugin implemented in a dynamic library (.so file) or a Python plugin implemented in a Python module (.py).
It is possible to implement two kinds of algorithm : synchronous or asynchronous.

Synchronous algorithm
--------------------------------------------------
In synchronous mode, the OptimizerLoop calls the algorithm to know what are the types of the input port (sample sent to the internal node), 
and of the output port (data returned by the internal node). Then it calls the algorithm to initializes
it. At each iteration, it calls the algorithm to produce new sample or to stop the iteration. Finally, it calls the algorithm
to finalize the optimization process.

A synchronous algorithm is implemented in a class derived from the base class OptimizerAlgSync with several methods that 
must be implemented (in C++ and in Python):

- **getTCForIn**, this method must return the YACS type of the input port of the internal node
- **getTCForOut**, this method must return the YACS type of the output port of the internal node
- **initialize**, this method is called during the algorithm initialization
- **parseFileToInit**, this method is called during the algorithm initialization with a file name as argument. This file can be used by the algorithm.
- **start**, this method is called at the beginning of iterations
- **takeDecision**, this method is called at each iteration
- **finish**, this method is called to finish the algorithm at the end of the iteration process

In Python you need to implement another method:

- **setPool**, this method is used to set the data pool that is used to exchange data

C++ plugin example
''''''''''''''''''''
Here is a small example of a C++ synchronous algorithm ::

  #include "OptimizerAlg.hxx"

  extern "C"
  {
    YACS::ENGINE::OptimizerAlgBase *PluginOptEvTest1Factory(YACS::ENGINE::Pool *pool);
  }

  class PluginOptEvTest1 : public YACS::ENGINE::OptimizerAlgSync
    {
    private:
      int _idTest;
      YACS::ENGINE::TypeCode *_tcIn;
      YACS::ENGINE::TypeCode *_tcOut;
    public:
      PluginOptEvTest1(YACS::ENGINE::Pool *pool);
      virtual ~PluginOptEvTest1();
      YACS::ENGINE::TypeCode *getTCForIn() const;
      YACS::ENGINE::TypeCode *getTCForOut() const;
      void parseFileToInit(const std::string& fileName);
      void start();
      void takeDecision();
      void initialize(const YACS::ENGINE::Any *input) throw(YACS::Exception);
      void finish();
    };

  using namespace YACS::ENGINE;

  PluginOptEvTest1::PluginOptEvTest1(Pool *pool):OptimizerAlgSync(pool),_tcIn(0),_tcOut(0),_idTest(0)
  {
    _tcIn=new TypeCode(Double);
    _tcOut=new TypeCode(Int);
  }

  PluginOptEvTest1::~PluginOptEvTest1()
  {
    _tcIn->decrRef();
    _tcOut->decrRef();
  }

  TypeCode *PluginOptEvTest1::getTCForIn() const
  {
    return _tcIn;
  }

  TypeCode *PluginOptEvTest1::getTCForOut() const
  {
    return _tcOut;
  }

  void PluginOptEvTest1::parseFileToInit(const std::string& fileName)
  {
  }

  void PluginOptEvTest1::start()
  {
    _idTest=0;
    Any *val=AtomAny::New(1.2);
    _pool->pushInSample(4,val);
    val=AtomAny::New(3.4);
    _pool->pushInSample(9,val);
  }

  void PluginOptEvTest1::takeDecision()
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

  void PluginOptEvTest1::initialize(const Any *input) throw(YACS::Exception)
  {
  }

  void PluginOptEvTest1::finish()
  {
  }

  OptimizerAlgBase *PluginOptEvTest1Factory(Pool *pool)
  {
    return new PluginOptEvTest1(pool);
  }

Here, the entry point in the dynamic library is the name of the factory function : PluginOptEvTest1Factory
that returns an instance of the PluginOptEvTest1 class that implements the algorithm.

Python plugin example
''''''''''''''''''''''
Here, the same example of a synchronous algorithm in Python ::

  import SALOMERuntime
  import pilot

  class myalgo(pilot.OptimizerAlgSync):
    def __init__(self,pool):
      pilot.OptimizerAlgSync.__init__(self,pool)
      self.pool=pool
      r=SALOMERuntime.getSALOMERuntime()
      self.tin=r.getTypeCode("double")
      self.tout=r.getTypeCode("int")

    def setPool(self,pool):
      self.pool=pool

    def getTCForIn(self):
      """returns typecode of type expected as Input"""
      return self.tin

    def getTCForOut(self):
      """returns typecode of type expected as Output"""
      return self.tout

    def initialize(self,input):
      """Called on initialization. Do nothing here"""

    def parseFileToInit(self,fileName):
      """Routine to read and parse an init file given by the OptimizerLoop. Do nothing here"""

    def start(self):
      """Update _pool attribute before performing anything."""
      self.iter=0
      self.pool.pushInSample(4,1.2)
      self.pool.pushInSample(9,3.4)

    def takeDecision(self):
      """ _pool->getCurrentId gives the id at the origin of this call.
          Perform the job of analysing to know what new jobs to do (_pool->pushInSample)
          or in case of convergence _pool->destroyAll
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
      """Called when optimization has succeeded. Do nothing here"""

Here, the entry point in the Python module is directly the name of the class that implements the algorithm : myalgo.


Asynchronous algorithm
--------------------------------------------------
In asynchronous mode, all is the same except that after the initialization phase, the OptimizerLoop calls the algorithm only one time
to start it in a separate thread.

An asynchronous algorithm is implemented in a class derived from the base class OptimizerAlgASync with several methods that 
must be implemented (in C++ and in Python):

- **getTCForIn**, this method must return the YACS type of the input port of the internal node
- **getTCForOut**, this method must return the YACS type of the output port of the internal node
- **initialize**, this method is called during the algorithm initialization
- **parseFileToInit**, this method is called during the algorithm initialization with a file name as argument. This file can be used by the algorithm.
- **startToTakeDecision**, this method is called to start the iteration process in a separate thread. It is the body of the algorithm.
- **finish**, this method is called to finish the algorithm at the end of the iteration process

In Python you need to implement another method:

- **setPool**, this method is used to set the data pool that is used to exchange data

Python plugin example
''''''''''''''''''''''''
Here is an example of an asynchronous algorithm implemented in Python ::

  import SALOMERuntime
  import pilot

  class async(pilot.OptimizerAlgASync):
    def __init__(self,pool):
      pilot.OptimizerAlgASync.__init__(self,pool)
      self.pool=pool
      r=SALOMERuntime.getSALOMERuntime()
      self.tin=r.getTypeCode("double")
      self.tout=r.getTypeCode("int")

    def setPool(self,pool):
      self.pool=pool

    def getTCForIn(self):
      """returns typecode of type expected as Input"""
      return self.tin

    def getTCForOut(self):
      """returns typecode of type expected as Output"""
      return self.tout

    def parseFileToInit(self,fileName):
      """Routine to read and parse an init file given by the OptimizerLoop. Do nothing here"""

    def startToTakeDecision(self,condition):
      """Routine to pilot the algo"""
      val=1.2
      for iter in xrange(5):
        #push a sample in the input of the slave node
        self.pool.pushInSample(iter,val)
        #wait until next sample is ready
        condition.wait()
        #get a sample from the output of the slave node
        currentId=self.pool.getCurrentId()
        v=self.pool.getCurrentOutSample()
        val=val+v.getIntValue()

      #in the end destroy the pool content and release the condition object
      self.pool.destroyAll()
      condition.signal()

Here, the entry point in the Python module is directly the name of the class that implements the algorithm : async.
As you can see, in Python, it is not mandatory to implement the methods initialize and finish as they are not called
by the OptimizerLoop but they are needed in C++ as they are pure virtual methods. 
