
.. _batch:

Lancement d'une application Salomé dans un gestionnaire de Batch
================================================================

Cette section explique comment utiliser Salomé avec les gestionnaires
de batch qui sont utilisés dans l'exploitation de clusteurs.
L'objectif est de lancer une application Salomé avec un script de commande sur un clusteur à partir 
d'une session Salomé lancée sur la machine personnelle de l'utilisateur. Le script
contient la tâche que l'utilisateur veut que Salomé exécute. Il s'agit le plus souvent
du lancement d'un schéma YACS.

Principes
---------

Le principe du lancement est le suivant: depuis une première session Salomé, une application
Salomé est lancée sur le clusteur par le biais du gestionnaire de batch. Il y a donc deux
installations de Salomé: une sur la machine de l'utilisateur et une autre sur le clusteur.
Sur le clusteur, l'utilisateur doit avoir un compte, y avoir accès en lecture/écriture. De plus, il doit 
avoir configuré correctement le protocole de connexion depuis son poste, qu'il utilise rsh ou ssh.

La suite de ce chapitre détaille les différentes étapes du lancement. Tout d'abord, une application Salomé 
est lancée sur la machine de l'utilisateur avec un fichier CatalogResources.xml contenant la description 
de la machine batch visée (voir :ref:`catalogresources_batch`). Ensuite, l'utilisateur appelle le service de Salomé pour 
le lancement sur machine batch. Pour cela, l'utilisateur décrit les fichiers d'entrées et de sorties de l'application 
Salomé lancée en batch ainsi que du script python à lancer (voir :ref:`service_launcher`). Ce service
lance ensuite l'application Salomé définit dans le fichier CatalogResources.xml sur la machine batch et 
exécute le fichier de commande python (voir :ref:`salome_clusteur_batch`).

.. _catalogresources_batch:

Description du clusteur par le biais du fichier CatalogResources.xml
--------------------------------------------------------------------

Le fichier CatalogResources.xml contient la description des différentes ressources de calcul (machines) 
distribuées que Salomé peut utiliser pour lancer ses containers. Il peut aussi contenir la description de 
clusteurs administrés par des gestionnaires de batch.

Voici un exemple de description d'un clusteur:

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
  
Voici la description des différents champs utilisés dans un lancement batch:

- **hostname**: nomme le clusteur pour les commandes de Salomé. Attention, ce nom n'est pas 
  utilisé pour identifier le frontal du clusteur.
- **alias**: nomme le frontal du clusteur. Ce nom de machine doit être atteignable par le protocole 
  définit dans le fichier. C'est cette machine qui sera utilisé pour lancer la session batch.
- **protocol**: fixe le protocole de connexion entre la session utilisateur et le frontal du clusteur.
  Choix possibles: **rsh** ou **ssh**.
- **userName**: nom de l'utilisateur sur le clusteur.
- **mode**: identifie la description de la machine comme un clusteur géré par un batch.
  Choix possibles: **interactive** ou **batch**. Pour que la machine soit prise en compte comme étant un clusteur
  avec un gestionnaire de batch, il faut choisir l'option **batch**.
- **batch**: identifie le gestionnaire de batch. Choix possibles: **pbs**, **lsf** ou **sge**.
- **mpi**: Salomé utilise **mpi** pour lancer la session Salomé et les containers sur les différents noeuds 
  de calcul alloués par le gestionnaire de batch. Choix possibles: **lam**, **mpich1**, **mpich2**, **openmpi**, 
  **slurm** ou **prun**. Il faut noter que certains gestionnaires de batch remplacent le lanceur de mpi 
  par leur propre lanceur pour la gestion des ressources, d'où les options **slurm** et **prun**.
- **appliPath**: contient le chemin de l'application Salomé préalablement installée sur le clusteur.

Il existe deux champs optionnels qui peuvent être utiles selon la configuration des clusteurs:

- **batchQueue**: spécifie la queue du gestionnaire de batch à utiliser.
- **userCommands**: permet d'insérer du code **sh** lors du lancement de Salomé. Ce code est exécuté sur tous
  les noeuds.  


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

