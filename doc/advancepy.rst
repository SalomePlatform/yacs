
.. _advancepy:

Utilisation avancée de l'interface de programmation Python
==========================================================================

Passage d'objets Python entre noeuds de calcul
--------------------------------------------------
Le modèle de données standard de YACS permet d'échanger un certain nombre de types
de données (voir :ref:`datatypes`) qui sont limités aux types supportés par CORBA.
Le langage Python permet de manipuler des types de données qui ne sont pas gérés par YACS.
En particulier, le dictionnaire Python avec des types de données hétérogènes n'est pas géré
par le modèle de données de YACS.

Il est cependant possible d'échanger des dictionnaires python entre certains types de noeuds de calcul
en utilisant des références d'objets avec un protocole non standard. Le protocole standard est
le protocole IDL qui correspond à une sérialisation des données gérée par CORBA.
Il existe deux autres protocoles (python et json) qui utilisent des mécanismes de sérialisation
non CORBA qui supportent plus de types de données dont les dictionnaires.
Le nom du protocole apparait dans la première partie du Repositiory ID du type (avant le premier :).

Le protocole python
++++++++++++++++++++++
Le protocole python s'appuie sur une sérialisation faite par le module cPickle (implémentation C
du module pickle).
Il suffit de définir un type référence d'objet avec le protocole python pour pouvoir échanger
des objets python entre noeuds inline python et avec des composants SALOME implémentés en python.
Les composants SALOME implémentés en python qui veulent supporter ce type d'échange doivent être
conçus pour recevoir une chaine de caractères qui contient l'objet sérialisé. La désérialisation 
de l'objet reste à la charge du composant. Dans l'autre sens, la sérialisation est à la charge 
du composant qui doit retourner une chaine de caractères pour ce type d'objet.

Définition du type "objet python"::

  tc1=p.createInterfaceTc("python:obj:1.0","PyObj",[])

Définition de deux noeuds Python qui utilisent ce type::

  n2=r.createScriptNode("","node2")
  p.edAddChild(n2)
  n2.setScript("""
  p1={'a':1,'b':5.6,'c':[1,2,3]}
  """)
  n2.edAddOutputPort("p1",tc1)

  n3=r.createScriptNode("","node3")
  n3.setScript("""
  print "p1=",p1
  """)
  n3.edAddInputPort("p1",tc1)
  p.edAddChild(n3)
  p.edAddDFLink(n2.getOutputPort("p1"),n3.getInputPort("p1"))

Définition d'un noeud de service SALOME qui utilise ce type::

  n1=r.createCompoNode("","node1")
  n1.setRef("compo1")
  n1.setMethod("run")
  n1.edAddInputPort("p1",tc1)
  n1.edAddOutputPort("p1",tc1)
  p.edAddChild(n1)
  p.edAddDFLink(n2.getOutputPort("p1"),n1.getInputPort("p1"))

L'implémentation du composant compo1 doit prendre en charge la sérialisation/désérialisation
comme dans l'exemple de la méthode run qui suit::

  def run(self,s):
    o=cPickle.loads(s)
    ret={'a':6, 'b':[1,2,3]}
    return cPickle.dumps(ret,-1)
 
Le protocole json
++++++++++++++++++++++
Le protocole json s'appuie sur la sérialisation/désérialisation `JSON <http://www.json.org/>`_ 
(JavaScript Object Notation) à la place de cPickle. json supporte moins de types de données
et nécessite l'installation du module python simplejson mais il a l'avantage d'être
plus interopérable. En particulier, il existe des librairies C++ qui sérialisent/désérialisent
du JSON.

Pour utiliser ce protocole dans YACS, il suffit de remplacer python par json dans la définition
du type. Par exemple::

  tc1=p.createInterfaceTc("json:obj:1.0","PyObj",[])

Le reste est identique sauf l'implémentation du composant qui devient en reprenant l'exemple
ci-dessus::

  def run(self,s):
    o=simplejson.loads(s)
    ret={'a':6, 'b':[1,2,3]}
    return simplejson.dumps(ret)

Définition de composants Python inline
--------------------------------------------------
Normalement, un composant SALOME Python doit être développé en dehors de YACS soit
à la main soit en utilisant un générateur de module SALOME comme :ref:`yacsgen`.
Il est possible de définir un composant SALOME implémenté en Python directement
dans un script python. Ce type de composant peut être utile dans les phases de test,
par exemple.

La première étape consiste à compiler l'interface IDL directement dans le script python
ce qui a pour effet de créer les modules Python CORBA nécessaires. Par exemple, voici
comment on produit les modules CORBA compo1 et compo1__POA qui contiennent l'interface 
base avec une seule méthode run::

  idlcompo="""
  #include "DSC_Engines.idl"
  #include "SALOME_Exception.idl"
  module compo1{
    interface base :Engines::Superv_Component {
      string run(in string s) raises (SALOME::SALOME_Exception);
    };
  };
  """
  m=omniORB.importIDLString(idlcompo,["-I/local/chris/SALOME2/RELEASES/Install/KERNEL_V4_0/idl/salome"])

La deuxième étape consiste à définir le corps du composant compo1 et donc de sa méthode run.

Voici un exemple de définition faite dans le corps du script Python::

  import compo1
  import compo1__POA

  class compo(compo1__POA.base,dsccalcium.PyDSCComponent):
    def run(self,s):
      print "+++++++++++run+++++++++++",s
      return "Bien recu"+s

  compo1.compo1=compo

Ce qui est important ici, c'est que SALOME trouve dans le module compo1, la classe de même nom
qui représente le composant (d'où la dernière ligne).

La troisième étape consiste à définir un container SALOME local au script car ce composant n'a
d'existence que dans le script. La définition d'un container de nom "MyContainerPy" se fera
comme suit::

  from omniORB import CORBA
  from SALOME_ContainerPy import SALOME_ContainerPy_i
  orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
  poa = orb.resolve_initial_references("RootPOA")
  poaManager = poa._get_the_POAManager()
  poaManager.activate()
  cpy_i = SALOME_ContainerPy_i(orb, poa, "MyContainerPy")

en prenant bien garde à activer CORBA avec poaManager.activate().

Ensuite, il ne reste plus qu'à créer un container YACS et à y placer un noeud SALOME 
comme pour un composant standard.
