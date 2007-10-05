# This awk program suppresses inline functions - but it doen't work if there is no inline function... Not used yet.
BEGIN { RS="\f"}

{ gsub(/{[^{}]*}[ \t]*;?/,";");print }

#{ gsub(/[ \t]+&/,"\\& ")
#  gsub(/[ \t]+\*/,"* ")
#  print $0 }
