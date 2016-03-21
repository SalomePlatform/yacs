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

// --- include from engine first, to avoid redifinition warning _POSIX_C_SOURCE
//
#include "Bloc.hxx"
#include "Proc.hxx"
#include "Logger.hxx"
#include "ElementaryNode.hxx"
#include "Loop.hxx"
#include "Switch.hxx"
#include "VisitorSaveState.hxx"
#include "SetOfPoints.hxx"

#include "SharedPtr.hxx"
#include "RuntimeForEngineTest.hxx"

#include "engineTest.hxx"

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <string.h>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace YACS;
using namespace std;

map<string, Node*> EngineTest::_nodeMap; 
map<string, ComposedNode*> EngineTest::_compoMap;

// --- init typecodes

TypeCode *EngineTest::_tc_bool   = new TypeCode(Bool);
TypeCode *EngineTest::_tc_int    = new TypeCode(Int);
TypeCode *EngineTest::_tc_double = new TypeCode(Double);
    

void EngineTest::setUp()
{
}

void EngineTest::tearDown()
{
}

void EngineTest::cleanUp()
{
  map<string, ComposedNode*>::iterator iter2,iter3;
  for(map<string, Node*>::iterator iter=_nodeMap.begin();iter!=_nodeMap.end();iter++)
    if((*iter).second->getFather()==0)
      delete (*iter).second;
}

void EngineTest::checkGetRuntime()
{
  CPPUNIT_ASSERT_THROW(Runtime *myrun = getRuntime(), YACS::Exception); 
  RuntimeForEngineTest::setRuntime();
  Runtime *myrun1 = getRuntime();
  CPPUNIT_ASSERT(myrun1);
  Runtime *myrun2 = getRuntime();
  CPPUNIT_ASSERT_EQUAL(myrun1, myrun2);
  }

void toto2(void *t)
{
  delete [] (char *)t;
}

void toto3(void *t)
{
  delete [] (int *)t;
}

void EngineTest::checkAny1()
{
  char *toto=new char[10];
  strcpy(toto,"lkjlkj");
  Any *tmp=AtomAny::New(toto,toto2);//no copy here
  CPPUNIT_ASSERT( tmp->getStringValue() == "lkjlkj");
  CPPUNIT_ASSERT( tmp->getStringValue() == "lkjlkj");
  tmp->incrRef();
  CPPUNIT_ASSERT( tmp->getStringValue() == "lkjlkj");
  CPPUNIT_ASSERT( tmp->getStringValue() == "lkjlkj");
  tmp->decrRef();
  CPPUNIT_ASSERT( tmp->getStringValue() == "lkjlkj");
  CPPUNIT_ASSERT( tmp->getStringValue() == "lkjlkj");
  tmp->decrRef();
  const char tmp2[]="coucou";
  tmp=AtomAny::New((char *)tmp2,0);
  CPPUNIT_ASSERT( tmp->getStringValue() == "coucou");
  CPPUNIT_ASSERT( tmp->getStringValue() == "coucou");
  tmp->decrRef();
  tmp=AtomAny::New(string("abcdef"));
  CPPUNIT_ASSERT( tmp->getStringValue() == "abcdef");
  CPPUNIT_ASSERT( tmp->getStringValue() == "abcdef");
  tmp->decrRef();
  tmp=AtomAny::New("ghijk");
  CPPUNIT_ASSERT( tmp->getStringValue() == "ghijk");
  CPPUNIT_ASSERT( tmp->getStringValue() == "ghijk");
  tmp->decrRef();
  tmp=AtomAny::New((char *)"ghijk");
  CPPUNIT_ASSERT( tmp->getStringValue() == "ghijk");
  CPPUNIT_ASSERT( tmp->getStringValue() == "ghijk");
  tmp->decrRef();
}

class A7
{
private:
  double _d;
  int _cnt;
public:
  A7(double toto):_d(toto),_cnt(1) { }
  static A7 *New(double toto) { return new A7(toto); }
  double getToto() const { return _d; }
  void setToto(double val) { _d=val; }
  void incrRef() { _cnt++; }
  void decrRef();
private:
  ~A7() { }
};

void A7::decrRef()
{
  if(--_cnt==0)
    delete this;
}


void checkSharedPtrFct2(const A7& a)
{
  a.getToto(); 
}

void checkSharedPtrFct1(const SharedPtr<A7>& a)
{
  checkSharedPtrFct2(a);
}


void EngineTest::checkSharedPtr()
{
  SharedPtr<A7> titi=A7::New(5.);
  SharedPtr<A7> toto=A7::New(5.1);
  SharedPtr<A7> tutu(toto);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(5.,titi->getToto(),1e-12);
  titi->setToto(7.1);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(7.1,titi->getToto(),1e-12);
  checkSharedPtrFct2(titi);
  checkSharedPtrFct2(toto);
  titi=toto;
  checkSharedPtrFct2(titi);
  checkSharedPtrFct2(tutu);
}

