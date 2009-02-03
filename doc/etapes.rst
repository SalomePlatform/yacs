
.. _secetapes:

Etapes dans l'intégration d'un composant
========================================


Notion de composant
-------------------

Il existe de nombreuses définitions de composant logiciel. Dans le cadre de
Salomé, nous retiendrons la suivante :      

**Composant**

  Un composant est une entité logicielle possédant les caractéristiques et  les comportements suivants :

  * **physiquement identifiable**,  c'est à dire qu'un composant n'est pas un
    concept ou un "pattern" de  conception mais un "vrai" morceau de code binaire,
    directement   exécutable - ou interprétable - après déploiement ;

  * **composable**, un composant doit pouvoir être mis en relation avec  d'autres
    composants : il expose une - ou plusieurs - interfaces qui  agissent comme des
    canaux de communication entre composants ;

  * un composant **interagit avec les autres composants** uniquement au travers
    de sa (ou ses) interface(s);

  * un composant est **intégré dans une architecture**  applicative par un
    processus de déploiement. Ce processus de déploiement  est dépendant de la
    plate-forme matérielle et logicielle sous-tendant  l'architecture ;


Schéma général
--------------

.. index:: single: architecture

L'intégration d'un code de calcul sous forme de composant dans l'architecture
PAL se fait en plusieurs étapes, schématisées par la figure suivante :

.. image:: images/phases.png
   :width: 56ex
   :align: center

.. centered:: Phases d'intégration d'un composant


Pour utiliser le composant :

* localement, via un interpréteur python, il faudra écrire  une interface python
  via des outils tels que **SWIG** [SWIG]_  ou **Boost** [BOOST]_;

* à distance, à travers CORBA [CORBA]_, il faudra écrire une interface CORBA
  (fichier IDL, classe d'implémentation), Salomé offrant des services permettant
  de simplifier l'utilisation de CORBA;

* via l'application SALOME, il faudra spécialiser les fichiers écrits au  point
  précédent et spécifier les fichiers de ressources éventuels (graphiques et
  autres).


Code de départ
--------------

Les cas de figures peuvent être très différents les uns des autres  suivant la
forme du code à intégrer dans Salomé.    Dans ce document, on envisagera les
situations dans lesquelles  on dispose au départ :

* d'un code sous forme d'un ou plusieurs exécutables séparés.

* d'un ensemble de fonctions f77/routines C/classes C++.

* d'un ensemble de scripts python, qui contrôlent éventuellement  des routines,
  fonctions ou classes f77/C/C++.


.. _compinterne:

Mise sous forme d'objet interne
-------------------------------

.. index:: single: objet interne

Pour simplifier les phases suivantes, un premier travail consiste à  présenter
le code de départ sous la forme d'un objet C++ ou python.      


**objet interne**

  Cet objet C++ ou python sera appelé "**objet interne**" dans ce document.    Il sera composé de
  méthodes et d'attributs, publics et privés   (suivant la terminologie standard
  orientée-objet) :

  .. index:: single: méthodes; publiques

  * les **méthodes publiques** sont les services de calcul que l'on veut rendre
    disponibles aux utilisateurs de l'objet interne,

  .. index:: single: attributs; publics

  * les **attributs publics** sont les données de l'objet que l'on veut   rendre
    disponibles aux utilisateurs de cet objet interne,

  .. index::
     single: méthodes; privées
     single: attributs; privés

  * les **méthodes et attributs privés** sont les services et les données de
    l'objet interne que l'on ne veut pas rendre disponibles aux utilisateurs de
    l'objet interne (et qui ne seront visibles que des autres méthodes de ce même
    objet).

.. index::
      single: etat interne
      single: common fortran77
      single: variables globales

