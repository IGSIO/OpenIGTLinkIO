/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __vtkIGTLToMRMLImage_h
#define __vtkIGTLToMRMLImage_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"


// OpenIGTLink includes
#include <igtlImageMessage.h>
#include <vtkMRMLNode.h>
#include <vtkObject.h>
#include "igtlImageConverter.h"

// VTK includes

class vtkMRMLVolumeNode;
class vtkImageData;

class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkIGTLToMRMLImage : public vtkIGTLToMRMLBase
{
 public:
  static vtkIGTLToMRMLImage *New();
  vtkTypeMacro(vtkIGTLToMRMLImage,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "IMAGE"; };
  virtual const char*  GetMRMLName() { return "Volume"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNodeWithMessage(vtkMRMLScene* scene, const char* name, igtl::MessageBase::Pointer incomingImageMessage);

  virtual int IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  virtual int MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);

 protected:
  vtkIGTLToMRMLImage();
  ~vtkIGTLToMRMLImage();

  void SetDefaultDisplayNode(vtkMRMLVolumeNode *volumeNode, int numberOfComponents);
  void CenterImage(vtkMRMLVolumeNode *volumeNode);

 protected:
  igtl::ImageMessage::Pointer OutImageMessage;

  igtl::GetImageMessage::Pointer GetImageMessage;

  igtl::ImageConverter::Pointer Converter;
};


#endif //__vtkIGTLToMRMLImage_h
