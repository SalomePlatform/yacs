// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//#define REFCNT
//
#ifdef REFCNT
#define private public
#define protected public
#include <omniORB4/CORBA.h>
#include <omniORB4/internal/typecode.h>
#endif

#include "RuntimeSALOME.hxx"
#include "CORBANode.hxx"
#include "CORBAComponent.hxx"
#include "SalomeComponent.hxx"
#include "CORBAPorts.hxx"
#include "OutputDataStreamPort.hxx"
#include "CalStreamPort.hxx"
#include "InPort.hxx"
#include "TypeCode.hxx"
#include "AutoLocker.hxx"

#ifdef SALOME_KERNEL
#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOME_Exception.hh"
#endif

#include <omniORB4/CORBA.h>
#include <omniORB4/minorCode.h>
#include <iostream>
#include <set>
#include <list>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char CORBANode::IMPL_NAME[]="CORBA";
const char CORBANode::KIND[]="CORBA";

std::string CORBANode::getKind() const
{
  return KIND;
}

//! CORBANode constructor
CORBANode::CORBANode(const std::string& name): ServiceNode(name)
{
  _implementation=IMPL_NAME;
}

CORBANode::CORBANode(const CORBANode& other,ComposedNode *father):ServiceNode(other,father)
{
  _implementation=IMPL_NAME;
}

//! Execute the service on the component associated to the node
void CORBANode::execute()
{
  YACSTRACE(1, "+++++++++++++ CorbaNode::execute: " << getName() << " +++++++++++++++" );
  {
    //DII request building :
    // a service gets all its in parameters first
    // then all its out parameters
    // no inout parameters
    // the return value (if any) is the first out parameter
    // not yet user exception (only CORBA exception)

    CORBA::Object_var objComponent=((CORBAComponent*)_component)->getCompoPtr();
    CORBA::Request_var req = objComponent->_request(_method.c_str());
    CORBA::NVList_ptr arguments = req->arguments() ;

    DEBTRACE( "+++++++++++++++++CorbaNode::inputs+++++++++++++++++" )
    int in_param=0;
    //in parameters
    list<InputPort *>::iterator iter2;
    for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++)
      {
        InputCorbaPort *p=(InputCorbaPort *)*iter2;
        DEBTRACE( "port name: " << p->getName() )
        DEBTRACE( "port kind: " << p->edGetType()->kind() )
        CORBA::Any* ob=p->getAny();
#ifdef _DEVDEBUG_
        CORBA::TypeCode_var typcod= ob->type();
        switch(p->edGetType()->kind())
          {
          case Double:
            CORBA::Double d;
            *ob >>= d;
            DEBTRACE( d )
            break;
          case Int:
            CORBA::Long l;
            *ob >>= l;
            DEBTRACE( l )
            break;
          case String:
            const char *s;
            *ob >>= s;
            DEBTRACE( s )
            break;
          case Bool:
            CORBA::Boolean b;
            if(*ob >>= CORBA::Any::to_boolean(b))
              DEBTRACE( b )
            else 
              DEBTRACE( "not a boolean" )
            break;
          case Objref:
            DEBTRACE( typcod->id() )
            break;
          default:
            break;
          }
#endif
        //add_value makes a copy of any (*ob). This copy will be deleted with the request
        arguments->add_value( p->getName().c_str() , *ob , CORBA::ARG_IN ) ;
        in_param=in_param+1;
      }
    
    //output parameters
    DEBTRACE( "+++++++++++++++++CorbaNode::outputs+++++++++++++++++" )
    list<OutputPort *>::iterator iter;
    for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
      {
        OutputCorbaPort *p=(OutputCorbaPort *)*iter;
        DEBTRACE( "port name: " << p->getName() )
        DEBTRACE( "port kind: " << p->edGetType()->kind() )
        CORBA::Any* ob=p->getAnyOut();
#ifdef REFCNT
        DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)ob->pd_tc.in())->pd_ref_count);
#endif
        //add_value makes a copy of any. Copy will be deleted with request
        arguments->add_value( p->getName().c_str() , *ob , CORBA::ARG_OUT );
#ifdef REFCNT
        DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)ob->pd_tc.in())->pd_ref_count);
