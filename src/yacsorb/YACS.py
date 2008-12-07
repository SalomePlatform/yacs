#  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
import sys
import YACS_ORB__POA
import YACS_ORB
import SALOME_ComponentPy
import SALOME_DriverPy

import threading
import tempfile

import SALOMERuntime
import loader
import salomeloader
import pilot
import traceback

class proc_i(YACS_ORB__POA.ProcExec):
    def __init__(self, xmlFile):
        self.l = loader.YACSLoader()
        self.e = pilot.ExecutorSwig()
        self.e.setExecMode(1) # YACS::STEPBYSTEP
        self.run1 = None
        self.p = self.l.load(xmlFile)
        pass

    def getNodeState(self,numid):
        return self.p.getNodeState(numid)

    def getXMLState(self, numid):
        return self.p.getXMLState(numid)

    def getInPortValue(self, nodeNumid, portName):
        return self.p.getInPortValue(nodeNumid, portName)

    def getOutPortValue(self, nodeNumid, portName):
        return self.p.getOutPortValue(nodeNumid, portName)

    def getErrorDetails(self, nodeNumid):
        return self.p.getNodeErrorDetails(nodeNumid)

    def getErrorReport(self, nodeNumid):
        return self.p.getNodeErrorReport(nodeNumid)

    def getContainerLog(self, nodeNumid):
        return self.p.getNodeContainerLog(nodeNumid)

    def getExecutorState(self):
        return self.e.getExecutorState()

    def getIds(self):
        numids = self.p.getNumIds()
        ids = self.p.getIds()
        return (numids,ids)

    def getNumIds(self):
        return self.p.getNumIds()

    def getNames(self):
        return self.p.getIds()

    def Run(self):
        execState = self.e.getExecutorState()
        if execState >= 305:
            # --- not clean, value from define.hxx (YACS::FINISHED)
            self.run1.join()
            self.run1 = None
            pass
        if self.run1 is None:
            self.run1 = threading.Thread(None, self.e.RunPy, "CORBAExec", (self.p,0,1,1))
            self.run1.start()
            pass
        pass

    def RunFromState(self, xmlFile):
        execState = self.e.getExecutorState()
        if execState >= 305:
            # --- not clean, value from define.hxx (YACS::FINISHED)
            self.run1.join()
            self.run1 = None
            pass
        try:
            self.p.init()
            self.p.exUpdateState();
            sp = loader.stateParser()
            sl = loader.stateLoader(sp,self.p)
            sl.parse(xmlFile)
        except IOError, ex:
            print "IO Error: ", ex
            return None
        except ValueError,ex:
            print "Caught ValueError Exception:",ex
            return None
        except pilot.Exception,ex:
            print ex.what()
            return None
        except:
            print "Unknown exception!"
            return None
        if self.run1 is None:
            self.run1 = threading.Thread(None, self.e.RunPy, "CORBAExec", (self.p,0,1,0))
            self.run1.start()
            pass
        pass
    
    def addObserver(self, obs, numid, event):
        disp = SALOMERuntime.SALOMEDispatcher_getSALOMEDispatcher()
        disp.addObserver(obs, numid, event)
        pass

    def setExecMode(self, mode):
        if mode == YACS_ORB.CONTINUE:
            self.e.setExecMode(0)
            pass
        if mode == YACS_ORB.STEPBYSTEP:
            self.e.setExecMode(1)
            pass
        if mode == YACS_ORB.STOPBEFORENODES:
            self.e.setExecMode(2)
            pass
        pass

    def setListOfBreakPoints(self, listOfBreakPoints):
        self.e.setListOfBreakPoints(listOfBreakPoints)
        pass

    def getTasksToLoad(self):
        return self.e.getTasksToLoad()

    def setStepsToExecute(self, listToExecute):
        return self.e.setStepsToExecute(listToExecute)

    def resumeCurrentBreakPoint(self):
        return self.e.resumeCurrentBreakPoint()

    def isNotFinished(self):
        return self.e.isNotFinished()

    def stopExecution(self):
        self.e.stopExecution()
        pass

    def saveState(self, xmlFile):
        return self.e.saveState(xmlFile)

    def setStopOnError(self, dumpRequested, xmlFile):
        self.e.setStopOnError(dumpRequested, xmlFile)
        pass

    def unsetStopOnError(self):
        self.e.unsetStopOnError()
        pass

    pass


class YACS(YACS_ORB__POA.YACS_Gen,
              SALOME_ComponentPy.SALOME_ComponentPy_i,
              SALOME_DriverPy.SALOME_DriverPy_i):
    """
    To be a SALOME component, this Python class must have the component name
    (YACS) and inherit the YACS_Gen class build from idl compilation
    with omniidl and also the class SALOME_ComponentPy_i which defines general
    SALOME component behaviour.
    """
    def __init__ ( self, orb, poa, contID, containerName, instanceName, 
                   interfaceName ):
        print "YACS.__init__: ", containerName, ';', instanceName
        SALOME_ComponentPy.SALOME_ComponentPy_i.__init__(self, orb, poa, contID,
                                                         containerName, instanceName,
                                                         interfaceName, 0)
        SALOME_DriverPy.SALOME_DriverPy_i.__init__(self, interfaceName)

        # --- store a naming service interface instance in _naming_service atribute
        self._naming_service = SALOME_ComponentPy.SALOME_NamingServicePy_i( self._orb )

        
        SALOMERuntime.RuntimeSALOME_setRuntime(1)
        SALOMERuntime.SALOMEDispatcher_setSALOMEDispatcher()
        pass

    def LoadProc(self,xmlFile):
        """
        load an XML graph in a YACS::ENGINE::proc, create a CORBA servant
        associated to the proc, and return a ref on the servant.
        """
        try:
            procExec_i = proc_i(xmlFile)
            logger=procExec_i.p.getLogger("parser")
            if not logger.isEmpty():
              print "The imported file has errors :"
              print logger.getStr()
              sys.stdout.flush()
              return None
        except IOError, ex:
            print >> sys.stderr ,"IO Error: ", ex
            return None
        except ValueError,ex:
            print >> sys.stderr ,"Caught ValueError Exception:",ex
            return None
        except pilot.Exception,ex:
            print >> sys.stderr ,ex.what()
            return None
        except:
            traceback.print_exc()
            return None
        procExec_o = procExec_i._this()
        return procExec_o
        
    def convertSupervFile(self,xmlFile):
        """
        load a SUPERV xml graph, convert it and return the new filename.
        """
        try:
            r = pilot.getRuntime()
            lo = salomeloader.SalomeLoader()
            e = pilot.ExecutorSwig()
            p = lo.load(xmlFile)
            s = pilot.SchemaSave(p)
            hnd, convertedFile = tempfile.mkstemp(".xml","yacs_","/tmp")
            s.save(convertedFile)
            return convertedFile
        except (IndexError):
            return ""

    pass
    
