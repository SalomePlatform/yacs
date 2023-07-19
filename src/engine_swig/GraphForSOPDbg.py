# Copyright (C) 2019-2023  CEA, EDF
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

import pilot
import SALOMERuntime
import loader
from collections import OrderedDict

fname = "graph.xml"
dbgFname = "dbg.xml"

def IntToStr(i,prefix=""):
    if i<26:
        return prefix+chr( ord('A')+i )
    else:
        factor = (i//26)-1 ; remain = i%26
        return IntToStr(factor,prefix)+IntToStr(remain)

def GenerateSimplifiedOne(p0):
    children = p0.getChildren()

    dico = OrderedDict()
    revDico = {}

    for i,c in enumerate(children):
        newName = IntToStr(i)
        #if newName in ["R","A"]:
        #    continue
        dico[c.getName()] = newName
        revDico[newName] = c.getName()
        pass

    newProc = r.createProc("TOP")
    for k,v in dico.items():
        node = r.createScriptNode("Salome",v)#createProc
        newProc.edAddChild(node)

    for c in children:
        if c.getName() not in dico:
            continue
        og = c.getOutGate()
        sn = newProc.getChildByName( dico[ c.getName() ] )
        outg = len( og.edMapInGate() )
        if outg > 1:
            print( "More than one link ( {} ) for {} !".format( outg, dico[c.getName()]  ) )
        for ol in og.edMapInGate():
            en = newProc.getChildByName( dico[ ol[0].getNode().getName() ])
            newProc.edAddCFLink(sn,en)
    return newProc, dico, revDico
    
    
SALOMERuntime.RuntimeSALOME.setRuntime()
r = SALOMERuntime.getSALOMERuntime()
l = loader.YACSLoader()
p = l.load(fname)

p0 = p.getChildren()[0]
newProc, dico, revDico = GenerateSimplifiedOne( p0 )
newProc.saveSchema( dbgFname )

indepGraphs = newProc.splitIntoIndependantGraph()

if len(indepGraphs) != 1:
    raise RuntimeError("Ooops !")

sop = pilot.SetOfPoints(indepGraphs[0])
sop.simplify()
