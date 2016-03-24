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

# This awk program deletes C like comments '*/  ...  /*'  

{
    if (t = index($0, "/*")) {
	if (t > 1)
	    tmp = substr($0, 1, t - 1)
	else
	    tmp = ""
	u = index(substr($0, t + 2), "*/")
	while (u == 0) {
	    getline
            t = -1
            u = index($0, "*/")
	}
	if (u <= length($0) - 2)
	    $0 = tmp substr($0, t + u + 3)
	else
	    $0 = tmp
    }
    print $0
}
