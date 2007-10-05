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
