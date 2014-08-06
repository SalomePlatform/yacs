
.. _batch:

Starting a SALOME application in a batch manager
================================================================

This section explains how SALOME is used with batch managers used in the operation of clusters.  
The objective is to run a SALOME application with a command script on a cluster starting from a 
SALOME session running on the user's personal machine.  The script contains the task that the user 
wants SALOME to execute.  The most usual task is to start a YACS scheme.

Principles
-----------
The start principle is as follows:  starting from a first SALOME session, a SALOME application is started 
on a cluster using the batch manager.  Therefore there are two SALOME installations:  one on the user’s machine 
and the other on the cluster.  The user must have an account on the cluster, and must have a read/write access to it.  
He must also have correctly configured the connection protocol from his own station, regardless of whether he uses rsh or ssh.

The remainder of this chapter describes the different run steps.  Firstly, a SALOME application is run on 
the user’s machine using a CatalogResources.xml file containing the description of the target batch 
machine (see :ref:`catalogresources_batch`).  The user then calls the SALOME service to run it on the batch machine.  
The user does this by describing input and output files for the SALOME application running in batch 
and for the Python script to be run (see :ref:`service_launcher`).  This service then starts the SALOME 
application defined in the CatalogResources.xml file on the batch machine and executes the Python 
command file (see :ref:`salome_clusteur_batch`).

.. _catalogresources_batch:

Description of the cluster using the CatalogResource.xml file
--------------------------------------------------------------------

The CatalogResources.xml file contains the description of the different distributed calculation 
resources (machines) that SALOME can use to launch its containers.  It can also contain the description 
of clusters administered by batch managers.

The following is an example of description of a cluster:

.. code-block:: xml

  <machine name = "clusteur1"
           hostname = "frontal.com"
           type = "cluster"
           protocol = "ssh"
           userName = "user"
           batch = "lsf"
           canLaunchBatchJobs = "true"
           mpi = "prun"
           appliPath = "/home/user/applis/batch_exemples"
           batchQueue = "mpi1G_5mn_4p"
           userCommands = "ulimit -s 8192"
           preReqFilePath = "/home/ribes/SALOME5/env-prerequis.sh"
           OS = "LINUX"
           CPUFreqMHz = "2800"
           memInMB = "4096"
           nbOfNodes = "101"
           nbOfProcPerNode = "2"/>
  
The following is the description of the different fields used when launching a batch:
 - **name**: names the cluster for SALOME commands. Warning, this name is not used to identify the cluster front end.
 - **hostname**: names the cluster front end. It must be possible to reach this machine name using the protocol 
   defined in the file.  This is the machine that will be used to start the batch session.
 - **protocol**:  fixes the connection protocol between the user session and the cluster front end.  
   The possible choices are rsh or ssh.
 - **userName**:  user name on the cluster.
 - **type**: identifies the machine as a single machine or a cluster managed by a batch. The possible choices are 
   "single_machine" or "cluster". The "cluster" option must be chosen for the machine to be accepted as a cluster with a batch manager.
 - **batch**:  identifies the batch manager.  Possible choices are:  pbs, lsf or sge.
 - **mpi**:  SALOME uses mpi to Start the SALOME session and containers on different calculation nodes allocated 
   by the batch manager.  Possible choices are lam, mpich1, mpich2, openmpi, slurm and prun.  Note that some 
   batch managers replace the mpi launcher with their own launcher for management of resources, which is the 
   reason for the slurm and prun options.
 - **appliPath**:  contains the path of the SALOME application previously installed on the cluster.
 - **canLaunchBatchJobs**: Indicates that the cluster can be used to launch batch jobs. Must be set to "true"
   in order to use this cluster to launch a schema in batch mode.

There are two optional fields that can be useful depending on the configuration of clusters.
 - **batchQueue**:  specifies the queue of the batch manager to be used
 - **userCommands**:  to insert the sh code when SALOME is started.  This code is executed on all nodes.

.. _service_launcher:


Using the Launcher service
-------------------------------
The Launcher service is a CORBA server started by the SALOME kernel.  Its interface is described in the 
**SALOME_Launcher.idl** file of the kernel.

Its interface is as follows:

