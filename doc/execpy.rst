
.. _execpy:

Execution of a calculation scheme with the Python programming interface
==========================================================================
The first step is to have a Python calculation scheme object that will be called p.  
It is obtained using :ref:`schemapy`.

If it has not already been done, YACS modules have to be imported and YACS needs to be initialised 
for SALOME.  An Executor object will then have to be created that will execute the calculation scheme.

Create the Executor object::

   e = pilot.ExecutorSwig()

There are two modes by which a scheme may be executed:

- standard mode that executes the scheme in block, waiting until the execution is completely finished
- step by step mode, to partially execute the scheme and make pauses and restarts.

Executing the scheme in standard mode
----------------------------------------
The Executor RunW method is used for this type of execution.  This method has three arguments.  
The first is the calculation scheme (or even a composite node under some conditions).  The second indicates 
the verbosity for execution in debug mode (0 is the level without debug, 1, 2 or 3 for debug).  
The third is a boolean that is equal to True if execution starts from zero (all nodes are calculated) or False if 
execution starts from a backed up state of the scheme.  
The default values of the latter two arguments are equal to 0 and True.

Execution of the scheme by the Executor object starting from zero, without debug::

   e.RunW(p)
   if p.getEffectiveState() != pilot.DONE:
     print p.getErrorReport()
     sys.exit(1)

If the execution took place correctly, the state of the scheme obtained by ``p.getEffectiveState()`` is equal to DONE.  
If this is not the case, the scheme is in error.  An error report (see :ref:`errorreport`) is given by ``p.getErrorReport()``.

In all cases, the values contained in the input and output ports can then be retrieved.

Saving the state of a scheme
------------------------------------
The state of a scheme after it has been executed can be saved in an XML file for a later restart if required.  
This save is done using the saveState method for the calculation scheme.  It uses an argument that is the name of 
the file in which the state will be saved.

This call is in the following format::

  p.saveState("state.xml")

Loading the state of a scheme
------------------------------------
A calculation scheme can be initialised with a state saved in a file.  Obviously, the calculation scheme must exist.  
Then, the loadState function of the loader module must be used.  It takes two arguments:  the calculation scheme and 
the name of the file containing the saved state.

For example, proceed as follows to initialise scheme p with the previously saved state::

  import loader
  loader.loadState(p,"state.xml")

This initialisation is only possible if the structures of the scheme and the saved state are identical.

After initialisation, execution is started by transferring False as a third argument of RunW::

  e.RunW(p,0,False)

Executing the scheme in step by step mode
-----------------------------------------------
This type of execution is for advanced users because it requires the use of thread programming in Python.  
Do not forget to import the threading module::

  import threading

If it is required to use step by step mode or to add breakpoints, the scheme will have to be executed in a 
separate thread and execution control operations will have to be done in the main thread.

Execution with stop on breakpoints
+++++++++++++++++++++++++++++++++++++++
Before execution with breakpoints is possible, it is necessary to define the list of breakpoints, to set 
execution mode to execution with breakpoints, and then to start execution in a separate thread.

The list of breakpoints is defined by transferring a list of node names to the executor setListOfBreakPoints method.  
The mode is defined using the executor setExecMode method with a parameter equal to 2.  
Possible values are:

- 0, for standard execution mode (CONTINUOUS mode)
- 1, for execution mode with pause at each execution step (STEPBYSTEP mode)
- 2, for execution mode with breakpoints (STOPBEFORENODES mode).

Execution is started using the executor RunB method instead of RunW for a standard execution.  
The three arguments are the same:

- the calculation scheme
- the display level (0,1,2,3)
- execution start from zero (True,  False)

For example, the following will be written to stop before node “node1”::

  e.setListOfBreakPoints(['node1'])
  e.setExecMode(2)
  mythread=threading.Thread(target=e.RunB, args=(p,1))
  mythread.start()

The state of nodes in the scheme can be displayed by calling the displayDot method during a pause::
 
  e.displayDot(p)

It is then necessary to wait until the execution changes to pause:  an execution changes to pause when 
all possible tasks before execution of node “node1” are terminated.  The waitPause method is used to 
control this wait.  The next step will be to stop execution by calling the stopExecution method and the 
thread will then be released.  It is recommended that there should be a short wait before waiting for the pause.  
In total, the following sequence will be added::

  time.sleep(0.1)
  e.waitPause()
  e.stopExecution()
  mythread.join()

A stop on breakpoint followed by resuming until the end of the scheme
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
After a stop on breakpoint, an execution is resumed by changing to CONTINUE mode and then restarting 
the execution by calling the resumeCurrentBreakPoint method.  
The wait for the end of scheme execution is controlled by calling waitPause.

The call sequence to resume on breakpoint is as follows::

  e.setExecMode(0)
  e.resumeCurrentBreakPoint()
  time.sleep(0.1)
  e.waitPause()
  mythread.join()

Execution in step by step mode
+++++++++++++++++++++++++++++++++++++
When executing in step by step mode, the calculation scheme executor pauses at each execution step.  
An execution step corresponds to execution of a group of calculation nodes.  This group may contain 
more than one calculation node if the calculation scheme contains parallel execution branches.

This mode is selected by calling the setExecMode method using the value 1 (STEPBYSTEP mode), and the executor 
is then started in a thread in the same way as above.  The waitPause method is used to wait until the end of 
the execution step.  The list of nodes in the next step is obtained using the getTasksToLoad method.  
The list of nodes to be executed is defined using the setStepsToExecute method and the execution is 
resumed using the resumeCurrentBreakPoint method.  
Everything must be placed in a loop that is exited at the end of the execution when there are no longer any nodes to be executed.

The complete sequence is as follows::

  e.setExecMode(1)
  mythread=threading.Thread(target=e.RunB, args=(p,0))
  mythread.start()
  time.sleep(0.1)

  tocont = True
  while tocont:
    e.waitPause()
    bp=e.getTasksToLoad()
    if bp:
      e.setStepsToExecute(bp)
    else:
      tocont=False
    e.resumeCurrentBreakPoint()

  time.sleep(0.1)
  mythread.join()


