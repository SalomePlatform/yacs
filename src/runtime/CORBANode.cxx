
//#define REFCNT
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

#ifdef SALOME_KERNEL
#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#endif

#include <omniORB4/CORBA.h>
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
  DEBTRACE( "+++++++++++++ CorbaNode::execute: " << getName() << " +++++++++++++++" );
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
      }

    //return value
    req->set_return_type(CORBA::_tc_void);
    //user exceptions
    //req->exceptions()->add(eo::_tc_SALOME_Exception);
    
    DEBTRACE( "+++++++++++++++++CorbaNode::calculation+++++++++++++++++" << _method )
    req->invoke();
    CORBA::Exception *exc =req->env()->exception();
    if( exc )
      {
        DEBTRACE( "An exception was thrown!" )
        DEBTRACE( "The raised exception is of Type:" << exc->_name() )
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
      throw Exception("Can't get reference to DSC object (or it was nil).");
    }
  compo->init_service(_method.c_str());
}

//! Connect the datastream ports of the component associated to the node
void SalomeNode::connectService()
{
  DEBTRACE( "SalomeNode::connectService: "<<getName())
  if(_setOfOutputDataStreamPort.size() == 0)return;

  CORBA::Object_var objComponent=((SalomeComponent*)_component)->getCompoPtr();
  SALOME_NamingService NS(getSALOMERuntime()->getOrb()) ;
  SALOME_LifeCycleCORBA LCC(&NS) ;
  CORBA::Object_var obj = NS.Resolve("/ConnectionManager");
  Engines::ConnectionManager_var manager=Engines::ConnectionManager::_narrow(obj);
  Engines::Superv_Component_var me=Engines::Superv_Component::_narrow(objComponent);
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
            throw Exception("Can't connect : not a SalomeNode");

          CORBA::Object_var comp=((SalomeComponent*)snode->getComponent())->getCompoPtr();
          Engines::Superv_Component_var other=Engines::Superv_Component::_narrow(comp);
          id=manager->connect(me,port->getName().c_str(),other,(*iterout)->getName().c_str());
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
  DEBTRACE( "SalomeNode::disconnectService: "<<getName())
  if(ids.size() == 0)return;

  SALOME_NamingService NS(getSALOMERuntime()->getOrb()) ;
  SALOME_LifeCycleCORBA LCC(&NS) ;
  CORBA::Object_var obj = NS.Resolve("/ConnectionManager");
  Engines::ConnectionManager_var manager=Engines::ConnectionManager::_narrow(obj);
  std::list<Engines::ConnectionManager::connectionId>::iterator iter;
  for(iter = ids.begin(); iter != ids.end(); iter++)
    {
      manager->disconnect(*iter,Engines::DSC::RemovingConnection);
    }
  ids.clear();
}
#endif

//! Execute the service on the component associated to the node
void SalomeNode::execute()
{
  DEBTRACE( "+++++++++++++++++ SalomeNode::execute: " << getName() << " +++++++++++++++++" )
  {
    CORBA::Object_var objComponent=((SalomeComponent*)_component)->getCompoPtr();
    //DII request building :
    // a service gets all its in parameters first
    // then all its out parameters
    // no inout parameters
    // the return value (if any) is the first out parameter
    // not yet user exception (only CORBA exception)
    //
    CORBA::Request_var req = objComponent->_request(_method.c_str());
    CORBA::NVList_ptr arguments = req->arguments() ;

    DEBTRACE( "+++++++++++++++++SalomeNode::inputs+++++++++++++++++" )
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
          CORBA::TypeCode_var tc;
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
    
    //out parameters
    DEBTRACE( "+++++++++++++++++SalomeNode::outputs+++++++++++++++++" )
    list<OutputPort *>::iterator iter;
    for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
          OutputCorbaPort *p=(OutputCorbaPort *)*iter;
          DEBTRACE( "port name: " << p->getName() )
          DEBTRACE( "port kind: " << p->edGetType()->kind() )
          CORBA::Any* ob=p->getAnyOut();
          //add_value makes a copy of any. Copy will be deleted with request
          arguments->add_value( p->getName().c_str() , *ob , CORBA::ARG_OUT );
    }

    //return value
    //if return type is set to void (not mandatory, it's set by default)
    //the return value will not be marshalled as a return value but 
    //as the first out argument (don't forget to add it as the first output argument)
    req->set_return_type(CORBA::_tc_void);
    //user exceptions
    //req->exceptions()->add(eo::_tc_SALOME_Exception);
    
    DEBTRACE( "+++++++++++++++++SalomeNode::calculation+++++++++++++++++" << _method )
    req->invoke();
    CORBA::Exception *exc =req->env()->exception();
    if( exc )
    {
      DEBTRACE( "An exception was thrown!" )
      DEBTRACE( "The raised exception is of Type:" << exc->_name() )
      throw Exception("Execution problem");
    }
    
    DEBTRACE( "++++++++++++SalomeNode::outputs++++++++++++" )
    int out_param=in_param;
    for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
          OutputCorbaPort *p=(OutputCorbaPort *)*iter;
          DEBTRACE( "port name: " << p->getName() )
          DEBTRACE( "port kind: " << p->edGetType()->kind() )
          DEBTRACE( "port number: " << out_param )
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
