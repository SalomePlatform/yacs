# Copyright (C) 2012-2016  CEA/DEN, EDF R&D
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

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF(SALOME_BUILD_TESTS)

# --- options ---

IF(SALOME_YACS_USE_KERNEL)
  SET(SALOME_LIBS 
    SalomeLifeCycleCORBA 
    SalomeIDLKernel 
    SalomeNS
    SalomeContainer 
    SALOMEBasics 
    SalomeResourcesManager 
    OpUtil 
    SALOMELocalTrace 
    Registry 
    SalomeNotification 
    ResourcesManager 
    SalomeHDFPersist 
    SalomeGenericObj
    )
  SET(SALOME_INCL_PATH ${KERNEL_INCLUDE_DIRS})

  IF(SALOME_YACS_USE_DSC)
    SET(DSC_LIBS SalomeDSCContainer)
  ENDIF(SALOME_YACS_USE_DSC)
ENDIF(SALOME_YACS_USE_KERNEL)

# additional include directories
INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${OMNIORB_ROOT_DIR}/include/omniORB4/internal
  ${SALOME_INCL_PATH}
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/idl
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/idl
  ${LIBXML2_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${DSC_LIBS}
  ${SALOME_LIBS}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  SalomeIDLYACS
  YACSlibEngine
  )

# --- headers ---

SET(YACSRuntimeSALOME_HEADERS
  YACSRuntimeSALOMEExport.hxx 
  AutoGIL.hxx
  CalStreamPort.hxx
  CORBAComponent.hxx
  CORBACORBAConv.hxx
  CORBACppConv.hxx
  CORBANeutralConv.hxx
  CORBANode.hxx
  CORBAPorts.hxx
  CORBAPythonConv.hxx
  CORBAXMLConv.hxx
  CppComponent.hxx
  CppContainer.hxx
  CppCORBAConv.hxx
  CppCppConv.hxx
  CppNeutralConv.hxx
  CppNode.hxx
  CppPorts.hxx
  CppPythonConv.hxx
  CppXMLConv.hxx
  NeutralCORBAConv.hxx
  NeutralCppConv.hxx 
  NeutralPythonConv.hxx
  NeutralXMLConv.hxx
  OutNode.hxx
  PresetNode.hxx 
  PresetPorts.hxx
  PyStdout.hxx
  PythonCORBAConv.hxx
  PythonCppConv.hxx 
  PythonNeutralConv.hxx
  PythonNode.hxx
  PythonPorts.hxx
  PythonXMLConv.hxx 
  RuntimeSALOME.hxx
  SalomeComponent.hxx
  SalomeHPComponent.hxx
  SalomeContainer.hxx
  SalomeContainerTools.hxx
  SalomeContainerHelper.hxx
  SalomeContainerTmpForHP.hxx
  SalomeHPContainer.hxx
  SalomeHPContainerTools.hxx
  SALOMEDispatcher.hxx
  SalomeProc.hxx
  SalomePythonComponent.hxx
  SalomePythonNode.hxx 
  SessionCataLoader.hxx
  StudyNodes.hxx
  StudyPorts.hxx
  TypeConversions.hxx 
  VisitorSaveSalomeSchema.hxx
  XMLCORBAConv.hxx
  XMLCppConv.hxx
  XMLNeutralConv.hxx 
  XMLNode.hxx
  XMLPorts.hxx
  XMLPythonConv.hxx
  PythonInitConv.hxx 
  NeutralInitConv.hxx
  SalomeOptimizerLoop.hxx
  DistributedPythonNode.hxx
  PyOptimizerAlg.hxx
  )

# --- sources ---

SET(YACSRuntimeSALOME_SOURCES
  TypeConversions.cxx            
  CORBACORBAConv.cxx             
  CORBAPythonConv.cxx            
  CORBAXMLConv.cxx               
  CORBANeutralConv.cxx           
  XMLCORBAConv.cxx               
  XMLPythonConv.cxx              
  XMLNeutralConv.cxx             
  CORBANode.cxx                  
  CORBAPorts.cxx                 
  CppNode.cxx                    
  CppPorts.cxx                   
  NeutralPythonConv.cxx          
  NeutralCORBAConv.cxx           
  NeutralXMLConv.cxx             
  NeutralInitConv.cxx            
  PythonCORBAConv.cxx            
  PythonNeutralConv.cxx          
  PythonXMLConv.cxx              
  PythonInitConv.cxx             
  PythonNode.cxx                 
  SalomePythonNode.cxx           
  DistributedPythonNode.cxx      
  SalomePythonComponent.cxx      
  SalomeContainer.cxx            
  SalomeContainerTools.cxx
  SalomeHPContainer.cxx
  SalomeHPContainerTools.cxx
  SalomeContainerHelper.cxx
  SalomeContainerTmpForHP.cxx
  PythonPorts.cxx                
  XMLNode.cxx                    
  XMLPorts.cxx                   
  RuntimeSALOME.cxx              
  SALOMEDispatcher.cxx           
  SalomeProc.cxx                 
  CalStreamPort.cxx              
  CORBAComponent.cxx             
  SalomeComponent.cxx   
  SalomeHPComponent.cxx         
  CppComponent.cxx               
  CppContainer.cxx               
  CppCORBAConv.cxx               
  CppNeutralConv.cxx             
  CppXMLConv.cxx                 
  CppCppConv.cxx                 
  CppPythonConv.cxx              
  CORBACppConv.cxx               
  NeutralCppConv.cxx             
  XMLCppConv.cxx                 
  PythonCppConv.cxx              
  SessionCataLoader.cxx          
  PresetPorts.cxx                
  PresetNode.cxx                 
  OutNode.cxx                    
  StudyNodes.cxx                 
  StudyPorts.cxx                 
  VisitorSaveSalomeSchema.cxx    
  PyStdout.cxx                   
  SalomeOptimizerLoop.cxx        
  PyOptimizerAlg.cxx             
  )

# --- rules ---

ADD_LIBRARY(YACSRuntimeSALOME ${YACSRuntimeSALOME_SOURCES})
TARGET_LINK_LIBRARIES(YACSRuntimeSALOME ${_link_LIBRARIES})
INSTALL(TARGETS YACSRuntimeSALOME EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${YACSRuntimeSALOME_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
