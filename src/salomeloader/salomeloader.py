# -*- coding: iso-8859-1 -*-
"""Ce module sert pour lire un schema de calcul Salome décrit en XML.
   et le convertir en schema de calcul YACS

   Cette lecture est réalisée au moyen de la classe SalomeLoader 
   et de sa méthode load.

"""

import sys,os
try:
  import cElementTree as ElementTree
except ImportError:
  import ElementTree

from sets import Set
import pilot
import SALOMERuntime

class UnknownKind(Exception):pass

debug=0
typeMap={}
streamTypes={
             '0':"Unknown",
             '1':"CALCIUM_int",
             '3':"CALCIUM_real",
            }

#Les fonctions suivantes : invert, reachable,InducedSubgraph,write_dot,display
#permettent de realiser des operations sur un graphe.
#Est considere comme un graphe un objet G qui supporte les
#operations suivantes : l'iteration sur les noeuds (for n in G parcourt
#tous les noeuds du graphe) et l'iteration sur les suivants
# (for v in G[n] parcourt tous les suivants du noeud n)
def invert(G):
  """Construit le graphe inverse de G en inversant les liens de voisinage"""
  I={}
  for n in G:
    I.setdefault(n,Set())
    for v in G[n]:
      I.setdefault(v,Set()).add(n)
  return I

def reachable(G,n):
  """Construit le set de noeuds atteignables depuis le noeud n

     Le noeud n n'est pas dans le set retourné sauf en cas de boucles
     Ce cas n'est pas traité ici (limitation)
  """
  s=G[n]
  for v in G[n]:
    s=s|reachable(G,v)
  return s

def InducedSubgraph(V,G,adjacency_list_type=Set):
  """ Construit un sous graphe de G avec les noeuds contenus dans V  """
  def neighbors(x):
    for y in G[x]:
      if y in V:
        yield y
  return dict([(x,adjacency_list_type(neighbors(x))) for x in G if x in V])

def write_dot(stream,G):
  """Ecrit la representation (au format dot) du graphe G dans le fichier stream"""
  name="toto"
  stream.write('digraph %s {\nnode [ style="filled" ]\n' % name)
  for node in G :
    try:
      label = "%s:%s"% (node.name,node.__class__.__name__)
    except:
      label=str(node)
    color='green'
    stream.write('   %s [fillcolor="%s" label=< %s >];\n' % ( id(node), color, label))
  for src in G:
    for dst in G[src]:
      stream.write('   %s -> %s;\n' % (id(src), id(dst)))
  stream.write("}\n")

def display(G,suivi="sync"):
  """Affiche le graphe G avec l'outil dot"""
  f=file("graph.dot", 'w')
  write_dot(f,G)
  f.close()
  cmd="dot -Tpng graph.dot |display" + (suivi == "async" and "&" or "")
  os.system(cmd)
      

def test():
  G={
  1:Set([2,3]),
  2:Set([4]),
  3:Set([5]),
  4:Set([6]),
  5:Set([6]),
  6:Set(),
  }
  display(G)
  I=invert(G)
  print reachable(G,2)
  print reachable(I,6)
  print reachable(G,2) & reachable(I,6)

#Fin des fonctions graphe

currentProc=None

class SalomeLoader:
  """Loader de schéma Salome natif et convertisseur en schéma 'nouvelle formule'.
     La méthode loadxml parse le fichier xml et retourne un objet SalomeProc 
     natif non converti
     La méthode load parse le fichier xml et retourne un objet représentant 
     un schéma Salome converti (objet Proc de YACS)
  """

  def loadxml(self,filename):
    """
       Lit un fichier XML du SUPERV de Salome et retourne la liste des 
       procedures (instances de SalomeProc)
    """
    tree = ElementTree.ElementTree(file=filename)
    root = tree.getroot()
    if debug:print "root.tag:",root.tag,root

    procs=[]
    if root.tag == "dataflow":
      #un seul dataflow
      dataflow=root
      if debug:print dataflow
      proc=SalomeProc(dataflow)
      procs.append(proc)
    else:
      #un ou plusieurs dataflow. Le schema contient des macros. 
      # Toutes les macros sont
      #décrites au meme niveau dans le fichier XML.
      for dataflow in root.findall("dataflow"):
        if debug:print dataflow
        proc=SalomeProc(dataflow)
        if debug:print "nom du dataflow:",proc.name
        procs.append(proc)
    return procs

  def load(self,filename):
    """Lit un fichier XML et convertit les procedures lues en procedures 
       nouvelle formule
    """
    global currentProc
    procs=self.loadxml(filename)
    #on récupère le schema de tete d'un coté et les éventuelles 
    #macros de l'autre: procs[0]
    proc=procs.pop(0)
    #proc.display()

    #Enregistrement des éventuelles macros dans macro_dict
    macro_dict={}
    for p in procs:
      if debug:print "proc_name:",p.name,"coupled_node:",p.coupled_node
      macro_dict[p.name]=p

    if debug:print filename
    yacsproc=ProcNode(proc,macro_dict)
    return yacsproc.createNode()

