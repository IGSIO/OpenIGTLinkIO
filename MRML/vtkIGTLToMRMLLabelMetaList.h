/*==========================================================================

  Portions (c) Copyright 2008-2013 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

==========================================================================*/

#ifndef __vtkIGTLToMRMLLabelMetaList_h
#define __vtkIGTLToMRMLLabelMetaList_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

// OpenIGTLink includes
#include "igtlLabelMetaMessage.h"

// MRML includes
#include <vtkMRMLNode.h>

class vtkMRMLVolumeNode;

class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkIGTLToMRMLLabelMetaList : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLLabelMetaList *New();
  vtkTypeRevisionMacro(vtkIGTLToMRMLLabelMetaList,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "LBMETA"; };
  virtual const char*  GetMRMLName() { return "LabelMetaList"; };

  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);


 protected:
  vtkIGTLToMRMLLabelMetaList();
  ~vtkIGTLToMRMLLabelMetaList();

  void CenterLabel(vtkMRMLVolumeNode *volumeNode);

 protected:

  //igtl::TransformMessage::Pointer OutTransformMsg;
  igtl::LabelMetaMessage::Pointer OutLabelMetaMsg;
  igtl::GetLabelMetaMessage::Pointer GetLabelMetaMessage;

};


#endif //__vtkIGTLToMRMLLabelMetaList_h
