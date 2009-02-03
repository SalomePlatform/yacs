
.. _execxml:

Exécution d'un schéma de calcul en mode console
========================================================
Pour exécuter un schéma de calcul, au format XML, en mode console, il faut disposer d'une installation
de SALOME bien configurée. Dans tout ce qui suit, on utilisera le mode d'exécution de SALOME
à partir d'une `application SALOME <http://nepal.der.edf.fr/pub/SALOME_userguide/KERNEL4/doc/salome/gui/KERNEL/SALOME_Application.html>`_.

 1. lancer SALOME : ./runAppli -t depuis le répertoire d'application, par exemple (on peut le faire de 
    n'importe quel autre répertoire).
 2. exécuter le superviseur YACS : ./runSession driver schema.xml
 3. stopper SALOME : ./runSession shutdownSalome.py pour arrêter proprement ou ./runSession killSalome.py pour
    un arrêt brutal

Il est possible de configurer une session d'exécution puis d'exécuter YACS et de stopper SALOME
au sein de cette session en faisant :

 2. initialiser la session : ./runSession
 3. exécuter le superviseur YACS : driver schema.xml
 4. stopper SALOME : shutdownSalome.py ou killSalome.py
 5. sortir de la session : CTRL+D

Le superviseur YACS en mode console (driver) accepte quelques options pour paramétrer
son exécution::

   Usage: driver [OPTION...] graph.xml
   driver -- a SALOME YACS graph executor

     -d, --display=level        Display dot files: 0=never to 3=very often
     -e, --dump-on-error[=file] Stop on first error and dump state
     -f, --dump-final[=file]    dump final state
     -l, --load-state=file      Load State from a previous partial execution
     -s, --stop-on-error        Stop on first error
     -v, --verbose              Produce verbose output
     -x, --save-xml-schema[=file]   dump xml schema
     -?, --help                 Give this help list
         --usage                Give a short usage message
     -V, --version              Print program version

Voici quelques utilisations typiques de driver.

Exécution standard
--------------------
Lors d'une exécution standard, le superviseur lit le fichier XML qui décrit le schéma,
l'exécute et produit un compte-rendu d'erreurs (voir :ref:`errorreport`) s'il y a lieu::

  driver schema.xml
  
Exécution avec affichage des états des noeuds pendant l'exécution
----------------------------------------------------------------------
::

  driver --display=1 schema.xml

Exécution avec sauvegarde de l'état final du schéma
---------------------------------------------------------
::

  driver --dump-final=mystate.xml schema.xml

Exécution avec chargement de l'état initial du schéma
---------------------------------------------------------
::

  driver --load-state=mystate.xml schema.xml