class Service:
    """Classe pour porter les caractéristiques d'un service"""
class Parameter:
    """Classe pour porter les caractéristiques d'un parametre"""
class Link:
    """Classe pour porter les caractéristiques d'un link"""
class Data:
    """Classe pour porter les caractéristiques d'un data"""

class Node:
    """Node de calcul simple : classe de base à spécialiser """
    label="Node: "
    def __init__(self):
      self.links=[]    # liste pour stocker les entrees sous forme de link
      # le link doit avoir deux attributs : from_node qui donne le node origine 
      # et to_node qui donne le node cible
      self.datas=[]
      self.inStreamLinks=[] #liste des liens dataStream associés à ce node (in)
      self.outStreamLinks=[] #liste des liens dataStream associés à ce node (out)
    def createNode(self):
      raise NotImplementedError
    def getInputPort(self,p):
      return self.node.getInputPort(".".join(p.split("__")))
    def getOutputPort(self,p):
      return self.node.getOutputPort(".".join(p.split("__")))
    def getInputDataStreamPort(self,p):
      return self.node.getInputDataStreamPort(p)
    def getOutputDataStreamPort(self,p):
      return self.node.getOutputDataStreamPort(p)

class InlineNode(Node):
    """Node de calcul inline salome : fonction dans self.codes[0]"""
    def __init__(self):
      Node.__init__(self)
      self.codes=[]
    def createNode(self):
      r = pilot.getRuntime()
      if self.fnames[0] == "?":
        n=r.createScriptNode("",self.name)
      else:
        n=r.createFuncNode("",self.name)
        n.setFname(self.fnames[0])
        n.setScript(self.codes[0])
      self.node=n
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        n.edAddInputPort(para.name,typeMap[para.type])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        n.edAddOutputPort(para.name,typeMap[para.type])

      return n

class ComputeNode(Node):
    """Node de calcul pour exécuter un service Salome"""
    def createNode(self):
      r = pilot.getRuntime()
      n=r.createCompoNode("",self.name)
      n.setRef(self.sComponent)
      n.setMethod(self.service.name)
      self.node=n

      #ajout des ports in et out du service
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #on cree le type manquant et on l'ajoute dans la table des types
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        n.edAddInputPort(para.name,typeMap[para.type])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #on cree le type manquant et on l'ajoute dans la table des types
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        pout=n.edAddOutputPort(para.name,typeMap[para.type])

      #ajout des ports datastream in et out
      for para in self.inStreams:
        if debug:print para.name,para.type,para.dependency,para.schema, para.interpolation,
        if debug:print para.extrapolation
        pin=n.edAddInputDataStreamPort(para.name,typeMap[streamTypes[para.type]])
      for para in self.outStreams:
        if debug:print para.name,para.type,para.dependency,para.values
        pout=n.edAddOutputDataStreamPort(para.name,typeMap[streamTypes[para.type]])

      return n