#endif
        delete ob;
      }

    //return value
    req->set_return_type(CORBA::_tc_void);
    
    DEBTRACE( "+++++++++++++++++CorbaNode::calculation+++++++++++++++++" << _method )
    req->invoke();
    CORBA::Exception *exc =req->env()->exception();
    if( exc )
      {
        DEBTRACE( "An exception was thrown!" )
        DEBTRACE( "The raised exception is of Type:" << exc->_name() )

        std::cerr << "The raised exception is of Type:" << exc->_name() << std::endl;
        /*
        if(strcmp(exc->_name(),"MARSHAL") == 0)
          {
            const char* ms = ((CORBA::MARSHAL*)exc)->NP_minorString();
            if (ms)
               std::cerr << "(CORBA::MARSHAL: minor = " << ms << ")" << std::endl;
            else
               std::cerr << "(CORBA::MARSHAL: minor = " << ((CORBA::MARSHAL*)exc)->minor() << ")" << std::endl;
          }
          */
        _errorDetails="Execution problem: the raised exception is of Type:";
        _errorDetails += exc->_name();
        throw Exception("Execution problem");
      }
    
    DEBTRACE( "++++++++++++CorbaNode::outputs++++++++++++" )
    int out_param=in_param;
    for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
      {
        OutputCorbaPort *p=(OutputCorbaPort *)*iter;
        DEBTRACE( "port name: " << p->getName() )
        DEBTRACE( "port kind: " << p->edGetType()->kind() )
        DEBTRACE( "port number: " << out_param )
        CORBA::Any *ob=arguments->item(out_param)->value();
#ifdef REFCNT
        DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)ob->pd_tc.in())->pd_ref_count);
#endif
#ifdef _DEVDEBUG_
        CORBA::TypeCode_var tc=ob->type();
        switch(p->edGetType()->kind())
          {
          case Double:
            CORBA::Double d;
            *ob >>= d;
            DEBTRACE( d )
            break;
          case Int:
            CORBA::Long l;
            *ob >>= l;
            DEBTRACE( l )
            break;
          case String:
            const char *s;
            *ob >>= s;
            DEBTRACE( s )
            break;
          case Objref:
            DEBTRACE( tc->id() )
            break;
          default:
            break;
          }
#endif
        //OutputPort must copy the input Any(ob). 
        //This Any will be deleted with the request.
        //Copy is made by the method put.
        p->put(ob);
        out_param=out_param+1;
#ifdef REFCNT
        DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)ob->pd_tc.in())->pd_ref_count);
#endif
      }
    DEBTRACE( "++++++++++++++++++++++++++++++++++++++++++" )
  }
  //Request has been deleted (_var )
  //All anys given to the request are deleted : don't forget to copy them 
  //if you want to keep them
#ifdef REFCNT
  list<OutputPort *>::const_iterator iter;
  for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
      OutputCorbaPort *p=(OutputCorbaPort *)*iter;
      CORBA::Any *ob=p->getAny();
      DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)ob->pd_tc.in())->pd_ref_count);
    }
#endif
  DEBTRACE( "+++++++++++++++++ End CorbaNode::execute: " << getName() << " +++++++++++++++++" )
}

//! Clone the node : must also clone the component instance ?
Node *CORBANode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new CORBANode(*this,father);
}

//! Create a CORBANode with the same component object and no input or output port
/*!
 *   \param name : node name
 *   \return       a new CORBANode node
 */
ServiceNode* CORBANode::createNode(const std::string& name)
{

  CORBANode* node=  new CORBANode(name);
  node->setComponent(_component);
  return node;
}

// SalomeNode Class

const char SalomeNode::KIND[]="Salome";

std::string SalomeNode::getKind() const
{
  return KIND;
}

//! SalomeNode constructor
SalomeNode::SalomeNode(const std::string& name):ServiceNode(name)
{
  _implementation=CORBANode::IMPL_NAME;
}

SalomeNode::SalomeNode(const SalomeNode& other,ComposedNode *father):ServiceNode(other,father)
{
  _implementation=CORBANode::IMPL_NAME;
}

SalomeNode::~SalomeNode()
{
}

