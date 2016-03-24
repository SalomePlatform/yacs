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

# This awk program extract public functions of the class definition present in hxx interface

BEGIN { public=0 }

# we want to extract each function that is public and that does'nt contain
# the patterns : public, protected, private, // (comments), { and }
public == 1     && 
$1 !~ /public/  && 
$1 !~ /protected/ && 
$1 !~ /private/ && 
$1 !~ /\/\/*/   && 
$1 !~ /{|}/  {
   for (i=1; i<=NF; i++)
      printf "%s ", $i
#  change line if last field contains ";" -> one function per line in output
   if ( $NF ~ /;/ ) 
      printf "\n"
}
   
$1 == "class" && $0 !~ /;/ {public=1} # we test matching against /;/  to get rid of forward declaration
$1 ~ /public/ {public=1}
$1 ~ /protected/ {public=0}
$1 ~ /private/ {public=0}
$1 ~ /}/      {public=0}
