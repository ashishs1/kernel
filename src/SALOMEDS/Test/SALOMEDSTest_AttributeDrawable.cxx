// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

/*!
 * Check all methods of SALOMEDS_AttributeDrawable
 * Use code of SALOMEDS_AttributeDrawable.cxx
 */

void SALOMEDSTest::testAttributeDrawable()
{
  //Create Study
  _PTR(Study) study(new SALOMEDS_Study(_study));

  CPPUNIT_ASSERT(study);

  //Create Study Builder
  _PTR(StudyBuilder) studyBuilder = study->NewBuilder();

  CPPUNIT_ASSERT(studyBuilder);

  //Create a SObject with entry 0:1:1
  _PTR(SObject) so = study->CreateObjectID("0:1:1");

  CPPUNIT_ASSERT(so);

  //Create an attribute AttributeDrawable
  _PTR(AttributeDrawable) _attr = studyBuilder->FindOrCreateAttribute(so, "AttributeDrawable");

  //Check the attribute creation
  CPPUNIT_ASSERT(_attr);

  //Check method IsDrawable
  bool isDrawable = _attr->IsDrawable();

  CPPUNIT_ASSERT(!isDrawable);

  //Check method SetDrawable
  _attr->SetDrawable(true);

  CPPUNIT_ASSERT(_attr->IsDrawable());

  _attr->SetDrawable(false);

  CPPUNIT_ASSERT(!_attr->IsDrawable());

  study->Clear();
}