#ifdef DSC_PORTS
//! Init the datastream ports of the component associated to the node
void SalomeNode::initService()
{
  DEBTRACE( "SalomeNode::initService: "<<getName())
  if(_setOfInputDataStreamPort.size() == 0 && _setOfOutputDataStreamPort.size() == 0)return;

  CORBA::Object_var objComponent=((SalomeComponent*)_component)->getCompoPtr();
  Engines::Superv_Component_var compo=Engines::Superv_Component::_narrow(objComponent);
  if( CORBA::is_nil(compo) )
    {
      std::string msg="Can't get reference to DSC object (or it was nil).";
      _errorDetails=msg;
      throw Exception(msg);
    }
  try
    {
      if (!_multi_port_node)
      {
        CORBA::Boolean ret=compo->init_service(_method.c_str());
        if(!ret)
          {
            _errorDetails="Problem with component '"+_ref+"' in init_service of service '"+ _method + "'";
            throw Exception(_errorDetails);
          }
        //Should check that component port types are the same as those declared in the xml file
      }
      else
      {
        CORBA::Boolean ret=compo->init_service_with_multiple(_method.c_str(), _param);
        if(!ret)
          {
            _errorDetails="Problem with component '"+_ref+"' in init_service_with_multiple of service '"+ _method + "'";
            throw Exception(_errorDetails);
          }
      }
    }
  catch(...)
    {
      _errorDetails="Problem with component '"+_ref+"' in init_service of service '"+ _method + "'";
      throw;
    }
}

void
SalomeNode::addDatastreamPortToInitMultiService(const std::string & port_name, int number)
{
  int index = _param.length();
  _param.length(index + 1);
  _param[index].name = CORBA::string_dup(port_name.c_str());
  _param[index].number = number;
}

//! Connect the datastream ports of the component associated to the node
void SalomeNode::connectService()
{
  DEBTRACE( "SalomeNode::connectService: "<<getName());
  if(_setOfOutputDataStreamPort.size() == 0)return;

  CORBA::Object_var objComponent=((SalomeComponent*)_component)->getCompoPtr();
  SALOME_NamingService NS(getSALOMERuntime()->getOrb()) ;
  SALOME_LifeCycleCORBA LCC(&NS) ;
  CORBA::Object_var obj = NS.Resolve("/ConnectionManager");
  Engines::ConnectionManager_var manager=Engines::ConnectionManager::_narrow(obj);
  Engines::Superv_Component_var me=Engines::Superv_Component::_narrow(objComponent);
  if( CORBA::is_nil(me) )
    {
      std::string msg="Can't get reference to Engines::Superv_Component: "+getName();
      _errorDetails=msg;
      throw Exception(msg);
    }
  std::list<OutputDataStreamPort *>::iterator iter;
  Engines::ConnectionManager::connectionId id;
  for(iter = _setOfOutputDataStreamPort.begin(); iter != _setOfOutputDataStreamPort.end(); iter++)
    {
      OutputDataStreamPort *port=(OutputDataStreamPort *)*iter;
      std::set<InPort *> ports=port->edSetInPort();
      std::set<InPort *>::iterator iterout;
      for(iterout=ports.begin();iterout != ports.end(); iterout++)
        {
          //It's only possible to connect 2 SalomeNode : try to get a SalomeNode
          SalomeNode* snode= dynamic_cast<SalomeNode*>((*iterout)->getNode());
          if(snode == 0) //don't connect, it's not a SalomeNode
            {
              std::string msg="Can't connect : not a SalomeNode";
              _errorDetails=msg;
              throw Exception(msg);
            }

          CORBA::Object_var comp=((SalomeComponent*)snode->getComponent())->getCompoPtr();
          if( CORBA::is_nil(comp))
            {
              std::string msg="Problem in connectService: " + snode->getName();
              msg=msg+" Component is probably not launched. Modify your YACS file";
              _errorDetails=msg;
              throw Exception(msg);
            }

          Engines::Superv_Component_var other=Engines::Superv_Component::_narrow(comp);
          if( CORBA::is_nil(other))
            {
              std::string msg="Can't connect to nil Engines::Superv_Component: " + snode->getName();
              _errorDetails=msg;
              throw Exception(msg);
            }
          try
            {
              id=manager->connect(me,port->getName().c_str(),other,(*iterout)->getName().c_str());
            }
          catch(Engines::DSC::PortNotDefined& ex)
            {
              std::string msg="Problem in connectService. Unknown port: "+port->getName()+" or "+(*iterout)->getName();
              _errorDetails=msg;
              throw Exception(msg);
            }
          catch(Engines::DSC::BadPortType& ex)
            {
              std::string msg="Problem in connectService. Type of provides port is bad. Expected: ";
              msg=msg + ex.expected.in();
              msg=msg + "Received: "+ex.received.in();
              _errorDetails=msg;
              throw Exception(msg);
            }
          catch(Engines::DSC::NilPort& ex)
            {
              std::string msg="Problem in connectService. Port is nil: "+port->getName()+" or "+(*iterout)->getName();
              _errorDetails=msg;
              throw Exception(msg);
            }
          catch( const SALOME::SALOME_Exception& ex )
            {
              std::string msg="Problem in connectService. ";
              msg += ex.details.text.in();
              msg=msg+getName()+" " + port->getName() + " " + snode->getName() + " " + (*iterout)->getName();
              _errorDetails=msg;
              throw Exception(msg);
            }
          catch(CORBA::SystemException& ex)
            {
              DEBTRACE( "minor code: " << ex.minor() );
              DEBTRACE( "completion code: " << ex.completed() );
              std::string msg="Problem in connectService. CORBA System exception ";
              std::string excname=ex._name();
              msg=msg+excname + " " +getName()+" " + port->getName() + " " + snode->getName() + " " + (*iterout)->getName();
              _errorDetails=msg;
              throw Exception(msg);
            }
          catch(...)
            {
              std::string msg="Problem in connectService. Unknown exception";
              msg=msg+getName()+" " + port->getName() + " " + snode->getName() + " " + (*iterout)->getName();
              _errorDetails=msg;
              throw Exception(msg);
            }
          DEBTRACE("Connected: " <<id<<" "<<getName()<<" "<<port->getName()<<" "<<snode->getName()<<" "<<(*iterout)->getName());
          ids.push_back(id);
        }
    }

  //Init component port properties
  for(iter = _setOfOutputDataStreamPort.begin(); iter != _setOfOutputDataStreamPort.end(); iter++)
    {
      (*iter)->initPortProperties();
    }
  std::list<InputDataStreamPort *>::iterator iterin;
  for(iterin = _setOfInputDataStreamPort.begin(); iterin != _setOfInputDataStreamPort.end(); iterin++)
    {
      (*iterin)->initPortProperties();
    }
}

