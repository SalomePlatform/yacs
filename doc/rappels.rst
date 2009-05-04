
:tocdepth: 3

.. _rappels:

================================================================
Reminders about the SALOME platform
================================================================

Definitions
================================================================

   - **SALOME module**:  a SALOME module is a group of SALOME components. It is located in a directory with a standard structure.
     Its components are described in an XML file named <module>Catalog.xml.
   - **SALOME component**:  this is the SALOME equivalent of a calculation code.  It may be in the form of a dynamic 
     library or a Python module.
   - **SALOME service**:  each component has one or several services.  A service is the equivalent of a Fortran 
     subroutine, a C function, a C++ method or a Python method.  Only component services can be coupled with the YACS coupler.  
     Coupling is done connecting input-output ports with which services are provided.  There are two types of these 
     ports, dataflow and datastream.
   - **Dataflow port**:  a dataflow port is a connection port for exchanging data.  It has a name that is the name of the 
     exchanged variable, a type that indicates the data type (double, long, string, etc.) and a direction (input or output).  
     An input dataflow port can be connected with an output dataflow port of a compatible type.  The variable carried by 
     the port is exchanged at the end of the service (output port) or at the beginning of the service (input port).  
     No data exchange takes place during execution of the service.
   - **Datastream port**:  a datastream port is used to exchange data during execution.  Like a dataflow port, it has a name, 
     a type and a direction but it also has an iterative (I) or time (T) dependence mode.  An input datastream port can only 
     be connected with an output datastream port of the same type.  There may be several classes of datastream port.  
     In this description, we only deal with CALCIUM datastream ports.
   - **YACS coupler**:  this is a SALOME module that edits and executes coupling schemes that are mainly in the form of an 
     XML file.  The module provides a graphic user interface to edit a scheme, to execute it and monitor its execution.
   - **Component instance**:  there can be several copies of the same component and they can be executed by SALOME in a 
     single session.  A distinction between these copies is made by referring to a component instance.  A parallel can be 
     made with the terms calculation code (component) and execution (component instance).
   - **Container**:  SALOME process that executes components.  It loads (dynamic library or Python module) and saves 
     components.  The choice of containers may be important if it is required that one component should not be in the same 
     executable as another.
   - **SALOME application**:  set of SALOME modules integrated into the SALOME platform.  This platform is built on KERNEL 
     and GUI basic modules that provide component running and integration services in the graphic MMI.  A SALOME application 
     provides several scripts to run the application (runAppli), enter the application environment (runSession) and other 
     more internal scripts such as remote run (runRemote).

Modules et composants SALOME
==================================================
Dans son principe, la plate-forme SALOME est décomposée en une plate-forme
de base nommée module KERNEL et une collection de modules divers.

Un module est un produit compilable et installable qui se concrétise par une arborescence source qui doit
respecter les règles générales SALOME et qui comprend une procédure de construction, installation qui
respecte également les règles SALOME.
Chaque module est géré en configuration dans un module CVS
indépendant. Chaque module peut livrer des versions à son rythme dans le respect des règles de
cohérence de SALOME. A chaque module est associée une base de tests de non régression.

Un module a des dépendances avec les autres modules (utilise, est utilisé).
Le module KERNEL constitue la base de la plate-forme SALOME. 
Tous les autres modules dépendent du module KERNEL.

===================================== ========= ============= ==================================
Thèmes                                 Modules    Utilise          Est utilisé par
===================================== ========= ============= ==================================
Architecture                           KERNEL                  MED, GEOM, SMESH, VISU,YACS  
Architecture                           MED       KERNEL         SMESH, VISU
Géométrie                              GEOM      KERNEL         SMESH
Maillage                               SMESH     KERNEL, MED
Supervision                            YACS      KERNEL
Visualisation                          VISU      KERNEL, MED
===================================== ========= ============= ==================================

