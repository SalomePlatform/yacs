
.. _batch:

Starting a SALOME application in a batch manager
================================================================

This section explains how Salome is used with batch managers used in the operation of clusters.  
The objective is to run a Salome application with a command script on a cluster starting from a 
Salome session running on the user's personal machine.  The script contains the task that the user 
wants Salome to execute.  The most usual task is to start a YACS scheme.

Principles
-----------
The start principle is as follows:  starting from a first Salome session, a Salome application is started 
on a cluster using the batch manager.  Therefore there are two Salome installations:  one on the user’s machine 
and the other on the cluster.  The user must have an account on the cluster, and must have a read/write access to it.  
He must also have correctly configured the connection protocol from his own station, regardless of whether he uses rsh or ssh.

The remainder of this chapter describes the different run steps.  Firstly, a Salome application is run on 
the user’s machine using a CatalogResources.xml file containing the description of the target batch 
machine (see :ref:`catalogresources_batch`).  The user then calls the Salome service to run it on the batch machine.  
The user does this by describing input and output files for the Salome application running in batch 
and for the Python script to be run (see :ref:`service_launcher`).  This service then starts the Salome 
application defined in the CatalogResources.xml file on the batch machine and executes the Python 
command file (see :ref:`salome_clusteur_batch`).

.. _catalogresources_batch:

Description of the cluster using the CatalogResource.xml file
--------------------------------------------------------------------

The CatalogResources.xml file contains the description of the different distributed calculation 
resources (machines) that Salome can use to launch its containers.  It can also contain the description 
of clusters administered by batch managers.

The following is an example of description of a cluster:

::

  <machine hostname="clusteur1" 
	   alias="frontal.com" 
	   protocol="ssh"
	   userName="user"
	   mode="batch" 
	   batch="lsf"
	   mpi="prun"
	   appliPath="/home/user/applis/batch_exemples" 
	   batchQueue="mpi1G_5mn_4p"
	   userCommands="ulimit -s 8192"
	   preReqFilePath="/home/ribes/SALOME4/env-prerequis.sh" 
	   OS="LINUX" 
	   CPUFreqMHz="2800" 
	   memInMB="4096" 
	   nbOfNodes="101" 
	   nbOfProcPerNode="2"/>
  
The following is the description of the different fields used when launching a batch:
 - **hostname**:  names the cluster for Salome commands.  Warning, this name is not used to identify the cluster front end.
 - **alias**:  names the cluster front end.  It must be possible to reach this machine name using the protocol 
   defined in the file.  This is the machine that will be used to start the batch session.
 - **protocol**:  fixes the connection protocol between the user session and the cluster front end.  
   The possible choices are rsh or ssh.
 - **userName**:  user name on the cluster.
 - **mode**:  identifies the description of the machine as a cluster managed by a batch.  The possible choices are 
   interactive and batch.  The batch option must be chosen for the machine to be accepted as a cluster with a batch manager.
 - **batch**:  identifies the batch manager.  Possible choices are:  pbs, lsf or sge.
 - **mpi**:  Salome uses mpi to Start the Salome session and containers on different calculation nodes allocated 
   by the batch manager.  Possible choices are lam, mpich1, mpich2, openmpi, slurm and prun.  Note that some 
   batch managers replace the mpi launcher with their own launcher for management of resources, which is the 
   reason for the slurm and prun options.
 - **appliPath**:  contains the path of the Salome application previously installed on the cluster.

There are two optional fields that can be useful depending on the configuration of clusters.
 - **batchQueue**:  specifies the queue of the batch manager to be used
 - **userCommands**:  to insert the sh code when salome is started.  This code is executed on all nodes.

.. _service_launcher:

Utilisation du service Launcher
-------------------------------

Le service Launcher est un serveur CORBA lancé par le noyau de Salomé. Son interface est décrite dans le
fichier **SALOME_ContainerManager.idl** du noyau.

Voici son interface:

::

  interface SalomeLauncher
  {
    long submitJob( in string xmlExecuteFile,
		    in string clusterName ) raises (SALOME::SALOME_Exception);
    long submitSalomeJob( in string fileToExecute,
			  in FilesList filesToExport,
			  in FilesList filesToImport,
			  in BatchParameters batch_params,
			  in MachineParameters params ) raises (SALOME::SALOME_Exception);

    string queryJob ( in long jobId, in MachineParameters params ) raises (SALOME::SALOME_Exception);
    void   deleteJob( in long jobId, in MachineParameters params ) raises (SALOME::SALOME_Exception);

    void getResultsJob( in string directory, in long jobId, in MachineParameters params ) 
         raises (SALOME::SALOME_Exception);

    boolean testBatch(in MachineParameters params) raises (SALOME::SALOME_Exception);

    void Shutdown();
    long getPID();
  };

La méthode **submitSalomeJob** permet de lancer une application Salomé sur un gestionnaire de batch. 
Cette méthode retourne un identifiant de **job** qui est utilisé dans les méthodes **queryJob**, 
**deleteJob** et **getResultsJob**.

