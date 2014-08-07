How to use yacsorb:

  1- install yacs : ./configure;make;make install
  2- go to Install directory
  3- set environment : python, omniorb, ...
  4- launch demo calculation server : ./bin/echoSrv& (need omniorb name server)
     4 objects are registered in name server : Echo, Obj, C, D
  5- launch supervisor server : ./bin/yacsSrv &
     supervisor object is registered in name server : Yacs
  6- launch the supervisor client : ./bin/yacs_clt
     This client load the aschema.xml schema in supervisor and 
     requests its execution. An observer is registered to be notified about
     status changes of all nodes in the schema. 
     When the observer is notified the status is printed.
     The client can be launched many times without reinitializing the supervisor.
