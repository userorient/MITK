/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "mitkPlaneGeometryDataMapper2D.h"

//mitk includes
#include "mitkDataNode.h"
#include "mitkProperties.h"
#include "mitkVtkPropRenderer.h"
#include "mitkPointSet.h"
#include "mitkPlaneGeometry.h"
#include "mitkPlaneOrientationProperty.h"
#include "mitkLine.h"

//vtk includes
#include <vtkActor.h>

#include <mitkAbstractTransformGeometry.h>
#include <mitkResliceMethodProperty.h>
#include <mitkSlicedGeometry3D.h>
#include <stdlib.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkUnsignedCharArray.h>

mitk::PlaneGeometryDataMapper2D::AllInstancesContainer mitk::PlaneGeometryDataMapper2D::s_AllInstances;

// input for this mapper ( = point set)
const mitk::PlaneGeometryData* mitk::PlaneGeometryDataMapper2D::GetInput() const
{
  return static_cast< PlaneGeometryData * >(GetDataNode()->GetData());
}

// constructor PlaneGeometryDataMapper2D
mitk::PlaneGeometryDataMapper2D::PlaneGeometryDataMapper2D()
: m_RenderOrientationArrows( false ),
  m_ArrowOrientationPositive( true ),
  m_DepthValue(1.0f)
{
  s_AllInstances.insert(this);
}

// destructor
mitk::PlaneGeometryDataMapper2D::~PlaneGeometryDataMapper2D()
{
  s_AllInstances.erase(this);
}

// returns propassembly
vtkProp* mitk::PlaneGeometryDataMapper2D::GetVtkProp(mitk::BaseRenderer * renderer)
{
  LocalStorage *ls = m_LSH.GetLocalStorage(renderer);
//  return ls->m_CrosshairAssembly;
  return ls->m_CrosshairActor;
}

void mitk::PlaneGeometryDataMapper2D::GenerateDataForRenderer( mitk::BaseRenderer *renderer )
{
  BaseLocalStorage *ls = m_LSH.GetLocalStorage(renderer);

  // The PlaneGeometryDataMapper2D mapper is special in that the rendering of
  // OTHER PlaneGeometryDatas affects how we render THIS PlaneGeometryData
  // (for the gap at the point where they intersect).  A change in any of the
  // other PlaneGeometryData nodes could mean that we render ourself
  // differently, so we check for that here.
  bool generateDataRequired = false;
  for (AllInstancesContainer::iterator it = s_AllInstances.begin();
       it != s_AllInstances.end();
       ++it)
  {
    generateDataRequired = ls->IsGenerateDataRequired(renderer, this, (*it)->GetDataNode());
    if (generateDataRequired) break;
  }

//  if (!generateDataRequired) return;

  ls->UpdateGenerateDataTime();

  // Collect all other PlaneGeometryDatas that are being mapped by this mapper
  m_OtherPlaneGeometries.clear();

  for (AllInstancesContainer::iterator it = s_AllInstances.begin(); it != s_AllInstances.end(); ++it)
  {
    Self *otherInstance = *it;

    // Skip ourself
    if (otherInstance == this) continue;

    mitk::DataNode *otherNode = otherInstance->GetDataNode();
    if (!otherNode) continue;

    // Skip other PlaneGeometryData nodes that are not visible on this renderer
    if (!otherNode->IsVisible(renderer)) continue;

    PlaneGeometryData* otherData = dynamic_cast<PlaneGeometryData*>(otherNode->GetData());
    if (!otherData) continue;

    PlaneGeometry* otherGeometry = dynamic_cast<PlaneGeometry*>(otherData->GetPlaneGeometry());
    if ( otherGeometry && !dynamic_cast<AbstractTransformGeometry*>(otherData->GetPlaneGeometry()) )
    {
      m_OtherPlaneGeometries.push_back(otherNode);
    }
  }

  CreateVtkCrosshair(renderer);

  ApplyAllProperties(renderer);
}

