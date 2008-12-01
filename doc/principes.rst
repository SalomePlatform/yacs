
:tocdepth: 3

.. _principes:

Principes généraux de YACS
===============================
La construction d'un schéma de calcul s'appuie sur le concept de noeud de calcul.
Un noeud de calcul représente un calcul élémentaire qui peut être l'exécution
locale d'un script Python ou l'exécution distante d'un service de composant SALOME.

Le schéma de calcul est un assemblage de noeuds de calcul plus ou moins complexe.

Cet assemblage est réalisé par connexion de ports d'entrée et de sortie de ces noeuds
de calcul.

Les données sont échangées entre noeuds via les ports. Elles sont typées.

Les noeuds composés : Bloc, Loop, Switch, permettent de modulariser un schéma de calcul 
et de définir des processus itératifs, des calculs paramétriques ou des débranchements.

Enfin les containers permettent de définir où seront exécutés les composants SALOME 
(sur un réseau ou un cluster).

.. _datatypes:

Les types de données
----------------------
Les données échangées entre noeuds de calcul via les ports sont typées. 
On a 4 catégories de types : les types de base, les références d'objets, les séquences et les
structures.
Il est possible de définir des types utilisateurs par composition de ces éléments de base.
De nombreux types sont prédéfinis soit par YACS soit par les composants utilisés comme GEOM ou SMESH.

Les types de base
'''''''''''''''''''''
Les types de base sont au nombre de 5 : int, double, bool, string et file. Ils sont prédéfinis par YACS.

================= =====================================
Type YACS          Commentaire
================= =====================================
int                 pour les entiers
double              pour les réels doubles
bool                pour les booléens
string              pour les chaines de caractères
file                pour les fichiers
================= =====================================

Les références d'objet
''''''''''''''''''''''''''
Les références d'objet sont utilisées pour typer les objets CORBA gérés par les composants SALOME.
En général, ces types sont définis par les composants qui les utilisent.
Pour définir un type YACS référence d'objet, il suffit de lui donner un nom et de l'associer
au Repository ID CORBA. Voici quelques exemples de types prédéfinis.

================= ==============================
Type YACS          Repository ID CORBA
================= ==============================
Study               IDL:SALOMEDS/Study:1.0
SObject             IDL:SALOMEDS/SObject:1.0
GEOM_Shape          IDL:GEOM/GEOM_Object:1.0
MESH                IDL:SALOME_MED/MESH:1.0
FIELD               IDL:SALOME_MED/FIELD:1.0
================= ==============================

Il est possible de définir des relations d'héritage simple ou multiple entre ces types.
Voici un exemple extrait du composant MED.

================= ============================== =====================================
Type YACS          Types de base                     Repository ID CORBA
================= ============================== =====================================
FIELDDOUBLE         FIELD                           IDL:SALOME_MED/FIELDDOUBLE:1.0
================= ============================== =====================================

YACS définit également des types pour les ports datastream :

================= =======================================================
Type YACS                Repository ID CORBA
================= =======================================================
CALCIUM_integer    IDL:Ports/Calcium_Ports/Calcium_Integer_Port:1.0
CALCIUM_real       IDL:Ports/Calcium_Ports/Calcium_Real_Port:1.0
CALCIUM_double     IDL:Ports/Calcium_Ports/Calcium_Double_Port:1.0
CALCIUM_string     IDL:Ports/Calcium_Ports/Calcium_String_Port:1.0
CALCIUM_logical    IDL:Ports/Calcium_Ports/Calcium_Logical_Port:1.0
CALCIUM_boolean    IDL:Ports/Calcium_Ports/Calcium_Logical_Port:1.0
CALCIUM_complex    IDL:Ports/Calcium_Ports/Calcium_Complex_Port:1.0
================= =======================================================

On peut avoir la liste des types disponsibles en consultant les catalogues des composants
SALOME : GEOMCatalog.xml, SMESHCatalog.xml, MEDCatalog.xml, etc. Ces catalogues sont au format XML
qui est décrit ici : :ref:`schemaxml`. On peut également en avoir une vue dans l'IHM graphique en 
consultant le catalogue de composants.

Les séquences
'''''''''''''''
Un type séquence sert à typer une liste d'éléments homogènes. Le type contenu dans la liste est le même
pour tous les éléments.
Pour définir un type séquence, on lui donne un nom et on spécifie le type des éléments de la séquence.

Le KERNEL de SALOME définit des types séquences pour les types de base.

================= ==============================
Type YACS          Type des éléments
================= ==============================
dblevec               double
intvec                int
stringvec             string
boolvec               bool
================= ==============================

