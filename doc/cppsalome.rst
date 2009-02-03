
:tocdepth: 3

.. _cppsalome:

===========================================================
Guide pour le développement d'un module SALOME en C++
===========================================================

Ce document a pour objectif de décrire les différentes étapes
du développement d'un module SALOME en C++.
Il fait suite au document :ref:`pysalome`, qui documente le module PYHELLO, et en reprend la démarche :
construction pas à pas d'un module HELLO.
Comme de nombreux points ne sont pas repris, il est recommendé de lire ce
document préalablement.


Les étapes de construction du module exemple
====================================================
Le composant choisi pour illustrer le processus de construction en C++
est le même que celui choisi pour illustrer la construction du module python :
il implémentera donc la même interface idl Corba.
Il sera complété par un GUI graphique écrit en Qt.

Les différentes étapes du développement seront les suivantes :

  - créer une arborescence de module
  - créer un composant SALOME chargeable par un container C++
  - configurer le module pour que le composant soit connu de SALOME
  - ajouter un GUI graphique
  - rendre le composant utilisable dans le superviseur

Création de l'arborescence du module
=======================================
Dans un premier temps, on se contentera de mettre dans le module exemple un composant
SALOME écrit en C++ qui sera chargeable par un container C++.
Il suffit donc d'une interface idl et d'une implantation C++ du composant.
Pour mettre en oeuvre ceci dans un module SALOME, il nous faut reproduire l'arborescence de
fichier standard suivante::

  + HELLO1_SRC
    + build_configure
    + configure.in.base
    + Makefile.in
    + adm_local
      + unix
        + make_commence.in
        + make_omniorb.in
        + config_files
    + bin
      + VERSION
      + runAppli.in
      + runSalome.py
    + idl
      + Makefile.in
      + HELLO_Gen.idl
    + src
      + Makefile.in
      + HELLO
        + Makefile.in
        + HELLO.cxx 
        + HELLO.hxx 
    + doc

Pour cela, on recopie l'arborescence de PYHELLO, et on modifie où nécessaire
PYHELLO en HELLO::

    cp -r PYHELLO1_SRC HELLO1_SRC
    cd HELLO1_SRC
    mv idl/PYHELLO_Gen.idl idl/HELLO_Gen.idl
    mv src/PYHELLO src/HELLO


Interface idl
==================
Dans le répertoire idl, nous modifions le fichier idl HELLO_Gen.idl : le
module défini est renommé HELLO_ORB, et l'interface en HELLO_Gen.
Le service rendu reste le même : à partir d'une chaine de caractères
fournie comme unique argument, retour d'une chaine de caractères obtenue
par concaténation de "Hello, " et de la chaine d'entrée.
Ce service est spécifié par la fonction makeBanner.

Un utilitaire de documentation basé sur doxygen a été mis en place pour
compiler une documentation des services corba à partir de commentaires se
trouvant dans les fichiers idl. Nous rajouter donc à notre idl quelques
commentaires, en respectant le formalisme doxygen.
Un commentaire doxygen commence par "/\*!" et se finit par "\*/".
Pour structurer un minimum les pages générées, on les regroupe par module ou
sujet. Dans notre exemple, nous utilisons la directive::

    \ingroup EXAMPLES 