void mitk::PlaneGeometryDataMapper2D::CreateVtkCrosshair(mitk::BaseRenderer *renderer)
{
  bool visible = true;

  GetDataNode()->GetVisibility(visible, renderer, "visible");

  if(!visible) return;

  PlaneGeometryData::Pointer input = const_cast< PlaneGeometryData * >(this->GetInput());
  mitk::DataNode* geometryDataNode = renderer->GetCurrentWorldPlaneGeometryNode();
  const PlaneGeometryData* rendererWorldPlaneGeometryData = dynamic_cast< PlaneGeometryData * >(geometryDataNode->GetData());

  // intersecting with ourself?
  if ( input.IsNull() || input.GetPointer() == rendererWorldPlaneGeometryData)
  {
    return; // do rectangle stuff!
  }

  const PlaneGeometry *inputPlaneGeometry = dynamic_cast< const PlaneGeometry * >( input->GetPlaneGeometry() );

  const PlaneGeometry* worldPlaneGeometry = dynamic_cast< const PlaneGeometry* >(
    rendererWorldPlaneGeometryData->GetPlaneGeometry() );

  if ( worldPlaneGeometry && dynamic_cast<const AbstractTransformGeometry*>(worldPlaneGeometry)==NULL
       && inputPlaneGeometry && dynamic_cast<const AbstractTransformGeometry*>(input->GetPlaneGeometry() )==NULL
       && inputPlaneGeometry->GetReferenceGeometry() )
  {
    const BaseGeometry *referenceGeometry = inputPlaneGeometry->GetReferenceGeometry();

    // calculate intersection of the plane data with the border of the
    // world geometry rectangle
    Point3D point1, point2;

    Line3D crossLine;

    // Calculate the intersection line of the input plane with the world plane
    if ( worldPlaneGeometry->IntersectionLine( inputPlaneGeometry, crossLine ) )
    {
      Point3D boundingBoxMin, boundingBoxMax;
      boundingBoxMin = referenceGeometry->GetBoundingBox()->GetMinimum();
      boundingBoxMax = referenceGeometry->GetBoundingBox()->GetMaximum();

      referenceGeometry->IndexToWorld(boundingBoxMin,boundingBoxMin);
      referenceGeometry->IndexToWorld(boundingBoxMax,boundingBoxMax);

      // Then, clip this line with the (transformed) bounding box of the
      // reference geometry.
      crossLine.BoxLineIntersection(
        boundingBoxMin[0], boundingBoxMin[1], boundingBoxMin[2],
        boundingBoxMax[0], boundingBoxMax[1], boundingBoxMax[2],
        crossLine.GetPoint(), crossLine.GetDirection(),
        point1, point2 );

      crossLine.SetPoints(point1,point2);

      vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
      vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
      vtkSmartPointer<vtkPolyData> linesPolyData = vtkSmartPointer<vtkPolyData>::New();


      // Now iterate through all other lines displayed in this window and
      // calculate the positions of intersection with the line to be
      // rendered; these positions will be stored in lineParams to form a
      // gap afterwards.
      NodesVectorType::iterator otherPlanesIt = m_OtherPlaneGeometries.begin();
      NodesVectorType::iterator otherPlanesEnd = m_OtherPlaneGeometries.end();

      std::vector<Point3D> intersections;

      intersections.push_back(point1);

      otherPlanesIt = m_OtherPlaneGeometries.begin();
      int gapsize = 32;
      this->GetDataNode()->GetPropertyValue( "Gap size",gapsize, NULL );


      ScalarType lineLength = point1.EuclideanDistanceTo(point2);
      DisplayGeometry *displayGeometry = renderer->GetDisplayGeometry();
      Point2D lengthBounds;
      lengthBounds.Fill(lineLength);
      displayGeometry->WorldToDisplay(lengthBounds,lengthBounds);

      float gapSizeParam = (1/lengthBounds[0]*gapsize)/2;

      while ( otherPlanesIt != otherPlanesEnd )
      {
        PlaneGeometry *otherPlane = static_cast< PlaneGeometry * >(
          static_cast< PlaneGeometryData * >((*otherPlanesIt)->GetData() )->GetPlaneGeometry() );

        if (otherPlane != inputPlaneGeometry && otherPlane != worldPlaneGeometry)
        {
          Point3D planeIntersection;
          otherPlane->IntersectionPoint(crossLine,planeIntersection);
          ScalarType sectionLength = point1.EuclideanDistanceTo(planeIntersection);
          ScalarType lineValue = sectionLength/lineLength;
          intersections.push_back(crossLine.GetPoint(lineValue-gapSizeParam));
          intersections.push_back(crossLine.GetPoint(lineValue+gapSizeParam));
        }
        ++otherPlanesIt;
      }
      intersections.push_back(point2);

      for(unsigned int i = 0 ; i< intersections.size()-1 ; i+=2)
      {
        this->DrawLine(intersections[i],intersections[i+1],lines,points);
      }

      // Add the points to the dataset
      linesPolyData->SetPoints(points);

      // Add the lines to the dataset
      linesPolyData->SetLines(lines);

      // Visualize
      vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      mapper->SetInputData(linesPolyData);

      LocalStorage* ls = m_LSH.GetLocalStorage(renderer);
      ls->m_CrosshairActor->SetMapper(mapper);

      ls->m_CrosshairAssembly->AddPart(ls->m_CrosshairActor);
    }
  }
}