class ComposedNode(Node):
  """Node de calcul composite Salome (classe de base)"""

  def reduceLoop(self):
    """Transforme un graphe de type Salome avec les boucles
       a plat en un graphe hierarchique 
       Le graphe de depart (Salome) est dans self.G.
       On le transforme en place
    """
    G=self.G
    if debug:display(G)
    #calcul du graphe inverse
    I=invert(G)
    #display(I)

    #on recherche toutes les boucles et leurs noeuds internes
    loops={}
    for n in G:
      if n.kind == 4:
        #Debut de boucle
        loops[n]=reachable(G,n)&reachable(I,n.endloop)
        n.inner_nodes=loops[n]
        n.G=InducedSubgraph(loops[n],G)

    if debug:print "toutes les boucles du graphe"
    if debug:print loops

    #on recherche les boucles les plus externes
    outer_loops=loops.keys()
    for l in loops:
      for ll in outer_loops:
        if loops[l] < loops[ll]:
          #boucle interne
          outer_loops.remove(l)
          ll.set_inner(l)
          break

    #a la fin, les boucles restantes dans outer_loops sont les plus externes
    if debug:print outer_loops

    #on supprime les noeuds internes des boucles les plus externes
    for l in outer_loops:
      #on enleve les noeuds internes
      for n in loops[l]:
        del G[n]
      #on enleve le noeud endloop
      suiv=G[l.endloop]
      del G[l.endloop]
      #on remplace les noeuds suivants de loop par ceux de endloop
      G[l]= suiv

      #on tente de transformer les liens entrants et sortants sur le noeud endloop
      #en noeuds directs. On ne traite probablement pas tous les cas.
      inputs={}
      for link in l.endloop.links:
        if debug:print link.from_node,link.to_node,link.from_param,link.to_param
        inputs[link.to_param]=link.from_node,link.from_param

      for s in suiv:
        for link in s.links:
          if link.from_node == l.endloop:
            link.from_node,link.from_param=inputs[link.from_param]
          if debug:print link.from_node,link.to_node,link.from_param,link.to_param

      if debug:display(G)

      #on applique le traitement de reduction aux boucles les plus externes (recursif)
      for l in outer_loops:
        l.reduceLoop()

  def connect_macros(self,macro_dict):
    """Cette methode rattache les macros salome contenues dans macro_dict
       a la procedure YACS proc
       On est ici dans le noeud auquel on veut rattacher une des macros
       macro_dict est un dictionnaire dont la cle est le nom de la macro
       et la valeur est un graphe Salome (objet SalomeProc)
       Les noeuds concernes sont les MacroNode et les SalomeProc
    """
    if debug:print "connect_macros",self.node,macro_dict
    for node in self.G:
      if isinstance(node,MacroNode):
        #c'est une macro, il faut rattacher sa description
        #p est le sous graphe Salome (objet SalomeProc)
        #node est le MacroNode Salome qui utilise le sous graphe p
        #node.node est le bloc YACS equivalent
        p=macro_dict[node.coupled_node]
        bloc=node.node
        if debug:print "macronode:",node.name,node.coupled_node,p
        #a partir de la procédure salome a plat on cree un
        #graphe d'exécution hiérarchique nouvelle formule
        G=p.create_graph()
        node.G=G
        for n in G:
          #chaque noeud du graphe G cree un noeud YACS equivalent
          nod=n.createNode()
          bloc.edAddChild(nod)

        #on demande le rattachement des macros aux nodes du macroNode node
        node.connect_macros(macro_dict)

        #on ajoute les liens de controle
        for n in G:
          for v in G[n]:
            bloc.edAddCFLink(n.node,v.node)
        #on ajoute les liens de donnees et les initialisations
        for n in G:
          #liens dataflow
          for l in n.links:
            bloc.edAddLink(l.from_node.getOutputPort(l.from_param),
                        l.to_node.getInputPort(l.to_param))
          #liens datastream
          for l in n.outStreamLinks:
            pout=l.from_node.getOutputDataStreamPort(l.from_param)
            pin=l.to_node.getInputDataStreamPort(l.to_param)
            bloc.edAddLink(pout,pin)
          #initialisations
          for l in n.datas:
            if l.type == 7:
              #double
              n.getInputPort(l.tonodeparam).edInitDbl(l.value)
            elif l.type == 3:
              #int
              n.getInputPort(l.tonodeparam).edInitInt(l.value)

