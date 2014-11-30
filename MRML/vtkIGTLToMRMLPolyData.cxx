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
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>


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
  displayNode->SetOpacity(1.0);

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

  // Vertices
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
        tstrip->GetPointIds()->SetId(j, *iter);
        j++;
        }
      tstripCells->InsertNextCell(tstrip);
      }
    poly->SetStrips(tstripCells);
    }

  // Attribute
  int nAttributes = polyDataMsg->GetNumberOfAttributes();
  for (int i = 0; i < nAttributes; i ++)
    {
    igtl::PolyDataAttribute::Pointer attribute;    
    attribute = polyDataMsg->GetAttribute(i);

    vtkSmartPointer<vtkFloatArray> data = 
      vtkSmartPointer<vtkFloatArray>::New();

    data->SetName(attribute->GetName()); //set the name of the value
    int n = attribute->GetSize();

    // NOTE: Data types for POINT (igtl::PolyDataMessage::POINT_*) and CELL
    // (igtl::PolyDataMessage::CELL_*) have the same lower 4 bit. 
    // By masking the values with 0x0F, attribute types (either SCALAR, VECTOR, NORMAL,
    // TENSOR, or RGBA) can be obtained. On the other hand, by masking the value
    // with 0xF0, data types (POINT or CELL) can be obtained.
    // See, igtlPolyDataMessage.h in the OpenIGTLink library.
    switch (attribute->GetType() & 0x0F)
      {
      case igtl::PolyDataAttribute::POINT_SCALAR:
        {
        data->SetNumberOfComponents(1);
        break;
        }
      case igtl::PolyDataAttribute::POINT_VECTOR:
      case igtl::PolyDataAttribute::POINT_NORMAL:
        {
        data->SetNumberOfComponents(3);
        break;
        }
      case igtl::PolyDataAttribute::POINT_TENSOR:
        {
        data->SetNumberOfComponents(9); // TODO: Is it valid in Slicer?
        break;
        }
      case igtl::PolyDataAttribute::POINT_RGBA:
        {
        data->SetNumberOfComponents(4); // TODO: Is it valid in Slicer?
        break;
        }
      default:
        {
        // ERROR
        break;
        }
      }
    data->SetNumberOfTuples(n);
    attribute->GetData(static_cast<igtl_float32*>(data->GetPointer(0)));

    if ((attribute->GetType() & 0xF0) == 0) // POINT
      {
      poly->GetPointData()->AddArray(data);
      }
    else // CELL
      {
      poly->GetCellData()->AddArray(data);
      }
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
      // TODO: the node not found
      return 0;
      }
    
    vtkSmartPointer<vtkPolyData> poly = modelNode->GetPolyData();
    if (poly.GetPointer() == NULL)
      {
      // TODO: poly data is not available
      return 0;
      }
    
    //------------------------------------------------------------
    // Allocate Status Message Class
    if (this->OutPolyDataMessage.IsNull())
      {
      this->OutPolyDataMessage = igtl::PolyDataMessage::New();
      }
    
    // Set message name -- use the same name as the MRML node 
    this->OutPolyDataMessage->SetDeviceName(modelNode->GetName());

    // Points
    vtkSmartPointer<vtkPoints> points = poly->GetPoints();
    if (points.GetPointer() != NULL)
      {
      int npoints = points->GetNumberOfPoints();
      if (npoints > 0)
        {
        igtl::PolyDataPointArray::Pointer pointArray = igtl::PolyDataPointArray::New();
        for (unsigned int i = 0; i < npoints; i ++)
          {
          double *p = points->GetPoint(i);
          pointArray->AddPoint(static_cast<igtlFloat32>(p[0]),
                               static_cast<igtlFloat32>(p[1]),
                               static_cast<igtlFloat32>(p[2]));
          }
        this->OutPolyDataMessage->SetPoints(pointArray);
        }
      }
    
    // Vertices
    vtkSmartPointer<vtkCellArray> vertCells = poly->GetVerts();
    if (vertCells.GetPointer() != NULL)
      {
      int nverts = vertCells->GetNumberOfCells();
      if (nverts > 0)
        {
        igtl::PolyDataCellArray::Pointer verticesArray = igtl::PolyDataCellArray::New();
        vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();

        vertCells->InitTraversal();
        while (vertCells->GetNextCell(idList))
          {
          std::list<igtlUint32> cell;
          int nIds = idList->GetNumberOfIds();
          for (int i = 0; i < nIds; i ++)
            {
            cell.push_back(idList->GetId(i));
            }
          verticesArray->AddCell(cell);
          }
        this->OutPolyDataMessage->SetVertices(verticesArray);
        }
      }

    // Lines
    igtl::PolyDataCellArray::Pointer linesArray;
    vtkSmartPointer<vtkCellArray> lineCells = poly->GetLines();
    if (lineCells.GetPointer() != NULL)
      {
      int nlines = lineCells->GetNumberOfCells();
      if (nlines > 0)
        {
        igtl::PolyDataCellArray::Pointer linesArray = igtl::PolyDataCellArray::New();
        vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();

        lineCells->InitTraversal();
        while (lineCells->GetNextCell(idList))
          {
          std::list<igtlUint32> cell;
          int nIds = idList->GetNumberOfIds();
          for (int i = 0; i < nIds; i ++)
            {
            cell.push_back(idList->GetId(i));
            }
          linesArray->AddCell(cell);
          }
        this->OutPolyDataMessage->SetLines(linesArray);
        }
      }


    // Polygons
    igtl::PolyDataCellArray::Pointer polygonsArray;
    vtkSmartPointer<vtkCellArray> polygonCells = poly->GetPolys();
    if (polygonCells.GetPointer() != NULL)
      {
      int npolygons = polygonCells->GetNumberOfCells();
      if (npolygons > 0)
        {
        igtl::PolyDataCellArray::Pointer polygonsArray = igtl::PolyDataCellArray::New();
        vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();

        polygonCells->InitTraversal();
        while (polygonCells->GetNextCell(idList))
          {
          std::list<igtlUint32> cell;
          int nIds = idList->GetNumberOfIds();
          for (int i = 0; i < nIds; i ++)
            {
            cell.push_back(idList->GetId(i));
            }
          polygonsArray->AddCell(cell);
          }
        this->OutPolyDataMessage->SetPolygons(polygonsArray);
        }
      }

    // Triangl strips
    igtl::PolyDataCellArray::Pointer triangleStripsArray;
    vtkSmartPointer<vtkCellArray> triangleStripCells = poly->GetStrips();
    if (triangleStripCells.GetPointer() != NULL)
      {
      int ntriangleStrips = triangleStripCells->GetNumberOfCells();
      if (ntriangleStrips > 0)
        {
        igtl::PolyDataCellArray::Pointer triangleStripsArray = igtl::PolyDataCellArray::New();
        vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();

        triangleStripCells->InitTraversal();
        while (triangleStripCells->GetNextCell(idList))
          {
          std::list<igtlUint32> cell;
          int nIds = idList->GetNumberOfIds();
          for (int i = 0; i < nIds; i ++)
            {
            cell.push_back(idList->GetId(i));
            }
          triangleStripsArray->AddCell(cell);
          }
        this->OutPolyDataMessage->SetTriangleStrips(triangleStripsArray);
        }
      }

    // Attributes
    vtkSmartPointer<vtkPointData> pdata = poly->GetPointData();
    int nPointAttributes = pdata->GetNumberOfArrays();
    if (nPointAttributes > 0)
      {
      for (int i = 0; i < nPointAttributes; i ++)
        {
        igtl::PolyDataAttribute::Pointer attribute = igtl::PolyDataAttribute::New();
        vtkSmartPointer<vtkDataArray> array = pdata->GetArray(i);
        int ncomps  = array->GetNumberOfComponents();
        if (ncomps == 1)
          {
          attribute->SetType(igtl::PolyDataAttribute::POINT_SCALAR);
          }
        else if (ncomps == 3)
          {
          // TODO: how to differenciate normal and vector?
          attribute->SetType(igtl::PolyDataAttribute::POINT_NORMAL);
          }
        else if (ncomps == 9)
          {
          attribute->SetType(igtl::PolyDataAttribute::POINT_TENSOR);
          }
        else if (ncomps == 4)
          {
          attribute->SetType(igtl::PolyDataAttribute::POINT_RGBA);
          }
        attribute->SetName(array->GetName());
        int ntuples = array->GetNumberOfTuples();
        attribute->SetSize(ntuples);

        for (int j = 0; j < ntuples; j ++)
          {
          double * tuple = array->GetTuple(j);
          igtlFloat32 data[9];
          for (int k = 0; k < ncomps; k ++)
            {
            data[k] = static_cast<igtlFloat32>(tuple[k]);
            }
          attribute->SetNthData(j, data);
          }
        this->OutPolyDataMessage->AddAttribute(attribute);
        }
      }

    vtkSmartPointer<vtkCellData> cdata = poly->GetCellData();
    int nCellAttributes = cdata->GetNumberOfArrays();
    if (nCellAttributes > 0)
      {
      for (int i = 0; i < nPointAttributes; i ++)
        {
        igtl::PolyDataAttribute::Pointer attribute = igtl::PolyDataAttribute::New();
        vtkSmartPointer<vtkDataArray> array = pdata->GetArray(i);
        int ncomps  = array->GetNumberOfComponents();
        if (ncomps == 1)
          {
          attribute->SetType(igtl::PolyDataAttribute::CELL_SCALAR);
          }
        else if (ncomps == 3)
          {
          // TODO: how to differenciate normal and vector?
          attribute->SetType(igtl::PolyDataAttribute::CELL_NORMAL);
          }
        else if (ncomps == 9)
          {
          attribute->SetType(igtl::PolyDataAttribute::CELL_TENSOR);
          }
        else if (ncomps == 4)
          {
          attribute->SetType(igtl::PolyDataAttribute::CELL_RGBA);
          }
        attribute->SetName(array->GetName());
        int ntuples = array->GetNumberOfTuples();
        attribute->SetSize(ntuples);

        for (int j = 0; j < ntuples; j ++)
          {
          double * tuple = array->GetTuple(j);
          igtlFloat32 data[9];
          for (int k = 0; k < ncomps; k ++)
            {
            data[k] = static_cast<igtlFloat32>(tuple[k]);
            }
          attribute->SetNthData(j, data);
          }
        this->OutPolyDataMessage->AddAttribute(attribute);
        }
      }
    
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


