.. _resource:

**************************************************
Concurrent branches and multiple machine execution
**************************************************

.. _concurrent:

Execution of concurrent branches
================================
YACS can execute calculation nodes of a scheme simultaneously.  
However, simultaneous execution of a large number of nodes can saturate the system.  
The maximum number of simultaneous executions can be controlled by fixing the maximum number of threads used with the 
YACS_MAX_THREADS environment variable. By default, this value is equal to 50.

Each of these threads needs some memory (stack) for its execution. If too much stack is allocated for each thread,
the system can run out of memory. If too little stack is allocated, you can experience some random crashes as the
thread writes to memory outside its allocated stack. The stack size for YACS threads can be controlled with the
YACS_THREADS_STACK_SIZE environment variable. It defines the size of the stack for each thread in bytes. The
default value is 1048576 (1MB).


.. _multi:

Execution on multiple machines
==============================
YACS can execute the nodes of a scheme on several machines where SALOME is
already installed.
Each machine is a resource which has to be declared in the resources catalog
(see :ref:`catalogResources`).

Every node is executed by a container.
Containers use a set of constraints and rules for choosing the resource where
the node will be executed (see :ref:`containers`).


Execution on multiple machines with mutliples HP container types
================================================================

For a complex graph using multiples HP containers it can be really difficult to determine the best values 
of pool sizes for each HP container type and the numbers of branchs of the ForEach loops. In order to let Yacs resolves 
this problem by itself, one can use the Playground feature: ::

   pilot.PlayGround()
   machineCatalog=[('is2541',8),] # [('machine name', 'nb cores on machine'), ...]
   pg.setData(machineCatalog)
   p.fitToPlayGround(pg)

Principle:
The graph is analyzed and recursively, for all fork, the current domain is equitably divided into sub-domains 
(one for each sub-branch). A domain of a branch corresponds to the cores from the machine catalog which will be used 
for the execution of the nodes of this branch. This division in sub-domains assures that one core will be used by at 
most one container at once, even if it can be used by different HP containers type during the whole execution of a graph.

Once all the domains have been set, the numbers of branchs of the ForEach loops are calculated in order to use the whole domain associated in runtime.
Note: If a branch does not contain any ForEach loop, the size of its domain is reduced to the minimal size.  
