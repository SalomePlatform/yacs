
.. _schemapy:

Définition d'un schéma de calcul avec l'interface de programmation Python
============================================================================
Un schéma de calcul YACS peut être défini à partir d'un programme écrit en langage Python (http://www.python.org/).
Pour une initiation au langage, on consultera le `tutorial Python <http://docs.python.org/tut/tut.html>`_.

L'interface de programmation (API) est portée par trois modules Python : pilot, SALOMERuntime et loader.

Le module SALOMERuntime sert pour initialiser YACS pour SALOME.

Le module loader sert pour créer des schémas de calcul en chargeant des fichiers au format XML.

Le module pilot est celui qui sert à créer des schémas de calcul.

Ces modules doivent être importés au début du programme Python et YACS doit être initialisé::

    import sys
    import pilot
    import SALOMERuntime
    import loader
    SALOMERuntime.RuntimeSALOME_setRuntime()

Pour pouvoir importer les modules YACS, l'environnement doit être correctement configuré ce qui est le 
cas si on utilise l'application SALOME. Sinon, il faut positionner la variable d'environnement PYTHONPATH
à <YACS_ROOT_DIR>/lib/lib/pythonX.Y/site-packages/salome.

.. _loadxml:

Créer un schéma de calcul par chargement d'un fichier XML
--------------------------------------------------------------
C'est la façon la plus simple de créer un schéma de calcul. Si on a un fichier conforme à la syntaxe YACS
(voir :ref:`schemaxml`), il suffit de créer un chargeur de fichier XML puis d'utiliser sa méthode load
pour obtenir un objet schéma de calcul en Python.

Voici le code Python suffisant pour charger un fichier XML::

  xmlLoader = loader.YACSLoader()
  try:
    p = xmlLoader.load("simple1.xml")
  except IOError,ex:
    print "IO exception:",ex
    sys.exit(1)

Ensuite, si on met dans un fichier de nom testLoader.py le code de l'initialisation et le code
du chargement, il suffit de faire::

  python testLoader.py

pour exécuter le programme. L'exception IOError peut être levée par l'opération de chargement 
principalement si le fichier n'existe pas ou ne peut pas être lu.
Dans le cas où aucune exception n'a été levé, il faut vérifier que l'analyse du fichier s'est
bien passée. Ceci est fait en utilisant l'objet Logger associé au schéma de calcul::

   logger=p.getLogger("parser")
   if not logger.isEmpty():
     print "The imported file has errors :"
     print logger.getStr()
     sys.exit(1)

