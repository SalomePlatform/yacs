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

# This awk program checks the arguments and return value compatibility
#
BEGIN { 

#
#
# allowed types for arguments
#
  arg_type["int"]= 1;
  arg_type["double"]= 1;
  arg_type["float"]= 1;
  arg_type["long"]= 1;
  arg_type["short"]= 1;
  arg_type["unsigned"]= 1;
  arg_type["const char*"]= 1;
  arg_type["const std::string&"]= 1;
  arg_type["int&"]= 1;
  arg_type["double&"]= 1;
  arg_type["float&"]= 1;
  arg_type["long&"]= 1;
  arg_type["short&"]= 1;
  arg_type["unsigned&"]= 1;
  arg_type["std::string&"]= 1;
  arg_type["const MEDMEM::MESH&"]= 1;
  arg_type["const MEDMEM::MESH*"]= 1;
  arg_type["const MEDMEM::FIELD<double>*"]= 1;
  arg_type["const MEDMEM::FIELD<double>&"]= 1;
  arg_type["MEDMEM::FIELD<double>*&"]= 1;
  arg_type["const std::vector<double>&"]= 1;
  arg_type["const std::vector<std::vector<double> >&"]= 1;
  arg_type["std::vector<double>&"]= 1;
  arg_type["std::vector<double>*&"]= 1;
  arg_type["const MEDMEM::FIELD<int>*"]= 1;
  arg_type["const MEDMEM::FIELD<int>&"]= 1;
  arg_type["MEDMEM::FIELD<int>*&"]= 1;
  arg_type["const std::vector<int>&"]= 1;
  arg_type["std::vector<int>*&"]= 1;
  arg_type["std::vector<int>&"]= 1;
#
#
# allowed types for return values
#
  rtn_type["void"]= 1;
  rtn_type["int"]= 1;
  rtn_type["double"]= 1;
  rtn_type["float"]= 1;
  rtn_type["long"]= 1;
  rtn_type["short"]= 1;
  rtn_type["unsigned"]= 1;
  rtn_type["const char*"]= 1;
  rtn_type["char*"]= 1;
  rtn_type["std::string"]= 1;
  rtn_type["const MEDMEM::MESH&"]= 1;
  rtn_type["MEDMEM::MESH&"]= 1;
  rtn_type["MEDMEM::MESH*"]= 1;
  rtn_type["const MEDMEM::MESH*"]= 1;
  rtn_type["const MEDMEM::FIELD<double>*"]= 1;
  rtn_type["MEDMEM::FIELD<double>*"]= 1;
  rtn_type["MEDMEM::FIELD<double>&"]= 1;
  rtn_type["const MEDMEM::FIELD<double>&"]= 1;
  rtn_type["std::vector<double>*"]= 1;
  rtn_type["std::vector<double>"]= 1;
  rtn_type["std::vector<std::vector<double> >*"]= 1;
  rtn_type["const MEDMEM::FIELD<int>*"]= 1;
  rtn_type["MEDMEM::FIELD<int>*"]= 1;
  rtn_type["MEDMEM::FIELD<int>&"]= 1;
  rtn_type["const MEDMEM::FIELD<int>&"]= 1;
  rtn_type["std::vector<int>*"]= 1;
#
#
# record sep is ");\n" whith blanks all around, and optional "(" at the beginning
  RS="[(]?[ \t]*[)][ \t]*;[ \t]*\n?"  
  FS="[ \t]*[(,][ \t]*"  # field sep is either "(" or "," surrounded by blanks 
}

# --------------------- treatment 1 ----------------------------------
#
#  extract from fields types, function name, and argument's names
#
{

  print "Function : ",$0 >> "parse_result"  # print for debug
  for (i=1; i<=NF; i++) {
      print "\t-> ",i," : ",$i >> "parse_result"
  }
  ok1=0;ok=1
  # check if returned type ($1) is one of the accepted types (rtn_type)
  for (cpptype in rtn_type) {
    if ( substr($1,1,length(cpptype)) == cpptype ) {
      # if compatible, store returned type and function name
      type[1]=cpptype
      name[1]=substr($1,length(cpptype)+1)
      sub("^[ \t]*","",name[1]) # get rid of leading blanks
      ok1=1
      break
    }
  }
  ok*=ok1
  # for each argument ($i), check if it is compatible (belongs to arg_type)
  for (i=2; i<=NF; i++) {
    ok2=0
    split($i,tab,"=") # get rid of default value
    item=tab[1]
    for (cpptype in arg_type) {
       if ( substr(item,1,length(cpptype)) == cpptype ) {
          # if compatible, store argument type and name
          type[i]=cpptype
          name[i]=substr(item,length(cpptype)+1)
          sub("^[ \t]*","",name[i]) # get rid of leading blanks
          if ( length(name[i]) == 0 ) # automatic name if argument's name wasn't precised
             name[i]=sprintf("_arg%d",i-1)
          ok2=1
	  break
       }
    }
    ok*=ok2 # ok=0 if one of the type is not compatible
  }

  # print compatibility 
  if ( $0 !~ class_name ) { # constructor are not considered, but we don't print it
      if ( ok == 0){ # if one of the c++ type is not compatible
          printf "     [KO]     :  %s",$0
      }
      else
          printf "     [OK]     :  %s",$0
	
      if ( $0 !~ /\(/  ) {
          printf "(" # if there is no argument, parenthesis was suppressed, so we add it for printing
      }
      printf ");\n"
  }    
  if ( ok == 0) # pass to the next function if one of the c++ type is not compatible
      next
}
#
#
END {
}