void mitk::PlaneGeometryDataMapper2D::DrawLine( mitk::Point3D p0,mitk::Point3D p1,
                                                vtkCellArray* lines,
                                                vtkPoints* points
                                                )
{

  vtkIdType pidStart = points->InsertNextPoint(p0[0],p0[1], p0[2]);
  vtkIdType pidEnd = points->InsertNextPoint(p1[0],p1[1], p1[2]);

  vtkSmartPointer<vtkLine> lineVtk =  vtkSmartPointer<vtkLine>::New();
  lineVtk->GetPointIds()->SetId(0,pidStart);
  lineVtk->GetPointIds()->SetId(1,pidEnd);

  lines->InsertNextCell(lineVtk);

}

int mitk::PlaneGeometryDataMapper2D::DetermineThickSliceMode( DataNode * dn, int &thickSlicesNum )
{
  int thickSlicesMode = 0;
  // determine the state and the extend of the thick-slice mode
  mitk::ResliceMethodProperty *resliceMethodEnumProperty=0;
  if( dn->GetProperty( resliceMethodEnumProperty, "reslice.thickslices" ) && resliceMethodEnumProperty )
    thickSlicesMode = resliceMethodEnumProperty->GetValueAsId();

  IntProperty *intProperty=0;
  if( dn->GetProperty( intProperty, "reslice.thickslices.num" ) && intProperty )
  {
    thickSlicesNum = intProperty->GetValue();
    if(thickSlicesNum < 1) thickSlicesNum=0;
    if(thickSlicesNum > 10) thickSlicesNum=10;
  }

  if ( thickSlicesMode == 0 )
    thickSlicesNum = 0;

  return thickSlicesMode;
}

void mitk::PlaneGeometryDataMapper2D::ApplyAllProperties( BaseRenderer *renderer )
{
  LocalStorage *ls = m_LSH.GetLocalStorage(renderer);
  Superclass::ApplyColorAndOpacityProperties(renderer, ls->m_CrosshairActor);

  PlaneOrientationProperty* decorationProperty;
  this->GetDataNode()->GetProperty( decorationProperty, "decoration", renderer );
  if ( decorationProperty != NULL )
  {
    if ( decorationProperty->GetPlaneDecoration() ==
      PlaneOrientationProperty::PLANE_DECORATION_POSITIVE_ORIENTATION )
    {
      m_RenderOrientationArrows = true;
      m_ArrowOrientationPositive = true;
    }
    else if ( decorationProperty->GetPlaneDecoration() ==
      PlaneOrientationProperty::PLANE_DECORATION_NEGATIVE_ORIENTATION )
    {
      m_RenderOrientationArrows = true;
      m_ArrowOrientationPositive = false;
    }
    else
    {
      m_RenderOrientationArrows = false;
    }
  }
}

void mitk::PlaneGeometryDataMapper2D::SetDefaultProperties(mitk::DataNode* node, mitk::BaseRenderer* renderer, bool overwrite)
{
  node->AddProperty( "Line width", mitk::IntProperty::New(2), renderer, overwrite );
  node->AddProperty( "Gap size", mitk::IntProperty::New(32), renderer, overwrite );
  node->AddProperty( "decoration", mitk::PlaneOrientationProperty
                     ::New(PlaneOrientationProperty::PLANE_DECORATION_NONE), renderer, overwrite );

  Superclass::SetDefaultProperties(node, renderer, overwrite);
}

void mitk::PlaneGeometryDataMapper2D::UpdateVtkTransform(mitk::BaseRenderer *renderer)
{

}

mitk::PlaneGeometryDataMapper2D::LocalStorage::LocalStorage()
{
  m_CrosshairAssembly = vtkSmartPointer <vtkPropAssembly>::New();
  m_CrosshairActor = vtkSmartPointer <vtkActor>::New();
}
// destructor LocalStorage
mitk::PlaneGeometryDataMapper2D::LocalStorage::~LocalStorage()
{
}
