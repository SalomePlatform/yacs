
Composant Salomé
================


Principe
--------

Un composant Salomé est un composant CORBA tel que décrit au chapitre
précédent, utilisant des services du noyau Salomé. Il faut également  "déclarer"
ce composant auprès de Salomé (i.e. fournir ou compléter  des fichiers de
ressources Salomé avec des informations relatives à ce  composant).    Les
différentes opérations à effectuer sont :

1. adapter le fichier IDL,

2. déclarer le composant auprès du catalogue de modules de Salomé,

3. modifier la classe d'implémentation (C++ ou python) pour

   - suivre l'adaptation du fichier IDL (point 1),
   - permettre la supervision du composant par Salomé, dans les différents services du composant.
   - préparer l'utilisation du service de notification (envoi de messages pour suivre 
     le déroulement des calculs dans le composant) - optionel (mais utile),

4. déclarer les éventuelles ressources graphiques.


Fichier IDL
-----------


Principe
^^^^^^^^

La description IDL est semblable à celle d'un composant CORBA standard avec  les
particularités :

* le composant doit faire partie du module (CORBA) de base ``Engines``,

* le composant doit hériter du composant de base de Salomé :
  ``Engines::Component``  (défini dans le fichier IDL ``"SALOME_Component.idl"``),

* les services du composant peuvent avoir des paramètres ``in``,    ``out``
  et/ou une valeur de retour, mais pas de paramètres ``inout``.


**Remarques**

  #. Le module ``Engines`` regroupe tous les composants de Salomé, autres  que les
     composants centraux de Salomé.

  #. Le composant de base ``Engines::Component`` permet de ne pas devoir
     redéfinir pour chaque composant ajouté au système, les services communs  (arrêt,
     reprise, etc.).

  #. Ceci n'est pas une limitation, un paramètre ``inout`` peut se  découpler en
     un paramètre ``in`` et un paramètre ``out`` (de toute  façon, c'est sous cette
     forme que le verront les clients python).


Exemple 9 (début)
^^^^^^^^^^^^^^^^^

Reprenons le fichier IDL ``alglin.idl`` et adaptons-le :


``alglin.idl``

.. include:: ./exemples/exemple9/alglin.idl
   :literal:

**Remarque**
  Il faut faire attention quand on définit des structures dans l'IDL (comme la
  structure ``vecteur`` dans ``alglin.idl``). Le nom choisi ``vecteur`` risque
  d'être  déjà "réservé" par un autre composant Salomé.


Exemple 10 (début)
^^^^^^^^^^^^^^^^^^

Des modifications analogues sont effectuées sur le fichier IDL   ``FreeFem.idl`` :


``FreeFem.idl``

.. include:: ./exemples/exemple10/FreeFemComponent.idl
   :literal:


Inscription au catalogue de modules
-----------------------------------

Pour être utilisables dans Salomé, les composants doivent s'inscrire auprès  de
l'un des catalogues de modules de Salomé.    Ce catalogue est réparti entre
plusieurs fichiers (catalogue général,   catalogue utilisateur). On supposera
que le composant sera déclaré dans  le catalogue personnel d'un utilisateur.
Le fichier de catalogue de composants est un fichier XML qui contient

#. la liste des services du composant et leurs paramètres,

#. la (les) machine(s) où le composant peut être chargé en mémoire,  le nom du
   fichier contenant l'icone du composant, des informations de version,
   commentaires, service par défaut, etc (ces informations sont optionelles).

On peut compléter à la main ce catalogue ou utiliser un utilitaire  fourni par
Salomé qui génère les informations du point 1 ci-dessus  (il faudra malgré tout
éditer le fichier pour entrer les informations  du point 2).    Cet outil est
disponible dans l'interface graphique Salomé (menu ``Tools->Catalog Generator``)
en indiquant le nom du catalogue (fichier XML) et le nom  du fichier IDL du
composant.


Classe d'implémentation C++
---------------------------

**On supposera dans ce paragraphe, que le fichier IDL définit une classe CORBA :
A  et qu'on utilisera la classe d'implémentation C++ : A_impl.**


Mise en conformité avec l'IDL du composant
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Pour adapter les classes d'implémentation CORBA/C++ standards (telles  que vues
au chapitre précédent), il faut :

#. insérer le fichier ``SALOMEconfig.h`` :   ::

             #include <SALOMEconfig.h>

   Le fichier ``SALOMEconfig.h`` contient un certain nombre de définitions  utiles
   pour assurer l'indépendance du code de la classe d'implémentation  par rapport à
   la version de CORBA utilisée.

#. insérer ensuite le fichier ``SALOME_Component_i.hxx``     ::

             #include "SALOME_Component_i.hxx"

   qui contient l'interface de la classe d'implémentation C++ du composant  de base
   de Salomé