Il est possible de définir un type séquence de séquence. Dans ce cas, le type de l'élément est un type séquence.

Les structures
''''''''''''''''
Le type structure sert à typer une donnée structurée comme un struct C. Cette donnée contient des membres nommés
et typés.
Pour définir un type structure, on lui donne un nom et on spécifie la liste des membres (nom, type).

Le composant GEOM définit un type structure de nom "BCError" comportant un seul membre de nom "incriminated" et
de type "ListOfLong". "ListOfLong" est lui-même une séquence d'"int".

Les ports
-------------
Un port peut être considéré comme une interface d'un noeud avec l'extérieur.
Il existe trois types de port : les ports de contrôle, les ports de données et les ports datastream.
Chacun a une sémantique différente.

Les ports de contrôle
''''''''''''''''''''''''
Ce type de port est utilisé pour mettre des contraintes sur des enchainements d'exécution de noeuds.
Un noeud élémentaire ou composé dispose, en général, d'un port de contrôle entrant et d'un port
sortant. 
Un noeud qui a son port de contrôle d'entrée connecté au port de sortie d'un autre noeud ne 
sera exécuté que lorsque ce deuxième noeud sera terminé.

Les ports de données
''''''''''''''''''''''''
Ce type de port est utilisé pour définir les données qui seront utilisées par le noeud lors de son exécution
et les données qui seront produites par le noeud à la fin de son exécution.
Un port de données a un nom, un sens (input, output) et les données qu'il portent ont un type.
L'ordre de définition des ports est important car il est utilisé comme ordre des arguments lors de l'appel
des services de composants SALOME.

De façon générale, l'exécution d'un noeud de calcul élémentaire se passe comme suit :
   - le contrôle est donné au noeud via le port de contrôle d'entrée
   - les données d'entrée sont acquises par le noeud via les ports de données d'entrée
   - le noeud s'exécute
   - les données de sortie sont fournies aux ports de sortie
   - le contrôle est rendu par le noeud via le port de contrôle de sortie

Un port de données d'entrée peut être initialisé ou connecté à un port de données de sortie.

Les ports datastream
''''''''''''''''''''''''
Ce type de port est utilisé pour l'échange de données pendant l'exécution. Tous les noeuds élémentaires ne supportent
pas ce type de port. Pour le moment seuls les noeuds liés à des composants SALOME supportent ce type de port.
Un port datastream a un nom, un sens (input, output) et un type.
Ce type n'est pas directement le type d'une donnée mais plutôt celui d'un objet CORBA qui gère l'échange de 
données (voir ports DSC pour plus d'informations).

Les noeuds de calcul élémentaires
-------------------------------------
Un noeud de calcul élémentaire représente une fonction de calcul particulière (multiplication de 2 matrices, par exemple). 
Tout noeud de calcul a un type. 
On peut avoir un type de noeud qui exécute un service d'un composant Salome et un autre 
type de noeud qui exécute un bout de script Python. 
Les noeuds de calcul élémentaires se répartissent en deux catégories : les noeuds inlines
qui s'exécutent en local dans le coupleur YACS et les noeuds de service qui s'exécutent à 
distance et correspondent à la mise en oeuvre de composants SALOME.

Tout noeud de calcul a un nom qui sert d'identificateur. Cet identificateur doit etre unique dans son contexte de définition. 
Un schéma de calcul ou un noeud composé définit un contexte.

Un noeud de calcul a un port de contrôle d'entrée et un port de contrôle de sortie. 
Ces ports de contrôle sont connectés à travers le flot de contrôle.

Un noeud de calcul a, en général, des ports de données d'entrée et de sortie. 
Le nombre et le type des ports de données est déterminé par le type de noeud de calcul. 
Ces ports de données sont connectés à travers le flot de données.

Un noeud de calcul peut avoir des propriétés. Une propriété est un couple (nom, valeur)
où nom est le nom de la propriété et valeur une chaine de caractère qui donne sa valeur.

Noeud inline script Python
''''''''''''''''''''''''''''''
Un noeud script Python exécute du code Python dans un contexte où sont présentes des variables
qui ont pour valeur le contenu des ports de données d'entrée au lancement de cette exécution.
Par exemple, si le noeud a un port de données d'entrée de nom "matrice", la variable "matrice" sera
présente dans le contexte d'exécution du script et aura pour valeur le contenu du port de même nom.
En fin d'exécution du script, les ports de données de sortie sont remplis avec les valeurs des variables
de même nom présentes dans le contexte d'exécution. Ces variables doivent être obligatoirement présentes.

Lorsque ce type de noeud est un noeud interne d'une boucle, le contexte d'exécution est réinitialisé
à chaque tour de boucle.

Noeud inline fonction Python
''''''''''''''''''''''''''''''
Un noeud fonction Python exécute une fonction Python dont les arguments correspondent aux ports de données
d'entrée du noeud. Le nom de la fonction à exécuter est donné par un paramètre du noeud.
Si un tel noeud a 3 ports de données d'entrée de nom 'a', 'b', 'c' et que le nom de la fonction est 'f', l'exécution
du noeud correspondra à l'appel de f(a,b,c) où a, b et c sont les valeurs des ports de données de même nom.

Les données de sortie du noeud sont attendues comme un retour de la fonction sous la forme d'un tuple Python.
Par exemple, si on a 3 ports de données de sortie de nom 'x', 'y', 'z', la fonction devra se terminer
par "return x,y,z" où x,y et z sont les valeurs pour les ports de sortie de même nom.

Lorsque ce type de noeud est un noeud interne d'une boucle, le contexte d'exécution est conservé
à chaque tour de boucle ce qui permet de réutiliser des variables pendant les itérations.

Noeud de service SALOME
''''''''''''''''''''''''''''''
Un noeud de service SALOME exécute un service d'un composant SALOME. 
On peut définir un noeud de service de deux façons :

  1. en indiquant le type de composant (GEOM, SMESH, ...) et le service à exécuter
  2. en indiquant un noeud de service existant et le service à exécuter

La deuxième forme existe car, dans certains cas, on veut utiliser l'état du composant à la
fin de l'exécution du premier service pour exécuter le deuxième service. L'état du composant
est conservé dans une instance de composant qui est créée à chaque fois qu'on utilise la
première forme. Si on utilise la deuxième forme, on ne crée pas une nouvelle instance de
composant mais on réutilise l'instance existante.

Un noeud de service a des ports de données d'entrée et de sortie et peut avoir également
des ports datastream d'entrée et de sortie.

Un noeud de service est chargé et exécuté sur un container SALOME. Ce placement est géré
au moyen du concept de container YACS (voir `Les containers`_) qui est une légère abstraction 
du container SALOME.
Pour gérer le placement du service SALOME, il est possible de désigner par son nom le container
YACS sur lequel on veut qu'il soit placé. Ceci n'est possible qu'avec la première forme de définition
du noeud. Si aucune information de placement n'est donnée, le service sera placé sur 
le container par défaut de la plate-forme SALOME : container FactoryServer sur la machine locale.

Les propriétés d'un noeud de service SALOME sont converties en variable d'environnement 
lors de l'exécution du service.

Noeud SalomePython
''''''''''''''''''''''''''''''
Un noeud SalomePython est un noeud fonction Python à qui YACS fournit (dans le contexte
d'exécution Python) les informations nécessaires pour lancer des composants SALOME et exécuter leurs services.
Il s'agit de l'adresse du container sur lequel il faut charger et exécuter le composant. Cette adresse
est donnée dans la variable "__container__from__YACS__" qui a la forme <nom machine>/<nom container>.
Ce noeud est donc paramétrable avec des informations de placement sur un container comme un noeud
de service SALOME.

Restriction : ce type de noeud ne peut pas exécuter de service SALOME doté de ports datastream. Le noeud
est vu par YACS comme un noeud Python. Les ports datastream ne sont pas gérés.

Noeuds Data
''''''''''''''''''''''''''''''
Un noeud Data sert à définir des données (noeud DataIn) ou à collecter des résultats (noeud DataOut)
d'un schéma de calcul.

Noeud DataIn
++++++++++++++++++
Un noeud DataIn a uniquement des ports de données sortants qui servent à définir les données d'entrée
du schéma de calcul. 
Ces données ont un nom (le nom du port), un type (le type du port) et une valeur initiale.

Noeud DataOut
++++++++++++++++++
Un noeud DataOut a uniquement des ports de données entrants qui servent pour stocker les résultats en sortie
du schéma de calcul.
Ces résultats ont un nom (le nom du port) et un type (le type du port).
Si le résultat est un fichier, on peut donner un nom de fichier dans lequel le fichier résultat sera copié.

L'ensemble des valeurs des résultats du noeud peut être sauvegardé dans un fichier en fin de calcul.

Noeuds Study
''''''''''''''''''''''''''''''
Un noeud Study sert à relier les éléments d'une étude SALOME aux données et résultats d'un schéma de calcul.

Noeud StudyIn
++++++++++++++++++
Un noeud StudyIn a uniquement des ports de données sortants. Il sert pour définir les données
du schéma de calcul provenant d'une étude SALOME.
L'étude associée est donnée par son StudyID SALOME.

Un port correspond à une donnée stockée dans l'étude associée. La donnée a un nom (le nom du port),
un type (le type du port) et une référence qui donne l'entrée dans l'étude. Cette référence est
soit une Entry SALOME (par exemple, 0:1:1:2) soit un chemin dans l'arbre d'étude SALOME (par exemple,
/Geometry/Box_1).


Noeud StudyOut
++++++++++++++++++
Un noeud StudyOut a uniquement des ports de données entrants. Il sert pour ranger des
résultats dans une étude SALOME.
L'étude associée est donnée par son StudyID SALOME.

Un port correspond à un résultat stocké dans l'étude associée. Le résultat a un nom (le nom du port),
un type (le type du port) et une référence qui donne l'entrée dans l'étude.
Cette référence est soit une Entry SALOME (par exemple, 0:1:1:2) soit un chemin dans l'arbre 
d'étude SALOME (par exemple, /Geometry/Box_1).

L'étude associée peut être sauvegardée dans un fichier en fin de calcul.

Les connexions
-----------------
Les connexions entre ports d'entrée et de sortie des noeuds élémentaires ou composés sont
réalisées en établissant des liens entre ces ports.

Les liens de contrôle
''''''''''''''''''''''''''''''
Les liens de contrôle servent à définir un ordre dans l'exécution des noeuds. Ils relient
un port de sortie d'un noeud à un port d'entrée d'un autre noeud. Ces deux noeuds 
doivent être définis dans le même contexte.
La définition du lien se réduit à donner le nom du noeud amont et le nom du noeud aval.

Les liens dataflow
''''''''''''''''''''''''''''''
Les liens dataflow servent à définir un flot de données entre un port de données de sortie d'un noeud
et un port de données d'entrée d'un autre noeud. Il n'est pas nécessaire que ces noeuds soient définis dans 
le même contexte.
Un lien dataflow ajoute un lien de contrôle entre les deux noeuds concernés
ou entre les noeuds parents adéquats pour respecter la règle de définition des liens de 
contrôle. Le lien dataflow garantit la cohérence entre le flot de données et l'ordre d'exécution.
Pour définir le lien, il suffit de donner les noms du noeud et du port amont et les noms du noeud
et du port aval.
Les types des ports doivent être compatibles (voir `Compatibilité des types de données`_).

Les liens data
''''''''''''''''''''''''''''''
Dans quelques cas (boucles principalement), il est utile de pouvoir définir des flots de données
sans définir le lien de contrôle associé comme dans le lien dataflow. On utilise alors le lien
data.
La définition est identique à celle du lien dataflow.
Les types des ports doivent être compatibles (voir `Compatibilité des types de données`_).

Les liens datastream
''''''''''''''''''''''''''''''
Les liens datastream servent à définir un flot de données entre un port datastream sortant d'un noeud et un 
port datastream entrant d'un autre noeud. Ces deux noeuds doivent être définis dans un même contexte
et pouvoir être exécutés en parallèle. Il ne doit donc exister aucun lien de contrôle direct
ou indirect entre eux.
Pour définir le lien, on donne les noms du noeud et du port sortant et les noms du noeud
et du port entrant. La définition des liens datastream peut être complétée par des propriétés
qui paramètrent le comportement du port DSC qui réalise l'échange de données (voir ports DSC).
Les types des ports doivent être compatibles (voir `Compatibilité des types de données`_).

Compatibilité des types de données
'''''''''''''''''''''''''''''''''''''''''
Un lien data, dataflow ou datastream peut être créé seulement si le type de données du port 
sortant est compatible avec le type de données du port entrant.
Il y a trois formes de compatibilité :

  - l'identité des types (par exemple double -> double)
  - la spécialisation des types (par exemple FIELDDOUBLE -> FIELD)
  - la conversion des types (par exemple int -> double)