spécifiant que la documentation générée fait partie du groupe EXAMPLES.
(pour plus d'information sur doxygen, consulter le site http://www.doxygen.org).

Pour finir, nous mettons à jour le Makefile avec le nouveau nom de composant::
    
    IDL_FILES = HELLO_Gen.idl


Implémentation C++
==================

Les sources
-----------

L'implémentation C++ de notre module CORBA HELLO (interface idl HELLO_Gen) est faite dans le répertoire
/src/HELLO::

    HELLO.hxx
    HELLO.cxx

Au début du header de notre module (HELLO.hxx), les inclusions suivantes sont
nécessaires::

    #include <SALOMEconfig.h>
    #include CORBA_SERVER_HEADER(HELLO_Gen)
    #include "SALOME_Component_i.hxx"

Le fichier SALOMEconfig.h contient un certain nombre de définitions utiles
pour assurer l'indépendance du code par rapport à la version de CORBA
utilisée. SALOME_Component_i.hxx contient l'interface de la classe
d'implémentation C++ du composant de base Salome (idl Engines::Component).
Enfin, la macro CORBA_SERVER_HEADER assure l'indépendance des noms de fichiers
d'inclusion par rapport à l'implémentation de l'ORB CORBA.

Après cela, nous définissons une classe d'implémentation, nommée HELLO, dérivant de
POA_HELLO_ORB::HELLO_Gen (classe abstraite générée automatiquement par CORBA lors de la
compilation de l'idl) et de Engines_Component_i (car l'interface idl HELLO_Gen
dérive de Engines::Component comme tout composant Salome). Cette classe
contient un constructeur dont les arguments sont imposés par CORBA, un
destructeur virtuel, et une méthode makeBanner fournissant le service souhaité::

    class HELLO:
      public POA_HELLO_ORB::HELLO_Gen,
      public Engines_Component_i
    {
    public:
	HELLO(CORBA::ORB_ptr orb,
		PortableServer::POA_ptr poa,
		PortableServer::ObjectId * contId,
		const char *instanceName,
		const char *interfaceName);
	virtual ~HELLO();
	char* makeBanner(const char* name);
    };

La fonction makeBanner prend comme argument et renvoit un char*, projection C++ du type CORBA/IDL
string. 
La documentation complète du mapping c++ de l'IDL est fournie par l'OMG sur
son site internet : http://www.omg.org/cgi-bin/doc?ptc/00-01-02.

Enfin, nous fournissons l'interface (normalisé) de la fonction HELLOEngine_factory, qui
sera appelée par le "FactoryServer C++" pour charger le composant HELLO:
::

    extern "C"
	  PortableServer::ObjectId * HELLOEngine_factory(
		CORBA::ORB_ptr orb,
		PortableServer::POA_ptr poa,
		PortableServer::ObjectId * contId,
		const char *instanceName,
		const char *interfaceName);


Dans le fichier source (HELLO.cxx) se trouvent les définitions 
du constructeur et de la fonction d'instanciation
HELLOEngine_factory (toutes deux normalisées!), et de makeBanner:

::

    char* HELLO::makeBanner(const char* name)
    {
	string banner="Hello, ";
	banner+=name;
	return CORBA::string_dup(banner.c_str());
    }

Dans cette fonction, l'emploi de string_dup (fonction déclarée dans le
namespace CORBA) n'est pas obligatoire (on aurait pu utiliser l'opérateur new),
mais conseillé car ces fonctions permettent aux ORB d'utiliser des mécanismes
spéciaux de gestion de la mémoire sans avoir à redéfinir les opérateurs new
globaux.

Makefile
--------

Dans le makefile, il faut définir certaines cibles::

    VPATH=.:@srcdir@:@top_srcdir@/idl
    LIB = libHELLOEngine.la
    LIB_SRC = HELLO.cxx
    LIB_SERVER_IDL = HELLO_Gen.idl
    LIB_CLIENT_IDL = SALOME_Component.idl SALOME_Exception.idl Logger.idl
    CPPFLAGS += -I${KERNEL_ROOT_DIR}/include/salome
    LDFLAGS+= -lSalomeContainer -lOpUtil -L${KERNEL_ROOT_DIR}/lib/salome

Passons en revue chacune de ces cibles.

- LIB contient le nom *normalisé* (lib<Nom_Module>Engine.la) le nom de la
  librairie, LIB_SRC définit le nom des fichiers sources, et VPATH les
  repertoire où l'on peut les trouver.
- LIB_SERVER_IDL contient le nom des fichiers idl implémentés par le module.
- LIB_CLIENT_IDL contient le nom des idl où sont définis les services CORBA
  utilisés par le module. HELLO utilise Logger.idl via les macros "MESSAGE",
  SALOME_Component.idl et SALOME_Exception.idl via l'héritage de HELLO_ORB
- Il faut ajouter à CPPFLAGS le chemin pour les fichiers includes utilisés
  (SALOMEconfig.h, SALOME_Component_i.hxx et utilities.h se trouvent dans
  ${KERNEL_ROOT_DIR}/include/salome)
- La classe HELLO utilise les librairies lib (pour Engines_Component_i) et
  libOptUtil (pour PortableServer et Salome_Exception). On indique donc le nom
  de ces librairies et leur chemin dans LDFLAGS.
  D'autres librairies sont souvent utiles, par exemple libsalomeDS si on
  implémente la persistence, ou libSalomeNS si on utilise le naming service.