#. pour la classe CORBA qui est implémentée, ajouter la ligne :     ::

             #include CORBA_SERVER_HEADER(A)

   ``CORBA_SERVER_HEADER`` est une macro définie dans ``SALOMEconfig.h``  et qui
   assure l'indépendance des noms de fichiers d'inclusion CORBA par  rapport à
   l'implémentation de CORBA utilisée.

#. pour chaque classe CORBA qui est utilisée dans la classe  d'implémentation,
   ajouter la ligne :     ::

            #include CORBA_CLIENT_HEADER(<nom de la classe CORBA>)

   ``CORBA_CLIENT_HEADER`` est une macro définie dans ``SALOMEconfig.h``  et qui
   assure l'indépendance des noms de fichiers d'inclusion CORBA par  rapport à
   l'implémentation de CORBA utilisée.

#. faire dériver la classe d'implémentation de celle du composant  Salome de
   base :     ::

      class A_impl :
           public POA_Engines::A,
           public Engines_Component_i {

#. définir le (seul) constructeur de la façon suivante dans le fichier  d'entête
   C++ (.hxx) :     ::

           A_impl(CORBA::ORB_ptr orb,
                  PortableServer::POA_ptr poa,
                  PortableServer::ObjectId * contId, 
                  const char *instanceName,
                  const char *interfaceName);

   et dans le fichier d'implémentation C++ (.cxx) :     ::

           A_impl:: A_impl
                     (CORBA::ORB_ptr orb,
                      PortableServer::POA_ptr poa,
                      PortableServer::ObjectId * contId, 
                      const char *instanceName, 
                      const char *interfaceName) :
                Engines_Component_i(orb, poa, contId, 
                                    instanceName, interfaceName)
           {
             _thisObj = this ;
             _id = _poa->activate_object(_thisObj);
           }

   Ce constructeur sera éventuellement responsable de la création et  de
   l'initialisation de l'objet interne associé au composant CORBA.

#. si des structures sont définies par le fichier IDL dans le module  Engines,
   adapter les déclarations des méthodes de la classe d'implémentation.

L'exemple ci-dessus illustre les modifications effectuées sur l'exemple 6.

.. _remsuper:


Permettre la supervision du composant
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Pour pouvoir piloter le composant depuis la supervision, il faut  insérer dans
chaque service du composant (i.e. dans chaque méthode  de la classe
d'implémentation qui est appelée lors d'une requète  CORBA),

* au début, l'instruction :   ::

     beginService(<nom du service>);

* à la fin, l'instruction :   ::

     endService(<nom du service>);

Ces deux instructions signalent à la supervision de Salomé  que le service du
composant a bien reçu la requète CORBA (beginService)  et que l'exécution du
service s'est bien terminée (endService).    

**Remarque**
  Mettre beginService et endService
  dans les méthodes de la classe d'implémentation d'un composant  déclare à Salomé
  que le composant est "supervisable". Ce n'est pas  une assurance que l'on pourra
  utiliser ce composant sans précautions  dans le cas où le composant possède un
  état interne modifié par  un ou plusieurs services.    
  
  **Exemple**
    On considère un composant ayant une variable interne ``Quantite``  et deux services :

    * ``S1(x) : Quantite = Quantite + x;`` retourne ``Quantite`` et

    * ``S2(x) : Quantite = Quantite * x;`` retourne ``Quantite``.

    Il n'est pas possible *a priori* de connaître la valeur de  ``Quantite`` après
    exécution du graphe de calcul sur la figure suivante.

.. _figpara:


.. image:: images/parallele.png
     :width: 26ex
     :align: center

.. centered::
     Graphe de calcul contenant des branches parallèles

Utilisation de la notification
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Pour signaler l'utilisation de la notification (qui provoquera l'ouverture
d'une connexion à un canal d'événements), il faut modifier  le constructeur de
la classe d'implémentation comme suit :     ::

        A_impl:: A_impl
                  (CORBA::ORB_ptr orb,
                   PortableServer::POA_ptr poa,
                   PortableServer::ObjectId * contId, 
                   const char *instanceName, 
                   const char *interfaceName) :
             Engines_Component_i(orb, poa, contId, 
                                 instanceName, interfaceName, 1)
        {
          _thisObj = this ;
          _id = _poa->activate_object(_thisObj);
        }

où on a ajouté le paramètre "1" à la fin de l'appel du constructeur  de
``Engines_Component_i``.    Le composant pourra ensuite utiliser l'instruction
::

   void sendMessage(const char *event_type, const char *message);

pour envoyer des messages indiquant le déroulement du calcul  ou une situation
anormale, etc. Ces messages seront visibles de  l'utilisateur de Salomé.    Le
premier paramètre indique le type de message ("warning", "step",  "trace",
"verbose"), le second paramètre, le contenu (chaîne de   caractères) du message.