//! Disconnect the datastream ports of the component associated to the node
void SalomeNode::disconnectService()
{
  DEBTRACE( "SalomeNode::disconnectService: "<<getName());
  // in some rare cases, disconnectService can be called from 2 different threads
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> lock(&_mutex);

  if(ids.size() == 0)
    return;

  SALOME_NamingService NS(getSALOMERuntime()->getOrb()) ;
  SALOME_LifeCycleCORBA LCC(&NS) ;
  CORBA::Object_var obj = NS.Resolve("/ConnectionManager");
  Engines::ConnectionManager_var manager=Engines::ConnectionManager::_narrow(obj);
  std::list<Engines::ConnectionManager::connectionId>::iterator iter;
  for(iter = ids.begin(); iter != ids.end(); iter++)
    {
      DEBTRACE("Trying to disconnect: " << *iter );
      try
        {
          manager->disconnect(*iter,Engines::DSC::RemovingConnection);
        }
      catch(Engines::ConnectionManager::BadId& ex)
        {
          DEBTRACE( "Problem in disconnect: " << *iter );
        }
      catch(Engines::DSC::PortNotDefined& ex)
        {
          DEBTRACE( "Problem in disconnect: " << *iter );
        }
      catch(Engines::DSC::PortNotConnected& ex)
        {
          DEBTRACE( "Problem in disconnect: " << *iter );
        }
      catch(Engines::DSC::BadPortReference& ex)
        {
          DEBTRACE( "Problem in disconnect (Engines::DSC::BadPortReference): " << *iter );
        }
      catch(CORBA::SystemException& ex)
        {
          DEBTRACE( "Problem in disconnect (CORBA::SystemException): " << *iter );
        }
      catch(...)
        {
          DEBTRACE( "Problem in disconnect: " << *iter );
        }
    }
  ids.clear();
}

void SalomeNode::cleanNodes()
{
  disconnectService();
}

#endif

