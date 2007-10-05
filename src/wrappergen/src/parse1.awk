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
