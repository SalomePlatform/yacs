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
  print "Functions parsing (for debug parse4)" > "parse_result";
  dispatch_file="code_dispatch"
  class_cpp=class_name"_cpp"
  print "//\n// generated part\n//\n" > dispatch_file

  print "#include <exception>\n" >> dispatch_file
  print "#include \"Any.hxx\"\n" >> dispatch_file
  print "struct returnInfo {\n   int code;\n   std::string message;\n};\n\n" >> dispatch_file
  print "extern \"C\"" >> dispatch_file
  print "void * __init() {" >> dispatch_file
  print "\n  "class_name " *Obj = new " class_name ";\n  return Obj;\n}\n" >> dispatch_file

  print "extern \"C\"" >> dispatch_file
  print "void __terminate(void ** vObj) {" >> dispatch_file
  print "\n  "class_name " *Obj = ("class_name" *) *vObj;\n  delete Obj;\n  *vObj = NULL;\n}\n" >> dispatch_file

  print "extern \"C\"" >> dispatch_file
  print "void __run(void * O, const char * service, int nbIn, int nbOut,"                    >> dispatch_file
  print "           YACS::ENGINE::Any ** argIn, YACS::ENGINE::Any ** argOut, returnInfo *r)" >> dispatch_file
  print "  {"                                                                                >> dispatch_file
  print "    if (O == NULL) {"                                                             >> dispatch_file
  print "       r->code = -1;"                                                               >> dispatch_file
  print "       r->message = \"Component "class_name" has not been initialized\";"       >> dispatch_file
  print "       return;"                                                                     >> dispatch_file
  print "       }\n"                                                                         >> dispatch_file
  print "    returnInfo return_code;"                                                        >> dispatch_file
  print "    return_code.message = \"\";"                                                      >> dispatch_file
  print "    return_code.code = 0;\n"                                                        >> dispatch_file
  print "    int kIn = 0, kOut = 0;" >> dispatch_file;
  print "    "class_name" * Obj = ("class_name" *) O;">> dispatch_file;
  print "\n    try {\n" >> dispatch_file;

