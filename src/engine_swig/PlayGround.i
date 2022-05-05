// Copyright (C) 2006-2022  CEA/DEN, EDF R&D
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
%include <std_vector.i>
%template()              std::pair< YACS::ENGINE::PartDefinition *, YACS::ENGINE::ComplexWeight *>;
%template(vecppdd)       std::vector< std::pair< YACS::ENGINE::PartDefinition *, YACS::ENGINE::ComplexWeight *> >;

%newobject YACS::ENGINE::PlayGround::copy;

%{
PyObject *convertPartDefinition(YACS::ENGINE::PartDefinition *pd)
{
  if(!pd)
    return SWIG_NewPointerObj(SWIG_as_voidptr(pd),SWIGTYPE_p_YACS__ENGINE__PartDefinition, SWIG_POINTER_OWN | 0 );
  YACS::ENGINE::ContigPartDefinition *pd1(dynamic_cast<YACS::ENGINE::ContigPartDefinition *>(pd));
  if(pd1)
    return SWIG_NewPointerObj(SWIG_as_voidptr(pd1),SWIGTYPE_p_YACS__ENGINE__ContigPartDefinition, SWIG_POINTER_OWN | 0 );
  YACS::ENGINE::NonContigPartDefinition *pd2(dynamic_cast<YACS::ENGINE::NonContigPartDefinition *>(pd));
  if(pd2)
    return SWIG_NewPointerObj(SWIG_as_voidptr(pd2),SWIGTYPE_p_YACS__ENGINE__NonContigPartDefinition, SWIG_POINTER_OWN | 0 );
  YACS::ENGINE::AllPartDefinition *pd3(dynamic_cast<YACS::ENGINE::AllPartDefinition *>(pd));
  if(pd3)
    return SWIG_NewPointerObj(SWIG_as_voidptr(pd3),SWIGTYPE_p_YACS__ENGINE__AllPartDefinition, SWIG_POINTER_OWN | 0 );
  return SWIG_NewPointerObj(SWIG_as_voidptr(pd),SWIGTYPE_p_YACS__ENGINE__PartDefinition, SWIG_POINTER_OWN | 0 );
  
}
%}

%typemap(out) std::vector< YACS::BASES::AutoRefCnt<YACS::ENGINE::PartDefinition> >
{
  std::size_t sz($1.size());
  std::vector< YACS::BASES::AutoRefCnt<YACS::ENGINE::PartDefinition> >::iterator it($1.begin());
  $result = PyList_New($1.size());
  for (std::size_t i=0; i<sz; i++,it++)
    PyList_SetItem($result,i,convertPartDefinition((*it).retn())); 
}

namespace YACS
{
  namespace ENGINE
  {
    class PartDefinition;
    
    class PlayGround : public RefCounter
    {
    public:
      PlayGround();
      void loadFromKernelCatalog();
      PlayGround(const std::vector< std::pair<std::string,int> >& defOfRes);
      std::string printSelf() const;
      std::vector< std::pair<std::string,int> > getData() const;
      void setData(const std::vector< std::pair<std::string,int> >& defOfRes);
      int getNumberOfCoresAvailable() const;
      int getMaxNumberOfContainersCanBeHostedWithoutOverlap(int nbCoresPerCont) const;
      std::string deduceMachineFrom(int workerId, int nbProcPerNode) const;
      %extend
         {
           std::string __str__() const
           {
             return self->printSelf();
           }

           std::vector< YACS::BASES::AutoRefCnt<YACS::ENGINE::PartDefinition> > partition(const std::vector< std::pair< YACS::ENGINE::PartDefinition *, YACS::ENGINE::ComplexWeight *> >& parts, const std::vector<int> &nbCoresPerShot) const
           {
             std::size_t sz(parts.size());
             std::vector< std::pair< const YACS::ENGINE::PartDefinition *, const YACS::ENGINE::ComplexWeight *> > partsCpp(sz);
             for(std::size_t i=0;i<sz;i++)
               partsCpp[i]=std::pair<const YACS::ENGINE::PartDefinition *, const YACS::ENGINE::ComplexWeight *>(parts[i].first, parts[i].second);
             return self->partition(partsCpp, nbCoresPerShot);
           }
         }
    private:
      ~PlayGround();
    };

    class PartDefinition : public RefCounter
    {
    public:
      virtual PartDefinition *copy() const;
      virtual std::string printSelf() const;
      virtual int getNumberOfCoresConsumed() const;
    protected:
      PartDefinition();
      ~PartDefinition();
    };

    class ContigPartDefinition : public PartDefinition
    {
    public:
      ContigPartDefinition(const PlayGround *pg, int zeStart, int zeStop);
      int getStart() const;
      int getStop() const;
      %extend
         {
           std::string __str__() const
           {
             return self->printSelf();
           }
         }
    private:
      ~ContigPartDefinition();
    };

    class NonContigPartDefinition : public PartDefinition
    {
    public:
      NonContigPartDefinition(const PlayGround *pg, const std::vector<int>& ids);
      std::vector<int> getIDs() const;
      %extend
         {
           std::string __str__() const
           {
             return self->printSelf();
           }
         }
    private:
      ~NonContigPartDefinition();
    };

    class AllPartDefinition : public PartDefinition
    {
    public:
      AllPartDefinition(const PlayGround *pg);
      %extend
         {
           std::string __str__() const
           {
             return self->printSelf();
           }
         }
    private:
      ~AllPartDefinition();
    };

    class PartDefinition;
    
    class ForTestOmlyHPContCls
    {
    public:
      std::string getContainerType() const;
      std::vector<int> getIDS() const;
    %extend
       {
         PyObject *getPD() const
         {
           const PartDefinition *ret(self->getPD());
           if(ret)
             ret->incrRef();
           return convertPartDefinition(const_cast<PartDefinition *>(ret));
         }
       }
    };
  }
}