Compatibilité par conversion
+++++++++++++++++++++++++++++++
La compatibilité par conversion s'applique uniquement aux types de base et à leurs dérivés 
(séquence, structure).
Les conversions acceptées sont les suivantes :

================= ============================== ====================================
Type YACS          Conversion possible en                Commentaire
================= ============================== ====================================
int                 double
int                 bool                           true si int != 0 false sinon
================= ============================== ====================================

La conversion s'applique également aux types construits comme une séquence d'int qui
peut être convertie en une séquence de double. YACS prend en charge la conversion.
Ceci s'applique également aux structures et aux types imbriqués séquence de séquence,
structure de structure, séquence de structure, etc.

Compatibilité par spécialisation
+++++++++++++++++++++++++++++++++++
La règle de compatibilité s'exprime différemment pour les liens data (ou dataflow) et les
liens datastream.

Dans le cas des liens data (ou dataflow), il faut que le type du port de données sortant
soit dérivé (ou identique) du type du port de données entrant. Par exemple un port de données
sortant avec un type FIELDDOUBLE pourra être connecté à un port de données entrant avec le
type FIELD car le type FIELDDOUBLE est dérivé du type FIELD (ou FIELD est type de base de
FIELDDOUBLE).

Dans le cas des liens datastream, la règle est l'exact inverse de celle pour les liens data :
le type du port datastream entrant doit être dérivé de celui du port sortant. Il n'existe 
pour le moment aucun type datastream dérivé. La seule règle qui s'applique est donc l'identité
des types.

