#!/usr/bin/env python

import xmlrpclib,sys

data="""
<methodCall>
  <methodName>echo</methodName>
  <params>
    <param><value>hello, world</value></param>
    <param><value><double>3.5</double></value></param>
    <param><value><string>coucou</string></value></param>
  </params>
</methodCall>
"""
def echo(args):
  print args
  return 96785439

f=open("input")
data=f.read()
f.close()

params, method = xmlrpclib.loads(data)

try:
   call=eval(method)
   response=call(params)
   response = (response,)
except:
   # report exception back to server
   response = xmlrpclib.dumps( xmlrpclib.Fault(1, "%s:%s" % sys.exc_info()[:2]))
else:
   response = xmlrpclib.dumps( response, methodresponse=1)

print response
f=open("output",'w')
f.write(response)
f.close()
