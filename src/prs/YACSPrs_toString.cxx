//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include <YACSPrs_toString.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sstream>
#include <TypeConversions.hxx>
#include <Any.hxx>
#include <TypeCode.hxx>

using namespace YACS::ENGINE;

char * toString(CORBA::Any* theAny, QString& theRetStr)
{
  if ( !theAny ) theRetStr += QString("< ? >");
  else
  {
    CORBA::Any anAny = *theAny;
    if ( !anAny.value() ) theRetStr += QString("< ? >");
    else
    {
      std::ostringstream astr;
      const char * retstr;
      int startstr = 0;
      switch ( anAny.type()->kind() )
      {
	case CORBA::tk_string: {
	  anAny >>= retstr;
	  theRetStr += QString(retstr);
	  break;
	}
	case CORBA::tk_long: {
	  CORBA::Long l;
	  anAny >>= l;
	  astr << l << std::ends;
	  theRetStr += QString(astr.str());
	  break;
	}
	case CORBA::tk_double: {
	  double d;
	  anAny >>= d;
	  astr << d << std::ends;
	  //astr << setw(25) << setprecision(18) << d << std::ends;
	  QString aRetStr = QString(astr.str());
	  int i = 0;
	  while ( i < (int ) theRetStr.length() && theRetStr.at(i++) == ' ' ) {
	    startstr = i;
	  }
	  theRetStr += aRetStr.mid(startstr,aRetStr.length());
	  break;
	}
        case CORBA::tk_sequence: {
	  theRetStr += QString("[");

	  CORBA::Long aSeqLength = 0;
	  *(anAny.type()->parameter(1)) >>= aSeqLength;

	  if ( aSeqLength == 0 )
	  {
	    theRetStr += QString("   ]");
	    break;
	  }
	  
	  // TO DO : implement recursion for the sequence type
	  /*CORBA::TypeCode* aType;
	  *(anAny.type()->parameter(0)) >>= aType;
	  switch ( aType->kind() )
	  {
	    case CORBA::tk_string: {
	      printf("StringElem\n");
	      CORBA::StringSeq* aStringSeq;
	      anAny >>= aStringSeq;
	      for (int i=0; i < aSeqLength; i++)
	      {
		CORBA::Any anArg;
		anArg <<= aStringSeq[i];
		toString( &anArg, theRetStr );
		if ( i < aSeqLength-1 ) theRetStr += QString(",");
	      }
	      break;
	    }
	    case CORBA::tk_double: {
	      printf("DoubleElem\n");
	      CORBA::DoubleSeq* aDoubleSeq;
	      anAny >>= aDoubleSeq;
	      for (int i=0; i < aSeqLength; i++)
	      {
		CORBA::Any anArg;
		anArg <<= aDoubleSeq[i];
		toString( &anArg, theRetStr );
		if ( i < aSeqLength-1 ) theRetStr += QString(",");
	      }
	      break;
	    }
  	    case CORBA::tk_sequence: {
	      printf("SequenceElem\n");
	      CORBA::Any* aSequenceSeq;
	      anAny >>= aSequenceSeq;
	      for (int i=0; i < aSeqLength; i++)
	      {
		CORBA::Any anArg;
		anArg <<= aSequenceSeq[i];
		toString( &anArg, theRetStr );
		if ( i < aSeqLength-1 ) theRetStr += QString(",");
	      }
	      break;
	    }
	    default: {
	      printf("DefaultElem\n");
	      theRetStr += QString("< ? >");
	      break;
	    }
	  }*/
	  theRetStr += QString("]");
	  break;
	}
	case CORBA::tk_objref: {
	  /*CORBA::Object_ptr obj;
	  try {
	    anAny >>= (CORBA::Any::to_object ) obj;
	    theRetStr += QString( _Orb->object_to_string( obj ) );
	  }
	  catch ( ... ) {
	    theRetStr += QString("object_to_string catched ");
	  }*/
	  theRetStr += QString("Objref");
	  break;
	}
	default: {
	  theRetStr += QString("< ? >");
	  break;
	}
      }
    }
  }
}