Liens multiples
'''''''''''''''''''
Les ports de contrôle supportent les liens multiples, aussi bien 1 vers N que N vers 1.

Les ports de données supportent les liens multiples 1 vers N et N vers 1. 
Les liens 1 vers N ne posent pas de problèmes. Les liens N vers 1 sont à utiliser avec 
précaution car le résultat final dépend de l'ordre dans lequel sont réalisés les échanges.
On réservera ce type de lien pour les rebouclages dans les boucles itératives. Dans ce 
cas l'ordre de réalisation des échanges est parfaitement reproductible.

Les ports datastream supportent également les liens multiples, 1 vers N et N vers 1. 
Les liens datastream 1 vers N ne posent pas de problèmes particuliers : les échanges de données sont
simplement dupliqués pour tous les ports d'entrée connectés.
Par contre, pour les liens datastream N vers 1, les échanges de données vont se recouvrir dans 
l'unique port d'entrée. Le résultat final peut dépendre de l'ordre dans lequel seront réalisés
les échanges.

Les noeuds composés
--------------------------------
Les noeuds composés sont de plusieurs types : bloc, boucles, noeud-switch.
Un noeud composé peut contenir un ou plusieurs noeuds de type quelconque (élémentaires ou composés).
Par défaut, l'ensemble des entrées et sorties des noeuds constituant le noeud composé sont accessibles de l'extérieur. 
On peut dire que les entrées du noeud composé sont constituées de l'ensemble des entrées des noeuds internes. 
Même chose pour les sorties. C'est le concept de boîte blanche.

