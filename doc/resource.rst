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


.. _multi_HP:

Execution on multiple machines with mutliples HP container types
================================================================

For a complex graph using multiples HP containers it can be really difficult to determine the best values 
of pool sizes for each HP container type and the numbers of branchs of the ForEach loops. In order to let Yacs resolves 
this problem by itself, one can use the Playground feature: ::

   pilot.PlayGround()
   machineCatalog=[('is2541',8),] # [('machine name', 'nb cores on machine'), ...]
   pg.setData(machineCatalog)
   p.fitToPlayGround(pg)

Principle
---------
The graph is analyzed and recursively, for all fork, the current domain is equitably divided into sub-domains 
(one for each sub-branch). A domain of a branch corresponds to the cores from the machine catalog which will be used 
for the execution of the nodes of this branch. This division in sub-domains assures that one core will be used by at 
most one container at once, even if it can be used by different HP containers type during the whole execution of a graph.

Once all the domains have been set, the numbers of branchs of the ForEach loops are calculated in order to use the whole domain associated in runtime.

Note: If a branch does not contain any ForEach loop, the size of its domain is reduced to the minimal size.  

Graph with multiples ForEach Loop executed in parallel
------------------------------------------------------
In some cases it can be better to dispatch unfairly the computing power between the ForEach Loops. In order to do so it is possible to allocate 
a weight to the nodes of the graph: ::

   elem_node.setWeight(30.) # means execution time of the node is 30s
   for_each_loop.setWeight(500.) # means execution time of the whole for each is 500s (exemple: 10 iterations and 50s each)
   ...
   p.fitToPlayGround(pg)
   
Assuming that the weight corresponds to the execution time of each node during a sequential execution, the algorithm automatically determines, at all fork, 
the best way to divide the domain into sub-domains in order to minimize the execution time of whole graph.

Explanation: When the computing power allocated to a branch is multiplied by a factor ‘a’, thus weights of all ForEach nodes of this branch 
are divided by the same factor. Using this the algorithm searchs the best sharing between all branchs at a fork in order the minimize the following 
parameter: max(for all branchs at the fork: weight of the branch).

Note: 

- If at least one node of a branch does not have a weight, the weight of the branch cannot be calculated and thus its domain receives 1/n of the computing power (n being the number of branchs containing at least one ForEach Loop at this fork point).
- The weight does not have a unit. Any can be used.
- The calculation of the weight of a whole branch is not recursive. It is the sum of the weight of all its elementary and ForEach nodes (but as seen previously the sharing is done recursively).
 



