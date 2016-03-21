#!/usr/bin/env python
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

def triangle(n):
    """generate a YACS graph for computation of the Pascal triangle

    parameter: rank of the triangle.
    Use module decimal for an exact calculation with big numbers.
    The last node gives the sum of rank n (=2**n) and also a direct calculation of 2**n.
    """
       
    print """
<proc>
    <!-- types -->
    <!-- inline -->

<inline name="node_0_0" >
<script><code>
import time
from decimal import *"""
    print "getcontext().prec = " + str(1+n/3)
    print """
aa=Decimal(a)
bb=Decimal(b)
cc=aa+bb
c=str(cc)
print "cc=",cc
time.sleep(1)
</code></script>
<inport name="a" type="string"/>
<inport name="b" type="string"/>
<outport name="c" type="string"/>
</inline>"""

    print """
<inline name="collect" >
<script><code>"""
    print "import time"
    print "from decimal import *"
    print "getcontext().prec = " + str(1+n/3)
    print "tot = Decimal(0)"
    print "for i in range (" + str(n+1) + "):"
    print "    v='a' + str(i)"
    print "    tot+=Decimal(eval(v))"
    print "print tot"
    print "result=str(tot)"
    print "ref=Decimal(2)**" + str(n)
    print "reference=str(ref)"
    print "time.sleep(1)"
    print "</code></script>"
    for i in range (n+1):
        inport='<inport name="a' + str(i) + '" type="string"/>'
        print inport
        pass
    print '<outport name="result" type="string"/>'
    print '<outport name="reference" type="string"/>'
    print "</inline>"
    print
    
    for i in range (1,n+1):
        for j in range (i+1):
            node="node_" + str(i)   +"_" + str(j)
            nodetxt='<node name="'+node+'" type="node_0_0"></node>'
            print nodetxt
            pass
        pass

    print """

    <!-- service -->
    <!-- control -->

    """
    
    for i in range (n):
        for j in range (i+1):
            fromnode="node_" + str(i)   +"_" + str(j)
            tonode1="node_" + str(i+1)   +"_" + str(j)
            tonode2="node_" + str(i+1)   +"_" + str(j+1)
            control1='<control> <fromnode>'+fromnode+'</fromnode> <tonode>'+tonode1+'</tonode> </control>'
            control2='<control> <fromnode>'+fromnode+'</fromnode> <tonode>'+tonode2+'</tonode> </control>'
            print control1
            print control2
            pass
        pass
    for i in range (n+1):
        fromnode="node_" + str(n)   +"_" + str(i)
        control='<control> <fromnode>'+fromnode+'</fromnode> <tonode>collect</tonode> </control>'
        print control
        pass

    print """

    <!-- datalinks -->

    """
    
    for i in range (n):
        for j in range (i+1):
            fromnode="node_" + str(i)   +"_" + str(j)
            tonode1="node_" + str(i+1)   +"_" + str(j)
            tonode2="node_" + str(i+1)   +"_" + str(j+1)
            datafrom='<fromnode>' + fromnode + '</fromnode> <fromport>c</fromport>'
            datato1 ='<tonode>'   + tonode1  + '</tonode> <toport>b</toport>'
            datato2 ='<tonode>'   + tonode2  + '</tonode> <toport>a</toport>'
            print '<datalink>'
            print '   ' + datafrom
            print '   ' + datato1
            print '</datalink>'
            print '<datalink>'
            print '   ' + datafrom
            print '   ' + datato2
            print '</datalink>'
            pass
        pass
    for i in range (n+1):
        fromnode="node_" + str(n)   +"_" + str(i)
        datafrom='<fromnode>' + fromnode + '</fromnode> <fromport>c</fromport>'
        toport='a' + str(i)
        datato  ='<tonode>collect</tonode> <toport>' + toport + '</toport>'
        print '<datalink>'
        print '   ' + datafrom
        print '   ' + datato
        print '</datalink>'
        
        
    print """

    <!--parameters -->

    """

    print """
    <parameter>
        <tonode>node_0_0</tonode> <toport>a</toport>
        <value><string>0</string></value>
    </parameter>
    <parameter>
        <tonode>node_0_0</tonode> <toport>b</toport>
        <value><string>1</string></value>
    </parameter>
    """

    for i in range (1,n+1):
        node1="node_" + str(i)   +"_" + str(0)
        node2="node_" + str(i)   +"_" + str(i)
        tonode1 ='   <tonode>' + node1 + '</tonode> <toport>a</toport>'
        tonode2 ='   <tonode>' + node2 + '</tonode> <toport>b</toport>'
        print '<parameter>'
        print tonode1
        print '   <value><string>0</string></value>'
        print '</parameter>'
        
        print '<parameter>'
        print tonode2
        print '   <value><string>0</string></value>'
        print '</parameter>'

    print """

</proc>
    """
     
if __name__ == "__main__":
    import sys
    usage ="""Usage: %s rank > file.xml
    where rank is positive integer > 2
    """
    try:
        rank = int(sys.argv[1])
        if rank <2:
            raise ValueError("rank must be >1")
    except (IndexError, ValueError):
        print usage%(sys.argv[0])
        sys.exit(1)
        pass
    triangle(rank)
    pass