Le noeud Bloc
''''''''''''''
C'est un regroupement de noeuds avec des liens de dépendance entre les noeuds internes. 
Le Bloc est une boite blanche (les noeuds internes sont visibles). 
Un schéma de calcul est un Bloc.
Le Bloc se manipule de façon similaire à un noeud élémentaire. 
Il dispose d'un seul port de contrôle en entrée et d'un seul en sortie. 
En conséquence, deux blocs raccordés par un lien de donnée dataflow s'exécutent en séquence, tous les noeuds du premier bloc sont exécutés avant de passer au second bloc.

Le noeud ForLoop
''''''''''''''''''''''
Une boucle permet des itérations sur un noeud interne.
Ce noeud interne peut être un noeud composé ou un noeud élémentaire. 
Certaines des sorties du noeud interne peuvent être explicitement rebouclées sur des entrées de ce même noeud interne.
Une boucle ForLoop exécute le noeud interne un nombre fixe de fois. Ce nombre est donné par un port
de donnée de la boucle de nom "nsteps" ou par un paramètre de la boucle de même nom. 

Le noeud While
''''''''''''''''''''''
Une boucle While exécute le noeud interne tant qu'une condition est vraie. La valeur de la condition
est donnée par un port de donnée de la boucle de nom "condition".

Le noeud ForEach
''''''''''''''''''''''
Le noeud ForEach est également une boucle mais il sert à exécuter, en parallèle, un corps de boucle en itérant 
sur une et une seule collection de données. Une collection de données est du type séquence.
Un port de données d'entrée du noeud ForEach de nom "SmplsCollection" reçoit la collection de données sur laquelle la boucle itère.
Cette collection de données est typée. Le type de donnée sur lequel la boucle itère est unique.
Le nombre de branches parallèles que la boucle gère est fixé par un paramètre de la boucle. Si la collection est de 
taille 100 et que ce paramètre est fixé à 25, la boucle exécutera 4 paquets de 25 calculs en parallèle.
Le noeud interne a accès à l'itéré courant de la collection de données via le port de données sortant de la boucle
de nom "SmplPrt".

En sortie de boucle il est possible de construire des collections de données typées. Il suffit de relier un port de 
données de sortie du noeud interne à un port de données d'entrée d'un noeud hors de la boucle. La collection de 
données est contruite automatiquement par la boucle.

Le noeud Switch
''''''''''''''''''''''
Le noeud Switch permet l'exécution conditionnelle (parmi N) d'un noeud (composé, élémentaire). 
Ces noeuds doivent avoir un minimum d'entrées et de sorties compatibles.
La condition de switch (entier, réel) permet d'aiguiller l'exécution d'un noeud parmi n.
La condition de switch est donné par un port de donnée entrant du noeud Switch de nom "select" ou par un paramètre 
de ce noeud de même nom.

