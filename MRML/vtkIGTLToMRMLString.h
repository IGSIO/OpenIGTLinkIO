/*==========================================================================

  Portions (c) Copyright 2008-2014 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Contributor: Tamas Ungi at Queen's University.

==========================================================================*/

#ifndef __vtkIGTLToMRMLString_h
#define __vtkIGTLToMRMLString_h

#include "vtkObject.h"
#include "vtkMRMLNode.h"
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

#include "igtlStringMessage.h"

class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkIGTLToMRMLString : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLString *New();
  vtkTypeMacro(vtkIGTLToMRMLString,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual int          GetConverterType() { return TYPE_NORMAL; };
  virtual const char*  GetIGTLName() { return "STRING"; };
  virtual const char*  GetMRMLName() { return "Text"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  // for TYPE_MULTI_IGTL_NAMES
  int                  GetNumberOfIGTLNames()   { return this->IGTLNames.size(); };
  const char*          GetIGTLName(int index)   { return this->IGTLNames[index].c_str(); };

  //BTX
  virtual int          IGTLToMRML( igtl::MessageBase::Pointer buffer, vtkMRMLNode* node );
  //ETX
  virtual int          MRMLToIGTL( unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg );

 
 protected:
  vtkIGTLToMRMLString();
  ~vtkIGTLToMRMLString();

 protected:
  //BTX
  igtl::StringMessage::Pointer StringMsg;
  //ETX
  
};


#endif
