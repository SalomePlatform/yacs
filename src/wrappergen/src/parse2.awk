# suppress blanks between type and indirection or reference operators (* and &)
{ gsub(/[ \t]+&/,"\\& ")
  gsub(/[ \t]+\*/,"* ")
  print $0 }