Si les noeuds sont terminaux (rien n'est exécuté à partir de leurs sorties), ils n'ont pas besoin d'avoir des sorties compatibles. 
Les ports de sortie exploités en sortie du noeud doivent être compatibles entre eux (i.e. dériver d'un type générique commun exploitable par un autre noeud en entrée).


Les containers
---------------------
La plate-forme SALOME exécute ses composants après les avoir chargés dans des containers. Un container SALOME est un
processus géré par la plate-forme qui peut être exécuté sur toute machine connue.

Un container YACS sert à définir des contraintes de placement des composants sans pour autant définir précisément
la machine à utiliser ou le nom du container.

Le container YACS a un nom. Les contraintes sont données sous la forme de propriétés du container.
La liste actuelle des propriétés est la suivante :

=================== ============= =============================================
Nom                   Type            Type de contrainte
=================== ============= =============================================
policy               "best",       Choisit dans la liste des machines, une fois
                     "first" ou    les autres critères appliqués, la meilleure
                     "cycl"        ou la première ou la suivante. Par défaut,
                                   YACS utilise la politique "cycl" qui
                                   prend la machine suivante dans la liste
                                   des machines connues.
container_name        string       si donné impose le nom du container SALOME
hostname              string       si donné impose la machine
OS                    string       si donné restreint le choix de l'OS
parallelLib           string       ??
workingdir            string      si donné, spécifie le répertoire d'exécution.
                                  Par défaut, c'est le répertoire de lancement
                                  de YACS sur la machine locale et le $HOME
                                  sur les machines distantes.
isMPI                 bool         indique si le container doit supporter MPI
mem_mb                int          taille mémoire minimale demandée
cpu_clock             int          vitesse cpu minimale demandée
nb_proc_per_node      int          ??
nb_node               int          ??
nb_component_nodes    int          ??
=================== ============= =============================================

Le catalogue des ressources matérielles
''''''''''''''''''''''''''''''''''''''''''
La liste des ressources matérielles (machines) connues de SALOME est donnée par le catalogue des
ressources : fichier CatalogResources.xml qui doit se trouver dans le répertoire de l'application SALOME utilisée.
Ce fichier est au format XML. Chaque machine est décrite avec le tag machine qui a plusieurs attributs
qui permettent de la caractériser.

================================== =========================== ==============================================
Caractéristique                        Attribut XML                Description
================================== =========================== ==============================================
nom                                 hostname                   soit le nom complet : c'est la clef 
                                                               qui détermine la machine de manière unique 
                                                               (exemple : "nickel.ccc.cea.fr") 
alias                               alias                      chaine de caractères qui permet d'identifier 
                                                               la machine (exemple : "pluton")
protocole d'accès                   protocol                   "rsh" (défaut) ou "ssh"
type d'accès                        mode                       interactif "i" ou batch "b". Par défaut "i"
username                            userName                   user avec lequel on va se connecter sur 
                                                               la machine
système d'exploitation              OS
taille de la mémoire centrale       memInMB
fréquence d'horloge                 CPUFreqMHz
nombre de noeuds                    nbOfNodes
nombre de processeurs par noeud     nbOfProcPerNode
application SALOME                  appliPath                  répertoire de l'application SALOME à utiliser
                                                               sur cette machine
implémentation mpi                  mpi                        indique quelle implémentation de MPI est
                                                               utilisée sur cette machine ("lam", "mpich1",
                                                               "mpich2", "openmpi")
gestionnaire de batch               batch                      si la machine doit être utilisée au travers
                                                               d'un système de batch donne le nom du
                                                               gestionnaire de batch ("pbs", "lsf", "slurm").
                                                               Pas de défaut.
================================== =========================== ==============================================

Il est possible d'indiquer également la liste des modules SALOME présents sur la machine.
Par défaut, SALOME suppose que tous les composants demandés par YACS sont présents.
Chaque module présent est donné par le sous tag modules et son attribut moduleName.

Voici un exemple de catalogue de ressources::

  <!DOCTYPE ResourcesCatalog>
  <resources>
    <machine hostname="is111790" alias="is111790" OS="LINUX" CPUFreqMHz="2992" memInMB="1024" 
             protocol="rsh" mode="interactif" nbOfNodes="1" nbOfProcPerNode="1" >
    </machine>
    <machine hostname="is111915" alias="is111915" OS="LINUX" CPUFreqMHz="2992" memInMB="1024" 
             protocol="ssh" mode="interactif" nbOfNodes="1" nbOfProcPerNode="1" 
             appliPath="SALOME43/Run">
             <modules moduleName="GEOM"/>
    </machine>
  </resources>

.. _etats:

Les états d'un noeud
-----------------------------
Au cours de l'édition d'un schéma de calcul, les états possibles d'un noeud sont : 

=================== =============================================
Etat                  Commentaire
=================== =============================================
READY                 le noeud est valide, prêt à être exécuté
INVALID               le noeud est invalide, le schéma ne peut
                      pas être exécuté
=================== =============================================

Au cours de l'exécution d'un schéma de calcul, les états possibles d'un noeud sont :

=================== =============================================================
Etat                  Commentaire
=================== =============================================================
READY                 le noeud est valide, prêt à être exécuté
TOLOAD                le composant associé au noeud peut être chargé
LOADED                le composant associé au noeud est chargé
TOACTIVATE            le noeud peut être exécuté
ACTIVATED             le noeud est en exécution
DONE                  l'exécution du noeud est terminée sans erreur
ERROR                 l'exécution du noeud est terminée avec erreur
FAILED                noeud en erreur car des noeuds précédents sont en erreur
DISABLED              l'exécution du noeud est désactivée
PAUSE                 l'exécution du noeud est suspendue
=================== =============================================================

.. _nommage:

Le nommage contextuel des noeuds
-------------------------------------
On a vu que les noeuds élémentaires et composés ont un nom unique dans le contexte de définition
qui correspond au noeud parent (schéma de calcul ou noeud composé). Pour pouvoir désigner les noeuds
dans toutes les situations possibles, on utilise plusieurs sortes de nommage :

  - le nommage local : c'est le nom du noeud dans son contexte de définition
  - le nommage absolu : c'est le nom du noeud vu depuis le niveau le plus haut du schéma de calcul
  - le nommage relatif : c'est le nom du noeud vu d'un noeud composé parent

La règle générale est que les noms absolu et relatif sont construits en concaténant les noms locaux
du noeud et de ses parents en les séparant par des points.

Prenons l'exemple d'un noeud élémentaire de nom "n" défini dans un bloc de nom "b" qui lui même est défini
dans un bloc de nom "c" lui même défini au niveau le plus haut du schéma. Le nom local du noeud est "n".
Le nom absolu est "c.b.n". Le nom relatif dans le bloc "c" est "b.n". On applique la même règle pour
nommer les ports. Si le noeud "n" a un port de nom "p". Il suffit d'ajouter ".p" au nom du noeud
pour avoir le nom du port.

Il y a une exception à cette règle. Elle concerne le noeud Switch. Dans ce cas, il faut tenir compte du case
qui n'est pas un vrai noeud. Si on dit que le bloc "b" de l'exemple précédent est un switch qui a un case avec
une valeur de 1 et un default, alors le nom absolu du noeud "n" dans le case sera "c.b.p1_n" et celui dans
le default sera "c.b.default_n".

.. _errorreport:

Rapport d'erreur
-------------------
Chaque noeud a un rapport d'erreur associé si son état est INVALID, ERROR ou FAILED.
Ce rapport est au format XML. 

Les noeuds élémentaires produisent un rapport simple qui contient un seul tag (error)
avec 2 attributs :

- node : qui donne le nom du noeud
- state : qui indique son état

Le contenu du tag est le texte de l'erreur. Pour un noeud script python ce sera la plupart du temps,
le traceback de l'exception rencontrée. Pour un noeud de service, ce sera soit le contenu d'une
exception SALOME soit le contenu d'une exception CORBA.

Les noeuds composés produisent un rapport composite contenu dans un tag de même nom (error) avec
les 2 mêmes attributs node et state.
Le tag contient l'ensemble des rapports d'erreur des noeuds fils erronés.

Voici un exemple de rapport d'erreur pour une division par zéro dans un noeud python 
contenu dans une boucle::

  <error node= proc state= FAILED>
  <error node= l1 state= FAILED>
  <error node= node2 state= ERROR>
  Traceback (most recent call last):
    File "<string>", line 1, in ?
  ZeroDivisionError: integer division or modulo by zero
  
  </error>
  </error>
  </error>

Trace d'exécution
------------------
Pour chaque exécution une trace d'exécution est produite dans un fichier de nom traceExec_<nom du schema>
où <nom du schema> est le nom donné au schéma.

Chaque ligne du fichier représente un évènement relatif à un noeud. Elle contient deux
chaines de caractères. La première est le nom du noeud. La deuxième décrit l'évènement.

Voici une trace pour le même exemple que ci-dessus::

  n load
  n initService
  n connectService
  n launch
  n start execution
  n end execution OK
  n disconnectService
  l1.node2 load
  l1.node2 initService
  l1.node2 connectService
  l1.node2 launch
  l1.node2 start execution
  l1.node2 end execution ABORT, Error during execution
  l1.node2 disconnectService


.. _archi:

YACS general architecture
------------------------------

YACS module implements API of a full SALOME module only for the schema execution.  The schema edition is done in the GUI process alone.  For execution, YACS has a CORBA servant that implements Engines::Component CORBA interface (see SALOME 4 KERNEL IDL interfaces).  YACS GUI and YACS CORBA engine share YACS core libraries (engine and runtime): GUI uses them at schema design time, then a schema XML file is saved and passed to YACS CORBA API, and finally YACS core libraries execute the schema at YACS CORBA server side.

YACS GUI differs from standard full SALOME modules (such as Geometry or Mesh) in that it does not use SALOMEDS objects to create Object Browser representation of its data, and creates this representation in a way light SALOME modules do.  This is done in order to avoid publishing lots of objects in SALOMEDS study just to create visual representation of data and thus to improve GUI performance.

YACS architecture scheme is shown on the picture below.

.. image:: images/general_architecture_0.jpg
     :align: center

The YACS module will be developed as a Salome module with one document (study) per desktop.

YACS is composed of several packages. The main things are mentioned in the next sections.

Bases package
'''''''''''''''''''''''
Bases package contains common base classes (exception, threads, etc.) and constants.

Engine package
'''''''''''''''''''''''
Engine package consists of calculation schema generic classes (calculation nodes, control nodes, control and data flow links, etc.) Engine is in charge to

    * edit,
    * check consistency,
    * schedule,
    * execute

graphs independently from the context (i.e. Runtime) the graph is destined to run.

SALOME Runtime package
'''''''''''''''''''''''
Runtime package provides implementation of YACS generic calculation nodes for SALOMR platform. Runtime exists in a given Context.  Runtime is in charge to:

    * treat physically the basic execution of elementary tasks in a given context,
    * transfer data in this context,
    * perform the physical deployment of the execution.

Runtime simply appears in Engine as an interface that a concrete Runtime must implement to be piloted by Engine.
SALOME Runtime nodes


The SALOME Runtime implements following nodes.

    * Inline function node.  A function inline node is implemented by a Python function.
    * Inline script node.  A script inline node is implemented by a Python script.
    * Component service node.  This is a calculation node associated with a component service.
    * CORBA reference service node.  Reference service node for CORBA objects.  This is a node that executes a CORBA service.
    * CPP node.  This is a C++ node (in process component), i.e. local C++ implementation - single process.

XML file loader package
''''''''''''''''''''''''''''''''
This is XML reader for generic calculation schema.

XML file loader provides

    * a possibility to load a calculation schema in memory by reading and parsing a XML file describing it,
    * an executable named driver that can be used to load and execute (see Using YACS driver) a calculation schema given as a XML file (see Writing XML file).

GUI design
''''''''''''''''''''''''''''''''
Goals of Graphic User Interface design are the following.

    * Provide a general mechanism for the synchronisation of several views (tree views, 2D canvas views, edition dialogs).  For this goal, a subject/observers design pattern is used: several observers can attach or detach themselves to/from the subject.  The subject send update events to the lists of observers and does not know the implementation of the observers.  The observers correspond to the different views in case of YACS.
    * Provide an interface of Engine for edition with a general mechanism for undo-redo (future version!).
    * Be as independent as possible of Qt (and SALOME), to avoid problems in Qt4 migration, and allow a potential re-use of YACS GUI outside SALOME.


