# -*- coding: utf-8 -*-
# Copyright (C) 2024  CEA, EDF
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

from salome.kernel import salome
import logging

DisplayEntryInCMD = "--display"
VerboseEntryInCMD = "--verbose"
VerboseLevelEntryInCMD = "--verbose-level"
StopOnErrorEntryInCMD = "--stop-on-error"
DumpOnErrorEntryInCMD = "--dump-on-error"
DumpEntryInCMD = "--dump"
KernelTraceEntryInCMD = "--kerneltrace"
DumpStateEntryInCMD = "--dump-final"
LoadStateEntryInCMD = "--load-state"
SaveXMLSchemaEntryInCMD = "--save-xml-schema"
ShutdownEntryInCMD = "--shutdown"
ResetEntryInCMD = "--reset"
InitPortEntryInCMD = "--init-port"
DoNotSqueezeEntryInCMD = "--donotsqueeze"
IOREntryInCMD = "--ior-ns"
CPUTimeResOfContainerEntryInCMD = "--cpu-mem-container-time-res"
HTOPFileEntryInCMD = "--htop-of-yacs-engine-process-file"
HTOPServerFileEntryInCMD = "--htop-of-servers"
HTOPFileTimeResEntryInCMD = "--htop-of-yacs-engine-process-time-res"
HTOPServerFileTimeResEntryInCMD = "--htop-of-servers-time-res"
MonitoringDirsEntryInCMD = "--monitoring-dirs-content"
MonitoringDirsResEntryInCMD = "--monitoring-dirs-content-res"
MonitoringDirsTimeResEntryInCMD = "--monitoring-dirs-content-time-res"
ReplayOnErrorEntryInCMD = "--replay-on-error"
ReplayDirInCMD = "--replay-dir"
BigObjDirInCMD = "--bigobj-dir"
BigObjThresInCMD = "--bigobj-thres"
CustomOverridesInCMD = "--activate-custom-overrides"

DisplayKeyInARGS = "display"
VerboseKeyInARGS = "verbose"
VerboseLevelKeyInARGS = "verbose_level"
StopOnErrorKeyInARGS = "stop"
DumpOnErrorKeyInARGS = "dumpErrorFile"
DumpKeyInARGS = "dump"
KernelTraceKeyInARGS = "kerneltrace"
DumpStateKeyInARGS = "finalDump"
LoadStateKeyInARGS = "loadState"
SaveXMLSchemaKeyInARGS = "saveXMLSchema"
ShutdownKeyInARGS = "shutdown"
ResetKeyInARGS = "reset"
InitPortKeyInARGS = "init_port"
DoNotSqueezeKeyInARGS = "donotsqueeze"
IORKeyInARGS = "iorNS"
CPUTimeResOfContainerKeyInARGS = "cpu_mem_container_time_res"
HTOPFileKeyInARGS = "htop_of_yacs_engine_process_file"
HTOPServerFileKeyInARGS = "htop_of_servers"
HTOPFileTimeResKeyInARGS = "htop_of_yacs_engine_process_time_res"
HTOPServerFileTimeResKeyInARGS = "htop_of_servers_time_res"
MonitoringDirsInARGS = "monitoring_dirs_content"
MonitoringDirsResInARGS = "monitoring_dirs_content_res"
MonitoringDirsTimeResInARGS = "monitoring_dirs_content_time_res"
ReplayOnErrorEntryInARGS = "replay_on_error"
ReplayDirInARGS = "replay_dir"
BigObjDirInARGS = "bigobj_dir"
BigObjThresInARGS = "bigobj_thres"
CustomOverridesInARGS = "activate_custom_overrides"

