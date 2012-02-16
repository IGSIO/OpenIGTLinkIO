/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLBase.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __vtkIGTLToMRMLBase_h
#define __vtkIGTLToMRMLBase_h

// OpenIGTLinkIF MRML includes
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

// OpenIGTLink includes
#include <igtlMessageBase.h>

// MRML includes
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkObject.h>

// STD includes
#include <vector>
#include <string>

class vtkMRMLIGTLQueryNode;
class vtkSlicerOpenIGTLinkIFLogic;
class vtkIGTLToMRMLBasePrivate;

class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkIGTLToMRMLBase : public vtkObject
{

 public:

  // IGTL to MRML Converter types (returned values from GetConverterType())
  // NOTE: if you want to define a child class that can handle multiple types
  // of OpenIGTLink messages, override GetConverterType() method to return
  // TYPE_MULTI_IGTL_NAME.
  enum {
    TYPE_NORMAL,            // supports only single IGTL message type (default)
    TYPE_MULTI_IGTL_NAMES,  // supports multiple IGTL message names (device types) 
 };

 public:

  static vtkIGTLToMRMLBase *New();
  vtkTypeRevisionMacro(vtkIGTLToMRMLBase,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual int          GetConverterType() { return TYPE_NORMAL; };

  // IGTL Device / MRML Tag names
  virtual const char*  GetIGTLName()      { return NULL;};
  virtual const char*  GetMRMLName()      { return NULL;};

  // Following functions are implemented only if exists in OpenIGTLink specification
  virtual const char*  GetIGTLStartQueryName() { return NULL; };
  virtual const char*  GetIGTLStopQueryName()  { return NULL; };
  virtual const char*  GetIGTLGetQueryName()   { return NULL; };
  virtual const char*  GetIGTLStatusName()     { return NULL; };

  // Description:
  // GetNodeEvents() returns a list of events, which an IGTLConnector should react to.
  // The first element should be an event to export data, although multiple events can be defined.
  virtual vtkIntArray* GetNodeEvents()    { return NULL; };
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* vtkNotUsed(scene), const char* vtkNotUsed(name))
    { return NULL; };

  // for TYPE_MULTI_IGTL_NAMES
  int                  GetNumberOfIGTLNames()   { return this->IGTLNames.size(); };
  const char*          GetIGTLName(int index)   { return this->IGTLNames[index].c_str(); };

  // Description:
  // Functions to convert OpenIGTLink message to MRML node.
  // If mrmlNode is QueryNode, the function will generate query node. (event is not used.)
  virtual int          IGTLToMRML(igtl::MessageBase::Pointer vtkNotUsed(buffer),
                                  vtkMRMLNode* vtkNotUsed(node)) { return 0; };

  // Description:
  // Functions to generate an OpenIGTLink message
  // If mrmlNode is QueryNode, the function will generate query node. (event is not used.)
  virtual int          MRMLToIGTL(unsigned long vtkNotUsed(event), vtkMRMLNode* vtkNotUsed(mrmlNode),
                                  int* vtkNotUsed(size), void** vtkNotUsed(igtlMsg)){ return 0; };

  // Check query que (called periodically by timer)
  // (implemeted only if ncessary)
  virtual int CheckQueryQue(double vtkNotUsed(ctime)) { return true; }

  vtkGetMacro( CheckCRC, int );
  vtkSetMacro( CheckCRC, int );

  // Set/Get pointer to OpenIGTlinkIFLogic 
  void SetOpenIGTLinkIFLogic(vtkSlicerOpenIGTLinkIFLogic* logic);
  vtkSlicerOpenIGTLinkIFLogic* GetOpenIGTLinkIFLogic();

  // Visualization
  // If an MRML node for this converter type can be visualized,
  // the following functions must be implemented.
  virtual int IsVisible() { return 0; };
  virtual void SetVisibility(int vtkNotUsed(sw),
                             vtkMRMLScene * vtkNotUsed(scene),
                             vtkMRMLNode * vtkNotUsed(node)) {};

 protected:
  vtkIGTLToMRMLBase();
  ~vtkIGTLToMRMLBase();

 protected:

  // list of IGTL names (used only when the class supports multiple IGTL names)
  std::vector<std::string>  IGTLNames;

  int CheckCRC;

  vtkIGTLToMRMLBasePrivate* Private;
};


#endif //__vtkIGTLToMRMLBase_h
