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

# This awk program contains the type mapping tables - and the treatments
# for code generation
#
BEGIN { 
#
# file name generation
  idl_file="code_idl"
  hxx_file="code_hxx"
  cxx_file="code_cxx"
  class_i=class_name"_i"
  print "\t// generated part" > idl_file
  printf "    // generated part\n" > hxx_file
  printf "//\n// generated part\n//\n" > cxx_file
  print "Functions parsing (for debug)" > "parse_result"
#
#
# type mapping from c++ component to idl
#
  idl_arg_type["int"]="in long"
  idl_arg_type["double"]="in double"
  idl_arg_type["float"]="in float"
  idl_arg_type["long"]="in long"
  idl_arg_type["short"]="in short"
  idl_arg_type["unsigned"]="in unsigned long"
  idl_arg_type["const char*"]="in string"
  idl_arg_type["const std::string&"]="in string"
  idl_arg_type["int&"]="out long"
  idl_arg_type["double&"]="out double"
  idl_arg_type["float&"]="out float"
  idl_arg_type["long&"]="out long"
  idl_arg_type["short&"]="out short"
  idl_arg_type["unsigned&"]="out unsigned long"
  idl_arg_type["std::string&"]="out string"
  idl_arg_type["const MEDMEM::MESH&"]="in SALOME_MED::MESH"
  idl_arg_type["const MEDMEM::MESH*"]="in SALOME_MED::MESH"
  idl_arg_type["const MEDMEM::FIELD<double>*"]="in SALOME_MED::FIELDDOUBLE"
  idl_arg_type["const MEDMEM::FIELD<double>&"]="in SALOME_MED::FIELDDOUBLE"
  idl_arg_type["MEDMEM::FIELD<double>*&"]="out SALOME_MED::FIELDDOUBLE"
  idl_arg_type["const std::vector<double>&"]="in SALOME::SenderDouble"
  idl_arg_type["const std::vector<std::vector<double> >&"]="in SALOME::Matrix"
  idl_arg_type["std::vector<double>*&"]="out SALOME::SenderDouble"
  idl_arg_type["const MEDMEM::FIELD<int>*"]="in SALOME_MED::FIELDINT"
  idl_arg_type["const MEDMEM::FIELD<int>&"]="in SALOME_MED::FIELDINT"
  idl_arg_type["MEDMEM::FIELD<int>*&"]="out SALOME_MED::FIELDINT"
  idl_arg_type["const std::vector<int>&"]="in SALOME::SenderInt"
  idl_arg_type["std::vector<int>*&"]="out SALOME::SenderInt"
#
#
# mapping for returned types
#
  idl_rtn_type["void"]="void"
  idl_rtn_type["int"]="long"
  idl_rtn_type["double"]="double"
  idl_rtn_type["float"]="float"
  idl_rtn_type["long"]="long"
  idl_rtn_type["short"]="short"
  idl_rtn_type["unsigned"]="unsigned long"
  idl_rtn_type["const char*"]="string"
  idl_rtn_type["char*"]="string"
  idl_rtn_type["std::string"]="string"
  idl_rtn_type["const MEDMEM::MESH&"]="SALOME_MED::MESH"
  idl_rtn_type["MEDMEM::MESH&"]="SALOME_MED::MESH"
  idl_rtn_type["MEDMEM::MESH*"]="SALOME_MED::MESH"
  idl_rtn_type["const MEDMEM::MESH*"]="SALOME_MED::MESH"
  idl_rtn_type["const MEDMEM::FIELD<double>*"]="SALOME_MED::FIELDDOUBLE"
  idl_rtn_type["MEDMEM::FIELD<double>*"]="SALOME_MED::FIELDDOUBLE"
  idl_rtn_type["MEDMEM::FIELD<double>&"]="SALOME_MED::FIELDDOUBLE"
  idl_rtn_type["const MEDMEM::FIELD<double>&"]="SALOME_MED::FIELDDOUBLE"
  idl_rtn_type["std::vector<double>*"]="SALOME::SenderDouble"
  idl_rtn_type["std::vector<std::vector<double> >*"]="SALOME::Matrix"
  idl_rtn_type["const MEDMEM::FIELD<int>*"]="SALOME_MED::FIELDINT"
  idl_rtn_type["MEDMEM::FIELD<int>*"]="SALOME_MED::FIELDINT"
  idl_rtn_type["MEDMEM::FIELD<int>&"]="SALOME_MED::FIELDINT"
  idl_rtn_type["const MEDMEM::FIELD<int>&"]="SALOME_MED::FIELDINT"
  idl_rtn_type["std::vector<int>*"]="SALOME::SenderInt"
#
#
# Corba mapping table (for argument's types and returned types)
#
  idl_impl_hxx["in long"]="CORBA::Long"
  idl_impl_hxx["in double"]="CORBA::Double"
  idl_impl_hxx["in float"]="CORBA::Float"
  idl_impl_hxx["in short"]="CORBA::Short"
  idl_impl_hxx["in unsigned long"]="CORBA::ULong"
  idl_impl_hxx["in string"]="const char*"
  idl_impl_hxx["out long"]="CORBA::Long_out"
  idl_impl_hxx["out double"]="CORBA::Double_out"
  idl_impl_hxx["out float"]="CORBA::Float_out"
  idl_impl_hxx["out short"]="CORBA::Short_out"
  idl_impl_hxx["out unsigned long"]="CORBA::ULong_out"
  idl_impl_hxx["out string"]="CORBA::String_out"
  idl_impl_hxx["in SALOME_MED::MESH"]="SALOME_MED::MESH_ptr"
  idl_impl_hxx["in SALOME_MED::FIELDDOUBLE"]="SALOME_MED::FIELDDOUBLE_ptr"
  idl_impl_hxx["out SALOME_MED::FIELDDOUBLE"]="SALOME_MED::FIELDDOUBLE_out"
  idl_impl_hxx["in SALOME::SenderDouble"]="SALOME::SenderDouble_ptr"
  idl_impl_hxx["out SALOME::SenderDouble"]="SALOME::SenderDouble_out"
  idl_impl_hxx["in SALOME::Matrix"]="SALOME::Matrix_ptr"
  idl_impl_hxx["in SALOME_MED::FIELDINT"]="SALOME_MED::FIELDINT_ptr"
  idl_impl_hxx["out SALOME_MED::FIELDINT"]="SALOME_MED::FIELDINT_out"
  idl_impl_hxx["in SALOME::SenderInt"]="SALOME::SenderInt_ptr"
  idl_impl_hxx["out SALOME::SenderInt"]="SALOME::SenderInt_out"
  idl_impl_hxx["void"]="void"
  idl_impl_hxx["long"]="CORBA::Long"
  idl_impl_hxx["double"]="CORBA::Double"
  idl_impl_hxx["unsigned long"]="CORBA::ULong"
  idl_impl_hxx["string"]="char*"
  idl_impl_hxx["SALOME_MED::MESH"]="SALOME_MED::MESH_ptr"
  idl_impl_hxx["SALOME_MED::FIELDDOUBLE"]="SALOME_MED::FIELDDOUBLE_ptr"
  idl_impl_hxx["SALOME::SenderDouble"]="SALOME::SenderDouble_ptr"
  idl_impl_hxx["SALOME::Matrix"]="SALOME::Matrix_ptr"
  idl_impl_hxx["SALOME_MED::FIELDINT"]="SALOME_MED::FIELDINT_ptr"
  idl_impl_hxx["SALOME::SenderInt"]="SALOME::SenderInt_ptr"
#
#
# table for c++ code generation : argument's processing
#
  cpp_impl_a["int"]="\tint _%s(%s);\n"
  cpp_impl_a["double"]="\tdouble _%s(%s);\n"
  cpp_impl_a["float"]="\tfloat _%s(%s);\n"
  cpp_impl_a["long"]="\tlong _%s(%s);\n"
  cpp_impl_a["short"]="\tshort _%s(%s);\n"
  cpp_impl_a["unsigned"]="\tunsigned _%s(%s);\n"
  cpp_impl_a["const char*"]="\tconst char* _%s(%s);\n"
  cpp_impl_a["const std::string&"]="\tconst std::string _%s(%s);\n"
  cpp_impl_a["int&"]="\tint _%s;\n"
  cpp_impl_a["double&"]="\tdouble _%s;\n"
  cpp_impl_a["float&"]="\tfloat _%s;\n"
  cpp_impl_a["long&"]="\tlong _%s;\n"
  cpp_impl_a["short&"]="\tshort _%s;\n"
  cpp_impl_a["unsigned&"]="\tunsigned _%s;\n"
  cpp_impl_a["std::string&"]="std::string _%s;\n"
  cpp_impl_a["const MEDMEM::MESH&"]="\tMEDMEM::MESHClient* _%s = new MEDMEM::MESHClient(%s);\n" # MESHClient cannot be created on the stack (private constructor), so we create it on the heap and dereference it later (in treatment 4)
  cpp_impl_a["const MEDMEM::MESH*"]="\tMEDMEM::MESHClient* _%s = new MEDMEM::MESHClient(%s);\n"
  cpp_impl_a["MEDMEM::FIELD<double>*&"]="\tMEDMEM::FIELD<double>* _%s;\n"
  cpp_impl_a["const MEDMEM::FIELD<double>*"]="\tstd::auto_ptr<MEDMEM::FIELD<double> > _%s ( new MEDMEM::FIELDClient<double,MEDMEM::FullInterlace>(%s) );\n"
  cpp_impl_a["const MEDMEM::FIELD<double>&"]="\tMEDMEM::FIELDClient<double,MEDMEM::FullInterlace> _%s(%s);\n"
  cpp_impl_a["const std::vector<double>&"]="\tlong _%s_size;\n\tdouble *_%s_value = ReceiverFactory::getValue(%s,_%s_size);\n"\
             "\tstd::vector<double> _%s(_%s_value,_%s_value+_%s_size);\n\tdelete [] _%s_value;"
  cpp_impl_a["std::vector<double>*&"]="\tstd::vector<double>* _%s;\n"
  cpp_impl_a["const std::vector<std::vector<double> >&"]="\tMatrixClient _%s_client;\n\tint _%s_nbRow;\n\tint _%s_nbCol;\n"\
             "\tdouble* _%s_tab = _%s_client.getValue(%s,_%s_nbCol,_%s_nbRow);\n\tstd::vector<std::vector<double> > _%s(_%s_nbRow);\n"\
	     "\tfor (int i=0; i!=_%s_nbRow; ++i)\n\t{\n\t    _%s.reserve(_%s_nbCol);\n"\
	     "\t    std::copy(_%s_tab+_%s_nbCol*i,_%s_tab+_%s_nbCol*(i+1), _%s[i].begin());\n\t}\n\tdelete [] _%s_tab;\n"
  cpp_impl_a["MEDMEM::FIELD<int>*&"]="\tMEDMEM::FIELD<int>* _%s;\n"
  cpp_impl_a["const MEDMEM::FIELD<int>*"]="\tstd::auto_ptr<MEDMEM::FIELD<int> > _%s ( new MEDMEM::FIELDClient<int>(%s) );\n"
  cpp_impl_a["const MEDMEM::FIELD<int>&"]="\tMEDMEM::FIELDClient<int> _%s(%s);\n"
  cpp_impl_a["const std::vector<int>&"]="\tlong _%s_size;\n\tint *_%s_value = ReceiverFactory::getValue(%s,_%s_size);\n"\
             "\tstd::vector<int> _%s(_%s_value,_%s_value+_%s_size);\n\tdelete [] _%s_value;"
  cpp_impl_a["std::vector<int>*&"]="\tstd::vector<int>* _%s;\n"

#
#
# table for c++ code generation : returned value processing
#
  cpp_impl_b["void"]=""
  cpp_impl_b["int"]="\tCORBA::Long _rtn_ior(_rtn_cpp);\n"
  cpp_impl_b["double"]="\tCORBA::Double _rtn_ior(_rtn_cpp);\n"
  cpp_impl_b["float"]="\tCORBA::Float _rtn_ior(_rtn_cpp);\n"
  cpp_impl_b["long"]="\tCORBA::Long _rtn_ior(_rtn_cpp);\n"
  cpp_impl_b["short"]="\tCORBA::Short _rtn_ior(_rtn_cpp);\n"
  cpp_impl_b["unsigned"]="\tCORBA::ULong _rtn_ior(_rtn_cpp);\n"
  cpp_impl_b["const char*"]="\tchar* _rtn_ior = CORBA::string_dup(_rtn_cpp);\n"
  cpp_impl_b["char*"]="\tchar* _rtn_ior(_rtn_cpp);\n"
  cpp_impl_b["std::string"]="\tchar* _rtn_ior=CORBA::string_dup(_rtn_cpp.c_str());\n"
             "\tstd::copy(_rtn_cpp.begin(),_rtn_cpp.end(),_rtn_ior);\n"    
  cpp_impl_b["const MEDMEM::MESH&"]=\
             "\tMEDMEM::MESH_i * _rtn_mesh_i = new MEDMEM::MESH_i(const_cast<MEDMEM::MESH*>(&_rtn_cpp));\n"\
	     "\tSALOME_MED::MESH_ptr _rtn_ior = _rtn_mesh_i->_this();\n"
  cpp_impl_b["MEDMEM::MESH&"]=\
             "\tMEDMEM::MESH_i * _rtn_mesh_i = new MEDMEM::MESH_i(&_rtn_cpp);\n"\
	     "\tSALOME_MED::MESH_ptr _rtn_ior = _rtn_mesh_i->_this();\n"
  cpp_impl_b["MEDMEM::MESH*"]=\
             "\tMEDMEM::MESH_i * _rtn_mesh_i = new MEDMEM::MESH_i(_rtn_cpp);\n"\
	     "\tSALOME_MED::MESH_ptr _rtn_ior = _rtn_mesh_i->_this();\n"
  cpp_impl_b["const MEDMEM::MESH*"]=\
             "\tMEDMEM::MESH_i * _rtn_mesh_i = new MEDMEM::MESH_i(const_cast<MEDMEM::MESH*>(_rtn_cpp));\n"\
	     "\tSALOME_MED::MESH_ptr _rtn_ior = _rtn_mesh_i->_this();\n"
  cpp_impl_b["const MEDMEM::FIELD<double>*"]=\
             "\tMEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace> * _rtn_field_i = new MEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace>(const_cast<MEDMEM::FIELD<double>*>(_rtn_cpp),false);\n"\
             "\tSALOME_MED::FIELDDOUBLE_ptr _rtn_ior = _rtn_field_i->_this();\n"
  cpp_impl_b["MEDMEM::FIELD<double>*"]=\
             "\tMEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace> * _rtn_field_i = new MEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace>(_rtn_cpp,true);\n"\
             "\tSALOME_MED::FIELDDOUBLE_ptr _rtn_ior = _rtn_field_i->_this();\n"
  cpp_impl_b["MEDMEM::FIELD<double>&"]=\
             "\tMEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace> * _rtn_field_i = new MEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace>(&_rtn_cpp,false);\n"\
	     "\tSALOME_MED::FIELDDOUBLE_ptr _rtn_ior = _rtn_field_i->_this();\n"
  cpp_impl_b["const MEDMEM::FIELD<double>&"]=\
             "\tMEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace> * _rtn_field_i = new MEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace>(const_cast<MEDMEM::FIELD<double>*>(&_rtn_cpp),false);\n"\
	     "\tSALOME_MED::FIELDDOUBLE_ptr _rtn_ior = _rtn_field_i->_this();\n"
  cpp_impl_b["std::vector<double>*"]=\
             "\tSALOME::SenderDouble_ptr _rtn_ior = SenderFactory::buildSender(*this,&(*_rtn_cpp)[0],(*_rtn_cpp).size(),true);\n"
  cpp_impl_b["std::vector<std::vector<double> >*"]=\
             "\tint _rtn_cpp_i=(*_rtn_cpp).size();\n\tint _rtn_cpp_j=(*_rtn_cpp)[0].size();\n"\
	     "\tdouble* _rtn_tab = new double[_rtn_cpp_i*_rtn_cpp_j];\n"\
	     "\tfor (int i=0; i!=_rtn_cpp_i; ++i)\n\t    std::copy((*_rtn_cpp)[i].begin(),(*_rtn_cpp)[i].end(),_rtn_tab+i*_rtn_cpp_j);\n"\
	     "\tSALOME_Matrix_i* _rtn_matrix_i = new SALOME_Matrix_i(*this,_rtn_tab,_rtn_cpp_j,_rtn_cpp_i,true);\n"\
	     "\tSALOME::Matrix_ptr _rtn_ior = _rtn_matrix_i->_this();\n\tdelete _rtn_cpp;\n"
  cpp_impl_b["const MEDMEM::FIELD<int>*"]=\
             "\tMEDMEM::FIELDINT_i * _rtn_field_i = new MEDMEM::FIELDINT_i(const_cast<MEDMEM::FIELD<int>*>(_rtn_cpp),false);\n"\
             "\tSALOME_MED::FIELDINT_ptr _rtn_ior = _rtn_field_i->_this();\n"
  cpp_impl_b["MEDMEM::FIELD<int>*"]=\
             "\tMEDMEM::FIELDINT_i * _rtn_field_i = new MEDMEM::FIELDINT_i(_rtn_cpp,true);\n"\
             "\tSALOME_MED::FIELDINT_ptr _rtn_ior = _rtn_field_i->_this();\n"
  cpp_impl_b["MEDMEM::FIELD<int>&"]=\
             "\tMEDMEM::FIELDINT_i * _rtn_field_i = new MEDMEM::FIELDINT_i(&_rtn_cpp,false);\n"\
	     "\tSALOME_MED::FIELDINT_ptr _rtn_ior = _rtn_field_i->_this();\n"
  cpp_impl_b["const MEDMEM::FIELD<int>&"]=\
             "\tMEDMEM::FIELDINT_i * _rtn_field_i = new MEDMEM::FIELDINT_i(const_cast<MEDMEM::FIELD<int>*>(&_rtn_cpp),false);\n"\
	     "\tSALOME_MED::FIELDINT_ptr _rtn_ior = _rtn_field_i->_this();\n"
  cpp_impl_b["std::vector<int>*"]=\
             "\tSALOME::SenderInt_ptr _rtn_ior = SenderFactory::buildSender(*this,&(*_rtn_cpp)[0],(*_rtn_cpp).size(),true);\n"

#
#
# table for c++ code generation : out parameters processing and removeRef for reference counted objects
#
  cpp_impl_c["MEDMEM::FIELD<double>*&"]=\
             "\tMEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace> * %s_ior = new MEDMEM::FIELDTEMPLATE_I<double,MEDMEM::FullInterlace>(_%s, true);\n"\
	     "\t%s = %s_ior->_this();\n"
  cpp_impl_c["MEDMEM::FIELD<int>*&"]=\
             "\tMEDMEM::FIELDINT_i * %s_ior = new MEDMEM::FIELDINT_i(_%s, true);\n"\
	     "\t%s = %s_ior->_this();\n"
  cpp_impl_c["std::vector<double>*&"]=\
             "\t%s = SenderFactory::buildSender(*this,&(*_%s)[0],(*_%s).size(),true);\n"
  cpp_impl_c["std::vector<int>*&"]=\
             "\t%s = SenderFactory::buildSender(*this,&(*_%s)[0],(*_%s).size(),true);\n"
  cpp_impl_c["std::string&"]="\t%s = CORBA::string_dup(_%s.c_str());\n"
  cpp_impl_c["int&"]="\t%s = _%s;\n"
  cpp_impl_c["double&"]="\t%s = _%s;\n"
  cpp_impl_c["float&"]="\t%s = _%s;\n"
  cpp_impl_c["long&"]="\t%s = _%s;\n"
  cpp_impl_c["short&"]="\t%s = _%s;\n"
  cpp_impl_c["unsigned&"]="\t%s = _%s;\n"
  cpp_impl_c["const MEDMEM::MESH&"]="\t_%s->removeReference();\n"
  cpp_impl_c["const MEDMEM::MESH*"]="\t_%s->removeReference();\n"
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
  # check if returned type ($1) is one of the accepted types (idl_rtn_type)
  for (cpptype in idl_rtn_type) {
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
  # for each argument ($i), check if it is compatible (belongs to idl_arg_type)
  for (i=2; i<=NF; i++) {
    ok2=0
    split($i,tab,"=") # get rid of default value
    item=tab[1]
    for (cpptype in idl_arg_type) {
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
  if ( ok == 0) # pass to the next function if one of the c++ type is not compatible
      next
}
#
# --------------------- treatment 2 ----------------------------------
#
#  generate the Corba interface (idl file)
#
{ 
  printf "\t%s %s(", idl_rtn_type[type[1]],name[1] >> idl_file  # return type and name of function
  if ( NF >= 2 ){  # if there is arguments, print them
    for (i=2; i<=NF-1; i++)
      printf "%s %s,",idl_arg_type[type[i]],name[i] >> idl_file
    printf "%s %s", idl_arg_type[type[NF]],name[NF] >> idl_file
  }
  printf ");\n" >> idl_file
}  
#
# --------------------- treatment 3 ----------------------------------
#
#  generate the C++ implementation of component (hxx file)
#
{ 
  printf "    %s %s(",idl_impl_hxx[idl_rtn_type[type[1]]],name[1] >> hxx_file
  if ( NF >= 2 ){  # if there is arguments, print them
      for (i=2; i<=NF-1; i++)
	  printf "%s %s,",idl_impl_hxx[idl_arg_type[type[i]]],name[i] >> hxx_file
      printf "%s %s", idl_impl_hxx[idl_arg_type[type[NF]]],name[NF] >> hxx_file
  }
  printf ");\n" >> hxx_file
}
#
# --------------------- treatment 4 ----------------------------------
#
#  generate the C++ implementation of component (cxx file)
#
{
  # a) generate the function declaration + macro declarations
  func_name=class_name"_i::"name[1]
  printf "%s %s(",idl_impl_hxx[idl_rtn_type[type[1]]],func_name >> cxx_file
  if ( NF >= 2 ){  # if there is arguments, print them
      for (i=2; i<=NF-1; i++)
	  printf "%s %s,",idl_impl_hxx[idl_arg_type[type[i]]],name[i] >> cxx_file
      printf "%s %s", idl_impl_hxx[idl_arg_type[type[NF]]],name[NF] >> cxx_file
  }
  printf ")\n{\n\tbeginService(\"%s\");\n\tBEGIN_OF(\"%s\");\n",func_name,func_name >> cxx_file

  # b) generate the argument processing part
  if ( NF >= 2 ){
      printf "//\tArguments processing\n" >> cxx_file
      for (i=2; i<=NF; i++)
          printf cpp_impl_a[type[i]],name[i],name[i],name[i],name[i],name[i],name[i],name[i],name[i],name[i],\
	                   name[i],name[i],name[i],name[i],name[i],name[i],name[i],name[i],name[i],name[i] >> cxx_file
  }

  # c) generate the call to the c++ component
  if ( type[1] == "void" ) # if return type is void, the call syntax is different.
      printf "//\tCall cpp component\n\tcppCompo_->%s(",name[1] >> cxx_file
  else
      printf "//\tCall cpp component\n\t%s _rtn_cpp = cppCompo_->%s(",type[1],name[1] >> cxx_file
  if ( NF >= 2 ){  # if there is arguments, print them
      for (i=2; i<=NF; i++) {
	  # special treatment for some arguments
	  post=""
	  pre=""
	  if ( cpp_impl_a[type[i]] ~ "auto_ptr" )
	     post=".get()" # for auto_ptr argument, retrieve the raw pointer behind
	  if ( type[i] == "const MEDMEM::MESH&" )
	     pre="*"  # we cannot create MESHClient on the stack (private constructor), so we create it on the heap and dereference it
	  if ( i < NF )
	     post=post"," # separator between arguments
	  printf " %s_%s%s",pre,name[i],post >> cxx_file
      }
  }

  # d) generate the post_processing of returned and out parameters
  printf ");\n//\tPost-processing & return\n" >> cxx_file
  for (i=2; i<=NF; i++)
      printf cpp_impl_c[type[i]],name[i],name[i],name[i],name[i] >> cxx_file  # process for out parameters
  printf cpp_impl_b[type[1]] >> cxx_file  # process for returned value
  printf "\tendService(\"%s\");\n\tEND_OF(\"%s\");\n",func_name,func_name >> cxx_file
  if ( type[1] != "void" )
      printf "\treturn _rtn_ior;\n" >> cxx_file
  printf "}\n\n" >> cxx_file
}
#
#
END {
# CNC peut être mis dans le template directement printf "\nprivate:\n    std::auto_ptr<%s> cppImpl_;\n",class_name >> hxx_file
}
