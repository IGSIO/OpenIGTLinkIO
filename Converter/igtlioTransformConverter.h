/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef IGTLIOTRANSFORMCONVERTER_H
#define IGTLIOTRANSFORMCONVERTER_H

#include "igtlioConverterExport.h"

#include <igtlTransformMessage.h>

#include "igtlioBaseConverter.h"

class vtkMatrix4x4;

/** Conversion between igtl::TransformMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT igtlioTransformConverter : public igtlioBaseConverter
{
public:
  /**
   * This structure contains everything that igtl::TransformMessage is able to contain,
   * in a vtk-friendly format.
   */
  struct ContentData
  {
  vtkSmartPointer<vtkMatrix4x4> transform;
  std::string deviceName;
  std::string streamIdTo;
  std::string streamIdFrom;
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); };
  static const char* GetIGTLTypeName() { return "TRANSFORM"; };

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::TransformMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

  static int IGTLToVTKTransform(const igtl::Matrix4x4& igtlTransform, vtkSmartPointer<vtkMatrix4x4> vtkTransform);
  static int VTKToIGTLTransform(const vtkMatrix4x4& vtkTransform, igtl::Matrix4x4& igtlTransform);

private:
  static int IGTLHeaderToTransformInfo(igtl::MessageBase::Pointer source, ContentData* dest);
  static int TransformMetaDataToIGTL(const ContentData& source, igtl::MessageBase::Pointer *dest);

};

#endif //IGTLIOTRANSFORMCONVERTER_H