Enfin, si l'analyse du fichier s'est bien passée, il faut vérifier la validité du schéma (complétude
des connexions, pas de port d'entrée non connecté, ...). On utilise pour celà la méthode isValid de 
l'objet schéma de calcul puis la méthode p.checkConsistency de ce même objet comme ci-dessous::

   if not p.isValid():
     print "The schema is not valid and can not be executed"
     print p.getErrorReport()
     sys.exit(1)

   info=pilot.LinkInfo(pilot.LinkInfo.ALL_DONT_STOP)
   p.checkConsistency(info)
   if info.areWarningsOrErrors():
     print "The schema is not consistent and can not be executed"
     print info.getGlobalRepr()
     sys.exit(1)


Si tous ces tests se sont bien passés, le schéma est prêt à être exécuté (voir :ref:`execpy`).

Créer un schéma de calcul de zéro
-----------------------------------
On suivra ici la même progression que dans :ref:`schemaxml`.

La première chose à faire avant de créer les objets constitutifs du schéma est d'obtenir
l'objet runtime qui va servir pour leur création::

  r = pilot.getRuntime()

Création d'un schéma vide
''''''''''''''''''''''''''''
On l'obtient par en utilisant la méthode createProc de l'objet runtime avec le nom
du schéma en argument::
 
  p=r.createProc("pr")

L'objet schéma de nom "pr" a été créé. Il est représenté par l'objet Python p.

Définition des types de données
'''''''''''''''''''''''''''''''''
Types de base
++++++++++++++++
On ne peut définir de type de base. Ils sont définis par YACS. Il faut cependant pouvoir
récupérer un objet Python équivalent à un type de base pour pouvoir créer par la suite des
ports.

On récupère un type de données de base en utilisant la méthode getTypeCode du schéma de calcul
avec le nom du type en argument. Par exemple::

   td=p.getTypeCode("double")

permet d'obtenir le type double (objet Python td).
Les autres types de base s'obtiennent par::

   ti=p.getTypeCode("int")
   ts=p.getTypeCode("string")
   tb=p.getTypeCode("bool")
   tf=p.getTypeCode("file")


Référence d'objet
+++++++++++++++++++++
Pour définir un type référence d'objet, on utilise la méthode createInterfaceTc du schéma de calcul. Cette méthode
prend trois arguments : le repository id de l'objet SALOME correspondant, le nom du type, une liste de types
qui seront des types de base de ce type. Si le repository id vaut "", la valeur par défaut sera utilisée.

Voici un exemple minimal de définition de référence d'objet de nom Obj (repository id par défaut, pas de type de base)::

  tc1=p.createInterfaceTc("","Obj",[])

On peut définir le même type Obj, en donnant le repository id::

  tc1=p.createInterfaceTc("IDL:GEOM/GEOM_Object","Obj",[])

Pour définir un type référence d'objet dérivé d'un autre type, on fournit en plus une liste de types de base.

Voici la définition du type MyObj dérivé du type Obj::

  tc2=p.createInterfaceTc("","MyObj",[tc1])

Séquence
+++++++++++
Pour définir un type séquence, on utilise la méthode createSequenceTc du schéma de calcul. Cette méthode
prend trois arguments : le repository id, le nom du type, le type des éléments de la séquence. Il n'est
généralement pas utile de spécifier le repository id. On donnera la valeur "".

Voici un exemple de définition du type séquence de double seqdbl::

  tc3=p.createSequenceTc("","seqdbl",td)

td est le type double que l'on obtiendra comme ci-dessus : `Types de base`_.

Pour définir un type séquence de séquence, on écrit::

  tc4=p.createSequenceTc("","seqseqdbl",tc3)

Pour définir un type séquence de référence, on écrit::

  tc5=p.createSequenceTc("","seqobj",tc1)


Structure
++++++++++++
Pour définir un type structure, on utilise la méthode createStructTc du schéma de calcul. Cette méthode
prend deux arguments : le repository id, le nom du type. Pour une utilisation standard, le repository
id prendra la valeur "". Le type structure est le seul qui se définit en deux étapes. Il est créé
vide suite à l'appel de la méthode createStructTc. Pour définir ses membres, il faut ensuite

Voici un exemple de définition du type structure s1 avec 2 membres (m1 et m2) de types double et séquence de doubles::

  ts1=p.createStructTc("","s1")
  ts1.addMember("m1",td);
  ts1.addMember("m2",tc3);

Récupérer les types prédéfinis
+++++++++++++++++++++++++++++++++
Par défaut, YACS définit seulement les types de base. Pour obtenir plus de types prédéfinis, il faut
les demander à SALOME. Ces autres types prédéfinis sont contenus dans les catalogues des modules
comme GEOM ou SMESH.

La séquence de code qui permet d'obtenir une image des catalogues SALOME dans YACS est la suivante::

  try:
    cata=r.loadCatalog("session","corbaname::localhost:2810/NameService#Kernel.dir/ModulCatalog.object")
  except CORBA.TRANSIENT,ex:
    print "Unable to contact server:",ex
  except CORBA.SystemException,ex:
    print ex,CORBA.id(ex)

Il faut que l'application SALOME ait été lancée pour que le catalogue soit accessible.
Ensuite, les types prédéfinis sont accessibles dans le dictionnaire cata._typeMap.
Si on connait le nom du type voulu ('GEOM_Shape', par exemple), on l'obtient par::

  tgeom=cata._typeMap['GEOM_Shape']

.. _typedict:

Ajouter un type dans le dictionnaire des types du schéma
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Certaines opérations nécessitent d'avoir les types définis dans le dictionnaire du schéma.
Pour mettre un type dans le dictionnaire, on fait, par exemple pour le type seqobj défini ci-dessus::

  p.typeMap["seqobj"]=tc5

avec le nom du type comme clé du dictionnaire et le type comme valeur.

Définition des noeuds de calcul élémentaires
''''''''''''''''''''''''''''''''''''''''''''''
Noeud script Python
+++++++++++++++++++++
Pour définir un noeud script dans un contexte donné (le schéma de calcul, par exemple), on
procède en plusieurs étapes. 
La première étape consiste à créer l'objet noeud par appel à la méthode createScriptNode du runtime.
Cette méthode a 2 arguments dont le premier doit valoir "" en utilisation standard et le deuxième
est le nom du noeud. Voici un exemple de création du noeud node1::

  n=r.createScriptNode("","node1")

La deuxième étape consiste à rattacher le noeud à son contexte de définition par appel à la méthode
edAddChild de l'objet contexte. Cette méthode a un argument : le noeud à rattacher. Voici un exemple
de rattachement du noeud node1 au schéma de calcul::

  p.edAddChild(n)

Attention, le nom de la méthode à utiliser dépend du type de noeud contexte. On verra plus tard pour d'autres
types de noeud quelle méthode utiliser.

La troisième étape consiste à définir le script Python associé au noeud. On utilise pour celà la méthode setScript
du noeud avec un argument chaine de caractères qui contient le code Python. Voici un exemple de définition
du code associé::

  n.setScript("p1=p1+2.5")

La quatrième étape consiste à définir les ports de données d'entrée et de sortie. Un port d'entrée est créé par appel
à la méthode edAddInputPort du noeud. Un port de sortie est créé par appel à la méthode edAddOutputPort du noeud.
Ces deux méthodes ont deux arguments : le nom du port et le type de données du port. Voici un exemple de création
d'un port d'entrée p1 de type double et d'un port de sortie p1 de type double::

  n.edAddInputPort("p1",td)
  n.edAddOutputPort("p1",td)

Maintenant notre noeud est complètement défini avec son nom, son script, ses ports et son contexte. Il récupère
un double dans le port d'entrée p1, lui ajoute 2.5 et met le résultat dans le port de sortie p1.

Noeud fonction Python
++++++++++++++++++++++
Pour définir un noeud fonction, on procède de la même manière. Les seules différences concernent la création :
utiliser la méthode createFuncNode et la définition de la fonction : il faut en plus appeler la méthode setFname
pour donner le nom de la fonction à exécuter. Voici un exemple complet de définition d'un noeud fonction
qui est fonctionnellement identique au noeud script précédent::

  n2=r.createFuncNode("","node2")
  p.edAddChild(n2)
  n2.setScript("""
  def f(p1):
    p1=p1+2.5
    return p1
  """)
  n2.setFname("f")
  n2.edAddInputPort("p1",td)
  n2.edAddOutputPort("p1",td)


Noeud de service SALOME
++++++++++++++++++++++++++
On a deux formes de définition d'un noeud de service SALOME.

La première forme dans laquelle on donne le nom du composant utilise la méthode createCompoNode
pour la création du noeud. Le nom du composant est donné en argument de la méthode setRef du noeud.
Le nom du service est donné en argument de la méthode setMethod du noeud. Le reste de la définition est 
identique à celui des noeuds Python précédents.

Voici un exemple de noeud qui appelle le service makeBanner d'un composant PYHELLO::

  n3=r.createCompoNode("","node3")
  p.edAddChild(n3)
  n3.setRef("PYHELLO")
  n3.setMethod("makeBanner")
  n3.edAddInputPort("p1",ts)
  n3.edAddOutputPort("p1",ts)

La deuxième forme qui permet de définir un noeud qui utilise le même composant qu'un autre noeud
utilise la méthode createNode de ce dernier noeud. Cette méthode n'a qu'un argument, le nom du noeud.
Le reste de la définition est identique à celui de la précédente forme.

Voici un exemple de noeud de service qui appelle une deuxième fois le service makeBanner de la même
instance de composant que le noeud précédent::

  n4=n3.createNode("node4")
  p.edAddChild(n4)
  n4.setMethod("makeBanner")
  n4.edAddInputPort("p1",ts)
  n4.edAddOutputPort("p1",ts)

Définition des connexions
''''''''''''''''''''''''''''
Obtenir un port d'un noeud
++++++++++++++++++++++++++++
Pour pouvoir définir des liens, il faut presque toujours disposer des objets Python représentant
le port de sortie à connecter au port d'entrée.
Il y a deux façons de disposer de cet objet.

La première façon est de récupérer le port lors de sa création avec les méthodes edAddInputPort et 
edAddOutputPort. On écrira alors, par exemple::

  pin=n4.edAddInputPort("p1",ts)
  pout=n4.edAddOutputPort("p1",ts)

pin et pout sont alors les objets nécessaires pour définir des liens.

La deuxième façon est d'interroger le noeud et de lui demander un de ses ports par son nom. On utilise pour
celà les méthodes getInputPort et getOutputPort.

On pourra alors obtenir pin et pout comme suit::

  pin=n4.getInputPort("p1")
  pout=n4.getOutputPort("p1")

Lien de contrôle
++++++++++++++++++
Pour définir un lien de contrôle entre deux noeuds, on utilise la méthode edAddCFLink du contexte en lui passant
en arguments les deux noeuds à connecter.
Par exemple, un lien de contrôle entre les noeuds n3 et n4 s'écrira::

  p.edAddCFLink(n3,n4)

Le noeud n3 sera exécuté avant le noeud n4.

Lien dataflow
++++++++++++++
Pour définir un lien dataflow, il faut tout d'abord obtenir les objets ports par une des méthodes vues 
ci-dessus. Ensuite on utilise la méthode edAddDFLink du noeud contexte en lui passant les deux ports à connecter.

Voici un exemple de lien dataflow entre le port de sortie p1 du noeud n3 et le port d'entrée du noeud n4::

  pout=n3.getOutputPort("p1")
  pin=n4.getInputPort("p1")
  p.edAddDFLink(pout,pin)

Lien data
++++++++++++
Un lien data se définit comme un lien dataflow en utilisant la méthode edAddLink au lieu de edAddDFLink.
Le même exemple que ci-dessus avec un lien data::

  pout=n3.getOutputPort("p1")
  pin=n4.getInputPort("p1")
  p.edAddLink(pout,pin)

Initialisation d'un port de données d'entrée
'''''''''''''''''''''''''''''''''''''''''''''''
Pour initialiser un port de données d'entrée, il faut tout d'abord obtenir l'objet port correspondant. Ensuite, il existe
deux méthodes pour l'initialiser.

La première initialise le port avec une valeur encodée en XML-RPC. On utilise alors la méthode edInitXML du
port. Voici un exemple qui initialise un port avec la valeur entière 5::

  pin.edInitXML("<value><int>5</int></value>")

La deuxième méthode initialise le port avec une valeur Python. On utilise alors la méthode edInitPy. Voici
un exemple qui initialise ce même port avec la même valeur::

  pin.edInitPy(5)

On peut également utiliser des méthodes spécifiques pour les types de base :
  
- ``edInitInt`` pour le type int
- ``edInitDbl`` pour le type double
- ``edInitBool`` pour le type bool
- ``edInitString`` pour le type string

Premier exemple à partir des éléments précédents
'''''''''''''''''''''''''''''''''''''''''''''''''''
En rassemblant tous les éléments de définition précédents, un schéma de calcul complet identique à celui
du chapitre :ref:`schemaxml` se présentera comme suit::

  import sys
  import pilot
  import SALOMERuntime
  import loader
  SALOMERuntime.RuntimeSALOME_setRuntime()
  r = pilot.getRuntime()
  p=r.createProc("pr")
  ti=p.getTypeCode("int")
  #node1
  n1=r.createScriptNode("","node1")
  p.edAddChild(n1)
  n1.setScript("p1=p1+10")
  n1.edAddInputPort("p1",ti)
  n1.edAddOutputPort("p1",ti)
  #node2
  n2=r.createScriptNode("","node2")
  p.edAddChild(n2)
  n2.setScript("p1=2*p1")
  n2.edAddInputPort("p1",ti)
  n2.edAddOutputPort("p1",ti)
  #node4
  n4=r.createCompoNode("","node4")
  p.edAddChild(n4)
  n4.setRef("ECHO")
  n4.setMethod("echoDouble")
  n4.edAddInputPort("p1",td)
  n4.edAddOutputPort("p1",td)
  #control links
  p.edAddCFLink(n1,n2)
  p.edAddCFLink(n1,n4)
  #dataflow links
  pout=n3.getOutputPort("p1")
  pin=n4.getInputPort("p1")
  #dataflow links
  p.edAddDFLink(n1.getOutputPort("p1"),n2.getInputPort("p1"))
  p.edAddDFLink(n1.getOutputPort("p1"),n4.getInputPort("p1"))
  #initialisation ports
  n1.getInputPort("p1").edInitPy(5)


Définition de noeuds composés
'''''''''''''''''''''''''''''''''
Bloc
+++++++
Pour définir un Bloc, on utilise la méthode createBloc du runtime en lui passant le nom du Bloc en argument. Ensuite,
on rattache le noeud à son contexte de définition comme un noeud élémentaire. Voici un exemple de définition de Bloc
dans un schéma de calcul::

  b=r.createBloc("b1")
  p.edAddChild(b)

Une fois le Bloc créé, il est possible d'ajouter tous les noeuds et liens possibles dans son contexte. En reprenant une partie
de l'exemple ci-dessus, on aura::

  n1=r.createScriptNode("","node1")
  b.edAddChild(n1)
  n1.setScript("p1=p1+10")
  n1.edAddInputPort("p1",ti)
  n1.edAddOutputPort("p1",ti)
  n2=r.createScriptNode("","node2")
  b.edAddChild(n2)
  n2.setScript("p1=2*p1")
  n2.edAddInputPort("p1",ti)
  n2.edAddOutputPort("p1",ti)
  b.edAddCFLink(n1,n2)
  b.edAddDFLink(n1.getOutputPort("p1"),n2.getInputPort("p1"))

ForLoop
++++++++
Pour définir une ForLoop, on utilise la méthode createForLoop du runtime en lui passant le nom du noeud en argument.
Ensuite on rattache le noeud à son contexte de définition. Voici un exemple de définition de ForLoop
dans un schéma de calcul::

  l=r.createForLoop("l1")
  p.edAddChild(l)

Pour initialiser le nombre de tours de boucle à exécuter, on utilise le port "nsteps" que l'on initialise 
avec un entier. Par exemple::

  ip=l.getInputPort("nsteps") 
  ip.edInitPy(3)

Il existe une méthode spéciale pour obtenir le port "nsteps" de la boucle : edGetNbOfTimesInputPort. On pourrait donc
l'écrire également ainsi::

  ip=l.edGetNbOfTimesInputPort()
  ip.edInitPy(3)

Enfin, pour ajouter un noeud (et un seul) dans le contexte d'une boucle, on n'utilisera pas la méthode edAddChild
mais une méthode différente de nom edSetNode. Voici un petit exemple de définition d'un noeud interne de boucle::

  n1=r.createScriptNode("","node1")
  l.edSetNode(n1)
  n1.setScript("p1=p1+10")
  n1.edAddInputPort("p1",ti)
  n1.edAddOutputPort("p1",ti)

WhileLoop
++++++++++
Un noeud WhileLoop se définit presque comme un noeud ForLoop. Les seules différences concernent la création et 
l'affectation de la condition de fin de boucle. Pour la création on utilise la méthode createWhileLoop. Pour la 
condition, on utilise le port "condition". Si on fait un rebouclage sur un noeud, il ne faut pas oublier d'utiliser
un lien data et non un lien dataflow.
Voici un exemple de définition de noeud WhileLoop avec un noeud interne script Python. La condition est initialisée
à True puis passée à False par le noeud interne. On a donc un rebouclage::

  wh=r.createWhileLoop("w1")
  p.edAddChild(wh)
  n=r.createScriptNode("","node3")
  n.setScript("p1=0")
  n.edAddOutputPort("p1",ti)
  wh.edSetNode(n)
  cport=wh.getInputPort("condition")
  cport.edInitBool(True)
  p.edAddLink(n.getOutputPort("p1"),cport)

Il existe une méthode spéciale pour obtenir le port "condition" de la boucle : edGetConditionPort.

Boucle ForEach
++++++++++++++++
Un noeud ForEach se définit à la base comme un autre noeud boucle. Il y a plusieurs différences. Le noeud
est créé avec la méthode createForEachLoop qui prend un argument de plus : le type de données géré par le ForEach.
Le nombre de branches du ForEach est spécifié avec le port "nbBranches". La gestion de la collection sur 
laquelle itère le ForEach est faite par connexion des ports "SmplPrt" et "SmplsCollection".

Voici un exemple de définition de noeud ForEach avec un noeud interne script Python qui incrémente l'élément
de la collection de 3.::

  fe=r.createForEachLoop("fe1",td)
  p.edAddChild(fe)
  n=r.createScriptNode("","node3")
  n.setScript("p1=p1+3.")
  n.edAddInputPort("p1",td)
  n.edAddOutputPort("p1",td)
  fe.edSetNode(n)
  p.edAddLink(fe.getOutputPort("SmplPrt"),n.getInputPort("p1"))
  fe.getInputPort("nbBranches").edInitPy(3)
  fe.getInputPort("SmplsCollection").edInitPy([2.,3.,4.])

Pour obtenir les ports spéciaux du ForEach, on peut utiliser les méthodes suivantes à la place de getInputPort et 
getOutputPort :

- edGetNbOfBranchesPort pour le port "nbBranches"
- edGetSamplePort pour le port "SmplPrt"
- edGetSeqOfSamplesPort pour le port "SmplsCollection"

Switch
++++++++
La définition d'un noeud Switch se fait en plusieurs étapes. Les deux premières sont la création et le rattachment
au noeud de contexte. Le noeud est créé par appel de la méthode createSwitch du runtime avec le nom du noeud
en argument. Le noeud est rattaché au noeud contexte par appel de la méthode edAddChild pour un schéma ou un bloc
ou edSetNode pour un noeud boucle.

Voici un exemple de création suivi du rattachement::

  sw=r.createSwitch("sw1")
  p.edAddChild(sw)

Ensuite on crée un noeud interne élémentaire ou composé par cas. Le noeud pour le cas par défaut est
rattaché au switch avec la méthode edSetDefaultNode. Les noeuds pour les autres cas sont rattachés au switch
avec la méthode edSetNode qui prend en premier argument la valeur du cas (entier) et en deuxième argument
le noeud interne.

Voici un exemple de switch avec un noeud script pour le cas "1" et un autre noeud script
pour le cas "default" et un noeud script pour initialiser une variable échangée::

  #init
  n=r.createScriptNode("","node3")
  n.setScript("p1=3.5")
  n.edAddOutputPort("p1",td)
  p.edAddChild(n)
  #switch
  sw=r.createSwitch("sw1")
  p.edAddChild(sw)
  nk1=r.createScriptNode("","ncas1")
  nk1.setScript("p1=p1+3.")
  nk1.edAddInputPort("p1",td)
  nk1.edAddOutputPort("p1",td)
  sw.edSetNode(1,nk1)
  ndef=r.createScriptNode("","ndefault")
  ndef.setScript("p1=p1+5.")
  ndef.edAddInputPort("p1",td)
  ndef.edAddOutputPort("p1",td)
  sw.edSetDefaultNode(ndef)
  #initialisation port select
  sw.getInputPort("select").edInitPy(1)
  #connexion des noeuds internes
  p.edAddDFLink(n.getOutputPort("p1"),nk1.getInputPort("p1"))
  p.edAddDFLink(n.getOutputPort("p1"),ndef.getInputPort("p1"))

Pour obtenir le port spécial "select" du Switch, on peut utiliser la méthode edGetConditionPort à la place de getInputPort.
  
Définition de containers
''''''''''''''''''''''''''''
Pour définir un container, on utilise la méthode createContainer du runtime puis on lui donne un nom avec sa méthode 
setName. Enfin on lui affecte des contraintes en lui ajoutant des propriétés.
Voici un exemple de création d'un container de nom "A"::

  c1=r.createContainer()
  c1.setName("A")

On ajoute une propriété à un container en utilisant sa méthode setProperty qui prend 2 arguments (chaines de caractères).
Le premier est le nom de la propriété. Le deuxième est sa valeur.
Voici un exemple du même container "A" avec des contraintes::

  c1=r.createContainer()
  c1.setName("A")
  c1.setProperty("container_name","FactoryServer")
  c1.setProperty("hostname","localhost")
  c1.setProperty("mem_mb","1000")

Une fois que les containers sont définis, on peut placer des composants SALOME sur ce container. 
Pour placer le composant d'un noeud de service SALOME, il faut tout d'abord obtenir l'instance de
composant de ce noeud de service en utilisant la méthode getComponent de ce noeud. Puis on affecte le container
précédemment défini à cette instance de composant en utilisant la méthode setContainer de l'instance
de composant.

Si on veut placer le service SALOME défini plus haut (noeud "node3") sur le container "A", on écrira::

  n3.getComponent().setContainer(c1)


Les propriétés de noeuds
'''''''''''''''''''''''''''
On ajoute (ou modifie) une propriété à un noeud élémentaire ou composé en utilisant sa méthode
setProperty qui prend 2 arguments (chaines de caractères).
Le premier est le nom de la propriété. Le deuxième est sa valeur.

Voici un exemple pour le noeud "node3" précédent::

  n3.setProperty("VERBOSE","2")

Les connexions datastream
''''''''''''''''''''''''''''
Les connexions datastream ne sont possibles que pour des noeuds de service SALOME comme on l'a vu dans Principes. 
Il faut tout d'abord définir les ports datastream dans le noeud de service. Un port datastream d'entrée se définit
en utilisant la méthode edAddInputDataStreamPort. Un port datastream de sortie se définit
en utilisant la méthode edAddOutputDataStreamPort.
Ces méthodes prennent en arguments le nom du port et le type du datastream.

Certains ports datastream (par exemple les ports CALCIUM) doivent être configurés avec des propriétés.
On utilise la méthode setProperty du port pour les configurer.

Voici un exemple de définition de noeud de service SALOME avec des ports datastream. 
Il s'agit du composant DSCCODC que l'on peut trouver dans le module DSCCODES de la base EXAMPLES. 
Les ports datastream sont de type "CALCIUM_integer" avec dépendance temporelle::

  calcium_int=cata._typeMap['CALCIUM_integer']
  n5=r.createCompoNode("","node5")
  p.edAddChild(n5)
  n5.setRef("DSCCODC")
  n5.setMethod("prun")
  pin=n5.edAddInputDataStreamPort("ETP_EN",calcium_int)
  pin.setProperty("DependencyType","TIME_DEPENDENCY")
  pout=n5.edAddOutputDataStreamPort("STP_EN",calcium_int)
  pout.setProperty("DependencyType","TIME_DEPENDENCY")

Une fois les noeuds de service dotés de ports datastream, il ne reste plus qu'à les connecter.
Cette connexion est réalisée en utilisant la méthode edAddLink du noeud contexte comme pour 
les liens data. Seul le type des ports passés en arguments est différent.

Pour compléter notre exemple on définit un deuxième noeud de service et on connecte les 
ports datastream de ces services::

  n6=r.createCompoNode("","node6")
  p.edAddChild(n6)
  n6.setRef("DSCCODD")
  n6.setMethod("prun")
  pin=n6.edAddInputDataStreamPort("ETP_EN",calcium_int)
  pin.setProperty("DependencyType","TIME_DEPENDENCY")
  pout=n6.edAddOutputDataStreamPort("STP_EN",calcium_int)
  pout.setProperty("DependencyType","TIME_DEPENDENCY")
  p.edAddLink(n5.getOutputDataStreamPort("STP_EN"),n6.getInputDataStreamPort("ETP_EN"))
  p.edAddLink(n6.getOutputDataStreamPort("STP_EN"),n5.getInputDataStreamPort("ETP_EN"))

D'autres noeuds élémentaires
'''''''''''''''''''''''''''''''
Noeud SalomePython
+++++++++++++++++++
La définition d'un noeud SalomePython est quasiment identique à celle d'un `Noeud fonction Python`_. On utilise
la méthode createSInlineNode du runtime à la place de createFuncNode et on ajoute une information de
placement sur un container comme pour un noeud de service SALOME (méthode setContainer).

Voici un exemple semblable à celui de :ref:`schemaxml`::

  n2=r.createSInlineNode("","node2")
  p.edAddChild(n2)
  n2.setScript("""
  import salome
  salome.salome_init()
  import PYHELLO_ORB
  def f(p1):
    print __container__from__YACS__
    machine,container=__container__from__YACS__.split('/')
    param={'hostname':machine,'container_name':container}
    compo=salome.lcc.LoadComponent(param, "PYHELLO")
    print compo.makeBanner(p1)
    print p1
  """)
  n2.setFname("f")
  n2.edAddInputPort("p1",ts)
  n2.getComponent().setContainer(c1)

Noeud DataIn
+++++++++++++++
Pour définir un noeud DataIn, on utilise la méthode createInDataNode du runtime. Elle prend deux arguments
dont le premier doit être "" et le deuxième le nom du noeud.
Pour définir les données du noeud, on lui ajoute des ports de données de sortie avec la méthode edAddOutputPort
en lui passant le nom de la donnée et son type en arguments.
Pour initialiser la valeur de la donnée, on utilise la méthode setData du port ainsi créé en lui passant 
la valeur encodée en XML-RPC (voir :ref:`initialisation`).

Voici un exemple de noeud DataIn qui définit 2 données de type double (b et c) et une donnée de type fichier (f)::

  n=r.createInDataNode("","data1")
  p.edAddChild(n)
  pout=n.edAddOutputPort('a',td)
  pout.setData("<value><double>-1.</double></value>")
  pout=n.edAddOutputPort('b',td)
  pout.setData("<value><double>5.</double></value>")
  pout=n.edAddOutputPort('f',tf)
  pout.setData("<value><objref>f.data</objref></value>")
  
Il est possible d'affecter une valeur à une donnée directement avec un objet Python en utilisant
la méthode setDataPy. Exemple pour une séquence::

  pout.setDataPy([1.,5.])

Noeud DataOut
+++++++++++++++++
Pour définir un noeud DataOut, on utilise la méthode createOutDataNode du runtime. Elle prend deux arguments
dont le premier doit être "" et le deuxième le nom du noeud.
Pour définir les résultats du noeud, on lui ajoute des ports de données d'entrée en utilisant la 
méthode edAddInputPort avec le nom du résultat et son type en arguments.
Pour sauvegarder les résultats dans un fichier on utilise la méthode setRef du noeud avec le nom
du fichier en argument.
Pour recopier un résultat fichier dans un fichier local, on utilise la méthode setData du port correspondant
au résultat avec le nom du fichier en argument.

Voici un exemple de noeud DataOut qui définit des résultats (a, b, c, d, f) de différents 
types (double, int, string, vecteur de doubles, fichier) et écrit les valeurs correspondantes 
dans le fichier g.data. Le fichier résultat sera copié dans le fichier local monfich::

  n=r.createOutDataNode("","data2")
  n.setRef("g.data")
  p.edAddChild(n)
  n.edAddInputPort('a',td)
  n.edAddInputPort('b',ti)
  n.edAddInputPort('c',ts)
  n.edAddInputPort('d',tc3)
  pin=n.edAddInputPort('f',tf)
  pin.setData("monfich")

Noeud StudyIn
++++++++++++++
Pour définir un noeud StudyIn, on utilise la méthode createInDataNode du runtime. Elle prend deux arguments
dont le premier doit être "study" et le deuxième le nom du noeud.
Pour spécifier l'étude associée, on ajoute la propriété "StudyID" au noeud en utilisant sa méthode setProperty.
Pour définir les données du noeud, on lui ajoute des ports de données de sortie avec la méthode edAddOutputPort
en lui passant le nom de la donnée et son type en arguments.
Pour initialiser la donnée avec la référence dans l'étude, on utilise la méthode setData du port ainsi créé 
en lui passant une chaine de caractères qui contient soit l'Entry SALOME soit le chemin dans l'arbre d'étude.

Voici un exemple de noeud StudyIn qui définit 2 données de type GEOM_Object (a et b). 
L'étude est supposée chargée en mémoire par SALOME sous le StudyID 1. 
La donnée a est référencée par une Entry SALOME. La donnée b est référencée par un chemin dans l'arbre d'étude::

  n=r.createInDataNode("study","study1")
  p.edAddChild(n)
  n.setProperty("StudyID","1")
  pout=n.edAddOutputPort('a',tgeom)
  pout.setData("0:1:1:1")
  pout=n.edAddOutputPort('b',tgeom)
  pout.setData("/Geometry/Sphere_1")


Noeud StudyOut
++++++++++++++++
Pour définir un noeud StudyOut, on utilise la méthode createOutDataNode du runtime. Elle prend deux arguments
dont le premier doit être "study" et le deuxième le nom du noeud.
Pour spécifier l'étude associée, on ajoute la propriété "StudyID" au noeud en utilisant sa méthode setProperty.
Pour spécifier un nom de fichier dans lequel sera sauvegardée l'étude, on utilise la méthode setRef du noeud
avec le nom du fichier en argument.
Pour définir les résultats du noeud, on lui ajoute des ports de données d'entrée avec la méthode edAddInputPort
en lui passant le nom de la donnée et son type en arguments.
Pour associer l'entrée dans l'étude au résultat, on utilise la méthode setData du port 
en lui passant une chaine de caractères qui contient soit l'Entry SALOME soit le chemin dans l'arbre d'étude.

Voici un exemple de noeud StudyOut qui définit 2 résultats (a et b) de type GEOM_Object. 
L'étude utilisée a le studyId 1. Le résultat a est référencé par une Entry SALOME.
Le résultat b est référencé par un chemin.
L'étude complète est sauvegardée en fin de calcul dans le fichier study1.hdf::

  n=r.createOutDataNode("study","study2")
  n.setRef("study1.hdf")
  p.edAddChild(n)
  n.setProperty("StudyID","1")
  pout=n.edAddInputPort('a',tgeom)
  pout.setData("0:1:2:1")
  pout=n.edAddInputPort('b',tgeom)
  pout.setData("/Save/Sphere_2")


Sauvegarder un schéma de calcul dans un fichier XML
------------------------------------------------------
Pour sauvegarder un schéma de calcul dans un fichier au format XML, il faut utiliser la méthode saveSchema
du schéma de calcul en lui passant le nom du fichier en argument.
Pour qu'un schéma de calcul, construit en Python, puisse être sauvegardé sous une forme cohérente
dans un fichier XML, il ne faut pas oublier d'ajouter dans le dictionnaire des types du schéma
tous les types qui ont été définis en Python (voir :ref:`typedict`). La sauvegarde ne le fait pas automatiquement. 

Pour sauver le schéma p construit ci-dessus, dans le fichier monschema.xml, il faut écrire::

  p.saveSchema("monschema.xml")

Le fichier ainsi obtenu pourra ensuite être chargé comme dans :ref:`loadxml`.


Quelques opérations utiles
------------------------------
Retrouver un noeud par son nom
'''''''''''''''''''''''''''''''''''
Pour retrouver un noeud (objet Python) quand on n'a que l'objet schéma de calcul et le nom absolu du noeud, il 
suffit d'appeler la méthode getChildByName du schéma en lui passant le nom absolu.

Pour retrouver le noeud script Python défini en `Noeud script Python`_::

  n=p.getChildByName("node1")

Pour retrouver le noeud "node1" dans le bloc "b1"::

  n=p.getChildByName("b1.node1")

Cette opération est également utilisable à partir d'un noeud composé à condition d'utiliser le nom
relatif du noeud.
On peut réécrire l'exemple précédent::

  n=b.getChildByName("node1")

Retrouver un port par son nom
''''''''''''''''''''''''''''''''
Pour retrouver un port d'un noeud par son nom, il faut d'abord récupérer le noeud par son nom. Puis
on retrouve un port de données d'entrée avec la méthode getInputPort et un port de données de sortie
avec la méthode getOutputPort.

Voici un exemple à partir du noeud n précédent::

  pin=n.getOutputPort("p1")
  pout=n.getInputPort("p2")

Obtenir la valeur d'un port
''''''''''''''''''''''''''''''''
Pour obtenir la valeur d'un port on utilise sa méthode getPyObj.
Exemple::

  print pin.getPyObj()
  print pout.getPyObj()

Obtenir l'état d'un noeud
''''''''''''''''''''''''''''
Pour obtenir l'état d'un noeud on utilise sa méthode getEffectiveState (voir les valeurs
posssibles dans :ref:`etats`)

Retirer un noeud de son contexte
''''''''''''''''''''''''''''''''''
Il est possible de retirer un noeud de son noeud contexte en utilisant une méthode du contexte. 
Le nom de la méthode diffère selon le type de contexte.

- Pour un Bloc ou un schéma de calcul on utilisera la méthode edRemoveChild avec le noeud à retirer en argument::

    p.edRemoveChild(n)

- Pour une boucle (ForLoop, WhileLoop ou ForEachLoop) on utilisera la méthode edRemoveNode sans argument::

    l.edRemoveNode()

- Pour un Switch, on utilisera la méthode edRemoveChild avec le noeud interne concerné en argument::

    sw.edRemoveChild(nk1)