Connexion à l'objet "fabrique" d'un conteneur
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Afin de permettre le chargement et l'initialisation d'un composant  par un
conteneur, il faut fournir une fonction C dont le nom  et le code sont imposés :
::

   extern "C"
   {
     PortableServer::ObjectId * <nom du composant>Engine_factory(
                                  CORBA::ORB_ptr orb,
                                  PortableServer::POA_ptr poa,
                                  PortableServer::ObjectId * contId,
                                  const char *instanceName,
                                  const char *interfaceName)
     {
       <classe d'implementation> * O
         = new <classe d'implementation>(orb, poa, contId, 
                                         instanceName, interfaceName);
       return O->getId() ;
     }
   }

``<nom du composant>`` (ou nom de la classe CORBA, ``A`` ici)  et ``<classe
d'implementation>`` (``A_impl`` ici) sont  spécifiques à chaque composant.
Cette fonction est appelée par le conteneur lors du chargement  d'un composant.
Elle crée un objet CORBA qui réceptionnera les requètes  au composant et les
transmettra au différents services du composant.


Exemple 9 (suite)
^^^^^^^^^^^^^^^^^

Reprenons les fichiers d'implémentation ``alglin_i.hxx`` et   ``alglin_i.cxx``
et adaptons-les :


``alglin_i.hxx``

.. include:: ./exemples/exemple9/alglin_i.hxx
   :literal:


``alglin_i.cxx``

.. include:: ./exemples/exemple9/alglin_i.cxx
   :literal:


Classe d'implémentation python
------------------------------

**On supposera dans ce paragraphe, que le fichier IDL définit une classe CORBA :
B  et qu'on utilisera la classe d'implémentation python : B.**    La démarche
est analogue au cas de l'interface serveur C++ :


Mise en conformité avec l'IDL du composant
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#. importer la classe du composant de base :     ::

      from SALOME_ComponentPy import *

#. faire dériver la classe d'implémentation de celle du composant  Salome de
   base :     ::

      class B(Engines__POA.B,
              SALOME_ComponentPy_i):

#. le constructeur de la classe d'implémentation doit débuter par :     ::

          def __init__(self, orb, poa, this, containerName,
                       instanceName, interfaceName):
              SALOME_ComponentPy_i.__init__(self, orb, poa, this, 
                                            containerName, instanceName, 
                                            interfaceName, 0)


Permettre la supervision du composant
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Pour pouvoir piloter le composant depuis la supervision, il faut  insérer dans
chaque service du composant (i.e. dans chaque méthode  de la classe
d'implémentation qui est appelée lors d'une requète  CORBA),

* au début, l'instruction :   ::

     beginService(<nom du service>);

* à la fin, l'instruction :   ::

     endService(<nom du service>);

Ces deux instructions signalent à la supervision de Salomé  que le service du
composant a bien reçu la requète CORBA (beginService)  et que l'exécution du
service s'est bien terminée (endService).    Même remarque que dans le cas C++
(:ref:`remsuper`)


Utilisation de la notification
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Pour signaler l'utilisation de la notification, il faut modifier  le
constructeur de la classe d'implémentation comme suit :     ::

       def __init__(self, orb, poa, this, containerName,
                    instanceName, interfaceName):
           SALOME_ComponentPy_i.__init__(self, orb, poa, this, 
                                         containerName, instanceName, 
                                         interfaceName, 1)

où on a ajouté le paramètre "1" à la fin de l'appel du constructeur  de
``SALOME_ComponentPy_i``.    Le composant pourra ensuite utiliser l'instruction
::

   sendMessage(event_type, message);

pour envoyer des messages indiquant le déroulement du calcul  ou une situation
anormale, etc. Ces messages seront visibles de  l'utilisateur de Salomé.    Le
premier paramètre indique le type de message ("warning", "step",  "trace",
"verbose"), le second paramètre, le contenu (chaîne de   caractères) du message.


Exemple 10 (suite)
^^^^^^^^^^^^^^^^^^

Reprenons le fichier d'implémentation ``FreeFem.py`` et adaptons le :


``FreeFemComponent.py``

.. include:: ./exemples/exemple10/FreeFemComponent.py
   :literal:


Utilisation de l'environnement de compilation et d'exécution de  Salomé
-----------------------------------------------------------------------

On utilisera les autotools comme Salomé.

Utilisation des structures de données fournies par Salomé ou renvoyées par le composant
---------------------------------------------------------------------------------------

Pour cette partie, on se référera aux spécifications CORBA [CORBA]_,  et en
particulier les spécifications IDL - langages [LANG]_ ou aux   différents
manuels CORBA, par exemple [HENVIN]_ (C++) et   [PyCorba]_ (python).


Maillages et champs MED
^^^^^^^^^^^^^^^^^^^^^^^

Pour cette partie, on se réfèrera à la documentation de MED et MEDMemory
[MEDDoc]_ et [MEDMemory]_.