Pilotage du composant depuis Python (mode TUI)
==============================================

Lors de la compilation du module, la cible lib du Makefile dans /idl a
provoqué la génération d'un stub python (souche côté client générée à partir
de l'idl et offrant une interface dans le langage client - ici python.
Concrètement, un module python HELLO_ORB contenant une classe
_objref_HELLO_Gen sont créés, permettant de faire appel aux services de notre
module C++ depuis python. Mettons ceci en application. Pour cela, nous lançons
Salome en mode TUI::

    cd $HELLO_ROOT_DIR/bin/salome
    python -i runSalome.py --modules=HELLO --xterm --logger --containers=cpp,python --killall

Depuis la fenêtre python, nous importons le module LifeCycle, et utilisons ses
services pour charger notre composant Dans la conteneur C++ FactoryServer::

    >>> import LifeCycleCORBA
    >>> lcc = LifeCycleCORBA.LifeCycleCORBA(clt.orb)
    >>> import HELLO_ORB
    >>> hello = lcc.FindOrLoadComponent("FactoryServer", "HELLO")

L'import de HELLO_ORB est nécessaire avant l'appel de FindOrLoadComponent,
pour permettre de retourner un objet typé (opération de "narrowing"). Sinon,
l'objet retourné est générique de type Engines::Component. Vérifions que notre
objet hello est correctement typé, et appelons le service makeBanner::

    >>> print hello
    <HELLO_ORB._objref_HELLO_Gen instance at 0x8274e94>
    >>> mybanner=hello.makeBanner("Nicolas")
    >>> print mybanner
    Hello, Nicolas

Les commandes précédentes ont été regroupées dans la fonction test du script
/bin/runSalome.py.


Interface graphique
===================

Introduction
------------

Pour aller plus loin dans l'intégration de notre module, nous allons ajouter
une interface graphique (développée en Qt), s'intégrant dans l'interface
applicative de Salome (IAPP).
On ne détaillera pas ici le fonctionnement de l'IAPP de Salome, mais pour
résumer, l'IAPP gère une boucle d'évènements (clics souris, clavier, etc), et
redirige après traitement ces évènements vers le module actif (le principe est
qu'à un instant donné, *un* module est actif. Lorsqu'un module est activé, son
IHM est chargée dynamiquement).
Le programmeur de la GUI d'un module a donc à charge de définir les méthodes
permettant de traiter correctement les évènements transmis. Parmi ces
méthodes, citons les principales : OnGUIEvent(), OnMousePress(), OnMouseMove(),
OnKeyPress(), DefinePopup(), CustomPopup().

Choix des widgets
-----------------