KeyValnARGS = [(DisplayEntryInCMD,DisplayKeyInARGS),
               (VerboseEntryInCMD,VerboseKeyInARGS),
               (VerboseLevelEntryInCMD,VerboseLevelKeyInARGS),
               (StopOnErrorEntryInCMD,StopOnErrorKeyInARGS),
               (DumpOnErrorEntryInCMD,DumpOnErrorKeyInARGS),
               (DumpEntryInCMD,DumpKeyInARGS),
               (KernelTraceEntryInCMD,KernelTraceKeyInARGS),
               (DumpStateEntryInCMD,DumpStateKeyInARGS),
               (LoadStateEntryInCMD,LoadStateKeyInARGS),
               (SaveXMLSchemaEntryInCMD,SaveXMLSchemaKeyInARGS),
               (ShutdownEntryInCMD,ShutdownKeyInARGS),
               (ResetEntryInCMD,ResetKeyInARGS),
               (InitPortEntryInCMD,InitPortKeyInARGS),
               (DoNotSqueezeEntryInCMD,DoNotSqueezeKeyInARGS),
               (CPUTimeResOfContainerEntryInCMD,CPUTimeResOfContainerKeyInARGS),
               (HTOPFileEntryInCMD,HTOPFileKeyInARGS),
               (HTOPFileTimeResEntryInCMD,HTOPFileTimeResKeyInARGS),
               (HTOPServerFileEntryInCMD,HTOPServerFileKeyInARGS),
               (HTOPServerFileTimeResEntryInCMD,HTOPServerFileTimeResKeyInARGS),
               (MonitoringDirsEntryInCMD,MonitoringDirsInARGS),
               (MonitoringDirsResEntryInCMD,MonitoringDirsResInARGS),
               (MonitoringDirsTimeResEntryInCMD,MonitoringDirsTimeResInARGS),
               (ReplayOnErrorEntryInCMD,ReplayOnErrorEntryInARGS),
               (ReplayDirInCMD,ReplayDirInARGS),
               (BigObjDirInCMD,BigObjDirInARGS),
               (BigObjThresInCMD,BigObjThresInARGS),
               (CustomOverridesInCMD,CustomOverridesInARGS),
               (IOREntryInCMD,IORKeyInARGS)]

my_runtime_yacs = None

my_ior_ns = None

my_replay_on_error = False

my_replay_dir = ""

def initializeSALOME():
  from salome.yacs import SALOMERuntime
  from salome.kernel import KernelBasis
  global my_runtime_yacs,my_ior_ns,my_runtime_yacs
  if my_runtime_yacs:
    return
  salome.salome_init()
  if my_replay_on_error:
    KernelBasis.SetPyExecutionMode("OutOfProcessWithReplay")
    KernelBasis.SetDirectoryForReplayFiles( my_replay_dir )
  if my_ior_ns:
    salome.naming_service.DumpIORInFile( my_ior_ns )
  flags = SALOMERuntime.RuntimeSALOME.UsePython + SALOMERuntime.RuntimeSALOME.UseCorba + SALOMERuntime.RuntimeSALOME.UseXml + SALOMERuntime.RuntimeSALOME.UseCpp + SALOMERuntime.RuntimeSALOME.UseSalome
  SALOMERuntime.RuntimeSALOME.setRuntime( flags )
  my_runtime_yacs = SALOMERuntime.getSALOMERuntime()
  anIOR = salome.orb.object_to_string ( salome.modulcat )
  aCatalog = my_runtime_yacs.loadCatalog( "session", anIOR )
  my_runtime_yacs.addCatalog( aCatalog )

def SALOMEInitializationNeeded(func):
  def decaratedFunc(*args,**kwargs):
    initializeSALOME()
    return func(*args,**kwargs)
  return decaratedFunc

@SALOMEInitializationNeeded
def loadGraph( xmlFileName ):
  """
  Args:
  -----
  xmlFileName : XML file containing YACS schema

  Returns
  -------

  SALOMERuntime.SalomeProc : YACS graph instance
  """
  from salome.yacs import loader
  l=loader.YACSLoader()
  p=l.load( xmlFileName )
  return p