class LoopNode(ComposedNode):
    """Objet qui simule le comportement d'une boucle Salome."""
    def __init__(self):
      ComposedNode.__init__(self)
      self.inner_loops=[]
      #inner_nodes contient les noeuds internes au sens Salome (a plat
      #avec les noeuds endloop)
      self.inner_nodes=[]

    def set_node(self,node):
      self.node=node

    def set_inner(self,loop):
      for i in self.inner_loops:
        if loop.inner_nodes < i.inner_nodes:
          #la boucle est contenue dans i
          i.set_inner(loop)
          break
      self.inner_loops.append(loop)

    def createNode(self):
      """Cree l'objet boucle equivalent

         Un objet boucle Salome a n ports d'entrée et les memes ports en sortie.
         La tete de boucle a 3 fonctions : init, next, more qui ont des signatures
         tres voisines. init et next ont la meme signature : en entree les parametres
         d'entree de la boucle et en sortie les parametres de sortie de la boucle c'est
         à dire les memes qu'en entrée. more a les memes parametres d'entree et a un 
         parametre de sortie supplementaire qui vient en premiere position. Ce 
         parametre indique si la boucle doit etre poursuivie ou stoppée.
         La fin de boucle a une fonction qui a la meme signature que next.

         Pour transformer ce type de boucle, on crée un ensemble de noeuds de calcul
         regroupés dans un bloc. Dans ce bloc, on crée un noeud externe pour init suivi
         d'une boucle while.
         Ensuite on crée un bloc qui contiendra 2 noeuds (next et more) plus tous 
         les noeuds internes de la boucle.
      """

      r = pilot.getRuntime()
      bloop=r.createBloc(self.name)

      #noeud init
      init=r.createFuncNode("","init")
      #print self.codes[0]
      init.setScript(self.codes[0])
      init.setFname(self.fnames[0])
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        init.edAddInputPort(para.name,typeMap[para.type])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        init.edAddOutputPort(para.name,typeMap[para.type])
      bloop.edAddChild(init)
      self.init=init

      wh=r.createWhileLoop(self.name)
      bloop.edAddChild(wh)
      blnode=r.createBloc(self.name)
      wh.edSetNode(blnode)
      cport=wh.edGetConditionPort()
      cport.edInitBool(True)

      #noeud next
      next=r.createFuncNode("","next")
      #print self.codes[2]
      next.setScript(self.codes[2])
      next.setFname(self.fnames[2])
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        next.edAddInputPort(para.name,typeMap[para.type])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        next.edAddOutputPort(para.name,typeMap[para.type])
      blnode.edAddChild(next)
      self.next=next

      #noeud more
      more=r.createFuncNode("","more")
      #print self.codes[1]
      more.setScript(self.codes[1])
      more.setFname(self.fnames[1])
      for para in self.service.inParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        more.edAddInputPort(para.name,typeMap[para.type])
      more.edAddOutputPort("DoLoop",typeMap["int"])
      for para in self.service.outParameters:
        if not typeMap.has_key(para.type):
          #create the missing type and add it in type map
          typeMap[para.type]= currentProc.createInterfaceTc("",para.type,[])
          currentProc.typeMap[para.type]=typeMap[para.type]
        more.edAddOutputPort(para.name,typeMap[para.type])
      blnode.edAddChild(more)
      self.more=more

      for para in self.service.outParameters:
        bloop.edAddDFLink(init.getOutputPort(para.name),next.getInputPort(para.name))

      for para in self.service.outParameters:
        blnode.edAddDFLink(next.getOutputPort(para.name),more.getInputPort(para.name))

      wh.edAddLink(more.getOutputPort("DoLoop"),wh.getInputPort("condition"))

      for para in self.service.outParameters:
        wh.edAddLink(more.getOutputPort(para.name),next.getInputPort(para.name))

      self.node=bloop

      for n in self.G:
        node=n.createNode()
        blnode.edAddChild(node)

      for n in self.G:
        for v in self.G[n]:
          blnode.edAddCFLink(n.node,v.node)

      for n in self.G:
        for l in n.links:
          print l.from_node.name,l.to_node.name
          print l.from_param,l.to_param
          blnode.edAddDFLink(l.from_node.getOutputPort(l.from_param),
                             l.to_node.getInputPort(l.to_param))

      return bloop

    def getInputPort(self,p):
      return self.init.getInputPort(p)

    def getOutputPort(self,p):
      return self.more.getOutputPort(p)

class Bloc(ComposedNode):
    """ Objet composé d'un ensemble de nodes enchaines et qui se 
        comporte comme un node simple.
    """
    label="Bloc: "
    def __init__(self):
      Node.__init__(self)
      self.nodes=[]

    def addLink(self,node1,node2):
      if node1 not in self.nodes:self.nodes.append(node1)
      if node2 not in self.nodes:self.nodes.append(node2)

class MacroNode(Bloc):
  """Objet qui représente une Macro Salome c'est a dire un node 
     composite avec une interface : ports in et out.
  """
  def createNode(self):
    """Cree l'objet correspondant a une Macro Salome : un Bloc"""
    r = pilot.getRuntime()
    macro=r.createBloc(self.name)
    self.node=macro
    return macro

def is_loop(n):
  """Indique si n est un node de début de boucle"""
  return isinstance(n,LoopNode)

