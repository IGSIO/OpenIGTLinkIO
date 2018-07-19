/*==========================================================================

Portions (c) Copyright 2018 IGSIO

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   igtlio
Module:    $RCSfile: igtlioConverterUtilities.h,v $
Date:      $Date: 2018/07/20 4:08:00 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef IGTLIOCONVERTERUTILITIES_H
#define IGTLIOCONVERTERUTILITIES_H

#include "igtlioConverterExport.h"

// OpenIGTLink includes
#include <igtlMath.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>


class OPENIGTLINKIO_CONVERTER_EXPORT igtlioConverterUtilities
{
public:

  static void VTKTransformToIGTLTransform(vtkSmartPointer<vtkMatrix4x4> ijk2ras, const int imageSize[3], const double spacing[3], igtl::Matrix4x4 &matrix);
  static void IGTLTransformToVTKTransform(const int imageSize[3], const float spacing[3], const igtl::Matrix4x4 matrix, vtkSmartPointer<vtkMatrix4x4> ijk2ras);

};

#endif // IGTLIOCONVERTERUTILITIES_H