::

    interface SalomeLauncher
    {
      // Main methods
      long   createJob    (in Engines::JobParameters job_parameters) raises (SALOME::SALOME_Exception);
      void   launchJob    (in long job_id)                           raises (SALOME::SALOME_Exception);
      string getJobState  (in long job_id)                           raises (SALOME::SALOME_Exception);
      string getAssignedHostnames  (in long job_id)                  raises (SALOME::SALOME_Exception); // Get names or ids of hosts assigned to the job
      void   getJobResults(in long job_id, in string directory)      raises (SALOME::SALOME_Exception);
      boolean getJobDumpState(in long job_id, in string directory)   raises (SALOME::SALOME_Exception);
      void   stopJob      (in long job_id)                           raises (SALOME::SALOME_Exception);
      void   removeJob    (in long job_id)                           raises (SALOME::SALOME_Exception);
    
      // Useful methods
      long    createJobWithFile(in string xmlJobFile, in string clusterName) raises (SALOME::SALOME_Exception);
      boolean testBatch        (in ResourceParameters params)                raises (SALOME::SALOME_Exception);
    
      // SALOME kernel service methods
      void Shutdown();
      long getPID();
    
      // Observer and introspection methods
      void addObserver(in Engines::SalomeLauncherObserver observer);
      void removeObserver(in Engines::SalomeLauncherObserver observer);
      Engines::JobsList getJobsList();
      Engines::JobParameters getJobParameters(in long job_id) raises (SALOME::SALOME_Exception);
    
      // Save and load methods
      void loadJobs(in string jobs_file) raises (SALOME::SALOME_Exception);
      void saveJobs(in string jobs_file) raises (SALOME::SALOME_Exception);
    
    };

The **createJob** method creates the job itself and returns a **job** identifier that can be used in the
**launchJob**, **getJobState**, **stopJob** and **getJobResults** methods. The **launchJob** method
submits the job to the batch manager.  

The following is an example using those methods:

::

  # Initialization
  import salome
  salome.salome_init()
  launcher = salome.naming_service.Resolve('/SalomeLauncher')

  # The python script that will be launched on the cluster 
  script = '/home/user/Dev/Install/BATCH_EXEMPLES_INSTALL/tests/test_Ex_Basic.py'

  # Define job parameters
  job_params = salome.JobParameters()
  job_params.job_name = "my_job"
  job_params.job_type = "python_salome"
  job_params.job_file = script
  job_params.in_files = []
  job_params.out_files = ['/scratch/user/applis/batch_exemples/filename']

  # Define resource parameters
  job_params.resource_required = salome.ResourceParameters()
  job_params.resource_required.name = "clusteur1"
  job_params.resource_required.nb_proc = 24

  # Create and submit the job
  jobId = launcher.createJob(job_params)
  launcher.submitJob(jobId)

The following is a description of the main parameters of **JobParameters** structure:

- **job_type**: This is the type of the job to run (use "python_salome" to run a Python script in a Salome session).
- **job_file**: This is the python script that will be executed in the SALOME application on the cluster.  
  This argument contains the script path **on** the local machine and **not on** the cluster.
- **in_files**:  this is a list of files that will be copied into the run directory on the cluster
- **out_files**:  this is a list of files that will be copied from the cluster onto the user machine when the **getJobResults** method is called.
- **resource_required**:  contains the description of the required machine. In this case, the cluster on which the application is to be launched 
  is clearly identified.

The **getJobState** method should be used to determine the state of the Job. The following is an example of how this method is used:

::

  status = launcher.getJobState(jobId)
  print jobId,' ',status
  while(status != 'FINISHED'):
    os.system('sleep 10')
    status = launcher.getJobState(jobId)
    print jobId,' ',status

Finally, the **getJobResults** method must be used to retrieve application results.  
The following is an example of how to use this method:
::

  launcher.getJobResults(jobId, '/home/user/Results')

The second argument contains the directory in which the user wants to retrieve the results.  The user automatically receives 
logs from the SALOME application and the different containers that have been started, in addition to those defined in the **out_files** list.

.. _salome_clusteur_batch:

SALOME on the batch cluster
----------------------------------------------------
SALOME does not provide a service for automatic installation of the platform from the user’s personal machine, for the moment.  
Therefore, SALOME (KERNEL + modules) and a SALOME application have to be installed beforehand on the cluster.  
In the example used in this documentation, the application is installed in the directory **/home/user/applis/batch_exemples**.

When the **submitJob** method is being used, SALOME creates a directory in $HOME/Batch/**run_date**.
The various input files are copied into this directory.

SALOME constraints on batch managers
----------------------------------------------------
SALOME needs some functions that the batch manager must authorise before SALOME applications can be run.

SALOME runs several processor **threads** for each CORBA server that is started.  
Some batch managers can limit the number of threads to a number that is too small, or the batch manager may configure the size 
of the thread stack so that it is too high.  
In our example, the user fixes the size of the thread stack in the **userCommands** field in the CatalogResources.xml file.

SALOME starts processes in the session on machines allocated by the batch manager.  Therefore, the batch manager must authorise this.
Finally, SALOME is based on the use of dynamic libraries and the **dlopen** function.  The system must allow this.
