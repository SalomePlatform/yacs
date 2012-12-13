.. _resource:

*******************************************************
Concurrent branches and multiple machine execution
*******************************************************

.. _concurrent:

Execution of concurrent branches
===================================
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
===================================
YACS can execute the nodes of a scheme on several machines where SALOME is
already installed.
Each machine is a resource which has to be declared in the resources catalog
(see :ref:`catalogResources`).

Every node is executed by a container.
Containers use a set of constraints and rules for choosing the resource where
the node will be executed (see :ref:`containers`).