def patchGraph( proc, squeezeMemory, initPorts, xmlSchema, loadStateXmlFile, reset, display):
  """
  Args:
  -----

  proc ( SALOMERuntime.SalomeProc ) : YACS Proc instance to be evaluated
  squeezeMemory ( bool ) : squeezememory to be activated
  initPorts (list<string>) : list of bloc.node.port=value.
  xmlSchema (string) :
  loadStateXmlFile (string) : file if any of state to be loaded inside proc
  reset (int) : 
  display (int) :
  """
  from salome.yacs import SALOMERuntime
  from salome.yacs import loader
  from salome.yacs import pilot
  def parse_init_port(input):
    """
    Returns
    -------
    node, port, value
    """
    node_port, value = input.split("=")
    nodePortSpl = node_port.split(".")
    port = nodePortSpl[-1]
    node = ".".join( nodePortSpl[:-1] )
    return node,port,value
      
  if squeezeMemory:
    logging.info("SqueezeMemory requested -> update proc")
    allNodes = proc.getAllRecursiveNodes()
    for node in allNodes:
      if isinstance(node,SALOMERuntime.PythonNode):
        node.setSqueezeStatus( True )
  #
  for initPort in initPorts:
      node,port,value = parse_init_port(initPort)
      init_state = proc.setInPortValue(node, port, value)
      if init_state != value:
        raise RuntimeError(f"Error on initialization of {initPort}")
  #
  if xmlSchema:
    SALOMERuntime.VisitorSaveSalomeSchemaUnsafe(proc,xmlSchema)
    pass
  #
  info = pilot.LinkInfo( pilot.LinkInfo.ALL_DONT_STOP )
  proc.checkConsistency(info)
  if info.areWarningsOrErrors():
    raise RuntimeError( info.getGlobalRepr() )
  #
  if loadStateXmlFile:
    loader.loadState( proc, loadStateXmlFile )
    if reset > 0:
      proc.resetState(reset)
      proc.exUpdateState()
  #
  if display > 0:
      proc.writeDotInFile("toto")
         
@SALOMEInitializationNeeded
def prepareExecution(proc, isStop, dumpErrorFile):
  """
  Returns
  -------

  pilot.ExecutorSwig : Instance of executor
  """
  from salome.yacs import pilot
  ex=pilot.ExecutorSwig()
  if isStop:
    logging.info(f"Stop has been activated with {dumpErrorFile}")
    ex.setStopOnError( dumpErrorFile!="", dumpErrorFile )
  return ex

@SALOMEInitializationNeeded
def executeGraph( executor, xmlfilename, proc, dump, finalDump, display, shutdown, CPUMemContainerTimeRes,
                 HTopOfThisProcessFile, HTopTimeRes,
                 HTopOfAllServersFile, HTopOfAllServersTimeRes, DirectoriesToMonitor):
  """
  Args:
  -----

  executor (pilot.ExecutorSwig) : Executor in charge of evaluation.
  proc ( SALOMERuntime.SalomeProc ) : YACS Proc instance to be evaluated
  xmlfilename (string)
  dump (int) : time interval between 2 dump state
  finalDump ( string ) : filename containing final result of graph, if any.
  display (int) :
  shutdown (int) : shutdown level
  CPUMemContainerTimeRes (int) : time in second between two measures of CPU/Mem in container processes
  HTopOfThisProcessFile (str) : file name (if not empty) containing the result of measure of current process
  HTopTimeRes (int) : time in second between two measures of CPU/Mem of current process
  HTopOfAllServersFile (str) : file name (if not empty) containing the result of measure of all servers
  HTopOfAllServersTimeRes (int) : time in second between two measures of CPU/Mem of any of server
  """
  from salome.yacs import SALOMERuntime
  from salome.yacs import pilot
  import os
  import contextlib

  class AutoShutdown:
    def __init__(self, proc, shutdown):
      self._proc = proc
      self._shutdown = shutdown
    def __enter__(self):
      pass
    
    def __exit__(self,exctype, exc, tb):
      if my_replay_on_error:
        listOfGrps = []
        for cont in salome.get_all_containers():
          listOfGrps += cont.getAllLogFileNameGroups()
        print("{} {} {}".format( 100*"=", "List of replay sessions of failing usecases" , 100*"="))
        for igrp,grp in enumerate(listOfGrps):
          print("{} : {}".format("Group {}".format(igrp)," ".join(grp)))
        print(300*"=")
      #
      if self._shutdown < 999:
        self._proc.shutdown(self._shutdown)
      salome.dsm.shutdownScopes()
      my_runtime_yacs.fini( False )

  class AutoDumpThread:
    def __init__(self, proc, dump, xmlfilename):
      self._dumpFile = "dumpState_{}".format( os.path.basename(xmlfilename) )
      self._lockFile = "{}.lock".format( os.path.splitext( os.path.basename(xmlfilename) )[0] )
    def __enter__(self):
      logging.info(f"Ready to launch thread of state dump with  dumpFile = {self._dumpFile}  lockFile = {self._lockFile}")
      self._dump_thread = SALOMERuntime.ThreadDumpState(proc,dump,self._dumpFile,self._lockFile)
      self._dump_thread.start()
    def __exit__(self,exctype, exc, tb):
      self._dump_thread.join()
  
  def MonitoringDirectories( DirectoriesToMonitor ):
    from salome.kernel import SALOME_PyNode
    if len( DirectoriesToMonitor ) > 0:
      return [ SALOME_PyNode.GenericPythonMonitoringLauncherCtxMgr( SALOME_PyNode.FileSystemMonitoring(timeRes*1000,zeDir,zeDirRes) ) for zeDir,zeDirRes,timeRes in DirectoriesToMonitor ]
    else:
      return [ ]

  def MonitoringThisProcess(HTopOfThisProcessFile,HTopTimeRes):
    from salome.kernel import SALOME_PyNode
    if HTopOfThisProcessFile:
      return [ SALOME_PyNode.GenericPythonMonitoringLauncherCtxMgr( SALOME_PyNode.CPUMemoryMonitoring(1000*HTopTimeRes,HTopOfThisProcessFile) ) ]
    else:
      return [ ]
    
  def MonitoringAllKernelServers(HTopOfAllServersFile, HTopOfAllServersTimeRes):
    if HTopOfAllServersFile:
      return [ salome.LogManagerLaunchMonitoringFileCtxMgr( 1000*HTopOfAllServersTimeRes, HTopOfAllServersFile ) ]
    else:
      return [ ]
  #
  salome.cm.SetDeltaTimeBetweenCPUMemMeasureInMilliSecond( 1000*CPUMemContainerTimeRes )
  # Start part of context manager instances
  ctxManagers = [ AutoShutdown(proc,shutdown) ] # the first one must be this one. Because orb.shutdown must be called last !
  #
  ctxManagers += MonitoringDirectories( DirectoriesToMonitor ) + MonitoringThisProcess(HTopOfThisProcessFile, HTopTimeRes) + MonitoringAllKernelServers(HTopOfAllServersFile, HTopOfAllServersTimeRes)
  #
  if dump != 0:
    ctxManagers += [ AutoDumpThread(proc,dump,xmlfilename) ]
  # end of part of context managers
  with contextlib.ExitStack() as stack:
    for mgr in ctxManagers:
      stack.enter_context(mgr)
    executor.RunPy(proc,display,isPyThread=True,fromscratch=True) # same as RunW but releasing GIL
  #
  if proc.getEffectiveState() != pilot.DONE:
    raise RuntimeError( proc.getErrorReport() )
  #
  if display > 0:
      proc.writeDotInFile("titi")
  #
  if finalDump:
    logging.info(f"Final dump requested : {finalDump}")
    SALOMERuntime.schemaSaveStateUnsafe( proc, finalDump )

