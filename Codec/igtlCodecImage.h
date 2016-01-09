/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __igtlCodecImage_h
#define __igtlCodecImage_h

#include "igtlSupportModuleCodecExport.h"

// OpenIGTLink includes
#include <igtlImageMessage.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

class vtkImageData;
class vtkMatrix4x4;

/** Conversion between igtl::ImageMessage and vtk classes.
 *
 */
class OPENIGTLINK_SUPPORT_MODULE_CODEC_EXPORT igtlCodecImage : public vtkObject
{
public:
  /**
   * This structure contains everything that igtl::ImageMessage is able to contain,
   * in a vtk-friendly format.
   */
  struct MessageContent
  {
  vtkSmartPointer<vtkImageData> image;
  vtkSmartPointer<vtkMatrix4x4> transform; // ijk2ras, From image pixel space to RAS
  std::string deviceName;
  };

  static igtlCodecImage *New();
  vtkTypeMacro(igtlCodecImage,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "IMAGE"; };

  virtual int IGTLToVTK(igtl::MessageBase::Pointer source, MessageContent* dest, bool checkCRC);
  virtual int VTKToIGTL(const MessageContent& source, igtl::ImageMessage::Pointer* dest);

protected:
  igtlCodecImage();
  ~igtlCodecImage();

  int IGTLToVTKScalarType(int igtlType);
  int IGTLToVTKImageData(igtl::ImageMessage::Pointer imgMsg, MessageContent *dest);
  int IGTLToVTKTransform(igtl::ImageMessage::Pointer imgMsg, vtkSmartPointer<vtkMatrix4x4> ijk2ras);
};


#endif //__igtlCodecImage_h
