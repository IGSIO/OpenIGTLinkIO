/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer4/trunk/Modules/OpenIGTLinkIF/vtkSlicerOpenIGTLinkIFLogic.h $
  Date:      $Date: 2010-06-10 11:05:22 -0400 (Thu, 10 Jun 2010) $
  Version:   $Revision: 13728 $

==========================================================================*/

/// This class manages the logic associated with tracking device for IGT.

#ifndef __vtkSlicerOpenIGTLinkIFLogic_h
#define __vtkSlicerOpenIGTLinkIFLogic_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkIGTLToMRMLLinearTransform.h"
#include "vtkIGTLToMRMLImage.h"
#include "vtkIGTLToMRMLPosition.h"
#include "vtkIGTLToMRMLImageMetaList.h"
#include "vtkIGTLToMRMLTrackingData.h"
#include "vtkSlicerOpenIGTLinkIFModuleLogicExport.h"

// OpenIGTLink includes
#include <igtlImageMessage.h>
#include <igtlTransformMessage.h>

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkCallbackCommand.h"

// MRML includes
#include <vtkMRMLTransformNode.h>
#include <vtkMRMLFiducialListNode.h>
#include <vtkMRMLSliceNode.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkMultiThreader.h>

// STD includes
#include <vector>

class vtkMRMLIGTLConnectorNode;
class vtkMRMLModelNode;
class vtkTransform;

/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class VTK_SLICER_OPENIGTLINKIF_MODULE_LOGIC_EXPORT vtkSlicerOpenIGTLinkIFLogic : public vtkSlicerModuleLogic
{
 public:

  enum {  // Events
    StatusUpdateEvent       = 50001,
  };

  typedef struct {
    std::string name;
    std::string type;
    int io;
    std::string nodeID;
  } IGTLMrmlNodeInfoType;

  typedef std::vector<IGTLMrmlNodeInfoType>         IGTLMrmlNodeListType;
  typedef std::vector< vtkSmartPointer<vtkIGTLToMRMLBase> >           MessageConverterListType;

  // Work phase keywords used in NaviTrack (defined in BRPTPRInterface.h)

 public:

  static vtkSlicerOpenIGTLinkIFLogic *New();
  vtkTypeRevisionMacro(vtkSlicerOpenIGTLinkIFLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream&, vtkIndent);

  /// The selected transform node is observed for TransformModified events and the transform
  /// data is copied to the slice nodes depending on the current mode

  virtual void SetMRMLSceneInternal(vtkMRMLScene * newScene);
  virtual void RegisterNodes();

  //----------------------------------------------------------------
  // Events
  //----------------------------------------------------------------

  //virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* /*node*/);
  virtual void OnMRMLNodeModified(vtkMRMLNode* /*node*/){}

  //----------------------------------------------------------------
  // Connector and converter Management
  //----------------------------------------------------------------

  // Access connectors
  vtkMRMLIGTLConnectorNode* GetConnector(const char* conID);
  void                      ImportFromCircularBuffers();
  void                      ImportEvents(); // check if there are any events in the connectors that should be invoked in the main thread (such as connected, disconnected)

  // Device Name management
  int  SetRestrictDeviceName(int f);

  int  RegisterMessageConverter(vtkIGTLToMRMLBase* converter);
  int  UnregisterMessageConverter(vtkIGTLToMRMLBase* converter);

  unsigned int       GetNumberOfConverters();
  vtkIGTLToMRMLBase* GetConverter(unsigned int i);
  vtkIGTLToMRMLBase* GetConverterByMRMLTag(const char* mrmlTag);
  vtkIGTLToMRMLBase* GetConverterByDeviceType(const char* deviceType);

  //----------------------------------------------------------------
  // MRML Management
  //----------------------------------------------------------------

  virtual void ProcessMRMLSceneEvents(vtkObject* caller,unsigned long event, void * callData);
  virtual void ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void * callData);
  //virtual void ProcessLogicEvents(vtkObject * caller, unsigned long event, void * callData);

  void GetDeviceNamesFromMrml(IGTLMrmlNodeListType &list);
  void GetDeviceNamesFromMrml(IGTLMrmlNodeListType &list, const char* mrmlTagName);
  //void GetDeviceTypes(std::vector<char*> &list);

 protected:

  //----------------------------------------------------------------
  // Constructor, destructor etc.
  //----------------------------------------------------------------

  vtkSlicerOpenIGTLinkIFLogic();
  virtual ~vtkSlicerOpenIGTLinkIFLogic();

  static void DataCallback(vtkObject*, unsigned long, void *, void *);

  void UpdateAll();
  vtkCallbackCommand *DataCallbackCommand;

 private:

  int Initialized;

  //----------------------------------------------------------------
  // Connector Management
  //----------------------------------------------------------------

  //ConnectorMapType              ConnectorMap;
  MessageConverterListType      MessageConverterList;

  //int LastConnectorID;
  int RestrictDeviceName;

  //----------------------------------------------------------------
  // Real-time image
  //----------------------------------------------------------------

private:

  vtkSlicerOpenIGTLinkIFLogic(const vtkSlicerOpenIGTLinkIFLogic&); // Not implemented
  void operator=(const vtkSlicerOpenIGTLinkIFLogic&);               // Not implemented
};

#endif
