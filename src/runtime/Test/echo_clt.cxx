#include <echo.hh>
#include <iostream>

int main(int argc, char** argv)
{
  try {
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
    CORBA::Object_var obj = orb->string_to_object("corbaname:rir:#test.my_context/Echo.Object");
    eo::Echo_var echoref = eo::Echo::_narrow(obj);
    if( CORBA::is_nil(echoref) ) {
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

    orb->destroy();
  }
  catch(CORBA::COMM_FAILURE& ex) {
      std::cerr << "Caught system exception COMM_FAILURE -- unable to contact the "
         << "object." << std::endl;
  }
  catch(CORBA::SystemException&) {
      std::cerr << "Caught a CORBA::SystemException." << std::endl;
  }
  catch(CORBA::Exception&) {
      std::cerr << "Caught CORBA::Exception." << std::endl;
  }
  catch(omniORB::fatalException& fe) {
      std::cerr << "Caught omniORB::fatalException:" << std::endl;
      std::cerr << "  file: " << fe.file() << std::endl;
      std::cerr << "  line: " << fe.line() << std::endl;
      std::cerr << "  mesg: " << fe.errmsg() << std::endl;
  }
  catch(...) {
      std::cerr << "Caught unknown exception." << std::endl;
  }
  return 0;
}