class ProcNode(ComposedNode):
  """Procedure YACS equivalente a une procedure Salome accompagnee
     de ses macros
  """
  def __init__(self,proc,macro_dict):
    ComposedNode.__init__(self)
    self.proc=proc
    self.macro_dict=macro_dict

  def createNode(self):
    """Cree l'objet YACS equivalent"""
    global currentProc
    r = pilot.getRuntime()

    #create_graph retourne un graphe representatif de la
    #procedure Salome transformee en un graphe hierarchique
    G=self.proc.create_graph()
    self.G=G

    #on utilise le graphe G pour construire la
    #procedure YACS equivalente p
    p=r.createProc("pr")
    self.node=p
    currentProc=p
    typeMap["double"]=p.typeMap["double"]
    typeMap["int"]=p.typeMap["int"]
    typeMap["long"]=p.typeMap["int"]
    typeMap["string"]=p.typeMap["string"]
    typeMap["bool"]=p.typeMap["bool"]
    typeMap["Unknown"]=p.createInterfaceTc("","Unknown",[])
    typeMap["GEOM_Object"]=p.createInterfaceTc("","GEOM_Object",[])
    typeMap["GEOM_Shape"]=typeMap["GEOM_Object"]
    typeMap["CALCIUM_int"]=p.createInterfaceTc("","CALCIUM_int",[])
    typeMap["CALCIUM_real"]=p.createInterfaceTc("","CALCIUM_real",[])
    currentProc.typeMap["Unknown"]=typeMap["Unknown"]
    currentProc.typeMap["GEOM_Object"]=typeMap["GEOM_Object"]
    currentProc.typeMap["GEOM_Shape"]=typeMap["GEOM_Shape"]
    currentProc.typeMap["CALCIUM_int"]=typeMap["CALCIUM_int"]
    currentProc.typeMap["CALCIUM_real"]=typeMap["CALCIUM_real"]

    for n in G:
      #chaque noeud du graphe G cree un noeud YACS equivalent
      node=n.createNode()
      p.edAddChild(node)

    #on demande le rattachement des macros aux nodes de la procédure p
    self.connect_macros(self.macro_dict)

    #on ajoute les liens de controle
    for n in G:
      for v in G[n]:
        p.edAddCFLink(n.node,v.node)

    #on ajoute les liens de donnees et les initialisations
    for n in G:
      #liens dataflow
      for l in n.links:
        print l.from_node.name,l.to_node.name
        print l.from_param,l.to_param
        p.edAddLink(l.from_node.getOutputPort(l.from_param),
                    l.to_node.getInputPort(l.to_param))

      #liens datastream
      for l in n.outStreamLinks:
        pout=l.from_node.getOutputDataStreamPort(l.from_param)
        pin=l.to_node.getInputDataStreamPort(l.to_param)
        p.edAddLink(pout,pin)
      #initialisations
      for l in n.datas:
        if l.type == 7:
          #double
          n.getInputPort(l.tonodeparam).edInitDbl(l.value)
        elif l.type == 3:
          #int
          n.getInputPort(l.tonodeparam).edInitInt(l.value)

    return p