Description xml
```````````````
La description des items de notre module se fait dans le fichier XML
/ressources/HELLO_en.xml. Ce fichier est utilisé par l'IAPP pour charger
dynamiquement l'IHM du module quand celle-ci est activée.
Le principe est de définir par des balises les menus et boutons souhaités, et
d'y associer des ID, qui seront récupérés par les fonctions gérant les
évènemements IHM. Plusieures possibilités sont offertes:

- ajout d'items à des menus déjà existant, auquel cas il faut reprendre les
  balises du menu pré-existant, et y ajouter les nouveaux items. Dans
  l'exemple qui suis, on ajoute le Menu **Hello** et l'item **MyNewItem** au
  menu File, dont l'ID vaut 1::

    <menu-item label-id="File" item-id="1" pos-id="">
	 <submenu label-id="Hello" item-id="19" pos-id="8">
	    <popup-item item-id="190" pos-id="" label-id="MyNewItem" icon-id="" tooltip-id="" accel-id="" toggle-id="" execute-action=""/>
	  </submenu>
	  <endsubmenu />
    </menu-item>

- Création de nouveaux menus. Pour le module HELLO, nous ajoutons un menu
  HELLO, avec un unique item de label "Get banner"::

    <menubar>
     <menu-item label-id="HELLO" item-id="90" pos-id="3">
      <popup-item item-id="901" label-id="Get banner" icon-id="" tooltip-id="Get HELLO banner" accel-id="" toggle-id="" execute-action=""/>
     </menu-item>
    </menubar>

- Ajout d'un bouton dans la barre à boutons. Dans l'exemple suivant, nous
  créons un deuxième point d'entrée pour notre action "Get banner", sous forme
  d'un bouton associé au même ID "901". L'icône est spécifiée par la le champ
  icon-id, qui doit être un fichier graphique 20x20 pixels au format png::

    <toolbar label-id="HELLO">
     <toolbutton-item item-id="901" label-id="Get banner" icon-id="ExecHELLO.png"
    tooltip-id="Get HELLO banner" accel-id="" toggle-id="" execute-action=""/>
    </toolbar>

Convention
``````````
A chaque menu ou item est associé un ID. Les numéros entre 1 et 40 sont
réservés à l'IAPP. Les numéros d'ID suivent une certaine règle, quoique
celle-ci ne soit pas obligatoire. Au menu "HELLO" est associé l'ID 90. Son
unique item "Get banner" a l'ID 901. Un deuxième item aurait l'ID 902, et un
sous item l'ID 9021.


Implémentation de l'IHM
-----------------------

L'implémentation C++ de l'IHM est faite dans le répertoire /src/HELLOGUI.
Le header HELLOGUI.h déclare de la classe HELLOGUI, et
contient des directives Qt (Q_OBJECT). De ce fait, il doit être processé par
le compilateur moc (Qt Meta Model Compiler). Pour cette raison, l'extension du
fichier est .h et dans le Makefile nous ajoutons la cible::

	LIB_MOC = HELLOGUI.h

Le fichier source HELLO.cxx contient la définition des fonctions membres, et
le Makefile permet de construire une librairie libHELLOGUI (le nom est
normalisé poour permettre le chargement dynamique : lib<NomModule>GUI.

Gestion des évènements
``````````````````````
Pour l'IHM d'HELLO, nous définissons la fonction HELLOGUI::OnGUIEvent, qui
sera appelé à chaque évènement. Cette fonction contient essentiellement une
structure switch permettant de traiter l'ID reçu en argument::

  switch (theCommandID)
    {
    case 901:
      // Traitement de "Get banner"
      ...
    case 190:
      // Traitement de "MyNewItem"
      ...
    }

Le traitement standard consiste à récupérer des données d'entrée (ici, le
prénom via une fenêtre de dialogue QInputDialog::getText), à récupérer une
poignée sur le composant CORBA interfacé afin d'appeler le service souhaité
(ici, getBanner), et d'afficher le résultat obtenu ().

Classes disponibles
````````````````````
Pour les dialogues avec l'utilisateur, il est possible d'utiliser n'importe
quelle classe fournie par Qt (http://doc.trolltech.com/3.2/classes.html). 
Cependant, lorque c'eset possible, il est préférable d'utiliser les fonctions
QAD (Qt Application Desktop), définies dans KERNEL_SRC/src/SALOMEGUI, qui
encapsulent les fonctions Qt correspondantes et gèrent mieux les
communications avec l'IAPP. Ainsi, dans HELLOGUI, nous utilisons la classe
QAD_MessageBox en lieu et place de la classe Qt QMessageBox.


Gestion du multi-linguisme
``````````````````````````
Qt fournit un outil d'aide au support du multi-linguisme. Celui-ci est
repris dans salome. Le principe est simple : toutes les chaînes de caractères
utilisées pour les labels des menus et les dialogues avec l'utilisateur 
sont encapsulés dans des appels à la fonction Qt tr() (pour translate), qui
prend en argument un nom de label. Par exemple, pour demander à l'utilisateur
de rentrer un prénom, nous utilisons la fonction getText, où les deux premiers
arguments sont des labels encapsulés par tr()::

        myName = QInputDialog::getText( tr("QUE_HELLO_LABEL"), tr("QUE_HELLO_NAME"),
                                        QLineEdit::Normal, QString::null, &ok);

Le nom des label est préfixé à titre indicatif par trois lettres et un underscore. Les codes
suivants sont utilisés::

 - MEN_ : label menu
 - BUT_ : label bouton
 - TOT_ : aide tooltip
 - ERR_ : message d'erreur
 - WRN_ : message d'alerte
 - INF_ : message d'information
 - QUE_ : question
 - PRP_ : prompt dans la barre des status


La traduction des labels encapsulés par tr() est faite pour différents
langages cibles (par exemple français et anglais) dans des fichiers nommés "<nom_module>_msg_<langage>.po". 
<langage> correspond au code du langage, on a choisi **en** pour l'anglais et
**fr** pour le français. Ce fichier doit contenir pour chaque clé sa
traduction, par exemple::

    msgid "HELLOGUI::INF_HELLO_BANNER"
    msgstr "HELLO Information"

Le squelette de ce fichier peut être généré par l'utilitaire Qt findtr::

    findtr HELLOGUI.cxx > HELLO_msg_en.po

puis éditer le fichier HELLO_msg_en.po pour remplir les traductions.
Ces fichiers sont ensuite compilés par l'utilitaire **msg2qm** pour générer
des binaires *.qm*. Pour cela, il faut remplir la cible LIB_MOC dans le
Makefile::

    PO_FILES =  HELLO_msg_en.po HELLO_msg_fr.po

Pour l'utilisateur final, le choix du langage se fait au niveau de chaque
module dans le fichier ressources/config, en utilisant la commande::

    langage=<langage>



Règles syntaxiques de nommage
=============================

Dans ce qui précède, nous avons utilisé un certain nombre de règles de
nommage. Le présent chapitre se propose de faire le point sur ces règles.
Celles-ci ne sont pas toutes obligatoires, mais simplifient la compréhension
si on les suit!

+-------------------+------------------+----------------+---------------------------------------+
| Règle             | Formalisme       | Exemple HELLO  | Commentaire                           |
+===================+==================+================+=======================================+
| Nom du module     | <Module>         | HELLO          | C'est le nom qui figure dans le       |
|                   |                  |                | catalogue des modules                 |
+-------------------+------------------+----------------+---------------------------------------+
| Base CVS          | <Module>         | EXAMPLES       | Si la base cvs contient plusieurs     | 
|                   |                  |                | modules, on prend un autre nom        |
+-------------------+------------------+----------------+---------------------------------------+
| Repertoire source | <Module>_SRC     | HELLO1_SRC     | L'indice 1 est utilisé car on prévoit |
|                   |                  |                | plusieurs version du module           |
+-------------------+------------------+----------------+---------------------------------------+
| Fichier idl       | <Module>_Gen.idl | HELLO_Gen.idl  |                                       |
|                   |                  |                |                                       |
+-------------------+------------------+----------------+---------------------------------------+
| Nom du module     | <Module>_ORB     | HELLO_ORB      | On évite d'utiliser le nom du module  |
| CORBA             |                  |                | (conflits)                            |
+-------------------+------------------+----------------+---------------------------------------+
| Nom de            | <Module>_Gen     | HELLO_Gen      | La compilation de l'idl génère une    |
| l'interface CORBA |                  |                | classe abstraite                      |
|                   |                  |                | POA_<Module>_ORB::<Module>_Gen        |
+-------------------+------------------+----------------+---------------------------------------+
| fichier source    | <Module>.cxx     | HELLO.cxx      | Dans le répertoire /src/<Module>      |
|                   |                  |                |                                       |
+-------------------+------------------+----------------+---------------------------------------+
| Classe            | <Module>         | HELLO          | Cette classe hérite de                |
| d'implémentation  |                  |                | POA_HELLO_ORB::HELLO_Gen              |
+-------------------+------------------+----------------+---------------------------------------+
| Fonction          | <Module>_        | HELLO_Engine   | Cette fonction est appelée par        |
| d'instanciation   | Engine_factory   | factory        | le FactoryServer de Salome            |
+-------------------+------------------+----------------+---------------------------------------+
| Catalogue des     | <Module>Catalog  | HELLOCatalog   | Dans /ressources                      |
| modules           | .xml             | .xml           |                                       |
+-------------------+------------------+----------------+---------------------------------------+
| Nom de la         | lib<Module>Engine| libHELLOEngine | Dans le répertoire /src/<Module>      |
| librairie C++     |                  |                |                                       |
+-------------------+------------------+----------------+---------------------------------------+
| Librairie C++     | lib<Module>GUI   | libHELLOGUI    | Dans le répertoire /src/<Module>GUI   |
| de l'IHM          |                  |                |                                       |
+-------------------+------------------+----------------+---------------------------------------+
| Variable          | <Module>_ROOT_DIR| HELLO_ROOT_DIR |                                       |
| d'environnement   |                  |                |                                       |
+-------------------+------------------+----------------+---------------------------------------+
| ...               | ...              | ...            | ...                                   |
|                   |                  |                |                                       |
+-------------------+------------------+----------------+---------------------------------------+

