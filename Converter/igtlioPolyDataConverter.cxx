/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

#include "igtlioPolyDataConverter.h"

#include <vtkPolyData.h>
#include <vtkVertex.h>
#include <vtkCellArray.h>
#include <vtkPolyLine.h>
#include <vtkPolygon.h>
#include <vtkTriangleStrip.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>


namespace igtlio
{

//---------------------------------------------------------------------------
int PolyDataConverter::IGTLToVTK(igtl::MessageBase::Pointer source, PolyDataConverter::MessageContent *dest, bool checkCRC)
{
 // Create a message buffer to receive image data
 igtl::PolyDataMessage::Pointer polyDataMsg;
 polyDataMsg = igtl::PolyDataMessage::New();
 polyDataMsg->Copy(source); // !! TODO: copy makes performance issue.

 // Deserialize the data
 // If CheckCRC==0, CRC check is skipped.
 int c = polyDataMsg->Unpack(checkCRC);

 if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
   {
   std::cerr << "Unable to create vtkPolyData from incoming POLYDATA message. Failed to unpack the message" << std::endl;
   return 0;
   }

 vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();

 if (poly.GetPointer()==NULL)
   {
   // TODO: Error handling
   }

 if (!IGTLToVTKPolyData(polyDataMsg, poly))
  return 0;

 dest->polydata = poly;
 dest->deviceName = polyDataMsg->GetDeviceName();

 return 1;
}

//---------------------------------------------------------------------------
int PolyDataConverter::IGTLToVTKPolyData(igtl::PolyDataMessage::Pointer polyDataMsg, vtkSmartPointer<vtkPolyData> poly)
{
 // Points
 igtl::PolyDataPointArray::Pointer pointsArray = polyDataMsg->GetPoints();
 int npoints = pointsArray->GetNumberOfPoints();
 if (npoints > 0)
   {
   vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
   for (int i = 0; i < npoints; i ++)
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
 igtl::PolyDataCellArray::Pointer verticesArray =  polyDataMsg->GetVertices();
 int nvertices = verticesArray.IsNotNull() ? verticesArray->GetNumberOfCells() : 0;
 if (nvertices > 0)
   {
   vtkSmartPointer<vtkCellArray> vertCells = vtkSmartPointer<vtkCellArray>::New();
   for (int i = 0; i < nvertices; i ++)
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
 igtl::PolyDataCellArray::Pointer linesArray = polyDataMsg->GetLines();
 int nlines = linesArray.IsNotNull() ? linesArray->GetNumberOfCells() : 0;
 if (nlines > 0)
   {
   vtkSmartPointer<vtkCellArray> lineCells = vtkSmartPointer<vtkCellArray>::New();
   for(int i = 0; i < nlines; i++)
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
 igtl::PolyDataCellArray::Pointer polygonsArray = polyDataMsg->GetPolygons();
 int npolygons = polygonsArray.IsNotNull() ? polygonsArray->GetNumberOfCells() : 0;
 if (npolygons > 0)
   {
   vtkSmartPointer<vtkCellArray> polygonCells = vtkSmartPointer<vtkCellArray>::New();
   for(int i = 0; i < npolygons; i++)
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
 igtl::PolyDataCellArray::Pointer triangleStripsArray = polyDataMsg->GetTriangleStrips();
 int ntstrips = triangleStripsArray.IsNotNull() ? triangleStripsArray->GetNumberOfCells() : 0;
 if (ntstrips > 0)
   {
   vtkSmartPointer<vtkCellArray> tstripCells = vtkSmartPointer<vtkCellArray>::New();
   for(int i = 0; i < ntstrips; i++)
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

   data->SetNumberOfComponents(attribute->GetNumberOfComponents());
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

 poly->Modified();

 return 1;
}

//---------------------------------------------------------------------------
int PolyDataConverter::VTKToIGTL(const PolyDataConverter::MessageContent &source, igtl::PolyDataMessage::Pointer *dest)
{
   if (source.polydata.GetPointer() == NULL)
     {
     // TODO: poly data is not available
     return 0;
     }

   //------------------------------------------------------------
   // Allocate Status Message Class
   if ((*dest).IsNull())
     {
     (*dest) = igtl::PolyDataMessage::New();
     }
   else
     {
     (*dest)->Clear();
     }
   igtl::PolyDataMessage::Pointer outMessage = *dest;

   // Set message name -- use the same name as the MRML node
   outMessage->SetDeviceName(source.deviceName.c_str());

   if (!VTKPolyDataToIGTL(source.polydata, outMessage))
     return 0;

   // Pack the message
   outMessage->Pack();

   return 1;
}

//---------------------------------------------------------------------------
int PolyDataConverter::VTKPolyDataToIGTL(vtkSmartPointer<vtkPolyData> poly, igtl::PolyDataMessage::Pointer outMessage)
{
   // Points
   vtkSmartPointer<vtkPoints> points = poly->GetPoints();
   if (points.GetPointer() != NULL)
     {
     int npoints = points->GetNumberOfPoints();
     if (npoints > 0)
       {
       igtl::PolyDataPointArray::Pointer pointArray = igtl::PolyDataPointArray::New();
       for (int i = 0; i < npoints; i ++)
         {
         double *p = points->GetPoint(i);
         pointArray->AddPoint(static_cast<igtlFloat32>(p[0]),
                              static_cast<igtlFloat32>(p[1]),
                              static_cast<igtlFloat32>(p[2]));
         }
       outMessage->SetPoints(pointArray);
       }
     }

   // Vertices
   vtkSmartPointer<vtkCellArray> vertCells = poly->GetVerts();
   if (vertCells.GetPointer() != NULL)
     {
     igtl::PolyDataCellArray::Pointer verticesArray = igtl::PolyDataCellArray::New();
     if (VTKToIGTLCellArray(vertCells, verticesArray) > 0)
       {
       outMessage->SetVertices(verticesArray);
       }
     }

   // Lines
   vtkSmartPointer<vtkCellArray> lineCells = poly->GetLines();
   if (lineCells.GetPointer() != NULL)
     {
     igtl::PolyDataCellArray::Pointer linesArray = igtl::PolyDataCellArray::New();
     if (VTKToIGTLCellArray(lineCells, linesArray) > 0)
       {
       outMessage->SetLines(linesArray);
       }
     }

   // Polygons
   vtkSmartPointer<vtkCellArray> polygonCells = poly->GetPolys();
   if (polygonCells.GetPointer() != NULL)
     {
     igtl::PolyDataCellArray::Pointer polygonsArray = igtl::PolyDataCellArray::New();
     if (VTKToIGTLCellArray(polygonCells, polygonsArray) > 0)
       {
       outMessage->SetPolygons(polygonsArray);
       }
     }

   // Triangle strips
   vtkSmartPointer<vtkCellArray> triangleStripCells = poly->GetStrips();
   if (triangleStripCells.GetPointer() != NULL)
     {
     igtl::PolyDataCellArray::Pointer triangleStripsArray = igtl::PolyDataCellArray::New();
     if (VTKToIGTLCellArray(triangleStripCells, triangleStripsArray) > 0)
       {
       outMessage->SetTriangleStrips(triangleStripsArray);
       }
     }

   // Attributes for points
   vtkSmartPointer<vtkPointData> pdata = poly->GetPointData();
   int nPointAttributes = pdata->GetNumberOfArrays();
   if (nPointAttributes > 0)
     {
     for (int i = 0; i < nPointAttributes; i ++)
       {
       igtl::PolyDataAttribute::Pointer attribute = igtl::PolyDataAttribute::New();
       if (VTKToIGTLAttribute(pdata, i, attribute) > 0)
         {
         outMessage->AddAttribute(attribute);
         }
       }
     }


   // Attributes for cells
   vtkSmartPointer<vtkCellData> cdata = poly->GetCellData();
   int nCellAttributes = cdata->GetNumberOfArrays();
   if (nCellAttributes > 0)
     {
     for (int i = 0; i < nCellAttributes; i ++)
       {
       igtl::PolyDataAttribute::Pointer attribute = igtl::PolyDataAttribute::New();
       if (VTKToIGTLAttribute(cdata, i, attribute) > 0)
         {
         outMessage->AddAttribute(attribute);
         }
       }
     }

   return 1;
}

//---------------------------------------------------------------------------
int PolyDataConverter::VTKToIGTLCellArray(vtkCellArray* src, igtl::PolyDataCellArray* dest)
{

  if (src && dest)
    {
    int ncells = src->GetNumberOfCells();
    if (ncells > 0)
      {
      vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
      src->InitTraversal();
      while (src->GetNextCell(idList))
        {
        std::list<igtlUint32> cell;
        int nIds = idList->GetNumberOfIds();
        for (int i = 0; i < nIds; i ++)
          {
          cell.push_back(idList->GetId(i));
          }
        dest->AddCell(cell);
        }
      }
    return ncells;
    }
  else
    {
    return 0;
    }

}


//---------------------------------------------------------------------------
int PolyDataConverter::VTKToIGTLAttribute(vtkDataSetAttributes* src, int i, igtl::PolyDataAttribute* dest)
{

  //vtkSmartPointer<vtkPointData> src = poly->GetPointData();
  if ((!src) || (!dest))
    {
    return 0;
    }

  // Check the range of index i
  if (i < 0 || i >= src->GetNumberOfArrays())
    {
    return 0;
    }

  // NOTE: Data types for POINT (igtl::PolyDataMessage::POINT_*) and CELL
  // (igtl::PolyDataMessage::CELL_*) have the same bits exept the 3rd bit (0x10).
  // attrType will contain the 3rd bit based on the type of vtkDataSetAttributes
  // (either vtkCellData or vtkPointData). See, igtlPolyDataMessage.h in the OpenIGTLink library.
  int attrTypeBit;
  if (src->IsTypeOf("vtkCellData"))
    {
    attrTypeBit = 0x10;
    }
  else // vtkPointData
    {
    attrTypeBit = 0x00;
    }

  vtkSmartPointer<vtkDataArray> array = src->GetArray(i);
  int attributeType = src->IsArrayAnAttribute(i);
  switch(attributeType)
    {
    case vtkDataSetAttributes::SCALARS:
      // OpenIGTLink designates special cases for scalars of a certain number of components
      switch(array->GetNumberOfComponents())
        {
        case 4:
          dest->SetType(igtl::PolyDataAttribute::POINT_RGBA | attrTypeBit);
          break;
        default:
          dest->SetType(igtl::PolyDataAttribute::POINT_SCALAR | attrTypeBit, array->GetNumberOfComponents());
          break;
        }
        break;
    case vtkDataSetAttributes::VECTORS:
      dest->SetType(igtl::PolyDataAttribute::POINT_VECTOR | attrTypeBit);
      break;
    case vtkDataSetAttributes::TCOORDS:
      dest->SetType(igtl::PolyDataAttribute::POINT_TCOORDS | attrTypeBit);
      break;
    case vtkDataSetAttributes::NORMALS:
      dest->SetType(igtl::PolyDataAttribute::POINT_NORMAL | attrTypeBit);
      break;
    case vtkDataSetAttributes::TENSORS:
      dest->SetType(igtl::PolyDataAttribute::POINT_TENSOR | attrTypeBit);
      break;
    case vtkDataSetAttributes::GLOBALIDS:
    case vtkDataSetAttributes::PEDIGREEIDS:
    case vtkDataSetAttributes::EDGEFLAG:
    default:
      // Unsupported types, pass as generic scalar data (will be impossible to identify on receive)
      dest->SetType(igtl::PolyDataAttribute::POINT_SCALAR, array->GetNumberOfComponents());
      break;
    }

  dest->SetName(array->GetName() ? array->GetName() : "");
  int ntuples = array->GetNumberOfTuples();
  dest->SetSize(ntuples);

  for (int j = 0; j < ntuples; j ++)
    {
    double * tuple = array->GetTuple(j);
    igtlFloat32 data[9];
    for (int k = 0; k < array->GetNumberOfComponents(); k ++)
      {
      data[k] = static_cast<igtlFloat32>(tuple[k]);
      }
    dest->SetNthData(j, data);
    }

  return 1;

}

} // namespace igtlio