void EngineTest::checkAny2()
{
  double tabStack[8]={1.2, 3.4, 5.6, 7.8, 9.0, 2.3, 4.5, 6.7};
  double *tabHeap=new double[8];
  memcpy(tabHeap,tabStack,8*sizeof(double));
  SequenceAnyPtr tmp(SequenceAny::New(tabStack,8,0));
  CPPUNIT_ASSERT_EQUAL(8,(int)tmp->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(5.6,(*tmp)[2]->getDoubleValue(),1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(6.7,(*tmp)[7]->getDoubleValue(),1e-12); 
  AtomAnyPtr tmp2(AtomAny::New(8.9));
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(9,(int)tmp->size());
  tmp2=AtomAny::New(10.2);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(10,(int)tmp->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(8.9,(*tmp)[8]->getDoubleValue(),1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(10.2,(*tmp)[9]->getDoubleValue(),1e-12); 
  tmp->popBack();
  CPPUNIT_ASSERT_EQUAL(9,(int)tmp->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(8.9,(*tmp)[8]->getDoubleValue(),1e-12);
  tmp2=AtomAny::New(10.3);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(10,(int)tmp->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(8.9,(*tmp)[8]->getDoubleValue(),1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(10.3,(*tmp)[9]->getDoubleValue(),1e-12); 
  tmp=SequenceAny::New(tabHeap,8,toto2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(5.6,(*tmp)[2]->getDoubleValue(),1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(6.7,(*tmp)[7]->getDoubleValue(),1e-12);
  tmp2=AtomAny::New(8.5);
  tmp->pushBack(tmp2);
  tmp2=AtomAny::New(10.27);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(8.5,(*tmp)[8]->getDoubleValue(),1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(10.27,(*tmp)[9]->getDoubleValue(),1e-12);
  CPPUNIT_ASSERT_EQUAL(10,(int)tmp->size()); 
  tmp->popBack();
  CPPUNIT_ASSERT_EQUAL(9,(int)tmp->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(8.5,(*tmp)[8]->getDoubleValue(),1e-12);
  tmp2=AtomAny::New(10.445);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(10,(int)tmp->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(8.5,(*tmp)[8]->getDoubleValue(),1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(10.445,(*tmp)[9]->getDoubleValue(),1e-12);
  // Idem but for int
  int tabStackI[8]={1, 3, 5, 7, 9, 2, 4, 6};
  int *tabHeapI=new int[8];
  memcpy(tabHeapI,tabStackI,8*sizeof(int));
  tmp=SequenceAny::New(tabStackI,8,0);
  CPPUNIT_ASSERT_EQUAL(5,(*tmp)[2]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(6,(*tmp)[7]->getIntValue());
  tmp2=AtomAny::New(8);
  tmp->pushBack(tmp2);
  tmp2=AtomAny::New(25);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(5,(*tmp)[2]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(6,(*tmp)[7]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(8,(*tmp)[8]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(25,(*tmp)[9]->getIntValue());
  //Same with no copy constructor
  tmp=SequenceAny::New(tabHeapI,8,toto3);
  CPPUNIT_ASSERT_EQUAL(5,(*tmp)[2]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(6,(*tmp)[7]->getIntValue());
  tmp2=AtomAny::New(8);
  tmp->pushBack(tmp2);
  tmp2=AtomAny::New(27);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(5,(*tmp)[2]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(6,(*tmp)[7]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(8,(*tmp)[8]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(27,(*tmp)[9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(10,(int)tmp->size()); 
  tmp->popBack();
  CPPUNIT_ASSERT_EQUAL(9,(int)tmp->size());
  CPPUNIT_ASSERT_EQUAL(8,(*tmp)[8]->getIntValue());
  tmp2=AtomAny::New(202);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(10,(int)tmp->size());
  CPPUNIT_ASSERT_EQUAL(8,(*tmp)[8]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp)[9]->getIntValue());
  try
    {
      double d=(*tmp)[2]->getDoubleValue();
      CPPUNIT_ASSERT(0);
    }
  catch(Exception& e)
    {
      CPPUNIT_ASSERT(std::string(e.what())=="Value is not a double");
    }
  SequenceAnyPtr tmp3=SequenceAny::New(tmp->getType());
  try
    {
      tmp3->pushBack(tmp2);
      CPPUNIT_ASSERT(0);
    }
  catch(Exception& e)
    {
      CPPUNIT_ASSERT(std::string(e.what())=="Invalid runtime of YACS::Any struct : having int and you want Sequence");
    }
  CPPUNIT_ASSERT_EQUAL(0,(int)tmp3->size());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp)[9]->getIntValue());
  tmp3->pushBack(tmp);
  CPPUNIT_ASSERT_EQUAL(1,(int)tmp3->size());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp3)[0][9]->getIntValue());
  tmp=SequenceAny::New(tabStackI,8,0);
  tmp3->pushBack(tmp);
  CPPUNIT_ASSERT_EQUAL(2,(int)tmp3->size());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp3)[0][9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(7,(*tmp3)[1][3]->getIntValue());
  tmp3->pushBack(tmp);
  tmp3->pushBack(tmp);
  CPPUNIT_ASSERT_EQUAL(4,(int)tmp3->size());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp3)[0][9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(7,(*tmp3)[1][3]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(7,(*tmp3)[3][3]->getIntValue());
  tmp2=AtomAny::New(89);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(4,(int)tmp3->size());
  CPPUNIT_ASSERT_EQUAL(9,(int)tmp->size());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp3)[0][9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(89,(*tmp3)[1][8]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(89,(*tmp3)[2][8]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(89,(*tmp3)[3][8]->getIntValue());
  tmp3->popBack();
  CPPUNIT_ASSERT_EQUAL(3,(int)tmp3->size());
  CPPUNIT_ASSERT_EQUAL(9,(int)tmp->size());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp3)[0][9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(89,(*tmp3)[1][8]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(89,(*tmp3)[2][8]->getIntValue());
  SequenceAnyPtr tmp4=(SequenceAny *)tmp3->clone();
  CPPUNIT_ASSERT_EQUAL(3,(int)tmp4->size());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp4)[0][9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(89,(*tmp4)[1][8]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(89,(*tmp4)[2][8]->getIntValue());
  tmp4->popBack();
  CPPUNIT_ASSERT_EQUAL(2,(int)tmp4->size());
  CPPUNIT_ASSERT_EQUAL(3,(int)tmp3->size());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp3)[0][9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp4)[0][9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(89,(*tmp3)[1][8]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(89,(*tmp4)[1][8]->getIntValue());
  tmp->popBack();
  tmp2=AtomAny::New(107);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(2,(int)tmp4->size());
  CPPUNIT_ASSERT_EQUAL(3,(int)tmp3->size());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp3)[0][9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(202,(*tmp4)[0][9]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(107,(*tmp3)[1][8]->getIntValue());//These 2 lines to show that deepCpy has been done
  CPPUNIT_ASSERT_EQUAL(89,(*tmp4)[1][8]->getIntValue());
}

void EngineTest::checkAny3()
{
  vector<string> vec;
  vec.push_back("tata00000"); vec.push_back("toto"); vec.push_back("tutu");
  SequenceAnyPtr tmp(SequenceAny::New(vec));
  CPPUNIT_ASSERT_EQUAL(3,(int)tmp->size());
  CPPUNIT_ASSERT((*tmp)[0]->getStringValue()=="tata00000");
  CPPUNIT_ASSERT((*tmp)[1]->getStringValue()=="toto");
  CPPUNIT_ASSERT((*tmp)[2]->getStringValue()=="tutu");
  tmp->pushBack(AtomAnyPtr(AtomAny::New("abcdefgh")));
  CPPUNIT_ASSERT_EQUAL(4,(int)tmp->size());
  CPPUNIT_ASSERT((*tmp)[0]->getStringValue()=="tata00000");
  CPPUNIT_ASSERT((*tmp)[1]->getStringValue()=="toto");
  CPPUNIT_ASSERT((*tmp)[2]->getStringValue()=="tutu");
  CPPUNIT_ASSERT((*tmp)[3]->getStringValue()=="abcdefgh");
  tmp->popBack();
  CPPUNIT_ASSERT_EQUAL(3,(int)tmp->size());
  CPPUNIT_ASSERT((*tmp)[0]->getStringValue()=="tata00000");
  CPPUNIT_ASSERT((*tmp)[1]->getStringValue()=="toto");
  CPPUNIT_ASSERT((*tmp)[2]->getStringValue()=="tutu");
  tmp->popBack();
  CPPUNIT_ASSERT_EQUAL(2,(int)tmp->size());
  CPPUNIT_ASSERT((*tmp)[0]->getStringValue()=="tata00000");
  CPPUNIT_ASSERT((*tmp)[1]->getStringValue()=="toto");
  tmp->popBack();
  CPPUNIT_ASSERT_EQUAL(1,(int)tmp->size());
  CPPUNIT_ASSERT((*tmp)[0]->getStringValue()=="tata00000");
  tmp->popBack();
  CPPUNIT_ASSERT_EQUAL(0,(int)tmp->size());
  //
  vector<bool> vec2;
  tmp=SequenceAny::New(vec2);
  CPPUNIT_ASSERT_EQUAL(0,(int)tmp->size());
  vec2.push_back(true); vec2.push_back(false); vec2.push_back(true); vec2.push_back(true); vec2.push_back(true);
  tmp=SequenceAny::New(vec2);
  CPPUNIT_ASSERT_EQUAL(5,(int)tmp->size());
  CPPUNIT_ASSERT((*tmp)[0]->getBoolValue() && !(*tmp)[1]->getBoolValue() && (*tmp)[2]->getBoolValue() && (*tmp)[3]->getBoolValue() && (*tmp)[4]->getBoolValue());
  //in perspective of SequenceAny of bool were optimized as std::vector<bool> does.
  tmp->pushBack(AtomAnyPtr(AtomAny::New(false))); tmp->pushBack(AtomAnyPtr(AtomAny::New(true))); tmp->pushBack(AtomAnyPtr(AtomAny::New(false)));
  tmp->pushBack(AtomAnyPtr(AtomAny::New(true)));
  CPPUNIT_ASSERT_EQUAL(9,(int)tmp->size());
  CPPUNIT_ASSERT((*tmp)[0]->getBoolValue() && !(*tmp)[1]->getBoolValue() && (*tmp)[2]->getBoolValue() && (*tmp)[3]->getBoolValue() && (*tmp)[4]->getBoolValue());
  CPPUNIT_ASSERT(!(*tmp)[5]->getBoolValue() && (*tmp)[6]->getBoolValue() && !(*tmp)[7]->getBoolValue() && (*tmp)[8]->getBoolValue());
  //
  vector<int> vec3;
  vec3.push_back(2); vec3.push_back(5); vec3.push_back(7); vec3.push_back(1); vec3.push_back(66); vec3.push_back(26);
  tmp=SequenceAny::New(vec3);
  CPPUNIT_ASSERT_EQUAL(6,(int)tmp->size());
  CPPUNIT_ASSERT_EQUAL(26,(*tmp)[5]->getIntValue());
  tmp->popBack();
  CPPUNIT_ASSERT_EQUAL(5,(int)tmp->size());
  //
  vector<double> vec4;
  vec4.push_back(2.78); vec4.push_back(3.14); vec4.push_back(0.07);
  tmp=SequenceAny::New(vec4);
  CPPUNIT_ASSERT_EQUAL(3,(int)tmp->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.07,(*tmp)[2]->getDoubleValue(),1e-12);
  tmp->popBack();
  CPPUNIT_ASSERT_EQUAL(2,(int)tmp->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.14,(*tmp)[1]->getDoubleValue(),1e-12);
}

void EngineTest::checkAny4()
{
  SequenceAnyPtr tmp(SequenceAny::New(Runtime::_tc_int,8));
  tmp=SequenceAny::New(Runtime::_tc_int,4);
  tmp=SequenceAny::New(Runtime::_tc_int);
  tmp=SequenceAny::New(Runtime::_tc_int,7);
  AnyPtr tmp2=AtomAny::New(107);
  tmp->setEltAtRank(3,tmp2);
  CPPUNIT_ASSERT_EQUAL(107,(*tmp)[3]->getIntValue());
  tmp=SequenceAny::New(Runtime::_tc_string,2);
  const char tmpSt[]="titi";
  tmp2=AtomAny::New((char*)tmpSt,0);
  tmp->setEltAtRank(1,tmp2);
  CPPUNIT_ASSERT((*tmp)[1]->getStringValue()=="titi");
  vector<double> vec4;
  vec4.push_back(2.78); vec4.push_back(3.14); vec4.push_back(0.07);
  tmp2=SequenceAny::New(vec4);
  tmp=SequenceAny::New(tmp2->getType(),3);
  tmp->setEltAtRank(0,tmp2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.14,(*tmp)[0][1]->getDoubleValue(),1e-12);
  CPPUNIT_ASSERT_EQUAL(3,(int)tmp->size());
  tmp->clear();
  CPPUNIT_ASSERT_EQUAL(0,(int)tmp->size());
  tmp->pushBack(tmp2);
  tmp->pushBack(tmp2);
  CPPUNIT_ASSERT_EQUAL(2,(int)tmp->size());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.14,(*tmp)[1][1]->getDoubleValue(),1e-12);
}

/*!
 * Testing Any::operator ==
 */
void EngineTest::checkAny5()
{
  //AtomAny
  AnyPtr tmp1=AtomAny::New(107);
  AnyPtr tmp2=AtomAny::New(107);
  CPPUNIT_ASSERT( *tmp1==*tmp2 );
  tmp1=AtomAny::New(106);
  CPPUNIT_ASSERT( ! (*tmp1==*tmp2) );
  CPPUNIT_ASSERT( ! (*tmp2==*tmp1) );
  tmp1=AtomAny::New("toto");
  CPPUNIT_ASSERT( ! (*tmp1==*tmp2) );
  tmp2=AtomAny::New("tot");
  CPPUNIT_ASSERT( ! (*tmp1==*tmp2) );
  tmp2=AtomAny::New("toto");
  CPPUNIT_ASSERT( (*tmp1==*tmp2) );
  tmp1=AtomAny::New("mmmmmlll");
  tmp2=tmp1->clone();
  CPPUNIT_ASSERT( (*tmp1==*tmp2) );
  //SequenceAny
  vector<string> vec;
  vec.push_back("tata00000"); vec.push_back("toto"); vec.push_back("tutu");
  SequenceAnyPtr tmp3=(SequenceAny::New(vec));
  CPPUNIT_ASSERT_EQUAL(3,(int)tmp3->size());
  CPPUNIT_ASSERT((*tmp3)[0]->getStringValue()=="tata00000");
  CPPUNIT_ASSERT((*tmp3)[1]->getStringValue()=="toto");
  CPPUNIT_ASSERT((*tmp3)[2]->getStringValue()=="tutu");
  AnyPtr tmp4=tmp3->clone();
  CPPUNIT_ASSERT( (*tmp3==*tmp4) );
  CPPUNIT_ASSERT( (*tmp3==*tmp3) );
  tmp3->popBack();
  CPPUNIT_ASSERT( !(*tmp3==*tmp4) );
  tmp1=AtomAny::New("tutu");
  tmp3->pushBack(tmp1);
  CPPUNIT_ASSERT( (*tmp3==*tmp4) );
   tmp3->pushBack(tmp1);
  CPPUNIT_ASSERT( !(*tmp3==*tmp4) );
  tmp3->popBack();
  CPPUNIT_ASSERT( *tmp3==*tmp4 );
  tmp3->popBack();
  CPPUNIT_ASSERT( !(*tmp3==*tmp4) );
  tmp1=AtomAny::New("tutug");
  tmp3->pushBack(tmp1);
  CPPUNIT_ASSERT( !(*tmp3==*tmp4) );
}

/*!
 * Test of ArrayAny. Only one level. 
 */
void EngineTest::checkAny6()
{
  const int lgth=8;
  const int tab[lgth]={0,1,1,9,2,7,2,3};
  const int tab2[lgth]={13,1,1,9,2,7,2,3};
  ArrayAnyPtr tmp1(ArrayAny::New(tab,lgth));
  ArrayAnyPtr tmp2(ArrayAny::New(tab,lgth));
  CPPUNIT_ASSERT((*tmp1)[3]->getIntValue()==9);
  CPPUNIT_ASSERT( (*tmp1==*tmp2) );
  CPPUNIT_ASSERT( (*tmp1==*tmp1) );
  CPPUNIT_ASSERT( 8==tmp1->size() );
  CPPUNIT_ASSERT( 8==tmp2->size() );
  tmp2=ArrayAny::New(tab,lgth-1);
  CPPUNIT_ASSERT( 7==tmp2->size() );
  CPPUNIT_ASSERT( !(*tmp1==*tmp2) );
  tmp2=ArrayAny::New(tab2,lgth);
  CPPUNIT_ASSERT( !(*tmp1==*tmp2) );
  tmp2=ArrayAny::New(tab,lgth);
  CPPUNIT_ASSERT( (*tmp1==*tmp2) );
  ArrayAnyPtr tmp3=(ArrayAny *)tmp1->clone();
  CPPUNIT_ASSERT( (*tmp1==*tmp3) );
  CPPUNIT_ASSERT((*tmp3)[3]->getIntValue()==9); CPPUNIT_ASSERT((*tmp3)[4]->getIntValue()==2);
  // Ok let's test with double.
  const double tab3[lgth]={0.1,1.1,1.1,9.1,2.1,7.1,2.1,3.1};
  const double tab4[lgth]={13.1,1.1,1.1,9.1,2.1,7.1,2.1,3.1};
  ArrayAnyPtr tmp4(ArrayAny::New(tab3,lgth));
  ArrayAnyPtr tmp5(ArrayAny::New(tab3,lgth));
  CPPUNIT_ASSERT_DOUBLES_EQUAL((*tmp4)[3]->getDoubleValue(),9.1,1e-12);
  CPPUNIT_ASSERT( (*tmp4==*tmp5) );
  CPPUNIT_ASSERT( (*tmp4==*tmp4) );
  CPPUNIT_ASSERT( 8==tmp4->size() );
  CPPUNIT_ASSERT( 8==tmp5->size() );
  tmp5=ArrayAny::New(tab3,lgth-1);
  CPPUNIT_ASSERT( 7==tmp5->size() );
  CPPUNIT_ASSERT( !(*tmp4==*tmp5) );
  tmp5=ArrayAny::New(tab4,lgth);
  CPPUNIT_ASSERT( !(*tmp4==*tmp5) );
  tmp5=ArrayAny::New(tab3,lgth);
  CPPUNIT_ASSERT( (*tmp4==*tmp5) );
  ArrayAnyPtr tmp6=(ArrayAny *)tmp4->clone();
  CPPUNIT_ASSERT( (*tmp4==*tmp6) );
  CPPUNIT_ASSERT_DOUBLES_EQUAL((*tmp6)[3]->getDoubleValue(),9.1,1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL((*tmp6)[4]->getDoubleValue(),2.1,1e-12);
  //Ok let's test with bool.
  const bool tab5[lgth]={false,true,false,true,false,false,false,false};
  const bool tab6[lgth]={true ,true,false,true,false,false,false,false};
  ArrayAnyPtr tmp7(ArrayAny::New(tab5,lgth));
  ArrayAnyPtr tmp8(ArrayAny::New(tab5,lgth));
  CPPUNIT_ASSERT((*tmp7)[3]->getBoolValue());
  CPPUNIT_ASSERT( (*tmp7==*tmp8) );
  CPPUNIT_ASSERT( (*tmp7==*tmp7) );
  CPPUNIT_ASSERT( 8==tmp7->size() );
  CPPUNIT_ASSERT( 8==tmp8->size() );
  tmp8=ArrayAny::New(tab5,lgth-1);
  CPPUNIT_ASSERT( 7==tmp8->size() );
  CPPUNIT_ASSERT( !(*tmp7==*tmp8) );
  tmp8=ArrayAny::New(tab6,lgth);
  CPPUNIT_ASSERT( !(*tmp7==*tmp8) );
  tmp8=ArrayAny::New(tab5,lgth);
  CPPUNIT_ASSERT( (*tmp7==*tmp8) );
  ArrayAnyPtr tmp9=(ArrayAny *)tmp7->clone();
  CPPUNIT_ASSERT( (*tmp9==*tmp7) );
  CPPUNIT_ASSERT((*tmp8)[3]->getBoolValue());
  CPPUNIT_ASSERT(!(*tmp8)[4]->getBoolValue());
  //Ok let's test with strings.
  vector<string> tab7(lgth); tab7[0]="abc"; tab7[1]="def"; tab7[2]="ghi"; tab7[3]="jkl"; tab7[4]="mno"; tab7[5]="pqr"; tab7[6]="stu"; tab7[7]="vwx";
  vector<string> tab8(lgth); tab8[0]="zbc"; tab8[1]="def"; tab8[2]="ghi"; tab8[3]="jkl"; tab8[4]="mno"; tab8[5]="pqr"; tab8[6]="stu"; tab8[7]="vwx";
  ArrayAnyPtr tmp10(ArrayAny::New(tab7));
  ArrayAnyPtr tmp11(ArrayAny::New(tab7));
  CPPUNIT_ASSERT((*tmp10)[3]->getStringValue()=="jkl");
  CPPUNIT_ASSERT( (*tmp10==*tmp11) );
  CPPUNIT_ASSERT( (*tmp10==*tmp10) );
  CPPUNIT_ASSERT( 8==tmp10->size() );
  CPPUNIT_ASSERT( 8==tmp11->size() );
  vector<string> tab9(tab7); tab9.pop_back();
  tmp11=ArrayAny::New(tab9);
  CPPUNIT_ASSERT( 7==tmp11->size() );
  CPPUNIT_ASSERT( !(*tmp10==*tmp11) );
  tmp11=ArrayAny::New(tab8);
  CPPUNIT_ASSERT( !(*tmp10==*tmp11) );
  tmp11=ArrayAny::New(tab7);
  CPPUNIT_ASSERT( (*tmp10==*tmp11) );
  ArrayAnyPtr tmp12=(ArrayAny *)tmp10->clone();
  CPPUNIT_ASSERT( (*tmp12==*tmp10) );
  CPPUNIT_ASSERT((*tmp11)[3]->getStringValue()=="jkl");
  CPPUNIT_ASSERT((*tmp11)[4]->getStringValue()=="mno");
}

/*!
 * Test of ArrayAny on multi-levels. To test recursion.
 */
void EngineTest::checkAny7()
{
  const int lgth=8;
  const int tab1[lgth]={0,1,1,9,2,7,2,3};
  const int tab2[lgth]={7,8,8,16,9,14,9,10};
  //Testing Arrays of arrays
  ArrayAnyPtr tmp11(ArrayAny::New(tab1,lgth));
  ArrayAnyPtr tmp12(ArrayAny::New(tab2,lgth));
  ArrayAnyPtr tmp12Throw(ArrayAny::New(tab2,lgth+1));
  ArrayAnyPtr tmp2(ArrayAny::New(tmp11->getType(),2));
  tmp2->setEltAtRank(0,tmp11);
  CPPUNIT_ASSERT_THROW(tmp2->setEltAtRank(1,tmp12Throw),Exception);
  tmp2->setEltAtRank(1,tmp12);
  CPPUNIT_ASSERT_EQUAL(0,(*tmp2)[0][0]->getIntValue()); CPPUNIT_ASSERT_EQUAL(7,(*tmp2)[1][0]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(9,(*tmp2)[0][3]->getIntValue()); CPPUNIT_ASSERT_EQUAL(16,(*tmp2)[1][3]->getIntValue());
  //    Show deep copy process of arrays
  AnyPtr tmp=AtomAny::New(49);
  tmp11->setEltAtRank(0,tmp);
  CPPUNIT_ASSERT_EQUAL(0,(*tmp2)[0][0]->getIntValue()); CPPUNIT_ASSERT_EQUAL(7,(*tmp2)[1][0]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(9,(*tmp2)[0][3]->getIntValue()); CPPUNIT_ASSERT_EQUAL(16,(*tmp2)[1][3]->getIntValue());
  ArrayAnyPtr tmp2Cloned=(ArrayAny *)(tmp2->clone());
  CPPUNIT_ASSERT_EQUAL(0,(*tmp2Cloned)[0][0]->getIntValue()); CPPUNIT_ASSERT_EQUAL(7,(*tmp2Cloned)[1][0]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(9,(*tmp2Cloned)[0][3]->getIntValue()); CPPUNIT_ASSERT_EQUAL(16,(*tmp2Cloned)[1][3]->getIntValue());
  CPPUNIT_ASSERT( *tmp2==*tmp2Cloned );
  CPPUNIT_ASSERT( *tmp2==*tmp2 );
  //Testing Array of Sequences
  SequenceAnyPtr tmp13(SequenceAny::New((int *)tab1,lgth,0));
  SequenceAnyPtr tmp14(SequenceAny::New((int *)tab2,lgth,0));
  tmp2=ArrayAny::New(tmp13->getType(),2);
  tmp2->setEltAtRank(0,tmp13);
  tmp2->setEltAtRank(1,tmp14);
  CPPUNIT_ASSERT_EQUAL(0,(*tmp2)[0][0]->getIntValue()); CPPUNIT_ASSERT_EQUAL(7,(*tmp2)[1][0]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(9,(*tmp2)[0][3]->getIntValue()); CPPUNIT_ASSERT_EQUAL(16,(*tmp2)[1][3]->getIntValue());
  tmp13->setEltAtRank(0,tmp);
  //    No deep copy here contrary to 14 lines ahead.
  CPPUNIT_ASSERT_EQUAL(49,(*tmp2)[0][0]->getIntValue()); CPPUNIT_ASSERT_EQUAL(7,(*tmp2)[1][0]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(9,(*tmp2)[0][3]->getIntValue()); CPPUNIT_ASSERT_EQUAL(16,(*tmp2)[1][3]->getIntValue());
  tmp2Cloned=(ArrayAny *)(tmp2->clone());
  CPPUNIT_ASSERT_EQUAL(49,(*tmp2Cloned)[0][0]->getIntValue()); CPPUNIT_ASSERT_EQUAL(7,(*tmp2Cloned)[1][0]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(9,(*tmp2Cloned)[0][3]->getIntValue()); CPPUNIT_ASSERT_EQUAL(16,(*tmp2Cloned)[1][3]->getIntValue());
  CPPUNIT_ASSERT( *tmp2==*tmp2Cloned );
  CPPUNIT_ASSERT( *tmp2==*tmp2 );
  //Testing Sequence of array
  tmp13=SequenceAny::New(tmp11->getType(),2);
  tmp13->setEltAtRank(0,tmp11);
  tmp13->setEltAtRank(1,tmp12);
  CPPUNIT_ASSERT_EQUAL(49,(*tmp13)[0][0]->getIntValue()); CPPUNIT_ASSERT_EQUAL(7,(*tmp13)[1][0]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(9,(*tmp13)[0][3]->getIntValue()); CPPUNIT_ASSERT_EQUAL(16,(*tmp13)[1][3]->getIntValue());
  tmp14=(SequenceAny *)tmp13->clone();
  CPPUNIT_ASSERT_EQUAL(49,(*tmp14)[0][0]->getIntValue()); CPPUNIT_ASSERT_EQUAL(7,(*tmp14)[1][0]->getIntValue());
  CPPUNIT_ASSERT_EQUAL(9,(*tmp14)[0][3]->getIntValue()); CPPUNIT_ASSERT_EQUAL(16,(*tmp14)[1][3]->getIntValue());
  CPPUNIT_ASSERT( *tmp13==*tmp14 );
  CPPUNIT_ASSERT( *tmp13==*tmp13 );
}

/*!
 * First test of structs.
 */
void EngineTest::checkAny8()
{
  TypeCodeStruct *tc=new TypeCodeStruct("","");
  tc->addMember("MyInt",Runtime::_tc_int);
  tc->addMember("MyDouble",Runtime::_tc_double);
  tc->addMember("MyBool",Runtime::_tc_bool);
  tc->addMember("MyStr",Runtime::_tc_string);
  StructAnyPtr tmp1=StructAny::New(tc);
  tmp1=StructAny::New(tc);
  AnyPtr tmpAtom=AtomAny::New(3791);
  tmp1->setEltAtRank("MyInt",tmpAtom);
  CPPUNIT_ASSERT_THROW(tmp1->setEltAtRank(1,tmpAtom),Exception);
  tmpAtom=AtomAny::New(3.14);
  CPPUNIT_ASSERT_THROW(tmp1->setEltAtRank("MyDoubl",tmpAtom),Exception);
  tmp1->setEltAtRank("MyDouble",tmpAtom);
  tmpAtom=AtomAny::New(false);
  tmp1->setEltAtRank("MyBool",tmpAtom);
  tmpAtom=AtomAny::New("abcdef");
  tmp1->setEltAtRank("MyStr",tmpAtom);
  CPPUNIT_ASSERT_EQUAL(3791,(*tmp1)["MyInt"]->getIntValue());
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.14,(*tmp1)["MyDouble"]->getDoubleValue(),1e-12);
  CPPUNIT_ASSERT_THROW((*tmp1)["MyIntT"],Exception);
  CPPUNIT_ASSERT(!(*tmp1)["MyBool"]->getBoolValue());
  CPPUNIT_ASSERT_THROW((*tmp1)["MyBool"]->getStringValue(),Exception);
  CPPUNIT_ASSERT((*tmp1)["MyStr"]->getStringValue()=="abcdef");
  tmpAtom=AtomAny::New("ghijklm");
  tmp1->setEltAtRank("MyStr",tmpAtom);
  CPPUNIT_ASSERT((*tmp1)["MyStr"]->getStringValue()=="ghijklm");
  //Introduce some recursive structure.
  SequenceAnyPtr tmp2=SequenceAny::New(tc,2);
  tmp2->setEltAtRank(0,tmp1);
  tmpAtom=AtomAny::New("opqrst");
  tmp1->setEltAtRank("MyStr",tmpAtom);
  tmp2->setEltAtRank(1,tmp1);
  CPPUNIT_ASSERT_EQUAL(3791,(*tmp2)[1]["MyInt"]->getIntValue());
  CPPUNIT_ASSERT((*tmp2)[0]["MyStr"]->getStringValue()=="ghijklm");
  CPPUNIT_ASSERT((*tmp2)[1]["MyStr"]->getStringValue()=="opqrst");
  CPPUNIT_ASSERT(*tmp2==*tmp2);
  AnyPtr tmp3=tmp2->clone();
  CPPUNIT_ASSERT(*tmp2==*tmp3);
  CPPUNIT_ASSERT((*tmp3)[0]["MyStr"]->getStringValue()=="ghijklm");
  CPPUNIT_ASSERT((*tmp3)[1]["MyStr"]->getStringValue()=="opqrst");
  tc->decrRef();
}

void EngineTest::checkInGateOutGate()
{
  string nodeName = "Node1";
  ElementaryNode* node1 = new TestElemNode(nodeName);
  _nodeMap[nodeName] = node1;

  nodeName = "Node2";
  ElementaryNode* node2 = new TestElemNode(nodeName);
  _nodeMap[nodeName] = node2;

  DEBTRACE(" --- check InGate OK after node creation" );
  {
    InGate *node1Ingate = node1->getInGate();
    CPPUNIT_ASSERT(node1Ingate);
    string name=node1Ingate->getNameOfTypeOfCurrentInstance();
    string expected="InGate";
    CPPUNIT_ASSERT_EQUAL(name,expected);
  }

  DEBTRACE(" --- check OutGate OK after node creation" );
  {
    OutGate *node1Outgate = node1->getOutGate();
    CPPUNIT_ASSERT(node1Outgate);
    string name=node1Outgate->getNameOfTypeOfCurrentInstance();
    string expected="OutGate";
    CPPUNIT_ASSERT_EQUAL(name,expected);
  }
}

void EngineTest::checkNodePortNumber()
{
  ElementaryNode* node1 = (ElementaryNode*) _nodeMap["Node1"];

  DEBTRACE(" --- check number of ports = 0 after node creation" );
  CPPUNIT_ASSERT(node1->getNumberOfInputPorts() == 0);
  CPPUNIT_ASSERT(node1->getNumberOfOutputPorts() == 0);

  InputPort  *in1 = node1->edAddInputPort("ib1",_tc_bool);
  InputPort  *in2 = node1->edAddInputPort("ii1",_tc_int);
  InputPort  *in3 = node1->edAddInputPort("ii2",_tc_int);
  InputPort  *in4 = node1->edAddInputPort("id1",_tc_double);

  OutputPort *ou1 = node1->edAddOutputPort("ob1",_tc_bool);
  OutputPort *ou2 = node1->edAddOutputPort("oi1",_tc_int);
  OutputPort *ou3 = node1->edAddOutputPort("od1",_tc_double);

  DEBTRACE(" --- check number of ports after ports creation" );
//   DEBTRACE("     node1->getNumberOfInputPorts(): "
//         << node1->getNumberOfInputPorts());
//   DEBTRACE("     node1->getNumberOfOutputPorts(): "
//         << node1->getNumberOfOutputPorts());
  CPPUNIT_ASSERT(node1->getNumberOfInputPorts() == 4);
  CPPUNIT_ASSERT(node1->getNumberOfOutputPorts() == 3);
}

void EngineTest::checkPortTypeName()
{
  ElementaryNode* node1 = (ElementaryNode*) _nodeMap["Node1"];

  DEBTRACE(" --- check InputPort name OK" );
  {
    string name=node1->getInputPort("ib1")->NAME;
    string expected="InputPort";
    CPPUNIT_ASSERT_EQUAL(name,expected);
  }

  DEBTRACE(" --- check OutputPort name OK" );
  {
    string name=node1->getOutputPort("ob1")->NAME;
    string expected="OutputPort";
    CPPUNIT_ASSERT_EQUAL(name,expected);
  }
}

void EngineTest::checkDuplicatePortName()
{
  ElementaryNode* node1 = (ElementaryNode*) _nodeMap["Node1"];
  DEBTRACE(" --- check duplicated name throws exception" );
  CPPUNIT_ASSERT_THROW(InputPort *in5=node1->edAddInputPort("ii2",_tc_int),
                       YACS::Exception); 
}

void EngineTest::checkRemovePort()
{
  ElementaryNode* node1 = (ElementaryNode*) _nodeMap["Node1"];
  ElementaryNode* node2 = (ElementaryNode*) _nodeMap["Node2"];

    DEBTRACE(" --- check remove port" );
  {
    node1->edRemovePort(node1->getInputPort("ib1"));
    CPPUNIT_ASSERT(node1->getNumberOfInputPorts() == 3);
    CPPUNIT_ASSERT(node1->getNumberOfOutputPorts() == 3);
  }

  DEBTRACE(" --- check remove wrong port throws exception" )
  {
    CPPUNIT_ASSERT_THROW(node1->edRemovePort(node1->getInputPort("ib1")),
                         YACS::Exception);
  }
}

void EngineTest::checkAddNodesToBloc()
{
  DEBTRACE(" --- delete node; // ====== NOT OK : done by bloc" );

  for (int i=0; i<10; i++)
    {
      ostringstream ss;
      ss << "Node_" << i;
      string s = ss.str();
      ElementaryNode* node = new TestElemNode(s);
      _nodeMap[s] = node;
      InputPort  *i1 = node->edAddInputPort("ib1",_tc_bool);
      InputPort  *i2 = node->edAddInputPort("ii1",_tc_int);
      InputPort  *i3 = node->edAddInputPort("ii2",_tc_int);
      InputPort  *i4 = node->edAddInputPort("id1",_tc_double);
      OutputPort *o1 = node->edAddOutputPort("ob1",_tc_bool);
      OutputPort *o2 = node->edAddOutputPort("oi1",_tc_int);
      OutputPort *o3 = node->edAddOutputPort("od1",_tc_double);
    }

  DEBTRACE(" --- create bloc, add two nodes, check constituants" );

  Bloc* bloc1 = new Bloc("bloc1");
  _nodeMap["bloc1"] = bloc1;
  _compoMap["bloc1"] = bloc1;
  bloc1->edAddChild(_nodeMap["Node_1"]);
  bloc1->edAddChild(_nodeMap["Node_2"]);
  {
    list<ElementaryNode *> setelem = bloc1->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 2);
  }
}

void EngineTest::checkAddingTwiceSameNodeInSameBloc()
{
  DEBTRACE(" --- add the same node two times does nothing: return false" );

  CPPUNIT_ASSERT(! ((Bloc*)_compoMap["bloc1"])->edAddChild(_nodeMap["Node_1"]));
}

void EngineTest::checkAddingTwiceSameNodeInTwoBlocs()
{
  DEBTRACE(" --- add a node already used elsewhere raises exception" );

  Bloc* bloc2 = new Bloc("bloc2");
  _nodeMap["bloc2"] = bloc2;
  _compoMap["bloc2"] = bloc2;
  bloc2->edAddChild(_nodeMap["Node_3"]);

  CPPUNIT_ASSERT_THROW(bloc2->edAddChild(_nodeMap["Node_1"]),
                       YACS::Exception);
}


void EngineTest::checkRecursiveBlocs_NumberOfNodes()
{
  Bloc *bloc1 = (Bloc*)_compoMap["bloc1"];
  Bloc *bloc2 = (Bloc*)_compoMap["bloc2"];

  DEBTRACE(" --- recursive blocs, check constituants" );

  Bloc* bloc3 = new Bloc("bloc3");
  _nodeMap["bloc3"] = bloc3;
  _compoMap["bloc3"] = bloc3;
  bloc3->edAddChild((bloc1));  // 2 elementary nodes 
  bloc3->edAddChild((bloc2));  // 1 elementary node
  bloc3->edAddChild(_nodeMap["Node_4"]);   // 1 elementary node
  {
    list<ElementaryNode *> setelem = bloc3->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 4);
    for (list<ElementaryNode*>::iterator it=setelem.begin(); it!=setelem.end(); it++)
      {
        DEBTRACE("     elem name = " << (*it)->getName());
      }
  }
}

void EngineTest::checkRecursiveBlocs_NumberOfPorts()
{
  Bloc *bloc3 = (Bloc*)_compoMap["bloc3"];

  DEBTRACE(" --- recursive blocs, check input & output ports // COMPLETER" );
  CPPUNIT_ASSERT(bloc3->getNumberOfInputPorts() == 4*4);
  DEBTRACE("     number of input ports: " << bloc3->getNumberOfInputPorts());
  DEBTRACE("     number of output ports: " << bloc3->getNumberOfOutputPorts());
  {
    list<InputPort *> inset = bloc3->getSetOfInputPort();
    list<OutputPort *> outset = bloc3->getSetOfOutputPort();
    for (list<InputPort *>::iterator it=inset.begin(); it!=inset.end(); it++)
      {
        DEBTRACE("     input port name = " << bloc3->getInPortName(*it));
      }
    for (list<OutputPort *>::iterator it=outset.begin(); it!=outset.end(); it++)
      {
        DEBTRACE("     output port name = " << bloc3->getOutPortName(*it));
      }
  }
}

void EngineTest::checkPortNameInBloc()
{

  DEBTRACE(" --- recursive blocs, check port names" );
  
  InputPort *inport = _nodeMap["Node_1"]->getInputPort("id1");
  CPPUNIT_ASSERT(_nodeMap["bloc3"]->getInPortName(inport) == "bloc1.Node_1.id1");
  CPPUNIT_ASSERT(((Bloc*)_nodeMap["bloc3"])->getChildName(_nodeMap["Node_1"]) == "bloc1.Node_1");
}

void EngineTest::checkGetNameOfPortNotInBloc()
{
  InputPort *inport = _nodeMap["Node_5"]->getInputPort("id1");
  CPPUNIT_ASSERT_THROW(string name = _nodeMap["bloc3"]->getInPortName(inport),
                       YACS::Exception);
}

void EngineTest::checkRemoveNode()
{
  DEBTRACE(" --- bloc and port inventory must be OK after bloc remove" );

  Bloc* bloc = new Bloc("blocR");
  _nodeMap["blocR"] = bloc;
  _compoMap["blocR"] = bloc;
  bloc->edAddChild(_nodeMap["Node_5"]);
  bloc->edAddChild(_nodeMap["Node_6"]);
  bloc->edAddChild(_nodeMap["Node_7"]);

  {
    list<ElementaryNode *> setelem = _nodeMap["blocR"]->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 3);

    for (list<ElementaryNode*>::iterator it=setelem.begin(); it!=setelem.end(); it++)
      {
        DEBTRACE("     elem name = " << (*it)->getName());
      }
  }

  ((Bloc *)_nodeMap["blocR"])->edRemoveChild(_nodeMap["Node_6"]);

  {
    list<ElementaryNode *> setelem = _nodeMap["blocR"]->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 2);
    for (list<ElementaryNode*>::iterator it=setelem.begin(); it!=setelem.end(); it++)
      {
        DEBTRACE("     elem name         = " << (*it)->getName());
        DEBTRACE("     elem name in Bloc = " << ((Bloc *)_nodeMap["blocR"])->getChildName(*it));
      }
  }

  {
    list<InputPort *> inset = _nodeMap["blocR"]->getSetOfInputPort();
    list<OutputPort *> outset = _nodeMap["blocR"]->getSetOfOutputPort();
    CPPUNIT_ASSERT(inset.size() == 8);
    CPPUNIT_ASSERT(outset.size() == 6);
    for (list<InputPort *>::iterator it=inset.begin(); it!=inset.end(); it++)
      {
        DEBTRACE("     input port name for blocR  = " << _nodeMap["blocR"]->getInPortName(*it));
      }
    for (list<OutputPort *>::iterator it=outset.begin(); it!=outset.end(); it++)
      {
        DEBTRACE("     output port name for blocR  = " << _nodeMap["blocR"]->getOutPortName(*it));
      }
  }

  ((Bloc *)_nodeMap["blocR"])->edRemoveChild(_nodeMap["Node_5"]);
  ((Bloc *)_nodeMap["blocR"])->edRemoveChild(_nodeMap["Node_7"]);
  {
    list<ElementaryNode *> setelem = _nodeMap["blocR"]->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 0);
  }
}

void EngineTest::RecursiveBlocs_multipleRecursion()
{
  {
    Bloc* bloc = new Bloc("bloc4");
    _nodeMap["bloc4"] = bloc;
    _compoMap["bloc4"] = bloc;
    bloc->edAddChild(_nodeMap["Node_5"]);
    bloc->edAddChild(_nodeMap["Node_6"]);
  }

  {
    Bloc* bloc = new Bloc("bloc5");
    _nodeMap["bloc5"] = bloc;
    _compoMap["bloc5"] = bloc;
    bloc->edAddChild(_nodeMap["Node_7"]);
    bloc->edAddChild(_nodeMap["Node_8"]);
  }

  {
    Bloc* bloc = new Bloc("bloc6");
    _nodeMap["bloc6"] = bloc;
    _compoMap["bloc6"] = bloc;
    bloc->edAddChild(_nodeMap["bloc4"]);
    bloc->edAddChild(_nodeMap["bloc5"]);
  }

  {   
    Bloc* bloc = new Bloc("bloc7");
    _nodeMap["bloc7"] = bloc;
    _compoMap["bloc7"] = bloc;
    bloc->edAddChild(_nodeMap["bloc3"]);
    bloc->edAddChild(_nodeMap["bloc6"]);
  }

  {   
    Bloc* bloc = new Bloc("graphe");
    _nodeMap["graphe"] = bloc;
    _compoMap["graphe"] = bloc;
    bloc->edAddChild(_nodeMap["bloc7"]);
    bloc->edAddChild(_nodeMap["Node_9"]);
  }

  {
    list<ElementaryNode *> setelem = _nodeMap["graphe"]->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 9);
    for (list<ElementaryNode*>::iterator it=setelem.begin(); it!=setelem.end(); it++)
      {
        DEBTRACE("     elem name = " << (*it)->getName());
      }
  }

  {
    list<InputPort *> inset = _nodeMap["bloc7"]->getSetOfInputPort();
    list<OutputPort *> outset = _nodeMap["bloc7"]->getSetOfOutputPort();
    for (list<InputPort *>::iterator it=inset.begin(); it!=inset.end(); it++)
      {
        DEBTRACE("     input port name for bloc7  = " << _nodeMap["bloc7"]->getInPortName(*it));
        DEBTRACE("     input port name for graphe = " << _nodeMap["graphe"]->getInPortName(*it));
      }
    for (list<OutputPort *>::iterator it=outset.begin(); it!=outset.end(); it++)
      {
        DEBTRACE("     output port name for bloc7  = " << _nodeMap["bloc7"]->getOutPortName(*it));
        DEBTRACE("     output port name for graphe = " << _nodeMap["graphe"]->getOutPortName(*it));
      }
    YACS::ENGINE::VisitorSaveState vst(_compoMap["graphe"]);
    vst.openFileDump("dumpState.xml");
    _compoMap["graphe"]->accept(&vst);
    vst.closeFileDump();
  }
}

void EngineTest::RecursiveBlocs_removeNodes()
{
//   {
//     set<Node *> setNodes = ((Bloc*)_nodeMap["graphe"])->getChildren();
//     for (set<Node*>::iterator it=setNodes.begin(); it!=setNodes.end(); it++)
//       {
//      DEBTRACE("     child name = " << (*it)->getName());
//       }
//   }

  {
    list<Node *> setNode = ((Bloc*)_nodeMap["graphe"])->getAllRecursiveConstituents();
    CPPUNIT_ASSERT(setNode.size() == 16);
    list<InputPort *> inset = _nodeMap["bloc7"]->getSetOfInputPort();
    list<OutputPort *> outset = _nodeMap["bloc7"]->getSetOfOutputPort();
    DEBTRACE("     input port number in graph:  " <<inset.size());
    DEBTRACE("     output port number in graph: " <<outset.size());
    CPPUNIT_ASSERT(inset.size() == 32);
    CPPUNIT_ASSERT(outset.size() == 24);
  }

  ((Bloc *)_nodeMap["bloc7"])->edRemoveChild(_nodeMap["bloc6"]);

  {
    list<Node *> setNode = ((Bloc*)_nodeMap["graphe"])->getAllRecursiveConstituents();
    CPPUNIT_ASSERT(setNode.size() == 9);
    for (list<Node*>::iterator it=setNode.begin(); it!=setNode.end(); it++)
      {
        DEBTRACE("     elem name = " << ((Bloc *)_nodeMap["graphe"])->getChildName(*it));
      }
    list<InputPort *> inset = _nodeMap["bloc7"]->getSetOfInputPort();
    list<OutputPort *> outset = _nodeMap["bloc7"]->getSetOfOutputPort();
    DEBTRACE("     input port number in graph:  " <<inset.size());
    DEBTRACE("     output port number in graph: " <<outset.size());
    CPPUNIT_ASSERT(inset.size() == 16);
    CPPUNIT_ASSERT(outset.size() == 12);
  }

}

void EngineTest::checkLogger()
{
  Proc* proc=new Proc("proc");
  Logger* logger=proc->getLogger("parser");
  logger->error("error1","file.cxx",324);
  logger->error("error2","file.cxx",852);
  CPPUNIT_ASSERT(logger->hasErrors()==true);
  CPPUNIT_ASSERT(logger->isEmpty()==false);
  const char* expected1="LogRecord: parser:ERROR:error1 (file.cxx:324)\n"
                        "LogRecord: parser:ERROR:error2 (file.cxx:852)\n";
  CPPUNIT_ASSERT(logger->getStr()==expected1);

  logger->reset();
  CPPUNIT_ASSERT(logger->hasErrors()==false);
  CPPUNIT_ASSERT(logger->isEmpty()==true);

  logger->error("error1","file.cxx",324);
  logger->error("error2","file.cxx",852);
  logger->error("error3","file.cxx",978);
  CPPUNIT_ASSERT(logger->hasErrors()==true);
  CPPUNIT_ASSERT(logger->isEmpty()==false);
  const char* expected2="LogRecord: parser:ERROR:error1 (file.cxx:324)\n"
                        "LogRecord: parser:ERROR:error2 (file.cxx:852)\n"
                        "LogRecord: parser:ERROR:error3 (file.cxx:978)\n";
  CPPUNIT_ASSERT(logger->getStr()==expected2);
  delete proc;
}

void EngineTest::checkGraphAnalyser0()
{
  {
    static const int N=2;
    Bloc *t[N];
    Bloc proc("proc");
    for(int i=0;i<N;i++)
      {
        std::ostringstream oss; oss << "n" << i+1;
        t[i]=new Bloc(oss.str());
        proc.edAddChild(t[i]);
      }
    proc.edAddCFLink(t[0],t[1]);
    std::vector< std::list<Node *> > r(proc.splitIntoIndependantGraph());
    CPPUNIT_ASSERT_EQUAL(1,(int)r.size());
    CPPUNIT_ASSERT_EQUAL(N,(int)r[0].size());
    SetOfPoints sop(r[0]);
    sop.simplify();
    CPPUNIT_ASSERT(sop.getRepr()=="(n1+n2) - ");
  }
}

void EngineTest::checkGraphAnalyser1()
{
  {
    static const int N=24;
    Bloc *t[N];
    Bloc proc("proc");
    for(int i=0;i<N;i++)
      {
        std::ostringstream oss; oss << "n" << i+1;
        t[i]=new Bloc(oss.str());
        proc.edAddChild(t[i]);
      }
    proc.edAddCFLink(t[20],t[21]); proc.edAddCFLink(t[21],t[15]); proc.edAddCFLink(t[15],t[14]); proc.edAddCFLink(t[14],t[0]); proc.edAddCFLink(t[0],t[1]); proc.edAddCFLink(t[1],t[23]); proc.edAddCFLink(t[23],t[5]);
    proc.edAddCFLink(t[14],t[13]); proc.edAddCFLink(t[13],t[5]);
    proc.edAddCFLink(t[15],t[12]); proc.edAddCFLink(t[12],t[11]); proc.edAddCFLink(t[11],t[9]); proc.edAddCFLink(t[9],t[8]); proc.edAddCFLink(t[8],t[7]); proc.edAddCFLink(t[7],t[6]); proc.edAddCFLink(t[6],t[5]);
    proc.edAddCFLink(t[11],t[10]); proc.edAddCFLink(t[10],t[8]);
    proc.edAddCFLink(t[15],t[16]); proc.edAddCFLink(t[16],t[17]); proc.edAddCFLink(t[17],t[19]); proc.edAddCFLink(t[19],t[2]); proc.edAddCFLink(t[2],t[4]); proc.edAddCFLink(t[4],t[5]);
    proc.edAddCFLink(t[19],t[18]); proc.edAddCFLink(t[18],t[4]);
    proc.edAddCFLink(t[19],t[3]); proc.edAddCFLink(t[3],t[4]);
    proc.edAddCFLink(t[22],t[21]);
    std::vector< std::list<Node *> > r(proc.splitIntoIndependantGraph());
    CPPUNIT_ASSERT_EQUAL(1,(int)r.size());
    CPPUNIT_ASSERT_EQUAL(N,(int)r[0].size());
    SetOfPoints sop(r[0]);
    sop.simplify();
    CPPUNIT_ASSERT(sop.getRepr()=="([n21*n23]+(n22+n16+[((n13+n12)+[n10*n11]+(n9+n8+n7))*((n17+n18+n20)+[n19*n3*n4]+n5)*(n15+[(n1+n2+n24)*n14])]+n6)) - ");
  }
  //
  {
    static const int N=10;
    Bloc *t[N];
    Bloc proc("proc");
    for(int i=0;i<N;i++)
      {
        std::ostringstream oss; oss << "n" << i+1;
        t[i]=new Bloc(oss.str());
        proc.edAddChild(t[i]);
      }
    proc.edAddCFLink(t[9],t[0]); proc.edAddCFLink(t[0],t[2]); proc.edAddCFLink(t[2],t[4]); proc.edAddCFLink(t[4],t[5]);
    proc.edAddCFLink(t[0],t[1]); proc.edAddCFLink(t[1],t[6]); proc.edAddCFLink(t[6],t[7]); proc.edAddCFLink(t[7],t[5]);
    proc.edAddCFLink(t[0],t[8]); proc.edAddCFLink(t[8],t[3]); proc.edAddCFLink(t[3],t[4]);
    std::vector< std::list<Node *> > r(proc.splitIntoIndependantGraph());
    CPPUNIT_ASSERT_EQUAL(1,(int)r.size());
    CPPUNIT_ASSERT_EQUAL(N,(int)r[0].size());
    SetOfPoints sop(r[0]);
    sop.simplify();
    CPPUNIT_ASSERT(sop.getRepr()=="((n10+n1)+[([(n9+n4)*n3]+n5)*(n2+n7+n8)]+n6) - ");
  }
}

void EngineTest::checkGraphAnalyser2()
{
  {
    static const int N=8;
    Bloc *t[N];
    Bloc proc("proc");
    for(int i=0;i<N;i++)
      {
        std::ostringstream oss; oss << "n" << i+1;
        t[i]=new Bloc(oss.str());
        proc.edAddChild(t[i]);
      }
    proc.edAddCFLink(t[7],t[4]); proc.edAddCFLink(t[4],t[0]); proc.edAddCFLink(t[0],t[1]); proc.edAddCFLink(t[1],t[5]); proc.edAddCFLink(t[5],t[6]);
    proc.edAddCFLink(t[4],t[2]); proc.edAddCFLink(t[2],t[3]); proc.edAddCFLink(t[3],t[5]);
    std::vector< std::list<Node *> > r(proc.splitIntoIndependantGraph());
    CPPUNIT_ASSERT_EQUAL(1,(int)r.size());
    CPPUNIT_ASSERT_EQUAL(N,(int)r[0].size());
    SetOfPoints sop(r[0]);
    sop.simplify();
    CPPUNIT_ASSERT(sop.getRepr()=="((n8+n5)+[(n1+n2)*(n3+n4)]+(n6+n7)) - ");
  }
  //
  {
    static const int NN=6;
    Bloc *tt[NN];
    Bloc proc2("proc2");
    tt[0]=new Bloc("n21") ; tt[1]=new Bloc("n22") ; tt[2]=new Bloc("n23") ; tt[3]=new Bloc("n16"); tt[4]=new Bloc("n21@1"); tt[5]=new Bloc("n23@1");
    for(int i=0;i<NN;i++)
      proc2.edAddChild(tt[i]);
    proc2.edAddCFLink(tt[0],tt[4]); proc2.edAddCFLink(tt[4],tt[1]); proc2.edAddCFLink(tt[2],tt[5]); proc2.edAddCFLink(tt[5],tt[1]); proc2.edAddCFLink(tt[1],tt[3]);
    std::vector< std::list<Node *> > rr(proc2.splitIntoIndependantGraph());
    CPPUNIT_ASSERT_EQUAL(1,(int)rr.size());
    CPPUNIT_ASSERT_EQUAL(NN,(int)rr[0].size());
    SetOfPoints sop2(rr[0]);
    sop2.simplify();
    CPPUNIT_ASSERT(sop2.getRepr()=="([(n21+n21@1)*(n23+n23@1)]+(n22+n16)) - ");
  }
  //
  {
    static const int NNN=6;
    Bloc *ttt[NNN];
    Bloc proc3("proc3");
    ttt[0]=new Bloc("n21") ; ttt[1]=new Bloc("n22") ; ttt[2]=new Bloc("n23") ; ttt[3]=new Bloc("n16"); ttt[4]=new Bloc("n21@1"); ttt[5]=new Bloc("n23@1");
    for(int i=0;i<NNN;i++)
      proc3.edAddChild(ttt[i]);
    proc3.edAddCFLink(ttt[0],ttt[4]); proc3.edAddCFLink(ttt[3],ttt[0]); proc3.edAddCFLink(ttt[2],ttt[5]); proc3.edAddCFLink(ttt[3],ttt[2]); proc3.edAddCFLink(ttt[1],ttt[3]);
    std::vector< std::list<Node *> > rrr(proc3.splitIntoIndependantGraph());
    CPPUNIT_ASSERT_EQUAL(1,(int)rrr.size());
    CPPUNIT_ASSERT_EQUAL(NNN,(int)rrr[0].size());
    SetOfPoints sop3(rrr[0]);
    sop3.simplify();
    CPPUNIT_ASSERT(sop3.getRepr()=="((n22+n16)+[(n21+n21@1)*(n23+n23@1)]) - ");
  }
}

void EngineTest::checkGraphAnalyser3()
{
  {
    static const int NNN=6;
    Bloc *ttt[NNN];
    Bloc proc3("proc3");
    for(int i=0;i<NNN;i++)
      {
        std::ostringstream oss; oss << "n" << i+1;
        ttt[i]=new Bloc(oss.str());
        proc3.edAddChild(ttt[i]);
      }
    proc3.edAddCFLink(ttt[0],ttt[1]); proc3.edAddCFLink(ttt[1],ttt[4]);
    proc3.edAddCFLink(ttt[0],ttt[2]); proc3.edAddCFLink(ttt[2],ttt[3]); proc3.edAddCFLink(ttt[3],ttt[4]);
    proc3.edAddCFLink(ttt[5],ttt[3]);
    std::vector< std::list<Node *> > rrr(proc3.splitIntoIndependantGraph());
    CPPUNIT_ASSERT_EQUAL(1,(int)rrr.size());
    CPPUNIT_ASSERT_EQUAL(NNN,(int)rrr[0].size());
    SetOfPoints sop3(rrr[0]);
    //sop3.simplify();
    //std:cerr  << std::endl << sop3.getRepr() << std::endl;
    //CPPUNIT_ASSERT(sop3.getRepr()=="((n22+n16)+[(n21+n21@1)*(n23+n23@1)]) - ");
  }
}

void EngineTest::checkGraphAnalyser4()
{
  {
    static const int NNN=3;
    Bloc *ttt[NNN];
    Bloc proc3("proc");
    for(int i=0;i<NNN;i++)
      {
        std::ostringstream oss; oss << "n" << i+1;
        ttt[i]=new Bloc(oss.str());
        proc3.edAddChild(ttt[i]);
      }
    proc3.edAddCFLink(ttt[2],ttt[1]); proc3.edAddCFLink(ttt[1],ttt[0]);
    std::vector< std::list<Node *> > rrr(proc3.splitIntoIndependantGraph());
    CPPUNIT_ASSERT_EQUAL(1,(int)rrr.size());
    CPPUNIT_ASSERT_EQUAL(NNN,(int)rrr[0].size());
    SetOfPoints sop(rrr[0]);
    sop.simplify();
    CPPUNIT_ASSERT(sop.getRepr()=="(n3+n2+n1) - ");
  }
}
