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

#include <echo.hh>
#include <iostream>
#include <vector>
#include <map>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

int main(int argc, char** argv)
{
  try 
    {
      CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
      CORBA::Object_var obj = orb->string_to_object("corbaname:rir:#test.my_context/Echo.Object");
      eo::Echo_var echoref = eo::Echo::_narrow(obj);
      if( CORBA::is_nil(echoref) ) 
        {
          std::cerr << "Can't narrow reference to type Echo (or it was nil)." << std::endl;
          return 1;
        }

      CORBA::String_var src = (const char*) "Hello!";
      CORBA::String_var dest = echoref->echoString(src);
      std::cerr << "I said, \"" << (char*)src << "\"." << std::endl
       << "The Echo object replied, \"" << (char*)dest <<"\"." << std::endl;

      CORBA::Object_var ob = orb->string_to_object("corbaname:rir:#test.my_context/D.Object");
      eo::D_var Dref = eo::D::_narrow(ob);
      if( CORBA::is_nil(Dref) ) {
        std::cerr << "Can't narrow reference to type D (or it was nil)." << std::endl;
        return 1;
        }
      Dref->echoLong2(10);

      eo::D_var dout;
      echoref->echoD(Dref,dout);
      std::cerr << dout->echoLong2(10) << std::endl;

      eo::D_var ddout;
      echoref->echoD(dout,ddout);
      std::cerr << dout->echoLong2(10) << std::endl;

      CORBA::Object_var oob = orb->string_to_object("corbaname:rir:#test.my_context/Obj.Object");
      eo::Obj_var Objref = eo::Obj::_narrow(oob);
      Objref->echoLong(10);
      eo::Obj_var Objout;
      echoref->echoObj2(Objref,Objout);
      std::cerr << Objout->echoLong(10) << std::endl;

      CORBA::Object_var cob = orb->string_to_object("corbaname:rir:#test.my_context/C.Object");
      eo::C_var Cref = eo::C::_narrow(cob);
      eo::C_var Cout;
      echoref->echoC(Cref,Cout); 

      echoref->echoObj2(Cref,Objout); 
      //echoref->echoC(Cref,Objout); compilation impossible
      //echoref->echoObj2(Cref,Cout);  compilation impossible
      //echoref->echoC(Objref,Cout); compilation impossible

      CORBA::Object_var dobj = orb->resolve_initial_references("DynAnyFactory");
      DynamicAny::DynAnyFactory_var dynFactory = DynamicAny::DynAnyFactory::_narrow(dobj);

      CORBA::TypeCode_var content_type=orb->create_interface_tc("IDL:eo/Obj:1.0","Obj");

      std::vector<CORBA::Any*> v;
      std::vector<CORBA::Any*>::const_iterator iter;

      CORBA::Any *any = new CORBA::Any();
      *any <<= cob;
      v.push_back(any);
      /*
      */
      CORBA::Any *any2 = new CORBA::Any();
      *any2 <<= oob;
      v.push_back(any2);
#ifdef REFCNT
      std::cerr << "refcount: " << oob->_PR_getobj()->pd_refCount << std::endl;
#endif

      CORBA::Any *any3 = new CORBA::Any();
      *any3 <<= oob;
      v.push_back(any3);
#ifdef REFCNT
      std::cerr << "refcount: " << oob->_PR_getobj()->pd_refCount << std::endl;
#endif

      eo::Obj_ptr xobj;
      *any2 >>= xobj;
#ifdef REFCNT
      std::cerr << "refcount: " << xobj->_PR_getobj()->pd_refCount << std::endl;
#endif
      eo::Obj_ptr yobj;
      *any2 >>= yobj;
#ifdef REFCNT
      std::cerr << "refcount: " << yobj->_PR_getobj()->pd_refCount << std::endl;
#endif

      CORBA::Object_var zobj ;
      *any2 >>= CORBA::Any::to_object(zobj) ;
#ifdef REFCNT
      std::cerr << "refcount: " << zobj->_PR_getobj()->pd_refCount << std::endl;
#endif

      // Build an Any from vector v
      int homog=1;
      CORBA::TypeCode_var tc;

      if(v.size() != 0)
        {
          // Check if it's an homogeneous vector : all TypeCodes must be equivalent
          CORBA::TypeCode_var any_tc=v[0]->type();
          tc=orb->create_sequence_tc(0,any_tc);

          for(iter=v.begin();iter!=v.end();iter++)
            {
              CORBA::Any* a=*iter;
              CORBA::TypeCode_var tc2=a->type();
              if(!tc2->equivalent(any_tc))
                {
                  // It's a non homogeneous vector : create a sequence of any
                  tc=orb->create_sequence_tc(0,CORBA::_tc_any);
                  homog=0;
                  break;
                }
            }
          // It's a homogeneous vector : create a sequence of the homogeneous type
        }
      else
        {
          // It's an empty vector : create a sequence of any
          tc=orb->create_sequence_tc(0,CORBA::_tc_any);
          homog=0;
        }

      DynamicAny::DynAny_var dynany = dynFactory->create_dyn_any_from_type_code(tc);
      DynamicAny::DynSequence_var ds = DynamicAny::DynSequence::_narrow(dynany);

      ds->set_length(v.size());
      for(iter=v.begin();iter!=v.end();iter++)
        {
          DynamicAny::DynAny_var temp=ds->current_component();
          CORBA::Any* a=*iter;
          if(homog)
            temp->from_any(*a);
          else
            ds->insert_any(*a);
          //delete intermediate any
          delete a;
          ds->next();
        }
      CORBA::Any_var seqany=ds->to_any();
      ds->destroy();

      //DynStruct from map
      std::map<std::string,CORBA::Any*> m;
      CORBA::Any *any4 = new CORBA::Any();
      *any4 <<= oob;
      //*any4 <<= cob;
      m["ob"]=any4;
      CORBA::Any *any5 = new CORBA::Any();
      *any5 <<= 1.0;
      m["x"]=any5;
      CORBA::Any *any6 = new CORBA::Any();
      *any6 <<= (long)1;
      m["y"]=any6;
      CORBA::Any *any7 = new CORBA::Any();
      *any7 <<= (const char*)"coucou";
      m["s"]=any7;
      CORBA::Any *any8 = new CORBA::Any();
      *any8 <<= CORBA::Any::from_boolean(1);
      m["b"]=any8;

/*
  struct S3
  {
    double x;
    long y;
    string s;
    boolean b;
    Obj ob;
  };
*/

      int nMember=5;
      CORBA::StructMemberSeq mseq;
      mseq.length(nMember);
      mseq[0].name=CORBA::string_dup("x");
      mseq[0].type=CORBA::TypeCode::_duplicate(CORBA::_tc_double);
      mseq[1].name=CORBA::string_dup("y");
      mseq[1].type=CORBA::TypeCode::_duplicate(CORBA::_tc_long);
      mseq[2].name=CORBA::string_dup("s");
      mseq[2].type=CORBA::TypeCode::_duplicate(CORBA::_tc_string);
      mseq[3].name=CORBA::string_dup("b");
      mseq[3].type=CORBA::TypeCode::_duplicate(CORBA::_tc_boolean);
      mseq[4].name=CORBA::string_dup("ob");
      mseq[4].type=content_type;
      for(int i=0;i<nMember;i++)
        {
          const char* name=mseq[i].name;
          std::cerr << mseq[i].name << std::endl;
          if(m.count(name) !=0)
            {
              std::cerr << "member: " << mseq[i].name << std::endl;
              mseq[i].type=m[name]->type();
            }
        }
      CORBA::TypeCode_var ts= orb->create_struct_tc("","",mseq);
      //CORBA::TypeCode_var ts= orb->create_struct_tc("IDL:eo/S3:1.0","S3",mseq);
      dynany = dynFactory->create_dyn_any_from_type_code(ts);
      DynamicAny::DynStruct_var dst = DynamicAny::DynStruct::_narrow(dynany);

      for(int i=0;i<nMember;i++)
        {
          DynamicAny::DynAny_var temp=dst->current_component();
          const char* name=mseq[i].name;
          std::cerr << name << std::endl;
          CORBA::Any* a=m[name];
          temp->from_any(*a);
          //delete intermediate any
          delete a;
          dst->next();
        }
      CORBA::Any_var structany=dst->to_any();
      dst->destroy();

      /*
      for(int i=0;i<nMember;i++)
        {
              const char * name=tst->memberName(i);
              //TypeCode* tm=tst->memberType(i);
              //do not test member presence : test has been done in ToYacs convertor
              CORBA::Any* a=m[name];
              members[i].id=CORBA::string_dup(name);
              members[i].value=*a;
              //delete intermediate any
              delete a;
         }
         */


      //DII invoke
      CORBA::TypeCode_var tcout=orb->create_sequence_tc(0,content_type);

      CORBA::Request_var req = echoref->_request("echoStruct2");
      //CORBA::Request_var req = echoref->_request("echoObjectVec");
      CORBA::NVList_ptr arguments = req->arguments() ;

      arguments->add_value( "s1" , structany , CORBA::ARG_IN ) ;
      //arguments->add_value( "s1" , seqany , CORBA::ARG_IN ) ;

      CORBA::Any out;
      out.replace(ts, (void*) 0);
      //out.replace(tcout, (void*) 0);
      arguments->add_value( "s2" , out , CORBA::ARG_OUT );

      req->set_return_type(CORBA::_tc_void);
      //user exceptions
      //req->exceptions()->add(eo::_tc_SALOME_Exception);

      req->invoke();
      CORBA::Exception *exc =req->env()->exception();
      if( exc )
        {
          std::cerr << "The raised exception is of Type:" << exc->_name() << std::endl;
          if(strcmp(exc->_name(),"MARSHAL") == 0)
            {
              const char* ms = ((CORBA::MARSHAL*)exc)->NP_minorString();
              if (ms)
                std::cerr << "(CORBA::MARSHAL: minor = " << ms << ")" << std::endl;
            }
        }

      //DII on echoObj2
      std::cerr << "Before request" << std::endl;
      req = echoref->_request("echoObj2");
      std::cerr << "After request" << std::endl;
      req->add_in_arg() <<= Objref;
      std::cerr << "After add_in_arg" << std::endl;
      req->add_out_arg() <<= Objout;
      std::cerr << "After add_out_arg" << std::endl;
      req->set_return_type(CORBA::_tc_void);
      std::cerr << "After set_return_type" << std::endl;
      req->invoke();
      std::cerr << "After invoke" << std::endl;
      exc =req->env()->exception();
      if( exc )
        {
          std::cerr << "The raised exception is of Type:" << exc->_name() << std::endl;
        }
      //end of DII on echoObj2

      //DII on echoObjVec
      req = echoref->_request("echoObjVec");
      CORBA::Any *aAny = new CORBA::Any();
      *aAny <<= oob;

      CORBA::TypeCode_var any_tc2=aAny->type();
      CORBA::TypeCode_var tc2=orb->create_sequence_tc(0,any_tc2);
      DynamicAny::DynAny_var dynany2 = dynFactory->create_dyn_any_from_type_code(tc2);
      DynamicAny::DynSequence_var ds2 = DynamicAny::DynSequence::_narrow(dynany2);
      ds2->set_length(2);
      DynamicAny::DynAny_var temp=ds2->current_component();
      temp->from_any(*aAny);
      ds2->next();
      temp=ds2->current_component();
      CORBA::Any *aAny2 = new CORBA::Any();
      *aAny2 <<= cob;
      CORBA::Object_var zzobj ;
      *aAny2 >>= CORBA::Any::to_object(zzobj) ;
      temp->insert_reference(zzobj);
      ds2->next();

      CORBA::Any_var seqany2=ds2->to_any();
      ds2->destroy();

      arguments = req->arguments() ;
      arguments->add_value( "s1" , seqany2 , CORBA::ARG_IN ) ;

      CORBA::Any out2;
      out2.replace(tc2, (void*) 0);
      arguments->add_value( "s2" , out2 , CORBA::ARG_OUT );
      req->set_return_type(CORBA::_tc_void);
      req->invoke();

      CORBA::Exception *exc2 =req->env()->exception();
      if( exc2 )
        {
          std::cerr << "The raised exception is of Type:" << exc2->_name() << std::endl;
          if(strcmp(exc2->_name(),"MARSHAL") == 0)
            {
              const char* ms = ((CORBA::MARSHAL*)exc2)->NP_minorString();
              if (ms)
                std::cerr << "(CORBA::MARSHAL: minor = " << ms << ")" << std::endl;
            }
        }
      else
        {
          CORBA::Any *ob=arguments->item(1)->value();
          DynamicAny::DynAny_var dynany= dynFactory->create_dyn_any(*ob);
          DynamicAny::DynSequence_ptr ds=DynamicAny::DynSequence::_narrow(dynany);
          DynamicAny::AnySeq_var as=ds->get_elements();
          int length=as->length();
          DEBTRACE(length);
          for(int i=0;i<length; i++)
            {
              CORBA::TypeCode_var t=as[i].type();
              DEBTRACE(t->id());
              eo::Obj_ptr xobj;
              as[i] >>= xobj;
              DEBTRACE(xobj->_PD_repoId);
              xobj->echoLong(12);
            };
        }

      //end of DII on echoObjVec

      // Delete allocated objects to remove memory leaks (valgrind tests)

      orb->destroy();
    }
  catch(DynamicAny::DynAny::TypeMismatch& ex)
    {
      std::cerr << "Caught a DynamicAny::DynAny::TypeMismatch exception" << std::endl;
    }
  catch(DynamicAny::DynAny::InvalidValue& ex)
    {
      std::cerr << "Caught a DynamicAny::DynAny::InvalidValue exception" << std::endl;
    }
  catch(CORBA::COMM_FAILURE& ex) 
    {
      std::cerr << "Caught system exception COMM_FAILURE -- unable to contact the object." << std::endl;
    }
  catch(CORBA::SystemException&) 
    {
      std::cerr << "Caught a CORBA::SystemException." << std::endl;
    }
  catch(CORBA::Exception&) 
    {
      std::cerr << "Caught CORBA::Exception." << std::endl;
    }
  catch(omniORB::fatalException& fe) 
    {
      std::cerr << "Caught omniORB::fatalException:" << std::endl;
      std::cerr << "  file: " << fe.file() << std::endl;
      std::cerr << "  line: " << fe.line() << std::endl;
      std::cerr << "  mesg: " << fe.errmsg() << std::endl;
    }
  catch(...) 
    {
      std::cerr << "Caught unknown exception." << std::endl;
    }
  return 0;
}