def EntryFromCoarseEntry( entry ):
  if entry[:2] != "--":
    raise RuntimeError("Unexpected entry")
  return entry[2:]

def toDict( args ):
  """
  Convert argparse.Namespace to dict
  """
  return {EntryFromCoarseEntry(entry):getattr(args,key) for entry,key in KeyValnARGS}

def reprAfterArgParsing( args ):
  """
  Args:
  -----

  args (argparse.Namespace) : instance after parsing
  """
  return "\n".join( [ f"{EntryFromCoarseEntry(entry)} : {args[key]}" for entry,key in KeyValnARGS ] )

def getArgumentParser():
  from salome.kernel import KernelBasis
  import argparse
  parser = argparse.ArgumentParser()
  parser.add_argument('xmlfilename',help = "XML file containing YACS schema to be executed")
  parser.add_argument("-d", DisplayEntryInCMD, dest = DisplayKeyInARGS, type=int, const=1, nargs='?', default=0, help="Display dot files: 0=never to 3=very often")
  parser.add_argument("-v", VerboseEntryInCMD, dest = VerboseKeyInARGS,help="Produce verbose output", action='store_true')
  parser.add_argument("-vl", VerboseLevelEntryInCMD, dest = VerboseLevelKeyInARGS, type=str, choices=["ERROR", "WARNING", "INFO", "DEBUG"], default="INFO", help="Specifies level of verbosity")
  parser.add_argument("-s",StopOnErrorEntryInCMD,dest=StopOnErrorKeyInARGS,help="Stop on first error", action='store_true')
  parser.add_argument("-e",DumpOnErrorEntryInCMD,dest=DumpOnErrorKeyInARGS, type=str, const='dumpErrorState.xml', default="", nargs='?', help="Stop on first error and dump state")
  parser.add_argument("-g",DumpEntryInCMD,dest=DumpKeyInARGS, type=int, const=60, default=0, nargs='?', help="dump state")
  parser.add_argument("-kt", KernelTraceEntryInCMD, dest = KernelTraceKeyInARGS,help="Produce verbose of SALOME/KERNEL", action='store_true')
  parser.add_argument("-f",DumpStateEntryInCMD, dest =DumpStateKeyInARGS, type=str, const='finalDumpState.xml', default="", nargs='?', help="dump final state")
  parser.add_argument("-l",LoadStateEntryInCMD, dest=LoadStateKeyInARGS, type=str, default="", help="Load State from a previous partial execution")
  parser.add_argument("-x",SaveXMLSchemaEntryInCMD, dest=SaveXMLSchemaKeyInARGS, type=str, const="saveSchema.xml", nargs='?', default="", help = "dump xml schema")
  parser.add_argument("-t",ShutdownEntryInCMD, dest = ShutdownKeyInARGS, type=int , default=3, help="Shutdown the schema: 0=no shutdown to 3=full shutdown")
  parser.add_argument("-r",ResetEntryInCMD, dest = ResetKeyInARGS, type=int , default = 0, help="Reset the schema before execution: 0=nothing, 1=reset error nodes to ready state")
  parser.add_argument("-i",InitPortEntryInCMD, dest = InitPortKeyInARGS, type=str, default =[], action='append', help="Initialisation value of a port, specified as bloc.node.port=value. For multiple settings use comma.")
  parser.add_argument("-z",DoNotSqueezeEntryInCMD, dest = DoNotSqueezeKeyInARGS, help = "Desactivate squeeze memory optimization.", action='store_true')
  parser.add_argument(CPUTimeResOfContainerEntryInCMD, dest = CPUTimeResOfContainerKeyInARGS, type=int, default = 10, help="Time in second between two measures of CPU/Mem in container processes")
  parser.add_argument(HTOPFileEntryInCMD, dest = HTOPFileKeyInARGS, type=str, default ="", help="File name (if not empty) containing the result of measure of current process")
  parser.add_argument(HTOPFileTimeResEntryInCMD, dest = HTOPFileTimeResKeyInARGS, type=int, default = 60, help="Time in second between between two measures of CPU/Mem of current process")
  parser.add_argument(HTOPServerFileEntryInCMD, dest = HTOPServerFileKeyInARGS, type=str, default ="", help="File name (if not empty) containing the result of measure of all server processes")
  parser.add_argument(HTOPServerFileTimeResEntryInCMD, dest = HTOPServerFileTimeResKeyInARGS, type=int, default = 30, help="Time in second between between two measures of CPU/Mem of any server process")
  parser.add_argument(MonitoringDirsEntryInCMD, dest = MonitoringDirsInARGS, nargs='+', type=str, default =[], help="List of directories to be monitored")
  parser.add_argument(MonitoringDirsResEntryInCMD, dest = MonitoringDirsResInARGS, nargs='+', type=str, default =[], help=f"List of files with result of monitoring of directories to be monitored (see {MonitoringDirsInARGS}). The size of lists are expected to be the same.")
  parser.add_argument(MonitoringDirsTimeResEntryInCMD, dest = MonitoringDirsTimeResInARGS, nargs='+', type=int, default =[], help=f"List of time resolution (in second) of monitoring of directories to be monitored (see {MonitoringDirsInARGS}). The size of lists are expected to be the same.")
  parser.add_argument("-w",ReplayOnErrorEntryInCMD,dest=ReplayOnErrorEntryInARGS,help="Mode of execution of YACS where all python execution are wrapped into a subprocess to be able to resist against failure (such as SIGSEV)", action='store_true')
  parser.add_argument(ReplayDirInCMD, dest=ReplayDirInARGS, type=str, default ="", help="Directory storing replay scenarii if any (see replay-on-error option.")
  parser.add_argument(BigObjDirInCMD, dest=BigObjDirInARGS, type=str, default ="", help="Directory storing big obj files exchanged between YACS python nodes.")
  parser.add_argument(BigObjThresInCMD, dest=BigObjThresInARGS, type=int, default = KernelBasis.GetBigObjOnDiskThreshold(), help="Objects whose size exceeds this threshold in bytes will be written inside directory")
  parser.add_argument(CustomOverridesInCMD, dest=CustomOverridesInARGS, help="Activate custom overrides. These overrides are incarnated by yacs_driver_overrides module that is expected to loadable in current application.", action='store_true')
  parser.add_argument(IOREntryInCMD, dest = IORKeyInARGS, type=str, default ="", help="file inside which the ior of NS will be stored")
  parser.add_argument("--options_from_json", dest = "options_from_json", type=str, default ="", help="Json file of options. If defined options in json will override those specified in command line.")
  return parser

