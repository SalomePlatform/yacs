
#include "RuntimeSALOME.hxx"
#include "guiContext.hxx"
#include "Proc.hxx"
#include "Container.hxx"
#include "ComponentInstance.hxx"
#include "commandsProc.hxx"
#include "guiObservers.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

GuiContext * GuiContext::_current = 0;

GuiContext::GuiContext()
  : Subject(0)
{
  _builtinCatalog = getSALOMERuntime()->getBuiltinCatalog();
  _sessionCatalog = 0;
  _procCatalog = 0;
  _currentCatalog = 0;

  _invoc = new ProcInvoc();
  _proc = 0;
  _subjectProc = 0;
  _numItem = 0;
  _selectedCanvasItem = 0;

  _canvas = new QCanvas(800,2000);
  _widgetStack = 0;
  _mapOfSubjectNode.clear();
  _mapOfSubjectDataPort.clear();
  _mapOfSubjectLink.clear();
  _mapOfSubjectControlLink.clear();
  _mapOfSubjectComponent.clear();
  _mapOfSubjectContainer.clear();
  _mapOfSubjectDataType.clear();
  _lastErrorMessage ="";
  _xmlSchema ="";
  _YACSEngineContainer = pair<string, string>("YACSServer","localhost");
}

GuiContext::~GuiContext()
{
  if (_invoc) delete _invoc;
  _invoc = 0;
  if (_subjectProc)
    {
      _subjectProc->clean();
      delete _subjectProc;
      _subjectProc = 0;
    }
}

void GuiContext::setProc(YACS::ENGINE::Proc* proc)
{
  DEBTRACE("GuiContext::setProc ");
  if (_subjectProc)
    {
      Subject::erase(_subjectProc);
      _subjectProc = 0;
    }
  _proc = proc;
  _mapOfSubjectNode.clear();
  _mapOfSubjectDataPort.clear();
  _mapOfSubjectLink.clear();
  _mapOfSubjectControlLink.clear();
  _mapOfSubjectComponent.clear();
  _mapOfSubjectContainer.clear();
  _mapOfSubjectDataType.clear();

  _subjectProc = new SubjectProc(proc, this);
  _mapOfSubjectNode[static_cast<Node*>(proc)] = _subjectProc;
  update(YACS::HMI::NEWROOT, 0, _subjectProc);
  _subjectProc->loadProc();
}

long GuiContext::getNewId(YACS::HMI::TypeOfElem type)
{
  long anId = 0;

  if ( Proc* aProc = getProc() )
  { 
    set<long> anIds;
       
    if ( type == CONTAINER )
    {
      QString aTemplate("container");

      map<string, Container*>::const_iterator itCont = aProc->containerMap.begin();
      for ( ; itCont != aProc->containerMap.end(); ++itCont)
      {
	QString aName( (*itCont).second->getName() );
	if ( aName.length() > aTemplate.length() && !aName.left(aTemplate.length()).compare(aTemplate) )
	{
	  aName = aName.right(aName.length()-aTemplate.length());
	  bool ok;
	  long aNum = aName.toLong(&ok);
	  if (ok) anIds.insert(aNum);
	}
      }
    }
    else if ( type == COMPONENT )
    {
      QString aTemplate("component");

      map<std::pair<std::string,int>, ComponentInstance*>::const_iterator itComp = 
	aProc->componentInstanceMap.begin();
      for ( ; itComp != aProc->componentInstanceMap.end(); ++itComp)
      {
	QString aName( (*itComp).second->getCompoName() );
	printf(">> component name : %s\n",aName.latin1());
	if ( aName.length() > aTemplate.length() && !aName.left(aTemplate.length()).compare(aTemplate) )
	{
	  aName = aName.right(aName.length()-aTemplate.length());
	  bool ok;
	  long aNum = aName.toLong(&ok);
	  if (ok) anIds.insert(aNum);
	}
      }
    }
    else
    {
      list<Node*> aNodes = aProc->getAllRecursiveConstituents();
      list<Node*>::iterator it = aNodes.begin();
      for ( ; it != aNodes.end(); it++ )
      {
	QString aName( (*it)->getName() );

	if ( ProcInvoc::getTypeOfNode(*it) != type ) continue;
	
	QString aTemplate;
	switch(type)
	{
	case BLOC:             aTemplate= QString("Bloc");              break;
	case FOREACHLOOP:      aTemplate= QString("ForEachLoopDouble"); break;
	case FORLOOP: 	       aTemplate= QString("ForLoop");           break;
	case WHILELOOP:        aTemplate= QString("WhileLoop");         break;
	case SWITCH:	       aTemplate= QString("Switch");            break;
	case PYTHONNODE:       aTemplate= QString("PyScript");          break;
	case PYFUNCNODE:       aTemplate= QString("PyFunction");        break;
	case CORBANODE:	       aTemplate= QString("CORBANode");         break;
	case SALOMENODE:       aTemplate= QString("SalomeNode");        break;
	case CPPNODE:	       aTemplate= QString("CppNode");           break;
	case SALOMEPYTHONNODE: aTemplate= QString("SalomePythonNode");  break;
	case XMLNODE:	       aTemplate= QString("XmlNode");           break;
	default:	                                                break;
	}

	if ( aName.length() > aTemplate.length() && !aName.left(aTemplate.length()).compare(aTemplate) )
	{
	  aName = aName.right(aName.length()-aTemplate.length());
	  bool ok;
	  long aNum = aName.toLong(&ok);
	  if (ok) anIds.insert(aNum);
	}
      }
    }

    if ( !anIds.empty() )
      anId = (*anIds.rbegin()) + 1;
  }

  return anId;
}