class SalomeProc(ComposedNode):
    """Objet pour décrire un schéma Salome natif avec ses liens 
       dataflow, datastream et gate
       L'objet est construit en parsant un fichier XML
    """
    def __init__(self,dataflow):
        self.name="name"
        self.parse(dataflow)
        #self.links : liste des liens dataflow du graphe (objets Link)
        #self.nodes : liste des noeuds du graphe
        #self.node_dict : le dictionnaire des noeuds (nom,node)
        #self.datas : liste des datas du graphe
        #chaque noeud a 2 listes de liens datastream (inStreams, outStreams)

    def parse(self,dataflow):
        if debug:print "Tous les noeuds XML"
        for node in dataflow:
            if debug:print node.tag,node

        #Récupération des informations du dataflow
        self.dataflow_info=self.parseService(dataflow.find("info-list/node/service"))
        if debug:print self.dataflow_info
        if debug:print self.dataflow_info.inParameters
        if debug:print self.dataflow_info.outParameters
        if debug:
            for para in self.dataflow_info.inParameters:
                print "inParam:",para.name,para.name.split("__",1)

        self.name=dataflow.findtext("info-list/node/node-name")
        self.coupled_node=dataflow.findtext("info-list/node/coupled-node")

        if debug:print "Tous les noeuds XML dataflow/node-list"
        nodes=[]
        node_dict={}
        #on parcourt tous les noeuds
        for n in dataflow.findall('node-list/node'):
            #n est un node de node-list
            kind=n.findtext("kind")
            comp=n.find("component-name").text
            name=n.find("node-name").text
            coupled_node=n.find("coupled-node").text
            interface=n.find("interface-name").text
            container=n.find("container").text

            #kind=1 : dataflow ?
            #kind=2 : ?
            #kind=9 : schema avec datastream ?

            if kind == "0":
              #Il s'agit d'un service
              node=ComputeNode()
              node.kind=0
              node.sComponent = comp
              node.interface=interface
            elif kind == "3":
              #il s'agit d'une fonction
              node=InlineNode()
              node.kind=3
              codes=[]
              fnames=[]
              for pyfunc in n.findall("PyFunction-list/PyFunction"):
                fnames.append(pyfunc.findtext("FuncName"))
                codes.append(self.parsePyFunction(pyfunc))
              node.fnames=fnames
              node.codes=codes
            elif kind == "4":
              #si kind vaut 4 on a une boucle : on crée un LoopNode
              #les fonctions python (next, more, init) sont stockées dans codes
              node=LoopNode()
              node.kind=4
              codes=[]
              fnames=[]
              for pyfunc in n.findall("PyFunction-list/PyFunction"):
                fnames.append(pyfunc.findtext("FuncName"))
                codes.append(self.parsePyFunction(pyfunc))
              node.fnames=fnames
              node.codes=codes
            elif kind == "5":
              #noeud de fin de boucle : on crée un InlineNode
              node=InlineNode()
              node.kind=5
              codes=[]
              fnames=[]
              for pyfunc in n.findall("PyFunction-list/PyFunction"):
                fnames.append(pyfunc.findtext("FuncName"))
                codes.append(self.parsePyFunction(pyfunc))
              node.fnames=fnames
              node.codes=codes
            elif kind == "10":
              #si kind vaut 10 on a un noeud Macro : on cree un MacroNode
              node=MacroNode()
              node.kind=10
            else:
              raise UnknownKind,kind

            node.name=name
            node.container=container
            node.service=None
            node.coupled_node=coupled_node
            #on stocke les noeuds dans un dictionnaire pour faciliter les recherches
            node_dict[node.name]=node
            if debug:print "\tnode-name",node.name
            if debug:print "\tkind",node.kind,node.__class__.__name__
            if debug:print "\tcontainer",node.container

            s=n.find("service")
            if s:
                node.service=self.parseService(s)


            #on parcourt les ports datastream
            if debug:print "DataStream"
            inStreams=[]
            for indata in n.findall("DataStream-list/inParameter"):
                inStreams.append(self.parseInData(indata))
            node.inStreams=inStreams
            outStreams=[]
            outStreams_dict={}
            for outdata in n.findall("DataStream-list/outParameter"):
                p=self.parseOutData(outdata)
                outStreams.append(p)
                outStreams_dict[p.name]=p
            node.outStreams=outStreams
            node.outStreams_dict=outStreams_dict
            if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"
            nodes.append(node)

        self.nodes=nodes
        self.node_dict=node_dict
        #Le parcours des noeuds est fini.
        #On parcourt les connexions dataflow et datastream
        """
        <link>
        <fromnode-name>Node_A_1</fromnode-name>
        <fromserviceparameter-name>a_1</fromserviceparameter-name>
        <tonode-name>Node_B_1</tonode-name>
        <toserviceparameter-name>b_1</toserviceparameter-name>
        <coord-list/>
        </link>
        """
        if debug:print "Tous les noeuds XML dataflow/link-list"
        links=[]
        if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"
        for link in dataflow.findall('link-list/link'):
            l=Link()
            l.from_name=link.findtext("fromnode-name")
            l.to_name=link.findtext("tonode-name")
            l.from_param=link.findtext("fromserviceparameter-name")
            l.to_param=link.findtext("toserviceparameter-name")
            links.append(l)
            if debug:print "\tfromnode-name",l.from_name
            if debug:print "\tfromserviceparameter-name",l.from_param
            if debug:print "\ttonode-name",l.to_name
            if debug:print "\ttoserviceparameter-name",l.to_param
            if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"

        self.links=links
        if debug:print "Tous les noeuds XML dataflow/data-list"
        datas=[]
        for data in dataflow.findall('data-list/data'):
            d=self.parseData(data)
            datas.append(d)
            if debug:print "\ttonode-name",d.tonode
            if debug:print "\ttoserviceparameter-name",d.tonodeparam
            if debug:print "\tparameter-value",d.value
            if debug:print "\tparameter-type",d.type
            if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"

        self.datas=datas

    def parseService(self,s):
        service=Service()
        service.name=s.findtext("service-name")
        if debug:print "\tservice-name",service.name

        inParameters=[]
        for inParam in s.findall("inParameter-list/inParameter"):
            p=Parameter()
            p.name=inParam.findtext("inParameter-name")
            p.type=inParam.findtext("inParameter-type")
            if debug:print "\tinParameter-name",p.name
            if debug:print "\tinParameter-type",p.type
            inParameters.append(p)
        service.inParameters=inParameters
        if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"

        outParameters=[]
        for outParam in s.findall("outParameter-list/outParameter"):
            p=Parameter()
            p.name=outParam.findtext("outParameter-name")
            p.type=outParam.findtext("outParameter-type")
            if debug:print "\toutParameter-name",p.name
            if debug:print "\toutParameter-type",p.type
            outParameters.append(p)
        service.outParameters=outParameters
        if debug:print "\t++++++++++++++++++++++++++++++++++++++++++++"
        return service

    def parseData(self,d):
        da=Data()
        da.tonode=d.findtext("tonode-name")
        da.tonodeparam=d.findtext("toserviceparameter-name")
        da.value=d.findtext("data-value/value")
        da.type=eval(d.findtext("data-value/value-type"))
        if da.type < 9:
            da.value=eval(da.value)
        return da

    def parsePyFunction(self,pyfunc):
        if debug:print pyfunc.tag,":",pyfunc
        if debug:print "\tFuncName",pyfunc.findtext("FuncName")
        text=""
        for cdata in pyfunc.findall("PyFunc"):
            if text:text=text+'\n'
            text=text+ cdata.text
        return text

    """<inParameter-type>1</inParameter-type>
    <inParameter-name>istream</inParameter-name>
    <inParameter-dependency>2</inParameter-dependency>
    <inParameter-schema>0</inParameter-schema>
    <inParameter-interpolation>0</inParameter-interpolation>
    <inParameter-extrapolation>0</inParameter-extrapolation>
    </inParameter>
    <outParameter>
    <outParameter-type>1</outParameter-type>
    <outParameter-name>ostream</outParameter-name>
    <outParameter-dependency>2</outParameter-dependency>
    <outParameter-values>0</outParameter-values>
    </outParameter>
    """

    def parseInData(self,d):
        if debug:print d.tag,":",d
        p=Parameter()
        p.name=d.findtext("inParameter-name")
        p.type=d.findtext("inParameter-type")
        p.dependency=d.findtext("inParameter-dependency")
        p.schema=d.findtext("inParameter-schema")
        p.interpolation=d.findtext("inParameter-interpolation")
        p.extrapolation=d.findtext("inParameter-extrapolation")
        if debug:print "\tinParameter-name",p.name
        return p

    def parseOutData(self,d):
        if debug:print d.tag,":",d
        p=Parameter()
        p.name=d.findtext("outParameter-name")
        p.type=d.findtext("outParameter-type")
        p.dependency=d.findtext("outParameter-dependency")
        p.values=d.findtext("outParameter-values")
        if debug:print "\toutParameter-name",p.name
        return p

    def create_graph(self):
      #un graphe est un dictionnaire dont la clé est un noeud et la valeur
      #est la liste (en fait un set sans doublon) des noeuds voisins suivants
      #for v in graphe (python >= 2.3): parcourt les noeuds du graphe
      #for v in graphe[noeud] parcourt les voisins (suivants) de noeud
      G={}
      #on cree tous les noeuds avec un voisinage (suivants) vide
      for n in self.nodes:
        G[n]=Set()

      #on construit le voisinage en fonction des divers liens
      for link in self.links:
        from_node=self.node_dict[link.from_name]
        if link.from_param == "Gate" or link.to_param == "Gate":
          #control link salome : on ajoute le noeud to_name dans les voisins
          if debug:print "ajout control link",link.from_name,link.to_name
          G[self.node_dict[link.from_name]].add(self.node_dict[link.to_name])

        elif from_node.outStreams_dict.has_key(link.from_param):
          #lien datastream salome : 
          # 1- on ajoute le lien dans les listes de liens des noeuds
          # 2- on ajoute dans le lien des pointeurs sur les noeuds (from_node et to_node)
          if debug:print "ajout stream link",link.from_name,link.to_name
          self.node_dict[link.to_name].inStreamLinks.append(link)
          self.node_dict[link.from_name].outStreamLinks.append(link)
          link.from_node=self.node_dict[link.from_name]
          link.to_node=self.node_dict[link.to_name]

        else:
          #autre lien salome 
          #si c'est un lien entre Loop node et EndOfLoop node, on l'ignore
          #tous les autres sont conservés
          from_node=self.node_dict[link.from_name]
          to_node=self.node_dict[link.to_name]
          if isinstance(to_node,LoopNode):
            #Est-ce le lien entre EndOfLoop et Loop ? Si oui, on ne le garde pas
            if to_node.coupled_node == from_node.name:
              if debug:print "lien arriere loop:",from_node,to_node
              #lien ignoré
              continue
          if debug:print "ajout dataflow link",link.from_name,link.to_name
          G[self.node_dict[link.from_name]].add(self.node_dict[link.to_name])

          if link.from_param != "DoLoop" and link.to_param != "DoLoop":
            #Les liens sur parametre DoLoop servent au superviseur Salome, on les ignore
            #on ajoute dans le lien des pointeurs sur les noeuds (from_node et to_node)
            #on ajoute ce lien à la liste des liens du noeud cible (to) 
            self.node_dict[link.to_name].links.append(link)
            link.from_node=self.node_dict[link.from_name]
            link.to_node=self.node_dict[link.to_name]

          #Dans un graphe salome avec boucles, les noeuds de tete et de fin
          #de boucle sont reliés par 2 liens de sens opposés
          #on stocke le noeud de fin dans l'attribut endloop du noeud de tete.
          if link.from_param == "DoLoop" and link.to_param == "DoLoop" \
             and is_loop(self.node_dict[link.from_name]) \
             and isinstance(self.node_dict[link.to_name],InlineNode):
            #on repère le node inline de fin de boucle en le stockant 
            #dans l'attribut endloop du node loop
            #self.node_dict[link.to_name] est le node de fin de boucle 
            #de self.node_dict[link.from_name]
            if debug:print "ajout loop",link.from_name,link.to_name
            self.node_dict[link.from_name].endloop=self.node_dict[link.to_name]
            self.node_dict[link.to_name].loop=self.node_dict[link.from_name]

      for data in self.datas:
        self.node_dict[data.tonode].datas.append(data)

      self.G=G

      #on modifie le graphe en place : 
      # transformation des boucles a plat en graphe hierarchique
      self.reduceLoop()

      #on peut maintenant créer le schéma de calcul YACS
      return G

    def display(self,suivi="sync"):
        """Visualise la procedure Salome avec dot"""
        #pour visualiser : dot -Tpng salome.dot |display
        f=file("salome.dot", 'w')
        self.write_dot(f)
        f.close()
        cmd="dot -Tpng salome.dot |display" + (suivi == "async" and "&" or "")
        os.system(cmd)

    def write_dot(self,stream):
        """Dumpe la procedure Salome dans stream au format dot"""
        stream.write('digraph %s {\nnode [ style="filled" ]\n' % self.name)
        for node in self.nodes:
            label = "%s:%s"% (node.name,node.__class__.__name__)
            color='green'
            stream.write('   %s [fillcolor="%s" label=< %s >];\n' % (
                    id(node), color, label
                ))
        for link in self.links:
            from_node=self.node_dict[link.from_name]
            to_node=self.node_dict[link.to_name]
            stream.write('   %s -> %s;\n' % (id(from_node), id(to_node)))
        stream.write("}\n")

if __name__ == "__main__":

  import traceback
  usage ="""Usage: %s salomeFile convertedFile
    where salomeFile is the name of the input schema file (old Salome syntax)
    and convertedFile is the name of the output schema file (new YACS syntax)
    """
  try:
    salomeFile=sys.argv[1]
    convertedFile=sys.argv[2]
  except :
    print usage%(sys.argv[0])
    sys.exit(1)

  SALOMERuntime.RuntimeSALOME_setRuntime()
  loader=SalomeLoader()

  try:
    p= loader.load(salomeFile)
    s= pilot.SchemaSave(p)
    s.save(convertedFile)
  except:
    traceback.print_exc()
    f=open(convertedFile,'w')
    f.write("<proc></proc>\n")

