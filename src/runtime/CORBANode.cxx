
#include "CORBANode.hxx"
#include "RuntimeSALOME.hxx"

#include <omniORB4/CORBA.h>
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

CORBANode::CORBANode(const std::string& name): ElementaryNode(name)
{
  _implementation = "CORBA";
  cerr << "CORBANode::CORBANode " << name << endl;
}

void CORBANode::set_ref(const string& ref)
{
  _ref = ref;
}

void CORBANode::set_method(const string& method) 
{
  _method = method;
}

void CORBANode::execute()
{
  cerr << "+++++++++++++++++CorbaNode::run+++++++++++++++++" << endl;
  //recupération de l'objet CORBA dont l'IOR est _ref
  int argc=0;
  CORBA::ORB_var orb = CORBA::ORB_init(argc,0);
  //CORBA::Object_var obj = getObjectReference(orb);
  CORBA::Object_var obj = orb->string_to_object(_ref.c_str());
  if( CORBA::is_nil(obj) )
    {
      cerr << "Can't get reference to object (or it was nil)." << endl;
      return ;
    }
  {
    //construction de la requete DII : ATTENTION aux restrictions et approximations
    // on suppose qu'un service recoit tous ses parametres in en premier
    // puis tous ses parametres out
    // pas de parametre inout
    // pas de valeur de retour
    // pas encore d'exception utilisateur
    // seulement des exceptions CORBA
    //
    CORBA::Request_var req = obj->_request(_method.c_str());
    CORBA::NVList_ptr arguments = req->arguments() ;

    cerr << "+++++++++++++++++CorbaNode::inputs+++++++++++++++++" << endl;
    int in_param=0;
    //les parametres in
    set<InputPort *>::iterator iter2;
    for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++)
      {
	InputCorbaPort *p=(InputCorbaPort *)*iter2;
	cerr << "port name: " << p->getName() << endl;
	cerr << "port kind: " << p->type()->kind() << endl;
	CORBA::Any* ob=p->getAny();
	CORBA::TypeCode_var typcod= ob->type();
	switch(p->type()->kind())
	  {
	  case Double:
	    CORBA::Double d;
	    *ob >>= d;
	    cerr << d << endl;
	    break;
	  case Int:
	    CORBA::Long l;
	    *ob >>= l;
	    cerr << l << endl;
	    break;
	  case String:
	    char *s;
	    *ob >>= s;
	    cerr << s << endl;
	    break;
	  case Objref:
	    cerr << typcod->id() << endl;
	    break;
	  default:
	    break;
	  }
	//add_value fait une copie du any. La copie sera détruite avec la requete
	arguments->add_value( p->getName().c_str() , *ob , CORBA::ARG_IN ) ;
	in_param=in_param+1;
      }
    
    //les parametres out
    cerr << "+++++++++++++++++CorbaNode::outputs+++++++++++++++++" << endl;
    set<OutputPort *>::iterator iter;
    for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
      {
	OutputCorbaPort *p=(OutputCorbaPort *)*iter;
	cerr << "port name: " << p->getName() << endl;
	cerr << "port kind: " << p->type()->kind() << endl;
	CORBA::Any* ob=p->getAnyOut();
	//add_value fait une copie du any. La copie sera détruite avec la requete
	arguments->add_value( p->getName().c_str() , *ob , CORBA::ARG_OUT );
      }

    //valeur de retour
    req->set_return_type(CORBA::_tc_void);
    //autres exceptions
    //req->exceptions()->add(eo::_tc_SALOME_Exception);
    
    cerr << "+++++++++++++++++CorbaNode::calculation+++++++++++++++++" << _method << endl;
    req->invoke();
    CORBA::Exception *exc =req->env()->exception();
    if( exc )
      {
	cerr << "An exception was thrown!" << endl;
	cerr << "The raised exception is of Type:" << exc->_name() << endl;
	return ;
      }
    
    cerr << "++++++++++++CorbaNode::outputs++++++++++++" << endl;
    int out_param=in_param;
    for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
      {
	OutputCorbaPort *p=(OutputCorbaPort *)*iter;
	cerr << "port name: " << p->getName() << endl;
	cerr << "port kind: " << p->type()->kind() << endl;
	cerr << "port number: " << out_param << endl;
	CORBA::Any *ob=arguments->item(out_param)->value();
	switch(p->type()->kind())
	  {
	  case Double:
	    CORBA::Double d;
	    *ob >>= d;
	    cerr << d << endl;
	    break;
	  case Int:
	    CORBA::Long l;
	    *ob >>= l;
	    cerr << l << endl;
	    break;
	  case String:
	    char *s;
	    *ob >>= s;
	    cerr << s << endl;
	    break;
	  default:
	    break;
	  }
	//L'OutputPort doit copier l'Any car il sera détruit avec la requete
	//La copie est faite dans la methode put.
	p->put(ob);
	out_param=out_param+1;
      }

    cerr << "++++++++++++++++++++++++++++++++++++++++++" << endl;
  }
  //La requete n'existe plus ici (_var oblige)
  //Tous les any passés a la requete sont détruits : il faut les copier
}