Voici un exemple d'utilisation de cette méthode:

::

  # Initialisation
  import os
  import Engines
  import orbmodule
  import SALOME

  clt = orbmodule.client()
  cm  = clt.Resolve('SalomeLauncher')

  # Le script python qui va être lancé sur le clusteur
  script = '/home/user/Dev/Install/BATCH_EXEMPLES_INSTALL/tests/test_Ex_Basic.py'

  # Préparation des arguments pour submitSalomeJob
  filesToExport = []
  filesToImport = ['/home/user/applis/batch_exemples/filename']
  batch_params = Engines.BatchParameters('', '00:05:00', '', 4)
  params = Engines.MachineParameters('','clusteur1','','','','',[],'',0,0,1,1,0,'prun','lsf','','',4)

  # Utilisation de submitSalomeJob
  jobId = cm.submitSalomeJob(script, filesToExport, filesToImport, batch_params, params)


Voici la description des différents arguments de **submitSalomeJob**:

- **fileToExecute**: il s'agit du script python qui sera exécuté dans l'application Salomé sur le clusteur.
  Cet argument contient le chemin du script **sur** la machine locale et **non** sur le clusteur.
- **filesToExport**: il s'agit d'une liste de fichiers qui seront copiés dans le répertoire de lancement sur 
  le clusteur.
- **filesToImport**: il s'agit d'une liste de fichiers qui seront copiés depuis le clusteur sur la 
  machine utilisateur lors de l'appel à la méthode **getResultsJob**.
- **batch_params**: il s'agit d'une structure qui contient des informations qui seront données au gestionnaire 
  de batch. Cette structure est composée de quatre arguments. Le premier argument permettra de donner le nom du 
  répertoire où l'on veut que les fichiers et l'application Salomé soit lancée (actuellement cette fonction n'est pas 
  disponible). Le deuxième argument est le temps demandé. Il est exprimé sous cette forme: hh:mn:se, ex: 01:30:00. 
  Le troisième argument est la mémoire requise. Elle est exprimée sous la forme: 32gb ou encore 512mb. Enfin, 
  le dernier argument décrit le nombre de processeurs demandé.
- **params**: il contient la description de la machine souhaitée. Ici on identifie clairement sur quel clusteur
  on veut lancer l'application.

Pour connaitre dans quel état est le Job, il faut utiliser la méthode **queryJob**. Il y a trois états possibles: 
**en attente**, **en exécution** et **terminé**.
Voici un exemple d'utilisation de cette méthode:

::

  status = cm.queryJob(jobId, params)
  print jobId,' ',status
  while(status != 'DONE'):
    os.system('sleep 10')
    status = cm.queryJob(jobId, params)
    print jobId,' ',status

L'identifiant du job fournit par la méthode **submitSalomeJob** est utilisé dans cette méthode ainsi que la 
structure **params**.

Enfin pour récupérer les résultats de l'application, il faut utiliser la méthode **getResultsJob**.
Voici un exemple d'utilisation de cette méthode:
::

  cm.getResultsJob('/home/user/Results', jobId, params)

Le premier argument contient le répertoire où l'utilisateur veut récupérer les résultats. En plus de ceux
définis dans la liste **filesToImport**, l'utilisateur reçoit automatiquement les logs de l'application
Salomé et des différents containers qui ont été lancés.

.. _salome_clusteur_batch:

Salomé sur le clusteur batch
----------------------------

Salomé ne fournit par pour l'instant un service pour l'installation automatique de la plateforme depuis
la machine personnelle de d'utilisateur. Il faut donc que Salomé (KERNEL + des modules) et une application Salomé
soient préinstallés sur le clusteur. Dans l'exemple suivit dans cette documentation, l'application est installée dans
le répertoire **/home/user/applis/batch_exemples**.

Lors du l'utilisation de la méthode **submitSalomeJob**, Salomé crée un répertoire dans $HOME/Batch/**date_du_lancement**.
C'est dans ce répertoire que les différents fichiers d'entrées sont copiés.

Contraintes de Salomé sur les gestionnaires de batch
----------------------------------------------------

Salomé a besoin de certaines fonctionnalités que le gestionnaire de batch doit autoriser pour que le lancement
d'applications Salomé soit possible. 

Salomé lance plusieurs **threads** par processeurs pour chaque serveur CORBA qui est lancé.
Certains gestionnaires de batch peuvent limiter le nombre de threads à un nombre trop faible ou encore le gestionnaire 
de batch peut avoir configurer la taille de pile des threads à un niveau trop haut. Dans notre exemple, la taille
de pile des threads est fixée par l'utilisateur dans le champ **userCommands** du fichier CatalogResources.xml.

Salomé lance des processus dans la session sur les machines allouées par le gestionnaire de batch. 
Il faut donc que le gestionnaire de batch l'autorise.

Enfin, Salomé est basée sur l'utilisation de bibliothèques dynamiques et sur l'utilisation de la fonction **dlopen**. 
Il faut que le système le permette.