//! Execute the service on the component associated to the node
void SalomeNode::execute()
{
  YACSTRACE(1,"+++++++++++++++++ SalomeNode::execute: " << getName() << " " << _method << " +++++++++++++++++" );
  {
    CORBA::Object_var objComponent=((SalomeComponent*)_component)->getCompoPtr();
    Engines::EngineComponent_var compo=Engines::EngineComponent::_narrow(objComponent);

    // Set component properties
    std::map<std::string,std::string> amap=getProperties();
    if(amap.size() > 0)
      {
        Engines::FieldsDict_var dico = new Engines::FieldsDict;
        dico->length(amap.size());
        std::map<std::string,std::string>::const_iterator it;
        int i=0;
        for(it = amap.begin(); it != amap.end(); ++it)
          {
            dico[i].key=CORBA::string_dup(it->first.c_str());
            dico[i].value <<=it->second.c_str();
            i++;
          }
        compo->setProperties(dico);
      }

    //DII request building :
    // a service gets all its in parameters first
    // then all its out parameters
    // no inout parameters
    // the return value (if any) is the first out parameter
    //
    CORBA::Request_var req ;
    try
      {
        req = objComponent->_request(_method.c_str());
      }
    catch(CORBA::SystemException& ex)
      {
        std::string msg="component '" +_ref+ "' has no service '" + _method+ "'";
        _errorDetails=msg;
        throw Exception(msg);
      }
    CORBA::NVList_ptr arguments = req->arguments() ;

    DEBTRACE( "+++++++++++++++++SalomeNode::inputs+++++++++++++++++" );
    int in_param=0;
    //in parameters
    list<InputPort *>::iterator iter2;
    for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++)
    {
          InputCorbaPort *p=(InputCorbaPort *)*iter2;
          if(p->edGetType()->isA(Runtime::_tc_file))
            continue;
          DEBTRACE( "port name: " << p->getName() );
          DEBTRACE( "port kind: " << p->edGetType()->kind() );
          CORBA::Any* ob=p->getAny();
#ifdef _DEVDEBUG_
          CORBA::TypeCode_var tc=ob->type();
          switch(p->edGetType()->kind())
          {
          case Double:
            CORBA::Double d;
            *ob >>= d;
            DEBTRACE( d )
            break;
          case Int:
            CORBA::Long l;
            *ob >>= l;
            DEBTRACE( l )
            break;
          case String:
            const char *s;
            *ob >>= s;
            DEBTRACE( s )
            break;
          case Objref:
            DEBTRACE( tc->id() )
            break;
          default:
            break;
          }
#endif
          //add_value makes a copy of any. Copy will be deleted with request
          arguments->add_value( p->getName().c_str() , *ob , CORBA::ARG_IN ) ;
          in_param=in_param+1;
    }
    //in files
    int nfiles=0;
    DEBTRACE("checkInputFilesToService: " << _method);
    try
      {
        for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++)
          {
            InputCorbaPort *p=(InputCorbaPort *)*iter2;
            if(!p->edGetType()->isA(Runtime::_tc_file))
              continue;
            std::string filename=p->getName();
            // replace ':' by '.'. Needed because port name can not contain '.'
            string::size_type debut =filename.find_first_of(':',0);
            while(debut != std::string::npos)
              {
                 filename[debut]='.';
                 debut=filename.find_first_of(':',debut);
              }
            DEBTRACE( "inport with file: " << filename );
            Engines::Salome_file_var isf=compo->setInputFileToService(_method.c_str(),p->getName().c_str());
            isf->setDistributedFile(filename.c_str());
            Engines::Salome_file_ptr osf;
            CORBA::Any* any=p->getAny();
            *any >>= osf;
            isf->connect(osf);
            nfiles++;
          }
        if(nfiles)
          compo->checkInputFilesToService(_method.c_str());
      }
    catch( const SALOME::SALOME_Exception& ex )
      {
        std::string text="Execution problem in checkInputFilesToService: ";
        text += (const char*)ex.details.text;
        _errorDetails=text;
        throw Exception(text);
      }
    catch(CORBA::SystemException& ex)
      {
        std::string msg="Execution problem: component probably does not support files ??";
        _errorDetails=msg;
        throw Exception(msg);
      }
    
    //out parameters
    DEBTRACE( "+++++++++++++++++SalomeNode::outputs+++++++++++++++++" )
    list<OutputPort *>::iterator iter;
    for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
          OutputCorbaPort *p=(OutputCorbaPort *)*iter;
          DEBTRACE( "port name: " << p->getName() )
          DEBTRACE( "port kind: " << p->edGetType()->kind() )
          if(p->edGetType()->isA(Runtime::_tc_file))
            continue;
          CORBA::Any* ob=p->getAnyOut();
          //add_value makes a copy of any. Copy will be deleted with request
          arguments->add_value( p->getName().c_str() , *ob , CORBA::ARG_OUT );
          delete ob;
    }

    //return value
    //if return type is set to void (not mandatory, it's set by default)
    //the return value will not be marshalled as a return value but 
    //as the first out argument (don't forget to add it as the first output argument)
    req->set_return_type(CORBA::_tc_void);
    //user exceptions
    req->exceptions()->add(SALOME::_tc_SALOME_Exception);
    
    DEBTRACE( "+++++++++++++++++SalomeNode::calculation+++++++++++++++++" << _method )
    req->invoke();
    CORBA::Exception *exc =req->env()->exception();
    if( exc )
    {
      DEBTRACE( "An exception was thrown!" )
      DEBTRACE( "The raised exception is of Type:" << exc->_name() )

      CORBA::SystemException* sysexc;
      sysexc=CORBA::SystemException::_downcast(exc);
      if(sysexc != NULL)
        {
          // It's a SystemException
          DEBTRACE( "minor code: " << sysexc->minor() );
          DEBTRACE( "completion code: " << sysexc->completed() );
          std::string text="Execution problem: ";
          std::string excname=sysexc->_name();
          if(excname == "BAD_OPERATION")
            {
              text=text+"component '" +_ref+ "' has no service '" + _method+ "'";
            }
          else if(excname == "BAD_PARAM")
            {
              text=text+"A parameter (input or output) passed to the call is out of range or otherwise considered illegal.\n";
              text=text+"Minor code: "+sysexc->NP_minorString();
            }
          else if(excname == "MARSHAL" && sysexc->minor() == omni::MARSHAL_PassEndOfMessage)
            {
              text=text+"probably an error in arguments of service '" + _method + "' from component '" +_ref+ "'";
            }
          else if(excname == "COMM_FAILURE" && sysexc->minor() == omni::COMM_FAILURE_UnMarshalResults)
            {
              text=text+"probably an error in output arguments of service '" + _method + "' from component '" +_ref+ "'";
            }
          else if(excname == "COMM_FAILURE" && sysexc->minor() == omni::COMM_FAILURE_UnMarshalArguments)
            {
              text=text+"probably an error in input arguments of service '" + _method + "' from component '" +_ref+ "'";
            }
          else if(excname == "COMM_FAILURE" && sysexc->minor() == omni::COMM_FAILURE_WaitingForReply)
            {
              text=text+"probably an error in input arguments of service '" + _method + "' from component '" +_ref+ "'";
            }
          else
            {
              DEBTRACE(sysexc->NP_minorString() );
              text=text+"System Exception "+ excname;
            }
          _errorDetails=text;
          throw Exception(text);
        }

      // Not a System Exception
      CORBA::UnknownUserException* userexc;
      userexc=CORBA::UnknownUserException::_downcast(exc);
      if(userexc != NULL)
        {
          CORBA::Any anyExcept = userexc->exception(); 

          const SALOME::SALOME_Exception* salexc;
          if(anyExcept >>= salexc)
            {
              DEBTRACE("SALOME_Exception: "<< salexc->details.sourceFile);
              DEBTRACE("SALOME_Exception: "<<salexc->details.lineNumber);
              _errorDetails=salexc->details.text;
              throw Exception("Execution problem: Salome Exception occurred" + getErrorDetails() );
            }
          std::string msg="Execution problem: User Exception occurred";
          _errorDetails=msg;
          throw Exception(msg);
        }
      std::string msg="Execution problem";
      _errorDetails=msg;
      throw Exception(msg);
    }
    
    DEBTRACE( "++++++++++++SalomeNode::outputs++++++++++++" )
    int out_param=in_param;
    for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
          OutputCorbaPort *p=(OutputCorbaPort *)*iter;
          DEBTRACE( "port name: " << p->getName() );
          DEBTRACE( "port kind: " << p->edGetType()->kind() );
          DEBTRACE( "port number: " << out_param );
          if(p->edGetType()->isA(Runtime::_tc_file))
            continue;
          CORBA::Any *ob=arguments->item(out_param)->value();
