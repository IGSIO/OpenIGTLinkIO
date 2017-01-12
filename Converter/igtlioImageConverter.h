/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef IGTLIOIMAGECONVERTER_H
#define IGTLIOIMAGECONVERTER_H

// OpenIGTLink includes
#include "igtlLightObject.h"
#include <igtlImageMessage.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

class vtkImageData;
class vtkMatrix4x4;
#include "igtlioConverterExport.h"
#include "igtlioBaseConverter.h"

namespace igtlio
{

/** Conversion between igtl::ImageMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT ImageConverter : public BaseConverter
{
public:
  /**
   * This structure contains everything that igtl::ImageMessage is able to contain,
   * in a vtk-friendly format.
   */
  struct ContentData
  {
  vtkSmartPointer<vtkImageData> image;
  vtkSmartPointer<vtkMatrix4x4> transform; // ijk2ras, From image pixel space to RAS
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); };
  static const char* GetIGTLTypeName() { return "IMAGE"; };

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::ImageMessage::Pointer* dest);

protected:

  static int IGTLToVTKScalarType(int igtlType);
  static int IGTLToVTKImageData(igtl::ImageMessage::Pointer imgMsg, ContentData *dest);
  static int IGTLToVTKTransform(igtl::ImageMessage::Pointer imgMsg, vtkSmartPointer<vtkMatrix4x4> ijk2ras);
};

} //namespace igtlio


#endif //IGTLIOIMAGECONVERTER_H
