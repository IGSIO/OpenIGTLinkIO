/*==========================================================================

  Portions (c) Copyright 2008-2014 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    vtkIGTLToMRMLPolyData.cxx

==========================================================================*/

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLPolyData.h"
#include "vtkMRMLIGTLQueryNode.h"

// OpenIGTLink includes
#include <igtl_util.h>
#include <igtlPolyDataMessage.h>

// Slicer includes
//#include <vtkSlicerColorLogic.h>
#include <vtkMRMLColorLogic.h>
#include <vtkMRMLColorTableNode.h>

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLVectorVolumeDisplayNode.h>

// VTK includes
#include <vtkPolyData.h>
#include <vtkIntArray.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkVertex.h>
#include <vtkCellArray.h>
#include <vtkPolyLine.h>
#include <vtkPolygon.h>
#include <vtkTriangleStrip.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

#include "vtkSlicerOpenIGTLinkIFLogic.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLToMRMLPolyData);
//---------------------------------------------------------------------------
vtkIGTLToMRMLPolyData::vtkIGTLToMRMLPolyData()
{
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLPolyData::~vtkIGTLToMRMLPolyData()
{
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLPolyData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}


//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLPolyData::CreateNewNodeWithMessage(vtkMRMLScene* scene, const char* name, igtl::MessageBase::Pointer incomingPolyDataMessage)
{
  igtl::MessageBase* innerPtr = incomingPolyDataMessage.GetPointer();
  if( innerPtr == NULL )
    {
    vtkErrorMacro("Unable to create MRML node from incoming POLYDATA message: incoming PolyDataMessage is invalid");
    return 0;
    }


  // Create a node
  vtkSmartPointer<vtkMRMLModelNode> modelNode = vtkSmartPointer<vtkMRMLModelNode>::New();
  modelNode->SetName(name);

  scene->SaveStateForUndo();

  vtkDebugMacro("Setting scene info");
  modelNode->SetScene(scene);
  modelNode->SetDescription("Received by OpenIGTLink");


  // Display Node
  vtkSmartPointer< vtkMRMLModelDisplayNode > displayNode = vtkSmartPointer< vtkMRMLModelDisplayNode >::New();

  double color[3];
  color[0] = 0.5;
  color[1] = 0.5;
  color[2] = 1.0;
  displayNode->SetColor(color);
  displayNode->SetOpacity(0.5);

  //modelNode->SetAndObservePolyData(poly);
  //modelNode->SetModifiedSinceRead(1);
  //displayNode->SetModifiedSinceRead(1); 
  displayNode->SliceIntersectionVisibilityOn();  
  displayNode->VisibilityOn();

  scene->SaveStateForUndo();
  scene->AddNode(modelNode);
  scene->AddNode(displayNode);

  displayNode->SetScene(scene);
  modelNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  modelNode->SetHideFromEditors(0);
  
  return modelNode;
}

//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLPolyData::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLModelNode::PolyDataModifiedEvent);

  return events;
}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLPolyData::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{

  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(node);

  if (modelNode==NULL)
    {
    vtkErrorMacro("vtkIGTLToMRMLPolyData::IGTLToMRML failed: invalid node");
    return 0;
    }


  // Create a message buffer to receive image data
  igtl::PolyDataMessage::Pointer polyDataMsg;
  polyDataMsg = igtl::PolyDataMessage::New();
  polyDataMsg->Copy(buffer); // !! TODO: copy makes performance issue.

  // Deserialize the data
  // If CheckCRC==0, CRC check is skipped.
  int c = polyDataMsg->Unpack(this->CheckCRC);

  if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
    {
    vtkErrorMacro("Unable to create MRML node from incoming POLYDATA message. Failed to unpack the message");
    return 0;
    }


  igtl::PolyDataPointArray::Pointer pointsArray        = polyDataMsg->GetPoints();        
  igtl::PolyDataCellArray::Pointer verticesArray       = polyDataMsg->GetVertices();      
  igtl::PolyDataCellArray::Pointer linesArray          = polyDataMsg->GetLines();         
  igtl::PolyDataCellArray::Pointer polygonsArray       = polyDataMsg->GetPolygons();      
  igtl::PolyDataCellArray::Pointer triangleStripsArray = polyDataMsg->GetTriangleStrips();

  vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();

  if (poly.GetPointer()==NULL)
    {
    // TODO: Error handling
    }

  // Points
  int npoints = pointsArray->GetNumberOfPoints();
  if (npoints > 0)
    {
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (unsigned int i = 0; i < npoints; i ++)
      {
      igtlFloat32 point[3];
      pointsArray->GetPoint(i, point);
      points->InsertNextPoint(point); // TODO: use the id returned by this call?
      }
    poly->SetPoints(points);
    }
  else
    {
    // ERROR: No points defined
    }
    
  int nvertices = verticesArray->GetNumberOfCells();
  if (nvertices > 0)
    {
    vtkSmartPointer<vtkCellArray> vertCells = vtkSmartPointer<vtkCellArray>::New();
    for (unsigned int i = 0; i < nvertices; i ++)
      {
      vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();

      std::list<igtlUint32> cell;
      verticesArray->GetCell(i, cell);
      //for (unsigned int j = 0; j < cell.size(); j ++) // TODO: is cell.size() always 1?
      //{
      std::list<igtlUint32>::iterator iter; 
      iter = cell.begin();
      vertex->GetPointIds()->SetId(i, *iter);
      //}
      vertCells->InsertNextCell(vertex);
      }
    poly->SetVerts(vertCells);
    }

  // Lines
  int nlines = linesArray->GetNumberOfCells();
  if (nlines > 0)
    {
    vtkSmartPointer<vtkCellArray> lineCells = vtkSmartPointer<vtkCellArray>::New();
    for(unsigned int i = 0; i < nlines; i++)
      {
      vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
      
      std::list<igtlUint32> cell;
      linesArray->GetCell(i, cell);
      polyLine->GetPointIds()->SetNumberOfIds(cell.size());
      std::list<igtlUint32>::iterator iter;
      int j = 0;
      for (iter = cell.begin(); iter != cell.end(); iter ++)
        {
        polyLine->GetPointIds()->SetId(j, *iter);
        j++;
        }
      lineCells->InsertNextCell(polyLine);    
      }
    poly->SetLines(lineCells);
    }

  // Polygons
  int npolygons = polygonsArray->GetNumberOfCells();
  if (npolygons > 0)
    {
    vtkSmartPointer<vtkCellArray> polygonCells = vtkSmartPointer<vtkCellArray>::New();
    for(unsigned int i = 0; i < npolygons; i++)
      {
      vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();

      std::list<igtlUint32> cell;
      polygonsArray->GetCell(i, cell);
      polygon->GetPointIds()->SetNumberOfIds(cell.size());
      std::list<igtlUint32>::iterator iter;
      int j = 0;
      for (iter = cell.begin(); iter != cell.end(); iter ++)
        {
        polygon->GetPointIds()->SetId(j, *iter);
        j++;
        }
      polygonCells->InsertNextCell(polygon);
      }
    poly->SetPolys(polygonCells);
    }

  // Triangle Strips
  int ntstrips = triangleStripsArray->GetNumberOfCells();
  if (ntstrips > 0)
    {
    vtkSmartPointer<vtkCellArray> tstripCells = vtkSmartPointer<vtkCellArray>::New();
    for(unsigned int i = 0; i < ntstrips; i++)
      {
      vtkSmartPointer<vtkTriangleStrip> tstrip = vtkSmartPointer<vtkTriangleStrip>::New();

      std::list<igtlUint32> cell;
      triangleStripsArray->GetCell(i, cell);
      tstrip->GetPointIds()->SetNumberOfIds(cell.size());
      std::list<igtlUint32>::iterator iter;
      int j = 0;
      for (iter = cell.begin(); iter != cell.end(); iter ++)
        {
        tstrip->GetPointIds()->SetId(i, i);
        j++;
        }
      tstripCells->InsertNextCell(tstrip);
      }
    poly->SetPolys(tstripCells);
    }

  modelNode->SetAndObservePolyData(poly);

  poly->Modified();
  modelNode->Modified();

  return 1;

}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLPolyData::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (!mrmlNode)
    {
    return 0;
    }

  // If mrmlNode is PolyData node
  if (event == vtkMRMLModelNode::PolyDataModifiedEvent && strcmp(mrmlNode->GetNodeTagName(), "Model") == 0)
    {
    vtkMRMLModelNode* modelNode =
      vtkMRMLModelNode::SafeDownCast(mrmlNode);

    if (!modelNode)
      {
      return 0;
      }

    //TODO: Create IGTL message

    this->OutPolyDataMessage->Pack();

    *size = this->OutPolyDataMessage->GetPackSize();
    *igtlMsg = (void*)this->OutPolyDataMessage->GetPackPointer();

    return 1;
    }
  else if (strcmp(mrmlNode->GetNodeTagName(), "IGTLQuery") == 0)   // If mrmlNode is query node
    {
    vtkMRMLIGTLQueryNode* qnode = vtkMRMLIGTLQueryNode::SafeDownCast(mrmlNode);
    if (qnode)
      {
      if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_GET)
        {
        if (this->GetPolyDataMessage.IsNull())
          {
          this->GetPolyDataMessage = igtl::GetPolyDataMessage::New();
          }
        if (qnode->GetNoNameQuery() == 1)
          {
          this->GetPolyDataMessage->SetDeviceName("");
          }
        else
          {
          this->GetPolyDataMessage->SetDeviceName(mrmlNode->GetName());
          }
        this->GetPolyDataMessage->Pack();
        *size = this->GetPolyDataMessage->GetPackSize();
        *igtlMsg = this->GetPolyDataMessage->GetPackPointer();
        return 1;
        }
      /*
      else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_START)
        {
        *size = 0;
        return 0;
        }
      else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_STOP)
        {
        *size = 0;
        return 0;
        }
      */
      return 0;
      }
    }
  else
    {
    return 0;
    }

  return 0;
}