Un module contient un ou plusieurs composants SALOME. Un composant SALOME est un objet CORBA qui
respecte les règles SALOME et qui est déclaré à SALOME au moyen d'un catalogue. Un composant SALOME
peut être doté d'une interface utilisateur graphique (GUI) qui doit elle-même respecter les règles
SALOME.

Les containers
======================
Dans SALOME, les composants sont dynamiquement chargeables. Cette propriété est obtenu
en utilisant un mécanisme de container. 

Dans ses grandes lignes, un container est un serveur CORBA dont l'interface dispose 
des méthodes nécessaires pour effectuer le chargement déchargement de l'implémentation
d'un composant SALOME. Pour effectuer le chargement d'un composant, on appellera la méthode
load_impl du container. 

La mécanique de base du chargement d'un composant est dépendante du langage d'implémentation choisi.

En C++, la plate-forme utilise le chargement dynamique de bibliothèque (dlopen) et un mécanisme de fonction 
factory dont le nom doit être <Module>Engine_factory (par exemple GEOMEngine_factory, pour GEOM).
Cette fonction doit retourner l'objet CORBA effectif qui est le composant SALOME.

En Python, la plate-forme utilise le mécanisme d'import de Python (import <Module>) et instancie 
le composant SALOME Python en utilisant une classe (ou une factory) de même nom (<Module>) pour
ce faire.

.. _appli:

Construction et utilisation d'une application SALOME
=========================================================
Ce document explique comment configurer, installer et utiliser votre propre 
application SALOME à partir d'une liste de modules préinstallés.

Principes
------------

Une application SALOME est construite à partir d'une liste de modules (GEOM, SMESH, ASTER...) de
base de la plate-forme ou utilisateurs.
Elle consiste en un jeu de scripts shell et de répertoires qui permettent d'exécuter l'application
dans différents contextes.

Un utilisateur peut définir plusieurs applications SALOME. Ces applications
sont utilisables à partir d'un même compte utilisateur. Elles peuvent utiliser
les mêmes modules (KERNEL ou autre). L'application SALOME est indépendante de KERNEL
et **ne doit pas être installée dans KERNEL_ROOT_DIR**.

Les prérequis utilisés par chaque application peuvent être différents.

Une session SALOME lancée à partir d'une application SALOME peut s'exécuter
sur plusieurs calculateurs.

Pour une installation d'application multi-machines, les modules et les prérequis
doivent être installés sur toutes les machines impliquées. Il n'est pas nécessaire
d'installer tous les modules sur toutes les machines à part KERNEL.
L'utilisateur SALOME doit avoir un compte sur chaque machine. L'accès aux machines
distantes est réalisé par rsh ou ssh. Ces accès doivent être configurés pour 
un usage sans password. Les comptes peuvent être différents sur les différentes
machines.

.. raw:: latex

  \makeatletter
  \g@addto@macro\@verbatim\small
  \makeatother


