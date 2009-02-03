
:tocdepth: 3

.. raw:: latex

  \makeatletter
  \g@addto@macro\@verbatim\small
  \makeatother


.. _yacsgen:

YACSGEN : Générateur automatique de module SALOME 
==================================================

YACSGEN est un module python (module_generator.py) qui permet de fabriquer un module
SALOME automatiquement à partir d'une description synthétique des composants
qu'il contiendra. Cette description est faite en langage Python.

Les caractéristiques de ces composants ne sont pas générales mais devraient
faciliter l'intégration de nombreux composants de calcul scientifique.

Ce générateur ne prend pas en charge l'intégration d'une IHM graphique mais seulement
la partie calcul. L'objectif principal est l'intégration d'une bibliothèque Fortran dans laquelle 
on peut faire des appels aux ports datastream (Calcium, en particulier).

Obtenir module_generator.py
------------------------------------------------------------
Voir site PAL : http://pal.der.edf.fr/pal/projets/pal/superv/modulegenerator

Versions et architectures supportées
-----------------------------------------------------------------
Module_generator.py utilise des fonctionnalités de python 2.4 mais a un mode de compatibilité avec python 2.3.
Il fonctionne sur architecture 32 bits et 64 bits (testé sur machine Aster).

Installation
----------------------------
Il n'y a pas de procédure d'installation particulière. Il suffit de décompresser et détarrer l'archive 
obtenue à partir du site PAL (YACSGEN-x.y.tar.gz) et d'ajouter le répertoire ainsi créé au PYTHONPATH.

Description d'un module SALOME
--------------------------------------------------------
Un module SALOME est décrit au moyen d'instructions Python et en utilisant des définitions contenues
dans le module Python module_generator.py.

La première action à réaliser est d'importer ces définitions::

     from module_generator import Generator,Module,PYComponent
     from module_generator import CPPComponent,Service,F77Component

Pour décrire un module SALOME, on donne son nom <nommodule>, la liste de ses composants (<liste des composants>) 
et le nom du répertoire dans lequel il sera installé (<prefix>).

Sa description prend la forme suivante ::

     m=Module(<nommodule>,components=<liste des composants>,prefix=<prefix>)

Pour un module de nom "toto" avec un composant c1 (voir ci-dessous pour la description des composants) qui sera installé dans le
répertoire "Install", on aura ::

     m=Module("toto",components=[c1],prefix="Install")



Description des composants
------------------------------------------------

Il est possible de créer plusieurs types de composants :

  - le type C/C++
  - le type Fortran 77
  - le type Python
  - le type Aster

Tous ces types ont une description semblable. On commencera par le type C++ puis
on décrira les différences principales pour les autres types.

Composant C/C++
++++++++++++++++++++++++++++++++++++++++
Tout d'abord, un composant C++ a un nom. Ce nom sera utilisé par la suite quand on voudra créer
des instances de ce composant. Le module SALOME, une fois compilé et installé contiendra une
librairie dynamique de nom lib<compo>Engine.so, où <compo> est le nom du composant.
Un composant C++ est implémenté comme un objet C++ exécutable à distance.

Un composant C++ a un ou plusieurs services. Chaque service a un nom qui est le nom de
la méthode de l'objet C++ qui correspond au composant.
Chaque service a, éventuellement, des ports dataflow d'entrée et de sortie et des ports datastream
d'entrée et de sortie.

Un premier service avec des ports dataflow
"""""""""""""""""""""""""""""""""""""""""""""""""""""""
Pour le moment, les seuls types possibles pour les ports dataflow sont :

