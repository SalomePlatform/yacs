// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef __VISITORSAVESALOMESCHEMA_HXX__
#define __VISITORSAVESALOMESCHEMA_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "VisitorSaveSchema.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class YACSRUNTIMESALOME_EXPORT VisitorSaveSalomeSchema: public VisitorSaveSchema
    {
    public:
      VisitorSaveSalomeSchema(ComposedNode *root);
      virtual ~VisitorSaveSalomeSchema();
      virtual void visitPresetNode(DataNode *node);
      virtual void visitOutNode(DataNode *node);
      virtual void visitStudyInNode(DataNode *node);
      virtual void visitStudyOutNode(DataNode *node);
    protected:
      virtual void writeDataNodeParameters(DataNode *node);
      virtual void writeStudyInNodeParameters(DataNode *node);
      virtual void writeOutNodeParameters(DataNode *node);
      virtual void writeStudyOutNodeParameters(DataNode *node);
    };

    YACSRUNTIMESALOME_EXPORT void VisitorSaveSalomeSchemaUnsafe(ComposedNode* proc, const std::string& xmlSchema);
  }
}

#endif
