# -*- coding: iso-8859-1 -*-
# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""
  This module contains graph utilities 

  Following functions : invert, reachable,InducedSubgraph,write_dot,display operate on a graph.
  A graph is an object G which supports following operations:
    - iteration on nodes (for n in G gives all nodes of the graph)
    - iteration on next nodes (for v in G[n] gives all next nodes of node n)
"""

import os
#from sets import Set
Set=set

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

     Le noeud n n'est pas dans le set retourne sauf en cas de boucles
     Ce cas n'est pas traite ici (limitation)
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

if __name__ == "__main__":
  test()