- double : scalaire équivalent à un double C
- long : scalaire équivalent à un long C
- string : équivalent à un char* C (chaine de caractères de longueur quelconque)
- dblevec : vecteur de doubles
- stringvec : vecteur de string
- intvec : vecteur de longs
- pyobj : objet python serialisé avec pickle (ne marche qu'avec des composants implémentés en Python)

Un port est décrit par un tuple python de longueur 2 dont la première valeur est le nom du port
et la deuxième le nom du type. Les ports d'entrée sont décrits par une liste de ces tuples
ainsi que les ports de sortie.

Un petit exemple vaut mieux qu'un long discours. Un composant de nom "moncompo" avec un service
de nom "monservice" qui a un port dataflow d'entrée de nom "portentrant" de type double
et un port dataflow de sortie de nom "portsortant" de type double aura la description suivante ::

      c1=CPPComponent("moncompo",
                      services=[
                                Service("monservice",
                                        inport=[("portentrant","double"),],
                                        outport=[("portsortant","double")],
                                       ),
                               ]
                     )

c1 est une variable intermédiaire Python qui sera utilisée pour décrire la liste des composants d'un module : (components=[c1])
pour un module avec un seul composant.


En fait ce composant n'a pas grand intérêt car lors de l'exécution, il va prendre un double en entrée
d'exécution et fournir un double en sortie d'exécution mais il ne fait rien entre temps.
Il faut donc lui ajouter un contenu. Ce contenu sera spécifié dans 2 attributs du service : defs et body.
defs contiendra du code C++ de définition (par exemple, #include <iostream>) et body contiendra le code C++
qui sera exécuté entre l'entrée et la sortie (par exemple, portsortant=2*portentrant;).
Au final notre description devient ::

      c1=CPPComponent("moncompo",
                       services=[
                                 Service("monservice",
                                         inport=[("portentrant","double"),],
                                         outport=[("portsortant","double")],
                                         defs="#include <iostream>",
                                         body="portsortant=2*portentrant;",
                                        ),
                                ]
                     )

Ajouter des ports datastream au service
""""""""""""""""""""""""""""""""""""""""""""""
Pour ajouter des ports datastream au service "monservice", on ajoute à la description les attributs instream et outstream.
Ces attributs doivent être des listes de triplets dont les éléments sont :

  1. le nom du port
  2. le type du port.
  3. le mode de dépendance temporelle ("T") ou itérative ("I") (se référer à la documentation Calcium pour plus de détails)

Les types possibles sont "CALCIUM_double", "CALCIUM_integer", "CALCIUM_real", "CALCIUM_string", "CALCIUM_logical"
et "CALCIUM_complex".

Avec un port datastream entrant et un port sortant en dépendance temporelle, la description devient ::

      c1=CPPComponent("moncompo",
                      services=[
                                Service("monservice",
                                        inport=[("portentrant","double"),],
                                        outport=[("portsortant","double")],
                                        instream=[("porta","CALCIUM_double","T")],
                                        outstream=[("portb","CALCIUM_double","T")],
                                        defs="#include <iostream>",
                                        body="portsortant=2*portentrant;",
                                        ),
                               ]
                     )

Il faudrait bien sûr ajouter dans body des appels à la bibliothèque CALCIUM pour que le service soit vraiment fonctionnel.

Ajouter un deuxième service au composant
"""""""""""""""""""""""""""""""""""""""""""""""""
Si on veut un deuxième service pour le composant il suffit d'ajouter une autre description de service ::

      c1=CPPComponent("moncompo",
                      services=[
                                Service("monservice",
                                        inport=[("portentrant","double"),],
                                        outport=[("portsortant","double")],
                                        instream=[("porta","CALCIUM_double","T")],
                                        outstream=[("portb","CALCIUM_double","T")],
                                        defs="#include <iostream>",
                                        body="portsortant=2*portentrant;",
                                       ),
                                Service("serv2",
                                        inport=[("a","double"),("b","long")],
                                        outport=[("c","double")],
                                        body="c=b*a",
                                       ),
                               ]
                     )

Ici, on a ajouté un deuxième service de nom "serv2" avec 2 ports dataflow d'entrée (a et b) et un port dataflow de sortie (c).
Le service est réduit à sa plus simple expression : il retourne le produit de ses 2 entrées.

Assembler avec des bibliothèques externes
""""""""""""""""""""""""""""""""""""""""""""""""""""
On a vu que les attributs *defs* et *body* permettent de définir le corps du service mais il est souvent plus pratique d'utiliser des bibliothèques
externes plutôt que de tout mettre dans ces 2 attributs.
Ceci est possible à condition d'indiquer dans les attributs *libs* et *rlibs* du composant, tout ce qui est nécessaire pour l'étape de link
du composant.

On pourra avoir, par exemple::

      c1=CPPComponent("moncompo",
                      services=[
                                Service("monservice",
                                        inport=[("portentrant","double"),],
                                        outport=[("portsortant","double")],
                                        defs="extern double myfunc(double);",
                                        body="portsortant=myfunc(portentrant);",
                                       ),
                               ],
                      libs="-L/usr/local/mysoft -lmybib",
                      rlibs="-Wl,--rpath -Wl,/usr/local/mysoft"
                      )

L'attribut *rlibs* n'est pas obligatoire mais peut être utilisé pour indiquer un path de recherche pour des bibliothèques 
dynamiques à l'exécution.
*libs* est utilisé pendant la phase de link. *rlibs* est utilisé uniquement à l'exécution, il évite d'avoir à positionner 
la variable d'environnement LD_LIBRARY_PATH pour trouver la librairie dynamique.

Ajouter des includes
""""""""""""""""""""""""""""""""""""""""""""""""""""
Les includes seront ajoutés au moyen de l'attribut *defs*. Par exemple ::

   defs="#include "moninclude.h"

Le chemin des includes sera spécifié dans l'attribut *includes* du composant, sous la forme suivante ::

   defs="""#include "moninclude.h"
   extern double myfunc(double);
   """
   c1=CPPComponent("moncompo",
                   services=[
                             Service("monservice",
                                     inport=[("portentrant","double"),],
                                     outport=[("portsortant","double")],
                                     defs=defs,
                                     body="portsortant=myfunc(portentrant);",
                                    ),
                            ],
                   libs="-L/usr/local/mysoft -lmybib",
                   rlibs="-Wl,--rpath -Wl,/usr/local/mysoft",
                   includes="-I/usr/local/mysoft/include",
                  )


Composant Fortran
++++++++++++++++++++++++++++++++++++++++
Un composant Fortran se décrit comme un composant C++ à quelques différences près. 
Tout d'abord, on utilise l'objet de définition F77Component au lieu de CPPComponent.
Ensuite, un interfaçage supplémentaire spécial au Fortran est réalisé. On suppose que les fonctionnalités Fortran
sont implémentées dans une librairie (dynamique ou statique) qui sera linkée avec le composant et qui dispose de
plusieurs points d'entrée de mêmes noms que les services du composant. L'appel à ce point d'entrée sera ajouté
automatiquement après le code C++ fourni par l'utilisateur dans l'attribut body.

Ceci permet de découpler presque totalement l'implémentation du composant Fortran qui sera dans la bibliothèque
externe, de l'implémentation du composant SALOME qui ne sert que pour l'encapsulation.

L'exemple suivant permettra de préciser ces dernières notions ::

     c3=F77Component("compo3",
                     services=[
                               Service("s1",
                                       inport=[("a","double"),("b","long"),("c","string")],
                                       outport=[("d","double"),("e","long"),("f","string")],
                                       instream=[("a","CALCIUM_double","T"),
                                                 ("b","CALCIUM_double","I")],
                                       outstream=[("ba","CALCIUM_double","T"),
                                                  ("bb","CALCIUM_double","I")],
                                       defs="#include <unistd.h>",
                                       body="chdir(c);"
                                      ),
                              ],
                     libs="-L/usr/local/fcompo -lfcompo",
                     rlibs="-Wl,--rpath -Wl,/usr/local/fcompo"
                    )

Le composant Fortran "compo3" a des ports dataflow et datastream comme le composant C++. La bibliothèque dynamique Fortran
qui contient le point d'entrée Fortran s1 sera linkée grâce aux attributs libs et rlibs de la description. Le composant
Fortran supporte également l'attribut *includes*.

Il est possible d'ajouter un bout de code C++ avant l'appel au point d'entrée Fortan. Ce bout de code doit être mis
dans l'attribut body avec des définitions éventuelles dans defs. Ici, on utilise la variable dataflow entrante "c"
pour faire un changement de répertoire avec l'appel à chdir.

Composant Python
++++++++++++++++++++++++++++++++++++++++
Un composant Python se décrit également comme un composant C++. Les seules différences portent sur l'objet Python
à utiliser pour le définir : PYComponent au lieu de CPPComponent et sur le contenu des attributs *defs* et *body*
qui doivent contenir du code Python et non C++ (attention à l'indentation, elle n'est pas prise en charge automatiquement).

Exemple de composant Python ::

      pyc1=PYComponent("moncompo",
                       services=[
                                 Service("monservice",
                                         inport=[("portentrant","double"),],
                                         outport=[("portsortant","double")],
                                         defs="import sys",
                                         body="      portsortant=2*portentrant;",
                                        ),
                                ]
                      )

L'équivalent de l'assemblage avec des bibliothèques externes est réalisé ici avec la possibilité d'importer des modules
Python externes. Il suffit d'ajouter l'attribut *python_path* à la description du composant pour avoir cette possibilité. La valeur
à donner est une liste de répertoires susceptibles de contenir des modules à importer.

Exemple::

     pyc1=PYComponent("moncompo",
                      services=[
                                Service("monservice",
                                        inport=[("portentrant","double"),],
                                        outport=[("portsortant","double")],
                                       ),
                               ],
                      python_path=["/usr/local/mysoft","/home/chris/monsoft"],
                     )

Composant Aster
++++++++++++++++++++++++++++++++++++++++
Un composant Aster est un composant un peu particulier car les fonctionnalités du logiciel sont implémentées en Fortran mais elles
sont activées par un superviseur de commandes écrit en Python. Au final ce superviseur exécute un script Python mais il faut gérer le transfert
des données entre Python et Fortran et l'intégration du superviseur de commandes dans un composant SALOME.

Le point de départ est le suivant : on suppose que l'on dispose d'une installation d'Aster qui fournit un module python aster
sous la forme d'une bibliothèque dynamique importable (astermodule.so) et non comme c'est le cas dans l'installation actuelle
d'un interpréteur Python spécifique linké avec ce même module.

Un composant Aster se décrit comme un composant Python auquel il faut ajouter plusieurs attributs
importants.

     - l'attribut *python_path* : il indique le chemin du répertoire contenant le module aster (astermodule.so)
     - l'attribut *aster_dir* : il indique le chemin du répertoire d'installation d'Aster
     - l'attribut *argv* : il initialise les paramètres de la ligne de commande.  On y mettra, par exemple la valeur 
       de memjeveux (``argv=["-memjeveux","10"]``) ou de rep_outils.

Voici un petit exemple de description de composant Aster avec un seul service doté de 3 ports dataflow d'entrée, 
d'un port dataflow de sortie, de 7 ports datastream d'entrée et d'un port datastream de sortie::

    c1=ASTERComponent("caster",
                      services=[
                                Service("s1",
                                        inport=[("a","double"),("b","long"),("c","string")],
                                        outport=[("d","double")],
                                        instream=[("aa","CALCIUM_double","T"),
                                                  ("ab","CALCIUM_double","I"),
                                                  ("ac","CALCIUM_integer","I"),
                                                  ("ad","CALCIUM_real","I"),
                                                  ("ae","CALCIUM_string","I"),
                                                  ("af","CALCIUM_complex","I"),
                                                  ("ag","CALCIUM_logical","I"),
                                                 ],
                                       outstream=[("ba","CALCIUM_double","T"),
                                                  ("bb","CALCIUM_double","I")],
                                      ),
                               ],
                      aster_dir="/local/chris/ASTER/instals/NEW9",
                      python_path=["/local/chris/modulegen/YACSGEN/aster/bibpyt"],
                      argv=["-memjeveux","10",
                            "-rep_outils","/local/chris/ASTER/instals/outils"],
                     )

Attention à ne pas appeler le composant "aster" car ce nom est réservé au module python de Code_Aster. En cas 
d'utilisation du nom "aster", le comportement est complètement erratique.

Bien que sa description soit très semblable à celle d'un composant Python, il y a une différence importante à l'utilisation. 
En effet, le composant Aster a besoin de la description d'un jeu de commandes pour fonctionner. Ce jeu de commandes 
est passé sous la forme d'un texte à chaque service du composant dans un port dataflow d'entrée de nom "jdc" et 
de type "string". 
Après génération, ce composant Aster aura donc 4 ports dataflow d'entrée ("jdc","a","b","c") et non 3 comme indiqué 
dans la description. Il ne faut pas oublier d'initialiser le port "jdc" dans le fichier de couplage avec un jeu de commandes.

Le superviseur de commandes a été intégré dans un composant SALOME et les variables reçues dans les ports dataflow 
sont disponibles lors de l'exécution du jeu de commandes. 
De même pour les ports dataflow de sortie, ils sont alimentés par les valeurs des variables issues de l'exécution 
du jeu de commandes.

**Attention au mode d'exécution**. Le superviseur de commandes a 2 modes d'exécution (PAR_LOT="OUI" ou PAR_LOT="NON"
que l'on spécifie dans la commande DEBUT). En mode PAR_LOT="OUI", il est obligatoire de terminer le jeu de commandes 
par une commande FIN ce qui a pour effet d'interrompre l'exécution. Ce n'est pas le fonctionnement à privilégier
avec YACS. Il est préférable d'utiliser le mode PAR_LOT="NON" sans mettre de commande FIN ce qui évite d'interrompre
l'exécution prématurément.

Module Aster dynamiquement importable et lien avec YACS
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
Ces deux points ne sont pas pris en charge par *module_generator.py*. Ils doivent être traités séparément dans un contexte
proche de celui d'un développeur Aster.

On suppose que l'on dispose d'une installation d'Aster, que l'on veut créer un module Python aster dynamiquement importable
et que l'on veut ajouter à Aster quelques commandes pour échanger des données via des ports datastream de YACS.

Pour rester simple, on ajoute 3 commandes : YACS_INIT, ECRIRE_MAILLAGE et LECTURE_FORCE dont les catalogues sont::

             YACS_INIT=PROC(nom="YACS_INIT",op=181, fr="Initialisation YACS",
                                  COMPO=SIMP(statut='o',typ='I'),
                           )
             ECRIRE_MAILLAGE=PROC(nom="ECRIRE_MAILLAGE",op=78, fr="Ecriture du maillage")
             LECTURE_FORCE=PROC(nom="LECTURE_FORCE",op=189, fr="Lecture de la force")

La première commande YACS_INIT sert à initialiser Aster dans le contexte YACS. Elle a un seul mot-clé simple COMPO
(de type entier) qui sera utilisé pour passer aux autres commandes l'identificateur du composant SALOME. Cet
identificateur sera stocké dans un COMMON fortran. Il est indispensable pour les appels aux sous programmes
CPLxx et CPExx qui seront utilisés dans les 2 autres commandes ECRIRE_MAILLAGE et LECTURE_FORCE. 

Les 2 autres commandes ne prennent n'ont aucun mot-clé et récupèrent l'identificateur dans le COMMON.

Les opérateurs seront écrits comme suit (sans les déclarations)::

          SUBROUTINE OP0189 ( IER )
    C     COMMANDE:  LECTURE_FORCE
          include 'calcium.hf'
          COMMON/YACS/ICOMPO
          CALL cpldb(ICOMPO,CP_TEMPS,t0,t1,iter,'aa',1,n,ss,info)
          CALL cpldb(ICOMPO,CP_ITERATION,t0,t1,iter,'ab',1,n,zz,info)
          CALL cplen(ICOMPO,CP_ITERATION,t0,t1,iter,'ac',1,n,zn,info)
          CALL cplre(ICOMPO,CP_ITERATION,t0,t1,iter,'ad',1,n,yr,info)
          CALL cplch(ICOMPO,CP_ITERATION,t0,t1,iter,'ae',1,n,tch,info)
          CALL cplcp(ICOMPO,CP_ITERATION,t0,t1,iter,'af',1,n,tcp,info)
          CALL cpllo(ICOMPO,CP_ITERATION,t0,t1,iter,'ag',3,n,tlo,info)
          END

          SUBROUTINE OP0078 ( IER )
    C     COMMANDE:  ECRIRE_MAILLAGE
          include 'calcium.hf'
          COMMON/YACS/ICOMPO
          CALL cpeDB(ICOMPO,CP_TEMPS,t0,1,'ba',1,tt,info)
          CALL cpeDB(ICOMPO,CP_ITERATION,t0,1,'bb',1,tp,info)
          END

Enfin, il faut construire une bibliothèque dynamique astermodule.so, et placer tous les modules Python
nécessaires dans un répertoire que l'on indiquera dans l'attribut *python_path*.
On peut utiliser différentes méthodes pour obtenir ce résultat. Le Makefile suivant en est une::

     #compilateur
     FC=gfortran
     #SALOME
     KERNEL_ROOT_DIR=/local/chris/SALOME2/RELEASES/Install/KERNEL_V4_0
     KERNEL_INCLUDES=-I$(KERNEL_ROOT_DIR)/include/salome
     KERNEL_LIBS= -L$(KERNEL_ROOT_DIR)/lib/salome -lCalciumC -lSalomeDSCSuperv \
                  -lSalomeDSCContainer -lSalomeDatastream -lSalomeDSCSupervBasic \
                  -Wl,--rpath -Wl,$(KERNEL_ROOT_DIR)/lib/salome
     #ASTER
     ASTER_ROOT=/local/chris/ASTER/instals
     ASTER_INSTALL=$(ASTER_ROOT)/NEW9
     ASTER_PUB=$(ASTER_ROOT)/public
     ASTER_LIBS = -L$(ASTER_INSTALL)/lib -laster \
             -L$(ASTER_PUB)/scotch_4.0/bin -lscotch -lscotcherr \
             -lferm -llapack -lhdf5
     SOURCES=src/op0078.f src/op0189.f
     CATAPY=catalo/ecrire_maillage.capy  catalo/lecture_force.capy

     all:pyth cata astermodule
     pyth:
       cp -rf $(ASTER_INSTALL)/bibpyt .
     cata: commande/cata.py
       cp -rf commande/cata.py* bibpyt/Cata
     commande/cata.py:$(CATAPY)
       $(ASTER_ROOT)/ASTK/ASTK_SERV/bin/as_run make-cmd
     astermodule:astermodule.so pyth
       cp -rf astermodule.so bibpyt
     astermodule.so: $(SOURCES)
       $(FC) -shared -o $@ $(SOURCES) $(KERNEL_INCLUDES) $(ASTER_LIBS) $(KERNEL_LIBS)

Modifier les paramètres de la ligne de commande à l'exécution
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
L'attribut *argv* permet de donner des valeurs initiales aux arguments comme "memjeveux" mais
ces valeurs sont utilisées par le générateur pour construire le composant et restent donc constantes
par la suite, à l'exécution.

Pour modifier ces valeurs à l'exécution, il faut ajouter un port d'entrée de nom "argv" et de type "string".
La chaine de caractère qui sera donnée comme valeur de ce port sera utilisée par le composant pour
modifier les arguments de la ligne de commande (voir `Exemple d'exécution de composant Aster`_
pour un exemple d'utilisation).

Gestion du fichier elements
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
Le fichier des éléments finis est copié automatiquement dans le répertoire de travail sous le
nom elem.1. Le composant utilise l'attribut *aster_dir* pour localiser le fichier d'origine.

Version d'Aster supportées
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
Module_generator.py peut fonctionner avec Aster 9.0 et 9.2 (probablement avec 9.1 mais
ce n'est pas testé).

Générateur de module SALOME
-----------------------------------------------------------
Le module SALOME est créé par un générateur construit à partir de la description du module SALOME (m) vue
précédemment et d'un dictionnaire Python (context) qui fournit quelques paramètres d'environnement ::

     g=Generator(m,context)

Les paramètres indispensables pour context sont :

    - "prerequisites" : indique le chemin d'un shell script qui positionne les variables d'environnement
      des prérequis de SALOME
    - "kernel" : indique le chemin d'installation du module KERNEL de SALOME
    - "update" : mettre à 1 pour le moment (futurs développements)

Exemple de création de générateur ::

     context={'update':1,
              "prerequisites":"/local/cchris/.packages.d/envSalome40",
              "kernel":"/local/chris/SALOME2/RELEASES/Install/KERNEL_V4_0"
              }
     g=Generator(m,context)

Une fois ce générateur créé, il suffit d'appeler ses commandes pour réaliser les opérations nécessaires.

    - génération du module SALOME : ``g.generate()``
    - initialisation d'automake : ``g.bootstrap()``
    - exécution du script configure : ``g.configure()``
    - compilation : ``g.make()``
    - installation dans le répertoire <prefix> : ``g.install()``
    - création d'une application SALOME dans le répertoire appli_dir ::

        g.make_appli(appli_dir,restrict=<liste de modules>,
                               altmodules=<dictionnaire de modules>)

Ces commandes ne prennent pas de paramètres sauf make_appli qui en prend 3 :

    - **appliname** : le nom du répertoire qui contiendra l'application SALOME
    - **restrict** : une liste de noms de modules SALOME à mettre dans l'application. Par défaut, make_appli
      met dans l'application tous les modules SALOME qu'il est capable de détecter (répertoire voisins de
      KERNEL avec le même suffixe que KERNEL. Si le répertoire du module KERNEL s'appelle KERNEL_V41, il
      retiendra GUI_V41, GEOM_V41, etc.). Si restrict est fourni, make_appli ne retiendra que les modules listés.
    - **altmodules** : un dictionnaire de modules autres. La clé donne le nom du module. La valeur correspondante
      donne le chemin du répertoire d'installation du module. Exemple : ``altmodules={"monmodule":"/local/chris/unmodule"}``


Fabrication du module SALOME
-----------------------------------------------------
Le module sera fabriqué en exécutant un fichier Python qui contient sa description, la mise en données du 
générateur et les commandes du générateur.

Ce qui donne quelque chose comme ce qui suit pour un module avec un seul composant Fortran::

  from module_generator import Generator,Module
  from module_generator import PYComponent,CPPComponent,Service,F77Component

  context={"update":1,
                   "prerequisites":"/local/cchris/.packages.d/envSalome40",
                   "kernel":"/local/chris/SALOME2/RELEASES/Install/KERNEL_V4_0"
                 }
  c3=F77Component("compo",
                  services=[
                            Service("s1",
                                    inport=[("a","double"),("b","long"),("c","string")],
                                    outport=[("d","double"),("e","long"),("f","string")],
                                    instream=[("a","CALCIUM_double","T"),
                                              ("b","CALCIUM_double","I")],
                                    outstream=[("ba","CALCIUM_double","T"),
                                               ("bb","CALCIUM_double","I")],
                                    defs="#include <unistd.h>",
                                    body="chdir(c);"
                                   ),
                           ],
                  libs="-L/local/chris/modulegen/YACSGEN/fcompo -lfcompo"
                  rlibs="-Wl,--rpath -Wl,/local/chris/modulegen/YACSGEN/fcompo")

  m=Module("toto",components=[c1],prefix="Install")
  g=Generator(m,context)
  g.generate()
  g.bootstrap()
  g.configure()
  g.make()
  g.install()
  g.make_appli("appli",restrict=["KERNEL","GUI","YACS"])

Si cette description est dans le fichier monmodule.py, il suffit d'exécuter::

    python monmodule.py

ce qui a pour effet de créer le répertoire source du module (toto_SRC), le répertoire d'installation du module (Instal)
et un répertoire d'application SALOME (appli).

Il faut bien sûr que le module module_generator.py puisse être importé soit en étant dans le répertoire courant soit en étant
dans le PYTHONPATH.

Il est toujours préférable (bien que non indispensable) de faire le ménage dans le répertoire de travail avant d'exécuter
le générateur.

Mise en oeuvre du composant dans un couplage
-----------------------------------------------------------------------------------------
Creation du fichier de couplage YACS
++++++++++++++++++++++++++++++++++++++++
Un fichier de couplage YACS est un fichier XML qui décrit la façon dont des composants SALOME préalablement
installés dans une application SALOME sont couplés et exécutés.

Pour une documentation sur la façon d'écrire un fichier XML YACS, voir :ref:`schemaxml`.

Voici un exemple de fichier YACS mettant en oeuvre le composant Fortran défini ci-dessus ::

  <proc>
  <container name="A"> </container>
  <container name="B"> </container>

  <service name="pipo1" >
    <component>compo</component>
    <method>s1</method>
    <load container="A"/>
    <inport name="a" type="double"/>
    <inport name="b" type="int"/>
    <inport name="c" type="string"/>
    <outport name="d" type="double"/>
    <outport name="e" type="int"/>
    <outport name="f" type="string"/>
    <instream name="a" type="CALCIUM_double"/>
    <instream name="b" type="CALCIUM_double"/>
    <outstream name="ba" type="CALCIUM_double"/>
    <outstream name="bb" type="CALCIUM_double"/>
  </service>
  <service name="pipo2" >
    <component>compo</component>
    <method>s1</method>
    <load container="B"/>
    <inport name="a" type="double"/>
    <inport name="b" type="int"/>
    <inport name="c" type="string"/>
    <outport name="d" type="double"/>
    <outport name="e" type="int"/>
    <outport name="f" type="string"/>
    <instream name="a" type="CALCIUM_double"/>
    <instream name="b" type="CALCIUM_double"/>
    <outstream name="ba" type="CALCIUM_double"/>
    <outstream name="bb" type="CALCIUM_double"/>
  </service>

  <stream>
    <fromnode>pipo1</fromnode><fromport>ba</fromport>
    <tonode>pipo2</tonode><toport>a</toport>
  </stream>
  <stream>
    <fromnode>pipo1</fromnode><fromport>bb</fromport>
    <tonode>pipo2</tonode><toport>b</toport>
  </stream>
  <stream>
    <fromnode>pipo2</fromnode><fromport>ba</fromport>
    <tonode>pipo1</tonode><toport>a</toport>
  </stream>
  <stream>
    <fromnode>pipo2</fromnode><fromport>bb</fromport>
    <tonode>pipo1</tonode><toport>b</toport>
  </stream>
  <parameter>
    <tonode>pipo1</tonode> <toport>a</toport>
    <value><double>23</double> </value>
  </parameter>
  <parameter>
    <tonode>pipo1</tonode> <toport>b</toport>
    <value><int>23</int> </value>
  </parameter>
  <parameter>
    <tonode>pipo1</tonode> <toport>c</toport>
    <value><string>/local/cchris/SALOME2/SUPERV/YACS/modulegen/data1</string> </value>
  </parameter>
  <parameter>
    <tonode>pipo2</tonode> <toport>a</toport>
    <value><double>23</double> </value>
  </parameter>
  <parameter>
    <tonode>pipo2</tonode> <toport>b</toport>
    <value><int>23</int> </value>
  </parameter>
  <parameter>
    <tonode>pipo2</tonode> <toport>c</toport>
    <value><string>/local/cchris/SALOME2/SUPERV/YACS/modulegen/data2</string> </value>
  </parameter>

  </proc>

Dans les grandes lignes, le couplage fait intervenir 2 instances du composant compo (pipo1 et pipo2) dont on exécute
le service s1. Les ports datastream de ces services sont connectés au moyen des informations fromnode, fromport, tonode, toport
dans les sections stream.
Les ports dataflow sont initialisés par les sections parameter. En particulier, le répertoire de travail de chaque instance
de composant est initialisé à travers le port d'entrée "c" de chaque instance de composant.
Chaque instance de composant est exécuté dans un container différent (A et B). Ces noms sont virtuels. C'est SALOME qui au
moment du lancement décidera du nom effectif des containers. On ne donne ici que des contraintes sur les containers à utiliser.
En l'occurrence, il n'y en a qu'une : containers différents.

Exécution du couplage
+++++++++++++++++++++++++++++++++++++++++++++
Une fois le fichier de couplage écrit au moyen d'un éditeur classique ou de l'éditeur graphique YACS, il est possible
de lancer l'exécution.

Elle se passe en plusieurs temps :

  - le lancement de SALOME : exécution du script runAppli de l'application SALOME (``./appli/runAppli -t``). L'application tourne
    en tâche de fond jusqu'à ce qu'elle soit arrêtée.
  - le lancement du couplage : exécution du coupleur YACS dans l'environnement de l'application SALOME
    lancée (``./appli/runSession driver test.xml``) avec test.xml le fichier de couplage.
  - l'arrêt de l'application : ``./appli/runSession killSalome.py``

Les sorties du couplage sont multiples :

  - la sortie du coupleur lui-même. Si aucune erreur d'exécution ne remonte jusqu'à lui, elle ne contient qu'une information
    utile : le nom des containers lancé par SALOME pour exécuter les composants. Si des erreurs d'exécution sont remontées
    jusqu'au coupleur elles sont listées en fin d'exécution.
  - les sorties des containers. Elles se retrouvent dans le répertoire /tmp avec un nom construit sur la base du nom du container
    lu dans la sortie du coupleur.

Attention : lors de l'arrêt de l'application les containers sont arrêtés brutalement ce qui peut provoquer des pertes d'informations 
dans leurs fichiers de sortie.

La question du répertoire de travail
++++++++++++++++++++++++++++++++++++++
Chaque instance de composant est hébergée par un container. Toutes les instances hébergées par un container s'exécutent donc 
dans un même répertoire qui est celui du container. A partir de la version 4.1.1 de SALOME il est possible de spécifier le répertoire
de travail d'un container dans le fichier de couplage. Il suffit d'ajouter la propriété *workingdir* au container. Voici quelques
exemples::

   <container name="A">
     <property name="workingdir" value="/home/user/w1"/>
   </container>
   <container name="B">
     <property name="workingdir" value="$TEMPDIR"/>
   </container>
   <container name="C">
     <property name="workingdir" value="a/b"/>
   </container>

Le container A s'exécutera dans le répertoire "/home/user/w1". S'il n'existe pas il sera créé.
Le container B s'exécutera dans un nouveau répertoire temporaire.
Le container C s'exécutera dans le répertoire relatif "a/b" (par rapport au répertoire de l'application utilisée pour l'exécution). 
S'il n'existe pas il sera créé.

La question des fichiers
++++++++++++++++++++++++++++
Les composants sont des bibliothèques dynamiques ou des modules Python, il n'est pas possible de les lancer dans des scripts shell.
Pour les composants qui utilisent des fichiers en entrée et en sortie, il est possible de spécifier dans le fichier de couplage
des ports "fichiers" qui effectueront le transfert des fichiers et le nommage local adéquat.
Par exemple, un service qui utilise un fichier d'entrée a et produit un fichier de sortie b sera déclaré comme suit::

    <service name="pipo1">
      <component>caster</component>
      <method>s1</method>
      <inport name="a" type="file"/>
      <outport name="b" type="file"/>
    </service>

Ces ports pourront être initialisés ou connectés à d'autres ports "fichiers" comme des ports ordinaires.
Par exemple, l'initialisation pour le fichier d'entrée prendra la forme suivante::

    <parameter>
      <tonode>pipo1</tonode> <toport>a</toport>
      <value><objref>/local/chris/tmp/unfichier</objref> </value>
    </parameter>

Il n'est pas possible d'initialiser directement un port fichier de sortie. Il faut passer par un noeud spécial qui
collecte les sorties. On créera un noeud "dataout" et un lien entre le noeud "pipo1" et le noeud "dataout"::

    <outnode name="dataout" >
      <parameter name="f1" type="file" ref="monfichier"/>
    </outnode>
    <datalink>
       <fromnode>pipo1</fromnode><fromport>b</fromport>
       <tonode>dataout</tonode> <toport>f1</toport>
    </datalink>

ATTENTION: il n'est pas possible d'utiliser le caractère '.' dans les noms des ports. Ceci interdit l'utilisation de noms
tels que fort.8 qui sont assez fréquents. Un contournement existe : il suffit de remplacer le '.' par le caractère ':' (donc
fort:8 dans notre exemple) pour obtenir le résultat attendu. Bien entendu, les noms contenant des caractères ':' ne sont pas 
utilisables. Ils devraient être très rares.

Exemple d'exécution de composant Aster
+++++++++++++++++++++++++++++++++++++++++++
L'exécution d'un composant Aster présente quelques particularités qui sont exposées ici.

         - prise en charge du jeu de commande
         - spécification des paramètres de la ligne de commande
         - spécification d'un fichier maillage (.mail)
         - spécification de variables d'environnement (également valable pour les autres types de composant)

Voici un exemple simplifié de schéma YACS comportant un noeud de calcul devant exécuter le service s1 du 
composant caster (de type Aster) avec une variable d'environnement, un fichier mail un fichier comm
et des paramètres de la ligne de commande. Pour un exemple plus complet, voir les fichiers aster.xml et f.comm 
de la distribution::

    <service name="pipo1" >
      <component>caster</component>
      <property name="MYENVAR" value="25"/>
      <method>s1</method>
      <load container="A"/>
      <inport name="jdc" type="string"/>
      <inport name="argv" type="string"/>
      <inport name="a" type="double"/>
      <inport name="fort:20" type="file"/>
      <outport name="d" type="double"/>
      <instream name="aa" type="CALCIUM_double"/>
      <outstream name="ba" type="CALCIUM_double"/>
    </service>

    <inline name="ljdc" >
       <script>
       <code>f=open(comm)</code>
       <code>jdc=f.read()</code>
       <code>f.close()</code>
       </script>
       <inport name="comm" type="string"/>
       <outport name="jdc" type="string"/>
    </inline>

    <parameter>
      <tonode>ljdc</tonode> <toport>comm</toport>
      <value><string>/home/chris/jdc.comm</string> </value>
    </parameter>

    <datalink>
       <fromnode>ljdc</fromnode><fromport>jdc</fromport>
       <tonode>pipo1</tonode> <toport>jdc</toport>
    </datalink>

    <parameter>
      <tonode>pipo1</tonode> <toport>argv</toport>
      <value><string>-rep_outils /aster/outils</string> </value>
    </parameter>

    <parameter>
       <tonode>pipo1</tonode> <toport>fort:20</toport>
       <value><objref>/local/chris/ASTER/instals/NEW9/astest/forma01a.mmed</objref> </value>
    </parameter>

Tout d'abord, il faut spécifier le jeu de commande. Comme indiqué ci-dessus (`Composant Aster`_), il faut 
déclarer un port supplémentaire "jdc" de type "string" et l'initialiser ou le connecter. Ici, le port jdc est connecté à
un port de sortie d'un noeud python (ljdc) qui lira le fichier .comm dont le chemin lui est donné par
son port d'entrée comm. Le transfert de l'identificateur du composant à la commande YACS_INIT est réalisé au moyen
de la variable "component" qui est ajoutée automatiquement par le générateur et est disponible
pour écrire le fichier .comm.

Exemple succinct de .comm ::

   DEBUT(PAR_LOT="NON")
   YACS_INIT(COMPO=component)
   ECRIRE_MAILLAGE()
   LECTURE_FORCE()

Pour spécifier des valeurs de paramètres de la ligne de commande, il faut avoir créer un composant
avec un port de nom "argv" de type "string". Il suffit alors de donner une valeur à ce port. Ici, on modifie
le chemin du répertoire des outils avec le paramètre rep_outils.

Pour spécifier un fichier de maillage (.mail) à un composant Aster, il faut ajouter un port fichier au noeud de
calcul::

      <inport name="fort:20" type="file"/>

Ce port fichier doit avoir comme nom le nom local du fichier tel qu'attendu par Aster. En général Aster utilise
le fichier fort.20 comme entrée de LIRE_MAILLAGE. Comme indiqué plus haut, le point de fort.20 ne peut pas être
utilisé dans un nom de port, on donnera donc comme nom fort:20. Il faut ensuite donner une valeur à ce port
qui correspond au chemin du fichier à utiliser. Ceci est réalisé par une directive parameter::

    <parameter>
       <tonode>pipo1</tonode> <toport>fort:20</toport>
       <value><objref>/local/chris/ASTER/instals/NEW9/astest/forma01a.mmed</objref> </value>
    </parameter>

Pour spécifier des variables d'environnement, on passe par les properties du noeud de calcul. Ici, on
définit la variable d'environnement MYENVAR de valeur 25.

Composants standalone
--------------------------------------------------
Jusqu'à la version 4.1 de Salome, la seule méthode pour intégrer un composant était de produire
une bibliothèque dynamique (\*.so) ou un module python (\*.py).  Ce composant est chargé par un
exécutable Salome nommé Container soit par dlopen dans le cas de la bibliothèque soit par 
import dans le cas du module python. Cette méthode est un peu contraignante pour les codes de
calcul comme Code_Aster ou Code_Saturne qui sont exécutés dans un environnement particulier
de préférence à partir d'un shell script.

A partir de la version 4.1.3, il est possible d'intégrer un composant en tant qu'exécutable ou shell
script. Cette nouvelle fonctionnalité est pour le moment expérimentale et demande à être testée
plus complètement. Elle est cependant utilisable et module_generator a été adapté (à partir de 
la version 0.3) pour générer des composants standalone. On décrit ci-après les opérations à réaliser
pour passer au mode standalone pour chaque type de composant (C/C++, Python, Fortran ou Aster).

Composant C/C++
++++++++++++++++++++++++++++++++++++++++
Pour transformer un composant C/C++ qui se présente de façon standard sous forme d'une
bibliothèque dynamique en composant standalone, il suffit d'ajouter deux attributs à sa 
description :

  - l'attribut **kind** : en lui donnant la valeur "exe"
  - l'attribut **exe_path** : en lui donnant comme valeur le chemin de l'exécutable ou du script shell
    qui sera utilisé au lancement du composant

Voici un exemple de composant C++ modifié pour en faire un composant standalone::

      c1=CPPComponent("compo1",services=[
                      Service("monservice",inport=[("portentrant","double"),],
                               outport=[("portsortant","double")],
                             ),
                            ],
         kind="exe",
         exe_path="/local/chris/SALOME2/SUPERV/YACS/modulegen/execpp_essai/prog",
                     )

Le chemin donné pour exe_path correspond à un exécutable dont le source est le suivant::

   #include "compo1.hxx"

   int main(int argc, char* argv[])
   {
     yacsinit();
     return 0;
   }

Il doit être compilé et linké en utilisant l'include compo1.hxx et la librairie libcompo1Exelib.so
que l'on trouvera dans l'installation du module généré respectivement dans include/salome
et dans lib/salome. On pourra consulter un exemple plus complet dans les sources de la distribution
dans le répertoire cppcompo.

Il est possible de remplacer l'exécutable par un script shell intermédiaire mais il est bon de savoir que l'appel
à yacsinit récupère dans 3 variables d'environnement ( *SALOME_CONTAINERNAME*, *SALOME_INSTANCE*,
*SALOME_CONTAINER*), les informations nécessaires à l'initialisation du composant.

Composant Fortran
++++++++++++++++++++++++++++++++++++++++
Pour un composant Fortran, la méthode est identique. On ajoute les deux mêmes attributs :

  - l'attribut **kind** : en lui donnant la valeur "exe"
  - l'attribut **exe_path** : en lui donnant comme valeur le chemin de l'exécutable ou du script shell
    qui sera utilisé au lancement du composant

Voici un exemple de composant Fortran standalone::

     c3=F77Component("compo3",services=[
          Service("s1",inport=[("a","double"),("b","long"),
                               ("c","string")],
                       outport=[("d","double"),("e","long"),
                                ("f","string")],
                       instream=[("a","CALCIUM_double","T"),
                                 ("b","CALCIUM_double","I")],
                       outstream=[("ba","CALCIUM_double","T"),
                                  ("bb","CALCIUM_double","I")],
                             ),
                             ],
         kind="exe",
         exe_path="/local/chris/SALOME2/SUPERV/YACS/modulegen/YACSGEN/fcompo/prog",
                                     )

Le chemin donné pour exe_path correspond à un exécutable dont le source est le suivant::

       PROGRAM P
       CALL YACSINIT()
       END

Il doit être compilé et linké en utilisant la librairie libcompo3Exelib.so que l'on trouvera dans l'installation 
du module généré dans lib/salome ainsi qu'avec le source Fortran contenant la subroutine S1. 
On pourra consulter un exemple plus complet dans les sources de la distribution
dans le répertoire fcompo.

Composant Python
++++++++++++++++++++++++++++++++++++++++
Pour un composant Python, un générateur très rudimentaire a été codé. Il n'est possible que
d'ajouter l'attribut **kind** avec la valeur "exe". L'exécutable est automatiquement généré dans l'installation 
du module. Il n'est pas possible, sauf à modifier l'installation, de le remplacer par un script.

Composant Aster standalone
++++++++++++++++++++++++++++++++++++++++
Pour un composant Aster, il faut un peu plus de travail. Il faut spécifier 4 attributs :

  - l'attribut **aster_dir** : qui donne le chemin de l'installation de Code_Aster
  - l'attribut **kind** : avec la valeur "exe"
  - l'attribut **asrun** : qui donne le chemin d'accès au lanceur as_run
  - l'attribut **exe_path** : qui donne le chemin d'un REPERTOIRE dans lequel le générateur va
    produire plusieurs fichiers qui serviront au lancement de l'exécution de Code_Aster.

Voici un exemple de description d'un composant Aster standalone::

      c1=ASTERComponent("caster",services=[
                  Service("s1",inport=[("argv","string"),("a","double"),
                                       ("b","long"),("c","string")],
                               outport=[("d","double")],
                               instream=[("aa","CALCIUM_double","T"),
                                         ("ab","CALCIUM_double","I"),
                                         ("ac","CALCIUM_integer","I"),
                                         ("ad","CALCIUM_real","I"),
                                         ("ae","CALCIUM_string","I"),
                                         ("af","CALCIUM_complex","I"),
                                         ("ag","CALCIUM_logical","I"),
                                       ],
                               outstream=[("ba","CALCIUM_double","T"),
                                          ("bb","CALCIUM_double","I")],
                 ),
         ],
         aster_dir="/aster/NEW9",
         exe_path="/home/pora/CCAR/SALOME4/exeaster_essai",
         asrun="/aster/ASTK/ASTK_SERV/bin/as_run",
         kind="exe",
         )

Le générateur produit les fichiers suivants, dans le répertoire **exe_path** :

  - **aster_component.py** : qui est l'exécutable python qui remplace l'exécutable standard 
    E_SUPERV.py. Il n'a pas à être modifié.
  - **E_SUPERV.py** : une modification du fichier original contenu dans 
    ``bibpyt/Execution``. Il n'a pas à être modifié.
  - **config.txt** : le fichier config.txt de l'installation de Code_Aster modifié pour changer 
    l'exécutable python (ARGPYT). Il peut être modifié en dehors de ARGPYT.
  - **profile.sh** : une copie du fichier profile.sh de l'installation de Code_Aster (pour que çà marche).
  - **caster.comm** : un fichier de commande d'amorçage qui ne contient que la commande DEBUT 
    en mode PAR_LOT="NON". Il n'a pas à être modifié.
  - **make_etude.export** : un fichier de commande pour as_run simplifié. Il est complété 
    dynamiquement au lancement pour rediriger les fichiers 6,8 et 9 dans REP/messages, REP/resu 
    et REP/erre. REP est le répertoire d'exécution du composant standalone qui a pour 
    nom : <composant>_inst_<N>. <N> est un numéro d'exécution qui démarre à 1. 
    <composant> est le nom du composant (caster, dans notre exemple). Ce fichier peut être 
    modifié en particulier si on a modifié ou ajouté des commandes Aster.   

Bien que l'exécution soit lancée avec un fichier de commandes (caster.comm), il est toujours 
nécessaire de spécifier le fichier de commandes "effectif" dans le fichier de couplage xml. 
La seule différence avec un composant sous forme de bibliothèque est que ce dernier fichier de 
commandes NE DOIT PAS contenir de commande DEBUT (sinon, plantage inexplicable).
 
Exemple de couplage avec composants standalone
++++++++++++++++++++++++++++++++++++++++++++++++++++
En rassemblant tous les éléments ci-dessus, le couplage d'un composant Aster standalone avec un composant
Fortran standalone s'écrit comme suit (fichier astexe.py dans la distribution) ::

  from module_generator import Generator,Module
  from module_generator import ASTERComponent,Service,F77Component

  context={'update':1,"prerequisites":"/home/caremoli/pkg/env.sh",
          "kernel":"/home/pora/CCAR/SALOME4/Install/KERNEL_V4_1"}

  install_prefix="./exe_install"
  appli_dir="exe_appli"

  c1=ASTERComponent("caster",services=[
          Service("s1",inport=[("a","double"),("b","long"),
                               ("c","string")],
                       outport=[("d","double")],
                   instream=[("aa","CALCIUM_double","T"),
                             ("ab","CALCIUM_double","I"),
                             ("ac","CALCIUM_integer","I"),
                             ("ad","CALCIUM_real","I"),
                             ("ae","CALCIUM_string","I"),
                             ("af","CALCIUM_complex","I"),
                             ("ag","CALCIUM_logical","I"),
                         ],
                   outstream=[("ba","CALCIUM_double","T"),
                              ("bb","CALCIUM_double","I")],
                 ),
         ],
         aster_dir="/aster/NEW9",
         exe_path="/home/pora/CCAR/SALOME4/exeaster_essai",
         asrun="/aster/ASTK/ASTK_SERV/bin/as_run",
         kind="exe",
         )

  c2=F77Component("cedyos",services=[
          Service("s1",inport=[("a","double"),("b","long"),
                               ("c","string")],
                       outport=[("d","double"),("e","long"),
                                ("f","string")],
                  instream=[("a","CALCIUM_double","T"),
                            ("b","CALCIUM_double","I")],
                  outstream=[("ba","CALCIUM_double","T"),
                             ("bb","CALCIUM_double","I"),
                             ("bc","CALCIUM_integer","I"),
                             ("bd","CALCIUM_real","I"),
                             ("be","CALCIUM_string","I"),
                             ("bf","CALCIUM_complex","I"),
                             ("bg","CALCIUM_logical","I"),
                         ],
                       defs="",body="",
                 ),
         ],
           exe_path="/home/pora/CCAR/SALOME4/exeedyos_essai/prog",
           kind="exe")

  g=Generator(Module("titi",components=[c1,c2],prefix=install_prefix),context)
  g.generate()
  g.bootstrap()
  g.configure()
  g.make()
  g.install()
  g.make_appli(appli_dir,restrict=["KERNEL","YACS"])

Le fichier de couplage xml et le fichier de commandes Aster correspondants peuvent être consultés
dans la distribution (asterexe.xml et fexe.xml).
On trouvera les éléments complémentaires d'implantation dans le répertoire fcompo (composant cedyos)
et dans le répertoire aster (composant caster).