def mainRun( args, xmlFileName):
  """
  Args:
  -----

  args (dict) : options for treatment

  """
  global my_ior_ns,my_replay_on_error,my_replay_dir
  from salome.kernel.salome_utils import positionVerbosityOfLoggerRegardingState,setVerboseLevel,setVerbose,KernelLogLevelToLogging
  #
  iorNS = args[IORKeyInARGS]
  #
  if iorNS:
    my_ior_ns = iorNS
  #
  if args[ReplayOnErrorEntryInARGS]:
    my_replay_on_error = True
    my_replay_dir = args[ReplayDirInARGS]
  #
  if args[VerboseKeyInARGS]:
    setVerbose( args[ KernelTraceKeyInARGS ] )
    setVerboseLevel( KernelLogLevelToLogging[ args[VerboseLevelKeyInARGS] ] )
    positionVerbosityOfLoggerRegardingState()
    logging.info( reprAfterArgParsing(args) )
  #
  proc = loadGraph( xmlFileName )
  # work around a bug in Executor::Run when there are no tasks to launch.
  if len(proc.getChildren()) == 0 :
    return
  #
  patchGraph( proc, not args[DoNotSqueezeKeyInARGS], args[InitPortKeyInARGS], args[SaveXMLSchemaKeyInARGS], args[LoadStateKeyInARGS], args[ResetKeyInARGS], args[DisplayKeyInARGS])
  executor = prepareExecution( proc, args[StopOnErrorKeyInARGS], args[DumpOnErrorKeyInARGS])
  # proxy parameters management
  if args[ BigObjDirInARGS ]:
    salome.cm.SetBigObjOnDiskDirectory( args[ BigObjDirInARGS ] )
  salome.cm.SetBigObjOnDiskThreshold( args[ BigObjThresInARGS ] )
  # overrides
  if args[ CustomOverridesInARGS ]:
    try:
      import yacs_driver_overrides
      from salome.kernel import pylauncher
      allresources = pylauncher.RetrieveRMCppSingleton()# pylauncher.ResourcesManager_cpp singleton representing all resources (pylauncher.ResourceDefinition_cpp) devoted for the computation.
      yacs_driver_overrides.customize( salome.cm, allresources )
    except:
      raise RuntimeError("Overrides have be requested to be triggered by the user but module is not available")
  #
  executeGraph( executor, xmlFileName, proc, args[DumpKeyInARGS], args[DumpStateKeyInARGS], args[DisplayKeyInARGS], args[ShutdownKeyInARGS], args[CPUTimeResOfContainerKeyInARGS],
               args[HTOPFileKeyInARGS], args[HTOPFileTimeResKeyInARGS],
               args[HTOPServerFileKeyInARGS], args[HTOPServerFileTimeResKeyInARGS], [(dirToMonitor,resFile,timeRes) for dirToMonitor,resFile,timeRes in zip(args[MonitoringDirsInARGS],args[MonitoringDirsResInARGS],args[MonitoringDirsTimeResInARGS])] )

def parseArgs():
  """
  Returns
  -------

  - args (dict) : dictionnary containing all args taken into account. If json, the params in json will override entries
  - xmlFileName (str) : XML YACS schema
  
  """
  import json
  parser = getArgumentParser()
  args = parser.parse_args()
  iorNS = args.iorNS
  xmlFileName = args.xmlfilename
  optionFromJSon = args.options_from_json
  args = toDict( args )
  if optionFromJSon:
    # in case of Json overrides 
    with open( optionFromJSon ) as f:
      opts_from_json = json.load( f )
    for k,v in opts_from_json.items():
      if k != EntryFromCoarseEntry(IOREntryInCMD) or v:# for IOR if v is null -> do not override
        args[k] = v
  # change key of args from entryCMD to KeyInARGS
  args = {key:args[EntryFromCoarseEntry(entry)] for entry,key in KeyValnARGS}
  return args, xmlFileName

if __name__ == "__main__":
  args, xmlFileName = parseArgs()
  mainRun( args, xmlFileName)