#ifdef _DEVDEBUG_
      switch(p->edGetType()->kind())
      {
        case Double:
          CORBA::Double d;
          *ob >>= d;
          DEBTRACE( d )
          break;
        case Int:
          CORBA::Long l;
          *ob >>= l;
          DEBTRACE( l )
          break;
        case String:
          const char *s;
          *ob >>= s;
          DEBTRACE( s )
          break;
        default:
          break;
      }
#endif
        //OutputPort must copy the input Any(ob). 
        //This Any will be deleted with the request.
        //Copy is made by the method put.
        p->put(ob);
        out_param=out_param+1;
    }

    //Out files
    nfiles=0;
    DEBTRACE("checkOutputFilesToService: " << _method);
    try
      {
        for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
          {
            OutputCorbaPort *p=(OutputCorbaPort *)*iter;
            if(!p->edGetType()->isA(Runtime::_tc_file))
              continue;
            // The output port has a file object : special treatment
            std::string filename=p->getName();
            // replace ':' by '.'. Needed because port name can not contain '.'
            string::size_type debut =filename.find_first_of(':',0);
            while(debut != std::string::npos)
              {
                 filename[debut]='.';
                 debut=filename.find_first_of(':',debut);
              }
            DEBTRACE( "outport with file: " << filename );
            Engines::Salome_file_var osf=compo->setOutputFileToService(_method.c_str(),p->getName().c_str());
            osf->setLocalFile(filename.c_str());
            CORBA::Any any;
            any <<= osf;
            p->put(&any);
          }
        if(nfiles)
          compo->checkOutputFilesToService(_method.c_str());
      }
    catch( const SALOME::SALOME_Exception& ex )
      {
        std::string text=(const char*)ex.details.text;
        _errorDetails=text;
        throw Exception("Execution problem in checkOutputFilesToService: " + text);
      }
    catch(CORBA::SystemException& ex)
      {
        std::string msg="Execution problem: component probably does not support files ?";
        _errorDetails=msg;
        throw Exception(msg);
      }
  }
  //Request has been deleted (_var )
  //All anys given to the request are deleted : don't forget to copy them 
  //if you want to keep them
  DEBTRACE( "+++++++++++++++++ End SalomeNode::execute: " << getName() << " +++++++++++++++++" )
}

