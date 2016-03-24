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

# This awk program generates the catalog for C++ components
#
BEGIN { 
#
# file name generation
  catalog_file="catalog.xml"
  print "<?xml version='1.0' encoding='us-ascii' ?>\n" > catalog_file

  print "<!-- XML component catalog -->\n"\
        "<begin-catalog>\n"\
	"  <component-list>" >> catalog_file

  print "    <component>\n"\
        "      <component-name>"class_name"</component-name>\n"\
        "      <component-username>"class_name"</component-username>\n"\
        "      <component-type>Solver</component-type>\n"\
        "      <component-author>""</component-author>\n"\
        "      <component-version>1.0</component-version>\n"\
        "      <component-comment></component-comment>\n"\
        "      <component-multistudy>1</component-multistudy>\n"\
        "      <component-icone>"class_name".png</component-icone>\n"\
        "      <component-impltype>1</component-impltype>\n"\
        "      <component-interface-list>" >> catalog_file

  print "        <component-interface-name>"class_name"</component-interface-name>\n"\
        "        <component-interface-comment>No comment</component-interface-comment>\n"\
        "        <component-service-list>"\
        "\n" >> catalog_file

  type_in = 1
  type_out = 2

  type_arg["int"]= type_in
  type_arg["double"]= type_in
  type_arg["float"]= type_in
  type_arg["long"]= type_in
  type_arg["short"]= type_in
  type_arg["unsigned"]= type_in
  type_arg["const char*"]= type_in
  type_arg["const std::string&"]= type_in
  type_arg["const std::vector<double>&"]= type_in

  type_arg["int&"]= type_out
  type_arg["double&"]= type_out
  type_arg["float&"]= type_out
  type_arg["long&"]= type_out
  type_arg["short&"]= type_out
  type_arg["unsigned&"]= type_out
  type_arg["std::string&"]= type_out
  type_arg["std::vector<double>&"]= type_out

#
#
# record sep is ");\n" whith blanks all around, and optional "(" at the beginning
  RS="[(]?[   ]*[)][   ]*;[   ]*\n?"  
  FS="[   ]*[(,][   ]*"  # field sep is either "(" or "," surrounded by blanks 
}

# --------------------- treatment 1 ----------------------------------
#
#  extract from fields types, function name, and argument's names
#
{
  nitems = split($0, items);
  for (i=1; i<=nitems; i++) {
    split(items[i], j, " ");
    l=0; for (k in j) {l++;}
    k=j[1];
    for (ll=2; ll<l; ll++) k=k " " j[ll];
    type[i] = k;
    name[i] = j[ll];
    way[i] = type_arg[k];
  }

  print "          <component-service>\n"\
	"            <service-name>"name[1]"</service_name>\n"\
	"            <service-author></service-author>\n"\
	"            <service-version></service-version>\n"\
	"            <service-comment></service-comment>\n"\
	"            <service-by-default>0</service-by-default>\n"\
	>> catalog_file

  print "            <inParameter-list>" >> catalog_file
  for (i=2; i<=nitems; i++)
      if (way[i] == type_in) {
         print "              <inParameter>\n"\
	       "                <inParameter-name>"name[i]"</inParameter-name>\n"\
	       "                <inParameter-type>"type[i]"</inParameter-type>\n"\
	       "                <inParameter-comment></inParameter-comment>\n"\
	       "              </inParameter>" >> catalog_file
         }
  print "            <inParameter-list>\n" >> catalog_file

  print "            <outParameter-list>" >> catalog_file

  if (type[1] != "void")
      print "              <outParameter>\n"\
            "                <outParameter-name>return</outParameter-name>\n"\
            "                <outParameter-type>"type[1]"</outParameter-type>\n"\
            "                <outParameter-comment></outParameter-comment>\n"\
            "              </outParameter>" >> catalog_file

  for (i=2; i<=nitems; i++)
      if (way[i] == type_out) {
         print "              <outParameter>\n"\
	       "                <outParameter-name>"name[i]"</outParameter-name>\n"\
	       "                <outParameter-type>"type[i]"</outParameter-type>\n"\
	       "                <outParameter-comment></outParameter-comment>\n"\
	       "              </outParameter>" >> catalog_file
         }
  print "            <outParameter-list>\n" >> catalog_file

  print "         </component-service>\n" >> catalog_file
}
#
END {
  print "        </component-service-list>\n"\
	"      </component-interface-list>\n"\
        "    </component>\n"\
        "  </component-list>\n"\
        "</begin-catalog>" >> catalog_file
}
