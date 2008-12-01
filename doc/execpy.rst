
.. _execpy:

Exécution d'un schéma de calcul avec l'interface de programmation Python
==========================================================================
Il faut tout d'abord avoir un objet Python schéma de calcul que l'on appellera p.
On l'obtient en utilisant la :ref:`schemapy`.

Si ce n'est pas déjà fait, il faut importer les modules YACS et initialiser YACS
pour SALOME. Ensuite, il faut créer un objet Executor qui va exécuter le schéma de calcul.

Création de l'objet Executor::

   e = pilot.ExecutorSwig()

Un schéma peut être exécuté selon deux modes :
  - le mode standard qui exécute en bloc le schéma en attendant la fin complète 
    de l'exécution
  - le mode pas à pas qui permet d'exécuter partiellement le schéma et de faire
    des pauses et des reprises

Exécution du schéma en mode standard
----------------------------------------
Pour ce type d'exécution, on utilise la méthode RunW de l'Executor. Cette méthode a trois arguments.
Le premier est le schéma de calcul (ou même un noeud composé sous certaines conditions). Le deuxième
indique le niveau de verbosité pour une exécution en mode debug (0 est le niveau sans debug, 1, 2 ou 3
pour debug). Le troisième est un booléen qui vaut True si l'exécution part de zéro (tous les noeuds
sont calculés) ou False si l'exécution part d'un état sauvegardé du schéma. Ces deux derniers arguments
ont pour valeur par défaut 0 et True.

Exécution du schéma par l'objet Executor en partant de zéro, sans debug::

   e.RunW(p)
   if p.getEffectiveState() != pilot.DONE:
     print p.getErrorReport()
     sys.exit(1)

Si l'exécution s'est bien déroulée, l'état du schéma obtenu par ``p.getEffectiveState()``
vaut DONE. Si ce n'est pas le cas, le schéma est en erreur. Un compte-rendu d'erreurs (voir :ref:`errorreport`)
est donné par ``p.getErrorReport()``.

Dans tous les cas, il est ensuite possible de récupérer les valeurs contenues dans les ports
d'entrée et de sortie.

Sauvegarde de l'état d'un schéma 
------------------------------------
Suite à une exécution d'un schéma, il est possible de sauvegarder l'état du schéma dans un fichier
XML pour un redémarrage ultérieur éventuel. Cette sauvegarde est réalisée en utilisant la méthode
saveState du schéma de calcul. Elle prend un argument qui est le nom du fichier dans lequel
sera sauvegardé l'état.

Voici comme se présente cet appel::

  p.saveState("state.xml")

Chargement de l'état d'un schéma
------------------------------------
Il est possible d'initialiser un schéma de calcul avec un état sauvegardé dans un fichier. Le schéma
de calcul doit bien entendu exister. Ensuite, il faut utiliser la fonction loadState du module loader. 
Elle prend deux arguments : le schéma de calcul et le nom du fichier contenant l'état sauvegardé.

Par exemple, pour initialiser le schéma p avec l'état précédemment sauvegardé, on fera::

  import loader
  loader.loadState(p,"state.xml")

Cette initialisation n'est possible que si les structures du schéma et de l'état sauvegardé sont identiques.

Après initialisation, on démarre l'exécution en passant False en troisième argument de RunW::

  e.RunW(p,0,False)

Exécution du schéma en mode pas à pas
----------------------------------------
Ce type d'exécution est pour des utilisateurs avancés car il nécessite l'utilisation de la programmation
par thread en Python. Ne pas oublier d'importer le module threading::

  import threading

Si on veut utiliser le mode pas à pas ou mettre des breakpoints, il faut exécuter le schéma dans un thread
séparé et réaliser les opérations de contrôle de l'exécution dans le thread principal. 

Exécution avec arrêt sur breakpoints
+++++++++++++++++++++++++++++++++++++++
Pour réaliser une exécution avec breakpoints, il faut définir la liste des breakpoints, mettre le mode
d'exécution à exécution avec breakpoints puis de lancer l'exécution dans un thread séparé.

La liste des breakpoints est définie en passant une liste de noms de noeud à la méthode setListOfBreakPoints
de l'exécuteur.
Le mode est défini en utilisant la méthode setExecMode de l'exécuteur avec un paramètre de valeur 2.
Les valeurs possibles sont :
  
- 0, pour un mode d'exécution standard (mode CONTINUE)
- 1, pour un mode d'exécution avec pause à chaque pas d'exécution (mode STEPBYSTEP)
- 2, pour un mode d'exécution avec breakpoints (mode STOPBEFORENODES)

Le lancement de l'exécution se fait avec la méthode RunB de l'exécuteur à la place de RunW pour une exécution
standard. Les trois arguments sont les mêmes :

- le schéma de calcul
- le niveau d'affichage (0,1,2,3)
- exécution de zéro (True, False)

Par exemple, pour s'arrêter avant le noeud "node1", on écrira::

  e.setListOfBreakPoints(['node1'])
  e.setExecMode(2)
  mythread=threading.Thread(target=e.RunB, args=(p,1))
  mythread.start()

Il est possible de visualiser l'état des noeuds du schéma en appelant la méthode 
displayDot pendant une pause::

  e.displayDot(p)

Ensuite, il faut attendre que l'exécution passe en pause : une exécution passe en pause
quand toutes les taches possibles avant l'exécution du noeud "node1" sont terminées.
On utilise la méthode waitPause pour faire cette attente. Ensuite on arrête l'exécution
par appel de la méthode stopExecution et on libère le thread. Il est conseillé de faire
une petite attente avant de faire l'attente de la pause.
Au total, on ajoutera la séquence suivante::

  time.sleep(0.1)
  e.waitPause()
  e.stopExecution()
  mythread.join()

Arrêt sur breakpoint suivi d'une reprise jusqu'à la fin du schéma
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Pour reprendre une exécution après un arrêt sur breakpoint, il suffit de passer en mode
CONTINUE et de relancer l'exécution en appelant la méthode resumeCurrentBreakPoint.
L'attente de la fin de l'exécution du schéma est réalisée par appel de waitPause.

La séquence d'appel pour reprise sur breakpoint est la suivante::

  e.setExecMode(0)
  e.resumeCurrentBreakPoint()
  time.sleep(0.1)
  e.waitPause()
  mythread.join()

Exécution en mode pas à pas
+++++++++++++++++++++++++++++
Lors d'une exécution en mode pas à pas, l'exécuteur de schéma de calcul se met en pause
à chaque pas d'exécution. Un pas d'exécution correspond à l'exécution d'un groupe de noeuds
de calcul. Ce groupe peut contenir plus d'un noeud de calcul si le schéma de calcul 
présente des branches d'exécution parallèles.

Pour passer dans ce mode on appelle la méthode setExecMode avec la valeur 1 (mode STEPBYSTEP) puis
on lance l'exécuteur dans un thread comme précédemment. On attend la fin du pas d'exécution avec 
la méthode waitPause. On obtient la liste des noeuds du pas suivant avec la méthode getTasksToLoad.
On définit la liste des noeuds à exécuter avec la méthode setStepsToExecute et on relance l'exécution
avec la méthode resumeCurrentBreakPoint.
Le tout doit être mis dans une boucle dont on sort à la fin de l'exécution quand il n'y a plus de noeuds
à exécuter.

La séquence complète se présente comme suit::

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