Node *SalomeNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new SalomeNode(*this,father);
}

//! Create a SalomeNode with the same component object and no input or output port
/*!
 *   \param name : node name
 *   \return       a new SalomeNode node
 */
ServiceNode* SalomeNode::createNode(const std::string& name)
{
  SalomeNode* node=new SalomeNode(name);
  node->setComponent(_component);
  return node;
}

std::string SalomeNode::getContainerLog()
{
  std::string msg="Component is not loaded";
  try
    {
      CORBA::Object_var objComponent=((SalomeComponent*)_component)->getCompoPtr();
      Engines::EngineComponent_var compo=Engines::EngineComponent::_narrow(objComponent);
      if( !CORBA::is_nil(compo) )
        {
          Engines::Container_var cont= compo->GetContainerRef();
          CORBA::String_var logname = cont->logfilename();
          DEBTRACE(logname);
          msg=logname;
          std::string::size_type pos = msg.find(":");
          msg=msg.substr(pos+1);
        }
    }
  catch(CORBA::COMM_FAILURE& ex) 
    {
      msg = ":Component no longer reachable: Caught system exception COMM_FAILURE";
      msg += " -- unable to contact the object.";
    }
  catch(CORBA::SystemException& ex) 
    {
      msg = ":Component no longer reachable: Caught a CORBA::SystemException.\n";
      CORBA::Any tmp;
      tmp <<= ex;
      CORBA::TypeCode_var tc = tmp.type();
      const char *p = tc->name();
      if ( *p != '\0' ) 
        msg += p;
      else  
        msg += tc->id();
    }
  catch(CORBA::Exception& ex) 
    {
      msg = ":Component no longer reachable: Caught CORBA::Exception.\n";
      CORBA::Any tmp;
      tmp <<= ex;
      CORBA::TypeCode_var tc = tmp.type();
      const char *p = tc->name();
      if ( *p != '\0' ) 
        msg += p;
      else  
        msg += tc->id();
    }
  catch(omniORB::fatalException& fe) 
    {
      msg = ":Component no longer reachable: Caught omniORB::fatalException.\n";
      stringstream log;
      log << "  file: " << fe.file() << endl;
      log << "  line: " << fe.line() << endl;
      log << "  mesg: " << fe.errmsg() << endl;
      msg += log.str();
    }
  catch(...) 
    {
      msg = ":Component no longer reachable: Caught unknown exception.";
    }
  return msg;
}

void SalomeNode::shutdown(int level)
{
  DEBTRACE("SalomeNode::shutdown " << level);
  if(_component)
    _component->shutdown(level);
}