Créer une application SALOME
------------------------------
On crée une application SALOME avec l'outil appli_gen.py que l'on trouve dans l'installation du module KERNEL. 
Cet outil construit l'application en partant d'un fichier de configuration au format XML qui décrit la liste
des modules à utiliser (nom, chemin d'installation), le fichier qui positionne l'environnement 
pour les prérequis de SALOME et optionnellement le répertoire des exemples SALOME (SAMPLES_SRC).

La commande à utiliser est la suivante::

   python <KERNEL_ROOT_DIR>/bin/salome/appli_gen.py --prefix=<install directory> --config=<configuration file>

où <configuration file> est le nom du fichier de configuration et <install directory> est le nom du répertoire
dans lequel on veut créer l'application. <KERNEL_ROOT_DIR> indique le répertoire d'installation du module KERNEL.

On peut créer le fichier de configuration en modifiant une copie du fichier ${KERNEL_ROOT_DIR}/bin/salome/config_appli.xml.


En voici un exemple::

  <application>
  <prerequisites path="/data/tmplgls/secher/SALOME_V4.1.1_MD08/env_products.sh"/>
  <modules>
     <!-- variable name <MODULE>_ROOT_DIR is built with <MODULE> == name attribute value -->
     <!-- <MODULE>_ROOT_DIR values is set with path attribute value -->
     <!-- attribute gui (defaults = yes) indicates if the module has a gui interface -->
     <module name="KERNEL"       gui="no"  path="/data/SALOME_V4.1.1/KERNEL_INSTALL"/>
     <module name="GUI"          gui="no"  path="/data/SALOME_V4.1.1/GUI_4.1.1"/>
     <module name="MED"                    path="/data/SALOME_V4.1.1/MED_4.1.1"/>
     <module name="GEOM"                   path="/data/SALOME_V4.1.1/GEOM_4.1.1"/>
     <module name="SMESH"                  path="/data/SALOME_V4.1.1/SMESH_4.1.1"/>
     <module name="YACS"                   path="/data/SALOME_V4.1.1/YACS_4.1.1"/>
     <module name="VISU"                   path="/data/SALOME_V4.1.1/VISU_4.1.1"/>
     <module name="HELLO"                  path="/data/SALOME_V4.1.1/HELLO1_4.1.1"/>
     <module name="PYHELLO"                path="/data/SALOME_V4.1.1PYHELLO1_4.1.1"/>
     <module name="NETGENPLUGIN"           path="/data/SALOME_V4.1.1/NETGENPLUGIN_4.1.1"/>
  </modules>
  <samples path="/data/SALOME_V4.1.1/SAMPLES/4.1.1/SAMPLES_SRC"/>
  </application>

Quelques règles à suivre
------------------------------

Le répertoire d'application doit être créé sur tous les calculateurs qui devront exécuter des composants de cette application.
La méthode la plus simple est de créer le répertoire d'application en utilisant le même chemin relatif par rapport au
répertoire HOME sur chaque machine. Si ce n'est pas souhaité, il est possible d'utiliser des chemins différents suivant
les calculateurs mais il faudra le préciser dans le fichier de configuration CatalogRessources.xml.

Le répertoire d'application contient des scripts pour initialiser les variables d'environnement et faire des exécutions.

L'environnement est initialisé par des scripts placés dans le sous répertoire env.d. Les scripts pour SALOME sont créés
au moment de la création de l'application mais l'utilisateur peut ajouter ses propres scripts. Il suffit qu'ils aient
comme suffixe .sh. Ces scripts doivent être installés sur toutes les machines de l'application.

L'application SALOME fournit à l'utilisateur 4 scripts d'exécution :

  - **runAppli** lance une session SALOME (à la manière de ${KERNEL_ROOT_DIR}/bin/salome/runSalome). 
  - **runSession** permet de se connecter, dans un shell avec un environnement conforme, à une session SALOME lancée
    précédemment. Sans argument, le script ouvre un shell interactif. Avec arguments, il exécute la commande
    fournie dans l'environnement de l'application SALOME.
  - **runConsole** ouvre une console python connectée à la session SALOME courante. Il est également possible d'utiliser
    runSession puis de lancer python.


Les fichiers de configuration de l'application sont :

  - **SALOMEApp.xml** : ce fichier est semblable au fichier par défaut qui se trouve dans ${GUI_ROOT_DIR}/share/SALOME/resources/gui.
    Il peut être adapté aux besoins de l'utilisateur.
  - **CatalogRessources.xml** : ce fichier décrit tous les calculateurs que l'application peut utiliser. Le fichier initial
    ne contient que la machine locale. L'utilisateur doit ajouter les machines à utiliser. Si on veut utiliser
    des répertoires d'application quelconques sur les différents calculateurs, il faut préciser dans ce fichier 
    leur localisation avec l'attribut appliPath::

        appliPath="my/specific/path/on/this/computer"






