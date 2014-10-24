/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLPoint.h $
  Date:      $Date: 2009-08-12 21:30:38 -0400 (Wed, 12 Aug 2009) $
  Version:   $Revision: 10236 $

==========================================================================*/

#ifndef __vtkIGTLToMRMLPoint_h
#define __vtkIGTLToMRMLPoint_h

#include "vtkMRMLNode.h"
#include "vtkIGTLToMRMLBase.h"

#include "igtlPointMessage.h"

class vtkMRMLVolumeNode;

class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkIGTLToMRMLPoint : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLPoint *New();
  vtkTypeMacro(vtkIGTLToMRMLPoint,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "POINT"; };
  virtual const char*  GetMRMLName() { return "MarkupsFiducial"; };

  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  //BTX
  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  //ETX
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);


 protected:
  vtkIGTLToMRMLPoint();
  ~vtkIGTLToMRMLPoint();

  void CenterImage(vtkMRMLVolumeNode *volumeNode);

 protected:
  //BTX
  igtl::PointMessage::Pointer      PointMsg;
  igtl::GetPointMessage::Pointer   GetPointMsg;
  //ETX
  
};


#endif //__vtkIGTLToMRMLPoint_h