#
#
#
# table for c++ code generation : argument's processing
#
  cpp_arg["int"]="    int argIn%d = argIn[kIn++]->getIntValue();\n"
  cpp_arg["double"]="    double argIn%d = argIn[kIn++]->getDoubleValue();\n"
  cpp_arg["float"]="    float argIn%d = argIn[kIn++]->getDoubleValue();\n"
  cpp_arg["long"]="    long argIn%d = argIn[kIn++]->getIntValue();\n"
  cpp_arg["short"]="    short argIn%d = (short) argIn[kIn++]->getIntValue();\n"
  cpp_arg["unsigned"]="    unsigned argIn%d = (unsigned ) argIn[kIn++];->getIntValue()\n"
  cpp_arg["const char*"]="    const char * argIn%d = argIn[kIn++]->getStringValue().c_str();\n"
  cpp_arg["const std::string&"]="    const std::string& argIn%d = argIn[kIn++]->getStringValue();\n"
  cpp_arg["const std::vector<double>&"]="    YACS::ENGINE::SequenceAny * sA\n"\
	                                "          = dynamic_cast<YACS::ENGINE::SequenceAny *>(argIn[kIn]);\n"\
                                        "    if (NULL == sA) {\n"\
					"      r->code = -1;\n"\
					"      r->message = \"sequence expected\";\n"\
					"      return;\n"\
                                        "    }\n"\
					"    unsigned int i, n = sA->size();\n"\
					"    std::vector<double> argIn%d(n);\n"\
					"    for (i=0; i<n; i++) argIn%d[i] = ((*sA)[i])->getDoubleValue();\n"\
					"    kIn++;\n"

  cpp_arg["int&"]="    int argOut%d;\n"
  cpp_arg["double&"]="    double argOut%d;\n"
  cpp_arg["float&"]="    float argOut%d;\n"
  cpp_arg["long&"]="    long argOut%d;\n"
  cpp_arg["short&"]="    short argOut%d;\n"
  cpp_arg["unsigned&"]="    unsigned argOut%d;\n"
  cpp_arg["std::string&"]="    std::string argOut%d;\n"
  cpp_arg["std::vector<double>&"]="    std::vector<double> argOut%d;\n"

  cpp_out["int&"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["double&"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["float&"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["long&"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["short&"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["unsigned&"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["std::string&"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["std::vector<double>&"]="    argOut[kOut++] = YACS::ENGINE::SequenceAny::New(argOut%d);"

  cpp_out["int"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["double"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["float"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["long"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["short"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["unsigned"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["std::string"]="    argOut[kOut++] = YACS::ENGINE::AtomAny::New(argOut%d);"
  cpp_out["std::vector<double>"]="    argOut[kOut++] = YACS::ENGINE::SequenceAny::New(argOut%d);"

  type_in=1
  type_out=2
  
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
  print "Function : ",$0 >> "parse_result";  # print for debug
  for (i=1; i<=nitems; i++) {
    print "  -> ",i," : ",items[i] >> "parse_result";
    split(items[i], j, " ");
    l=0; for (k in j) {l++;}
    k=j[1];
    for (ll=2; ll<l; ll++) k=k " " j[ll];
    type[i] = k;
    name[i] = j[ll];
    way[i] = type_arg[k];
  }

  print "  if (strcmp(service, \""name[1]"\") == 0) {\n" >> dispatch_file;

  for (i=2; i<=nitems; i++) {
    printf cpp_arg[type[i]],(i-1),(i-1) >> dispatch_file;
  }

  # Internal function call with local arguments
  
  # if no return value, return NULL
  if (type[1] == "void") {
    s = "    ";
  }
  else {
    s="    "type[1]" argOut0 = ";
  }

  s = s"Obj->"name[1]"(";
  for (i=2; i<nitems; i++) {
    if (way[i] == type_in) {
       s=s"argIn"(i-1)",";
       }
    else if (way[i] == type_out) {
       s=s"argOut"(i-1)",";
       }
  }
  if (nitems>1) {
    if (way[nitems] == type_in) {
       s=s"argIn"(nitems-1);
       }
    else if (way[nitems] == type_out) {
       s=s"argOut"(nitems-1);
       }
  } 
  s=s");"
  print s >> dispatch_file;
  
  if (type[1] != "void") {
    printf cpp_out[type[1]],0  >> dispatch_file;
  }
  
  for (i=2; i<nitems; i++) {
    if (way[i] == type_out) {
       printf cpp_out[type[i]],(i-1) >> dispatch_file;
       }
    }
  
  print "\n  }\n  else" >> dispatch_file;

}
#
END {
  print "  {\n    // error in function name" >> dispatch_file
  print "    return_code.code = -1;" >> dispatch_file
  print "    return_code.message = std::string(service) + \" is not the name of a service\";\n  }" >> dispatch_file
  print "\n  }\n  catch(std::exception & e) {\n" >> dispatch_file
  print "    return_code.code = -1;" >> dispatch_file;
  print "    return_code.message = std::string(\"internal exception in \") + service + \" : \" + e.what();" >> dispatch_file;
  print "\n  }\n  catch(...) {\n" >> dispatch_file
  print "    return_code.code = -1;" >> dispatch_file;
  print "    return_code.message = std::string(\"internal exception in \") + service;" >> dispatch_file;
  print "  }" >> dispatch_file
  print "  *r = return_code;\n}" >> dispatch_file
  
  print "\n#include <iostream>\n" >> dispatch_file
  print "extern \"C\"" >> dispatch_file
  print "\nvoid __ping() {\n" >> dispatch_file
  print "  std::cerr << \"ping\" << std::endl;\n}" >> dispatch_file
}
