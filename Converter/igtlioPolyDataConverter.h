/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef IGTLIOPOLYDATACONVERTER_H
#define IGTLIOPOLYDATACONVERTER_H

#include "igtlioConverterExport.h"

#include <igtlPolyDataMessage.h>
#include "igtlioBaseConverter.h"

class vtkPolyData;
class vtkCellArray;
class vtkDataSetAttributes;

namespace igtlio
{

/** Conversion between igtl::PolyDataMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT PolyDataConverter : public BaseConverter
{
public:
  /**
   * This structure contains everything that igtl::PolyMessage is able to contain,
   * in a vtk-friendly format.
   */
  struct ContentData
  {
    vtkSmartPointer<vtkPolyData> polydata;
    std::string deviceName;
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "POLYDATA"; }

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* dest, bool checkCRC, igtl::MessageBase::MetaDataMap* metaInfo = NULL);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::PolyDataMessage::Pointer* dest, igtl::MessageBase::MetaDataMap* metaInfo = NULL);

  // Extract vtkPolyData from existing polyDataMsg, insert into existing poly.
  static int IGTLToVTKPolyData(igtl::PolyDataMessage::Pointer polyDataMsg, vtkSmartPointer<vtkPolyData> poly);

  // Insert an existing vtkPolyData into a cleared PolyDataMessage.
  static int VTKPolyDataToIGTL(vtkSmartPointer<vtkPolyData> poly, igtl::PolyDataMessage::Pointer outMessage);

  // Utility function for MRMLToIGTL(): Convert vtkCellArray to igtl::PolyDataCellArray
  static int VTKToIGTLCellArray(vtkCellArray* src, igtl::PolyDataCellArray* dest);

  // Utility function for MRMLToIGTL(): Convert i-th vtkDataSetAttributes (vtkCellData and vtkPointData)
  // to igtl::PolyDataAttribute
  static int VTKToIGTLAttribute(vtkDataSetAttributes* src, int i, igtl::PolyDataAttribute* dest);
};

} // namespace igtlio


#endif //IGTLIOPOLYDATACONVERTER_H