void toString(PyObject* theObject, QString& theRetStr)
{
  if ( !theObject ) theRetStr += QString("< ? >");

  std::ostringstream aStr;
  if ( PyString_CheckExact(theObject) )
    theRetStr += QString( PyString_AsString(theObject) );
  else if ( PyLong_CheckExact(theObject) )
  {
    long aVal = PyLong_AsLong(theObject);
    aStr << aVal << std::ends;
    theRetStr += QString( aStr.str() );
  }
  else if ( PyInt_CheckExact(theObject) )
  {
    long aVal = PyInt_AsLong(theObject);
    aStr << aVal << std::ends;
    theRetStr += QString( aStr.str() );
  }
  else if ( PyBool_Check(theObject) )
    theRetStr += QString( (theObject == Py_True) ? "true" : "false" );
  else if ( PyFloat_CheckExact(theObject) )
  {
    double aVal = PyFloat_AsDouble(theObject);
    aStr << aVal << std::ends;
    theRetStr += QString( aStr.str() );
  }
  else if ( PyList_CheckExact(theObject) )
  {
    theRetStr += QString("[");
    for (int i=0; i < PyList_Size(theObject); i++)
    {
      toString( PyList_GetItem(theObject, i), theRetStr );
      if ( i < PyList_Size(theObject)-1 ) theRetStr += QString(",");
    }
    theRetStr += QString("]");
  }
  //else if ( ... ) // objref case
  else
    theRetStr += QString("< ? >");
}

void toString(YACS::ENGINE::Any* theAny, QString& theValue)
{
  if ( !theAny ) theValue += QString("< ? >");
  else if ( theAny->getType() )
  {
    DynType aKind = theAny->getType()->kind();
    switch (aKind)
      {
      case Double:
	theValue += QString::number(theAny->getDoubleValue());
	break;
      case Int:
	theValue += QString::number(theAny->getIntValue());
	break;
      case String:
	theValue += QString(theAny->getStringValue());
	break;
      case Bool:
	theValue += QString(theAny->getBoolValue()?"true":"false");
	break;
      case Objref:
	theValue += QString("Objref"); /// ?
	break;
      case Sequence: {
	SequenceAny* aSeqAny = dynamic_cast<SequenceAny*>( theAny );
	if ( aSeqAny )
	{
	  theValue += QString("[");
	  for (int i=0; i < aSeqAny->size(); i++)
	  {
	    toString( (*aSeqAny)[i], theValue );
	    if ( i < aSeqAny->size()-1 ) theValue += QString(",");
	  }
	  theValue += QString("]");
	}
	break;
      }
      case NONE:
      default:
	theValue += QString("");
	break;
      }
  }
}

void toString(const std::string& value, const TypeCode * t,QString& theRetStr)
{
  if(value == "")
    {
      theRetStr="";
      return;
    }
  xmlDocPtr doc;
  xmlNodePtr cur;
  YACS::ENGINE::Any *ob=NULL;
  doc = xmlParseMemory(value.c_str(), value.size());
  if (doc == NULL )
    {
      theRetStr="< ? >";
      return;
    }
  cur = xmlDocGetRootElement(doc);
  if (cur == NULL)
    {
      xmlFreeDoc(doc);
      theRetStr="< ? >";
      return;
    }
  while (cur != NULL)
    {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"value")))
        {
          try
            {
              ob=convertXmlNeutral(t,doc,cur);
            }
          catch(...)
            {
            }
          break;
        }
      cur = cur->next;
    }
  xmlFreeDoc(doc);
  if(ob==NULL)
    {
      theRetStr="< ? >";
    }
  else
    {
      toString(ob,theRetStr);
      ob->decrRef();
    }
}