**Remarques**

  1. La distinction "public/privé" n'a pas réellement de sens en python où  tout
     est public par défaut.

  2. L'objet interne est vu de l'utilisateur comme un ensemble  de méthodes
     publiques (par ex. services de calcul) qui délèguent  les requêtes au code
     encapsulé (voir   figures :ref:`Objet interne englobant le code initial <figinterne1>`,
     :ref:`Objet interne communicant avec un code extérieur <figinterne2>` et
     :ref:`Code initial déjà sous forme d'objet interne <figinterne3>`.

  3. Entre 2 appels successifs de services de l'objet, les attributs gardent  la
     "mémoire de l'objet" (l'état interne,  par exemple : common fortran77 ou
     variables globales [#f1]_  C/C++).

.. index::
   single: service; de l'objet interne
   single: etat interne

La première partie du travail, une étape de conception, consiste à   définir les
services  et l'état interne de cet objet. En particulier :

* choisir les différents services,

* pour chaque service, définir les données d'entrée et de sortie

* pour chaque entrée et chaque sortie, définir le type de la donnée  et
  éventuellement les pré-conditions et post-conditions associées,

* définir l'état interne de l'objet et éventuellement sa valeur  avant et après
  l'appel aux différents services.

Suivant les cas, l'objet interne pourra :

* "englober" le code initial, c'est le cas d'un code initial sous forme d'une
  librairie ou dont  le code source est disponible;

    .. _figinterne1:


    .. image:: images/objint1.png
         :width: 40ex
         :align: center

    .. centered:: Objet interne englobant le code initial

* communiquer avec le code source via le système d'exploitation (Unix, Windows,
  etc), c'est le cas  d'un code initial sous la forme d'un exécutable binaire;

    .. _figinterne2:


    .. image:: images/objint2.png
         :width: 58ex
         :align: center

    .. centered:: Objet interne communicant avec un code extérieur

  **Remarque**
    La communication avec le code extérieur binaire pourra se faire par fichiers, ou
    par la ligne de commandes, par exemple.

* dans le cas où le code initial est déjà sous la forme d'une classe python, il
  n'y a rien à faire.

    .. _figinterne3:


    .. image:: images/objint3.png
         :width: 36ex
         :align: center

    .. centered:: Code initial déjà sous forme d'objet interne



Accès depuis un interpréteur python local
-----------------------------------------

L'objet C++/python construit au paragraphe (:ref:`compinterne`) 
peut être enrobé dans un composant manipulé   depuis un
interpréteur python local, suivant la figure suivante.

.. _python_local:


.. image:: images/accesLocal.png
   :width: 46ex
   :align: center

.. centered:: Accès depuis un interpréteur python local

Suivant le type et l'implémentation de l'objet interne (C++ ou python),   il
faudra ou non fournir une interface. Des outils tels que **swig**   
ou **boost** permettent de simplifier cette tâche [#f2]_.


Accès depuis CORBA
------------------

Indépendamment de l'accès depuis un interpréteur python local, il faut fournir
une interface  permettant un accès distant au composant via CORBA.    Le
fonctionnement de CORBA retenu est celui de client-serveur comme illustré par la
figure  :ref:`Accès depuis CORBA <figaccescorba>` où on représente la
communication entre un client et un serveur via CORBA.      

**Serveur CORBA**
  Dans ce document et
  dans l'environnement Salomé, un serveur est un exécutable.    Il est branché sur
  un canal de communication appelé bus CORBA qui lui transmet  des requêtes et
  vers lequel le serveur renvoie les résultats de ces requêtes.    Un serveur peut
  héberger plusieurs objets CORBA auxquels il délègue l'exécution des requêtes.
  La relation client-serveur CORBA est spécifiée par un fichier d'interface, dit
  **fichier IDL**. Celui-ci   représente un contrat entre le composant et les
  clients qui utilisent le composant.    

Les composants dans ce mode de
fonctionnement seront des objets CORBA.  La relation serveur - composant sera
précisée au paragraphe :ref:`contfab`.    

**Remarque**
  Le respect du fichier IDL par le
  client et le serveur conditionne la bonne exécution des requêtes.  Pour cette
  raison, dans Salomé, les différents fichiers IDL sont rassemblés dans des
  répertoires vus par tous les clients et serveurs CORBA.

.. _figaccescorba:


.. image:: images/accesCorba.png
   :width: 58ex
   :align: center

.. centered:: Accès depuis CORBA

L'intégrateur de code doit fournir une partie de l'interface serveur du
composant, sous forme d'une classe   dite d'**implémentation** (démarche
similaire en python et C++, côté serveur). Le reste de l'interface  est générée
à partir du fichier d'interface IDL.    Du côté des clients, par contre,
l'interface CORBA est presque entièrement générée (particulièrement  si le
client est écrit en python).


Particularités de l'environnement Salomé
----------------------------------------


Services de Salomé
^^^^^^^^^^^^^^^^^^

Salomé fournit un certain nombre de services au-dessus de CORBA permettant de
simplifier  son utilisation.    On citera:


L'accès au service de nommage:
""""""""""""""""""""""""""""""

Une IOR (référence CORBA) est un pointeur distant, dont le maniement  n'est pas
toujours aisé. Un exemple d'IOR est le suivant :     ::

   IOR:010000003400000049444c3a6174742e636f6d2f4174744e6f74696669
   636174696f6e2f4576656e744368616e6e656c466163746f72793a312e3000
   010000000000000026000000010100000a0000003132372e302e302e310006
   800e000000fedd112a3d000007ef0000000001

dont la signification est :   ::

   Type ID: "IDL:att.com/AttNotification/EventChannelFactory:1.0"
   Profiles: 1. IIOP 1.0 127.0.0.1 32774 POA(root) 0x00000001  (4 bytes)

Le service de nommage CORBA permet à l'utilisateur d'associer  un nom à cette
IOR.    Salomé fournit une classe (C++ ou python)  permettant d'utiliser de
façon simple cette association.


Des structures de données standardisées :
"""""""""""""""""""""""""""""""""""""""""

En plus des structures de données CORBA (scalaires, chaînes de caractères,
vecteurs,  structures génériques), Salomé propose des structures type pour
représenter  les maillages, support (parties de maillages) et champs de valeurs
sur ces  supports. Ces structures sont regroupées sous le nom de *modèle
d'échange de données (MED)* [MED]_.


L'utilisation du service de notification CORBA :
""""""""""""""""""""""""""""""""""""""""""""""""

CORBA propose un service d'envoi de notification (avancement du calcul  par
exemple).    Salomé propose des fonctionnalités pour simplifier l'envoi de
messages  par les composants et un canal d'événements où les clients peuvent
suivre l'évolution des calculs par les composants.


Un service d'enregistrement (registry) :
""""""""""""""""""""""""""""""""""""""""

Ce service permet à l'utilisateur de connaître l'ensemble des composants
auxquels il peut envoyer des requêtes.


Un catalogue de modules:
""""""""""""""""""""""""

Ce service permet à l'utilisateur et à Salomé de connaître les différents
composants disponibles avec les différents services disponibles ainsi  que
l'emplacement de ces composants sur les disques des différentes machines.


Un service de gestion d'études :
""""""""""""""""""""""""""""""""

Ce service permet de regrouper (au choix de l'utilisateur)   différentes
informations utilisées ou produites  par les composants  lors d'une session
d'utilisation de Salomé.


Des composants de base :
""""""""""""""""""""""""

Salomé est une plateforme générique de liaison CAO-Calcul qui propose des
composants  de CAO, de maillage et de visualisation et de supervision des
calculs.


.. _contfab:

Conteneurs et fabriques
^^^^^^^^^^^^^^^^^^^^^^^

.. index::
   single: container
   single: factory
   single: conteneur
   single: fabrique

Salomé est basé sur la notion de conteneur (ou "container") et de fabrique (ou
"factory").    

**Conteneur-fabrique**
  Un **conteneur** est un serveur CORBA chargé d'héberger des
  composants CORBA et est  responsable du cycle de vie (chargement/déchargement,
  initialisation) de ces composants.    Chaque conteneur contient un objet CORBA
  particulier appelé **fabrique**, auquel on peut envoyer des requêtes pour
  charger ou décharger des composants.

Exemple
"""""""

Supposons qu'un client désire utiliser des  services d'un composant nommé A dans
un conteneur nommé B sur une machine nommée M.     Les différentes flèches sur
la figure :ref:`Conteneur, fabrique et composants<figconteneur>` illustrent le fonctionnement :

#. Le client s'adresse au noyau Salomé (sur sa machine) et lui demande  de lui
   fournir une référence sur le composant A dans un conteneur B sur une machine M.
   Le noyau cherche si un tel composant est déjà référencé.

#. Si le composant n'est pas référencé, le noyau Salomé cherche le   conteneur B
   sur la machine M.   Si le conteneur n'existe pas, Salomé le crée.  Le noyau
   récupère une référence sur l'objet "fabrique" dans le conteneur B.

#. L'objet "fabrique" du conteneur B charge dynamiquement la librairie
   contenant le code du composant A à partir du disque.

#. Le composant est créé et enregistré dans le noyau Salomé, qui renvoie   au
   client la référence sur le composant.

#. Le client peut émettre des requêtes au composant.

**Remarque**
  Quand une référence sur un composant a été obtenue par le processus précédent
  (points 1 à 4), le client peut la conserver et émettre des requêtes au composant
  pendant toute la suite du calcul sans repasser par les mêmes étapes.  Dans la
  version actuelle de Salomé,  on n'a pas encore mis en place une procédure de
  reprise automatique si le composant  (ou le conteneur qui le contient) tombe.

.. _figconteneur:


.. image:: images/conteneur.png
   :width: 50ex
   :align: center

.. centered:: Conteneur, fabrique et composants

.. rubric:: Footnotes

.. [#f1] On donnera à l'annexe 1, quelques indications  sur le traitement des common fortran et variables globales C++.

.. [#f2] Au moment où est écrit ce document, l'outil  retenu par Salomé est swig. Tant que ce choix n'est pas remis en cause, on ne conseille  pas l'utilisation de boost pour l'intégration de composants dans Salomé (l'interopérabilité  des deux outils n'ayant pas été testée).
