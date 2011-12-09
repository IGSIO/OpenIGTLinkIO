/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImageMetaList.h $
  Date:      $Date: 2009-08-12 21:30:38 -0400 (Wed, 12 Aug 2009) $
  Version:   $Revision: 10236 $

==========================================================================*/

#ifndef __vtkIGTLToMRMLImageMetaList_h
#define __vtkIGTLToMRMLImageMetaList_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

// OpenIGTLink includes
#ifdef OpenIGTLinkIF_USE_VERSION_2
# include "igtlImageMetaMessage.h"
#endif

// MRML includes
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkObject.h>

class vtkMRMLVolumeNode;

class Q_SLICER_QTMODULES_OPENIGTLINKIF_MRML_EXPORT vtkIGTLToMRMLImageMetaList : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLImageMetaList *New();
  vtkTypeRevisionMacro(vtkIGTLToMRMLImageMetaList,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "IMGMETA"; };
  virtual const char*  GetMRMLName() { return "ImageMetaList"; };

  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  //BTX
  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  //ETX
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);


 protected:
  vtkIGTLToMRMLImageMetaList();
  ~vtkIGTLToMRMLImageMetaList();

  void CenterImage(vtkMRMLVolumeNode *volumeNode);

 protected:

#ifdef OpenIGTLinkIF_USE_VERSION_2
  //BTX
  //igtl::TransformMessage::Pointer OutTransformMsg;
  igtl::ImageMetaMessage::Pointer OutImageMetaMsg;
  igtl::GetImageMetaMessage::Pointer GetImageMetaMessage;
  //ETX
#endif // OpenIGTLinkIF_USE_VERSION_2

};


#endif //__vtkIGTLToMRMLImageMetaList_h
