/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __igtlImageConverter_h
#define __igtlImageConverter_h

// OpenIGTLink includes
#include "igtlLightObject.h"
#include <igtlImageMessage.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

class vtkImageData;
class vtkMatrix4x4;
#include "igtlioConverterExport.h"
#include "igtlBaseConverter.h"

namespace igtl
{

/** Conversion between igtl::ImageMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT ImageConverter : public BaseConverter
{
public:
 /** Standard class typedefs. */
 typedef ImageConverter        Self;
 typedef LightObject              Superclass;
 typedef SmartPointer<Self>       Pointer;
 typedef SmartPointer<const Self> ConstPointer;

 /** Method for creation through the object factory. */
 igtlNewMacro(Self);

 /** Run-time type information (and related methods). */
 igtlTypeMacro(ImageConverter, BaseConverter);

  /**
   * This structure contains everything that igtl::ImageMessage is able to contain,
   * in a vtk-friendly format.
   */
  struct ContentData
  {
  vtkSmartPointer<vtkImageData> image;
  vtkSmartPointer<vtkMatrix4x4> transform; // ijk2ras, From image pixel space to RAS
  };

  virtual void PrintSelf(std::ostream& os) const;

  virtual const char*  GetIGTLName() { return GetIGTLTypeName(); };
  static const char* GetIGTLTypeName() { return "IMAGE"; };

  int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC);
  int toIGTL(const HeaderData& header, const ContentData& source, igtl::ImageMessage::Pointer* dest);

protected:
  ImageConverter();
  ~ImageConverter();

  int IGTLToVTKScalarType(int igtlType);
  int IGTLToVTKImageData(igtl::ImageMessage::Pointer imgMsg, ContentData *dest);
  int IGTLToVTKTransform(igtl::ImageMessage::Pointer imgMsg, vtkSmartPointer<vtkMatrix4x4> ijk2ras);
};

}


#endif //__igtlImageConverter_h
