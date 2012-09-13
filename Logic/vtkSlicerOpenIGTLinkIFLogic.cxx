/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer4/trunk/Modules/OpenIGTLinkIF/vtkSlicerOpenIGTLinkIFLogic.cxx $
  Date:      $Date: 2011-06-12 14:55:20 -0400 (Sun, 12 Jun 2011) $
  Version:   $Revision: 16985 $

==========================================================================*/

// Slicer includes
#include <vtkSlicerColorLogic.h>

// OpenIGTLinkIF MRML includes
#include "vtkIGTLCircularBuffer.h"
#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLImageMetaListNode.h"
#include "vtkMRMLIGTLTrackingDataQueryNode.h"
#include "vtkMRMLIGTLTrackingDataBundleNode.h"

// OpenIGTLinkIF Logic includes
#include "vtkSlicerOpenIGTLinkIFLogic.h"

// MRML Logic includes
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLSliceLogic.h>

// VTK includes
#include <vtkNew.h>
#include <vtkCallbackCommand.h>
#include <vtkImageData.h>
#include <vtkTransform.h>

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkSlicerOpenIGTLinkIFLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkSlicerOpenIGTLinkIFLogic);

//---------------------------------------------------------------------------
vtkSlicerOpenIGTLinkIFLogic::vtkSlicerOpenIGTLinkIFLogic()
{
  // initialize slice driver information
  for (int i = 0; i < 3; i ++)
    {
    this->SliceDriver[i] = vtkSlicerOpenIGTLinkIFLogic::SLICE_DRIVER_USER;
    }

  //this->LocatorDriverFlag = 0;
  ////this->LocatorDriver = NULL;
  //this->LocatorDriverNodeID = "";
  this->RealTimeImageSourceNodeID = "";

  // If the following code doesn't work, slice nodes should be obtained from application GUI
  this->SliceNode[0] = NULL;
  this->SliceNode[1] = NULL;
  this->SliceNode[2] = NULL;

  this->SliceOrientation[0] = SLICE_RTIMAGE_PERP;
  this->SliceOrientation[1] = SLICE_RTIMAGE_INPLANE;
  this->SliceOrientation[2] = SLICE_RTIMAGE_INPLANE90;

  // Timer Handling
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkSlicerOpenIGTLinkIFLogic::DataCallback);

  this->EnableOblique = false;
  this->FreezePlane   = false;

  this->Initialized   = 0;
  this->RestrictDeviceName = 0;

  this->MessageConverterList.clear();

  // register default data types
  //this->LinearTransformConverter = vtkIGTLToMRMLLinearTransform::New();
  //this->ImageConverter           = vtkIGTLToMRMLImage::New();
  //this->PositionConverter        = vtkIGTLToMRMLPosition::New();
  //this->ImageMetaListConverter   = vtkIGTLToMRMLImageMetaList::New();
  //this->TrackingDataConverter    = vtkIGTLToMRMLTrackingData::New();
  //RegisterMessageConverter(this->LinearTransformConverter);
  //RegisterMessageConverter(this->ImageConverter);
  //RegisterMessageConverter(this->PositionConverter);
  //RegisterMessageConverter(this->ImageMetaListConverter);
  //RegisterMessageConverter(this->TrackingDataConverter);

  RegisterMessageConverter(vtkNew<vtkIGTLToMRMLLinearTransform>().GetPointer());
  RegisterMessageConverter(vtkNew<vtkIGTLToMRMLImage>().GetPointer());
  RegisterMessageConverter(vtkNew<vtkIGTLToMRMLPosition>().GetPointer());
  RegisterMessageConverter(vtkNew<vtkIGTLToMRMLImageMetaList>().GetPointer());
  RegisterMessageConverter(vtkNew<vtkIGTLToMRMLTrackingData>().GetPointer());

  //this->LocatorTransformNode = NULL;
}

//---------------------------------------------------------------------------
vtkSlicerOpenIGTLinkIFLogic::~vtkSlicerOpenIGTLinkIFLogic()
{
  std::cerr << "vtkSlicerOpenIGTLinkIFLogic::~vtkSlicerOpenIGTLinkIFLogic() is called." << std::endl;

  this->MessageConverterList.clear();

  //// Unregister all convertes in all connector nodes
  //std::vector<vtkMRMLNode*> nodes;
  //if (this->GetMRMLScene())
  //  {
  //  this->GetMRMLScene()->GetNodesByClass("vtkMRMLIGTLConnectorNode", nodes);
  //  std::vector<vtkMRMLNode*>::iterator iter;
  //  for (iter = nodes.begin(); iter != nodes.end(); iter ++)
  //    {
  //    vtkMRMLIGTLConnectorNode* connector = vtkMRMLIGTLConnectorNode::SafeDownCast(*iter);
  //    if (connector)
  //      {
  //      connector->UnregisterMessageConverterAll();
  //      }
  //    }
  //  }

}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "vtkSlicerOpenIGTLinkIFLogic:             " << this->GetClassName() << "\n";
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> sceneEvents;
  sceneEvents->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  sceneEvents->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);

  this->SetAndObserveMRMLSceneEventsInternal(newScene, sceneEvents.GetPointer());
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::RegisterNodes()
{
  vtkMRMLScene * scene = this->GetMRMLScene();
  if(!scene)
    {
    return;
   }
  scene->RegisterNodeClass(vtkNew<vtkMRMLIGTLConnectorNode>().GetPointer());
  scene->RegisterNodeClass(vtkNew<vtkMRMLImageMetaListNode>().GetPointer());
  scene->RegisterNodeClass(vtkNew<vtkMRMLIGTLTrackingDataQueryNode>().GetPointer());
  scene->RegisterNodeClass(vtkNew<vtkMRMLIGTLTrackingDataBundleNode>().GetPointer());
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::DataCallback(vtkObject *vtkNotUsed(caller),
                                               unsigned long vtkNotUsed(eid), void *clientData, void *vtkNotUsed(callData))
{
  vtkSlicerOpenIGTLinkIFLogic *self = reinterpret_cast<vtkSlicerOpenIGTLinkIFLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkSlicerOpenIGTLinkIFLogic DataCallback");
  self->UpdateAll();
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::UpdateAll()
{
}

//---------------------------------------------------------------------------

void vtkSlicerOpenIGTLinkIFLogic::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  //vtkDebugMacro("vtkSlicerOpenIGTLinkIFLogic::OnMRMLSceneNodeAdded");

  //vtkMRMLScene * scene = this->GetMRMLScene();
  //if (scene && scene->IsBatchProcessing())
  //  {
  //  return;
  //  }

  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (cnode)
    {
    // TODO Remove this line when the corresponding UI option will be added
    cnode->SetRestrictDeviceName(0);

    // Start observing the connector node
    vtkMRMLNode *node = NULL; // TODO: is this OK?
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(vtkMRMLIGTLConnectorNode::DeviceModifiedEvent);
    nodeEvents->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
    vtkSetAndObserveMRMLNodeEventsMacro(node,cnode,nodeEvents);
    nodeEvents->Delete();
    
    // Register converters
    unsigned int n = this->GetNumberOfConverters();
    for (unsigned short i = 0; i < n; i ++)
      {
      //bool ok = cnode->RegisterMessageConverter(this->GetConverter(i));
      cnode->RegisterMessageConverter(this->GetConverter(i));
      }
    }
}

//---------------------------------------------------------------------------
vtkMRMLIGTLConnectorNode* vtkSlicerOpenIGTLinkIFLogic::GetConnector(const char* conID)
{
  vtkMRMLNode* node = this->GetMRMLScene()->GetNodeByID(conID);
  if (node)
    {
    vtkMRMLIGTLConnectorNode* conNode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
    return conNode;
    }
  else
    {
    return NULL;
    }
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::ImportFromCircularBuffers()
{
  //ConnectorMapType::iterator cmiter;
  std::vector<vtkMRMLNode*> nodes;
  this->GetMRMLScene()->GetNodesByClass("vtkMRMLIGTLConnectorNode", nodes);

  std::vector<vtkMRMLNode*>::iterator iter;

  //for (cmiter = this->ConnectorMap.begin(); cmiter != this->ConnectorMap.end(); cmiter ++)
  for (iter = nodes.begin(); iter != nodes.end(); iter ++)
    {
    vtkMRMLIGTLConnectorNode* connector = vtkMRMLIGTLConnectorNode::SafeDownCast(*iter);
    if (connector == NULL)
      {
      continue;
      }

    connector->ImportDataFromCircularBuffer();
    }
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::ImportEvents()
{
  //ConnectorMapType::iterator cmiter;
  std::vector<vtkMRMLNode*> nodes;
  this->GetMRMLScene()->GetNodesByClass("vtkMRMLIGTLConnectorNode", nodes);

  std::vector<vtkMRMLNode*>::iterator iter;

  //for (cmiter = this->ConnectorMap.begin(); cmiter != this->ConnectorMap.end(); cmiter ++)
  for (iter = nodes.begin(); iter != nodes.end(); iter ++)
    {
    vtkMRMLIGTLConnectorNode* connector = vtkMRMLIGTLConnectorNode::SafeDownCast(*iter);
    if (connector == NULL)
      {
      continue;
      }

    connector->ImportEventsFromEventBuffer();
    }
}


//---------------------------------------------------------------------------
int vtkSlicerOpenIGTLinkIFLogic::SetRealTimeImageSource(const char* nodeID)
{
  vtkMRMLVolumeNode* volNode =
    vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));

  if (volNode && strcmp(volNode->GetNodeTagName(), "Volume") == 0)
    {
    // register the volume node in event observer
    vtkMRMLNode *node = NULL; // TODO: is this OK?
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent);
    vtkSetAndObserveMRMLNodeEventsMacro(node,volNode,nodeEvents);
    nodeEvents->Delete();
    this->RealTimeImageSourceNodeID = nodeID;
    return 1;
    }

  return 0;
}

//---------------------------------------------------------------------------
int vtkSlicerOpenIGTLinkIFLogic::SetSliceDriver(int index, int v)
{
  if (index < 0 || index >= 3)
    {
    return 0;
    }

  this->SliceDriver[index] = v;
  if (v == SLICE_DRIVER_LOCATOR)
    {
    //vtkMRMLLinearTransformNode* transNode =
    //  vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->LocatorDriverNodeID));
    //if (transNode)
    //  {
    //  vtkIntArray* nodeEvents = vtkIntArray::New();
    //  nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
    //  //vtkSetAndObserveMRMLNodeEventsMacro( this->LocatorTransformNode, transNode, nodeEvents);
    //  nodeEvents->Delete();
    //  transNode->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    //  }
    }

  return 1;
}

//---------------------------------------------------------------------------
int vtkSlicerOpenIGTLinkIFLogic::GetSliceDriver(int index)
{
  if (index < 0 || index >= 3)
    {
    return -1;
    }
  return this->SliceDriver[index];
}

//---------------------------------------------------------------------------
int vtkSlicerOpenIGTLinkIFLogic::SetRestrictDeviceName(int f)
{
  if (f != 0) f = 1; // make sure that f is either 0 or 1.
  this->RestrictDeviceName = f;

  std::vector<vtkMRMLNode*> nodes;
  this->GetMRMLScene()->GetNodesByClass("vtkMRMLIGTLConnectorNode", nodes);

  std::vector<vtkMRMLNode*>::iterator iter;

  for (iter = nodes.begin(); iter != nodes.end(); iter ++)
    {
    vtkMRMLIGTLConnectorNode* connector = vtkMRMLIGTLConnectorNode::SafeDownCast(*iter);
    if (connector)
      {
      connector->SetRestrictDeviceName(f);
      }
    }
  return this->RestrictDeviceName;
}

//---------------------------------------------------------------------------
int vtkSlicerOpenIGTLinkIFLogic::RegisterMessageConverter(vtkIGTLToMRMLBase* converter)
{
  if (converter == NULL)
    {
    return 0;
    }

  // Search the list and check if the same converter has already been registered.
  int found = 0;

  MessageConverterListType::iterator iter;
  for (iter = this->MessageConverterList.begin();
       iter != this->MessageConverterList.end();
       iter ++)
    {
    if ((converter->GetIGTLName() && strcmp(converter->GetIGTLName(), (*iter)->GetIGTLName()) == 0) &&
        (converter->GetMRMLName() && strcmp(converter->GetMRMLName(), (*iter)->GetMRMLName()) == 0))
      {
      found = 1;
      }
    }
  if (found)
    {
    return 0;
    }

  if (converter->GetIGTLName() && converter->GetMRMLName())
    // TODO: is this correct? Shouldn't it be "&&"
    {
    this->MessageConverterList.push_back(converter);
    converter->SetOpenIGTLinkIFLogic(this);
    }
  else
    {
    return 0;
    }

  // Add the converter to the existing connectors
  if (this->GetMRMLScene())
    {
    std::vector<vtkMRMLNode*> nodes;
  this->GetMRMLScene()->GetNodesByClass("vtkMRMLIGTLConnectorNode", nodes);

    std::vector<vtkMRMLNode*>::iterator niter;
    for (niter = nodes.begin(); niter != nodes.end(); niter ++)
      {
      vtkMRMLIGTLConnectorNode* connector = vtkMRMLIGTLConnectorNode::SafeDownCast(*niter);
      if (connector)
        {
        connector->RegisterMessageConverter(converter);
        }
      }
    }

  return 1;
}

//---------------------------------------------------------------------------
int vtkSlicerOpenIGTLinkIFLogic::UnregisterMessageConverter(vtkIGTLToMRMLBase* converter)
{
  if (converter == NULL)
    {
    return 0;
    }

  // Look up the message converter list
  MessageConverterListType::iterator iter;
  iter = this->MessageConverterList.begin();
  while ((*iter) != converter) iter ++;

  if (iter != this->MessageConverterList.end())
    // if the converter is on the list
    {
    this->MessageConverterList.erase(iter);
    // Remove the converter from the existing connectors
    std::vector<vtkMRMLNode*> nodes;
    if (this->GetMRMLScene())
      {
      this->GetMRMLScene()->GetNodesByClass("vtkMRMLIGTLConnectorNode", nodes);

      std::vector<vtkMRMLNode*>::iterator iter;
      for (iter = nodes.begin(); iter != nodes.end(); iter ++)
        {
        vtkMRMLIGTLConnectorNode* connector = vtkMRMLIGTLConnectorNode::SafeDownCast(*iter);
        if (connector)
          {
          connector->UnregisterMessageConverter(converter);
          }
        }
      }
    return 1;
    }
  else
    {
    return 0;
    }
}

//---------------------------------------------------------------------------
unsigned int vtkSlicerOpenIGTLinkIFLogic::GetNumberOfConverters()
{
  return this->MessageConverterList.size();
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLBase* vtkSlicerOpenIGTLinkIFLogic::GetConverter(unsigned int i)
{

  if (i < this->MessageConverterList.size())
    {
    return this->MessageConverterList[i];
    }
  else
    {
    return NULL;
    }
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLBase* vtkSlicerOpenIGTLinkIFLogic::GetConverterByMRMLTag(const char* mrmlTag)
{
  //Currently, this function cannot find multiple converters
  // that use the same mrmlType (e.g. vtkIGTLToMRMLLinearTransform
  // and vtkIGTLToMRMLPosition). A converter that is found first
  // will be returned.

  vtkIGTLToMRMLBase* converter = NULL;

  MessageConverterListType::iterator iter;
  for (iter = this->MessageConverterList.begin();
       iter != this->MessageConverterList.end();
       iter ++)
    {
    if (strcmp((*iter)->GetMRMLName(), mrmlTag) == 0)
      {
      converter = *iter;
      break;
      }
    }

  return converter;
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLBase* vtkSlicerOpenIGTLinkIFLogic::GetConverterByDeviceType(const char* deviceType)
{
  vtkIGTLToMRMLBase* converter = NULL;

  MessageConverterListType::iterator iter;
  for (iter = this->MessageConverterList.begin();
       iter != this->MessageConverterList.end();
       iter ++)
    {
    if ((*iter)->GetConverterType() == vtkIGTLToMRMLBase::TYPE_NORMAL)
      {
      if (strcmp((*iter)->GetIGTLName(), deviceType) == 0)
        {
        converter = *iter;
        break;
        }
      }
    else
      {
      int n = (*iter)->GetNumberOfIGTLNames();
      for (int i = 0; i < n; i ++)
        {
        if (strcmp((*iter)->GetIGTLName(i), deviceType) == 0)
          {
          converter = *iter;
          break;
          }
        }
      }
    }

  return converter;
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::ProcessMRMLSceneEvents(vtkObject* caller,unsigned long event, void * callData)
{
  if (event == vtkMRMLScene::NodeRemovedEvent)
    {
    vtkMRMLNode* node = (vtkMRMLNode*)callData;
    vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
    if (cnode)
      {
      vtkUnObserveMRMLNodeMacro(cnode);
      }
    }
}



//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::ProcessMRMLNodesEvents(vtkObject * caller, unsigned long event, void * callData)
{

  if (caller != NULL)
    {
    vtkSlicerModuleLogic::ProcessMRMLNodesEvents(caller, event, callData);

    vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(caller);
    if (cnode && event == vtkMRMLScene::NodeRemovedEvent)
      {
      vtkUnObserveMRMLNodeMacro(cnode);
      }
    else if (cnode && event == vtkMRMLIGTLConnectorNode::DeviceModifiedEvent)
      {
      // Check visibility
      int nnodes;

      // Incoming nodes
      nnodes = cnode->GetNumberOfIncomingMRMLNodes();
      for (int i = 0; i < nnodes; i ++)
        {
        vtkMRMLNode* inode = cnode->GetIncomingMRMLNode(i);
        if (inode)
          {
          vtkIGTLToMRMLBase* converter = GetConverterByMRMLTag(inode->GetNodeTagName());
          if (converter)
            {
            const char * attr = inode->GetAttribute("IGTLVisible");
            if (attr && strcmp(attr, "true") == 0)
              {
              converter->SetVisibility(1, this->GetMRMLScene(), inode);
              }
            else
              {
              converter->SetVisibility(0, this->GetMRMLScene(), inode);
              }
            }
          }
        }

      // Outgoing nodes
      nnodes = cnode->GetNumberOfOutgoingMRMLNodes();
      for (int i = 0; i < nnodes; i ++)
        {
        vtkMRMLNode* inode = cnode->GetOutgoingMRMLNode(i);
        if (inode)
          {
          vtkIGTLToMRMLBase* converter = GetConverterByMRMLTag(inode->GetNodeTagName());
          if (converter)
            {
            const char * attr = inode->GetAttribute("IGTLVisible");
            if (attr && strcmp(attr, "true") == 0)
              {
              converter->SetVisibility(1, this->GetMRMLScene(), inode);
              }
            else
              {
              converter->SetVisibility(0, this->GetMRMLScene(), inode);
              }
            }
          }
        }
      }
    }

    ////---------------------------------------------------------------------------
    //// Outgoing data
    //// TODO: should check the type of the node here
    //ConnectorListType* list = &MRMLEventConnectorMap[node];
    //ConnectorListType::iterator cliter;
    //for (cliter = list->begin(); cliter != list->end(); cliter ++)
    //  {
    //  vtkMRMLIGTLConnectorNode* connector =
    //    vtkMRMLIGTLConnectorNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(*cliter));
    //  if (connector == NULL)
    //    {
    //    return;
    //    }
    //
    //  MessageConverterListType::iterator iter;
    //  for (iter = this->MessageConverterList.begin();
    //       iter != this->MessageConverterList.end();
    //       iter ++)
    //    {
    //    if ((*iter)->GetMRMLName() && strcmp(node->GetNodeTagName(), (*iter)->GetMRMLName()) == 0)
    //      {
    //      // check if the name-type combination is on the list
    //      if (connector->GetDeviceID(node->GetName(), (*iter)->GetIGTLName()) >= 0)
    //        {
    //        int size;
    //        void* igtlMsg;
    //        (*iter)->MRMLToIGTL(event, node, &size, &igtlMsg);
    //        int r = connector->SendData(size, (unsigned char*)igtlMsg);
    //        if (r == 0)
    //          {
    //          // TODO: error handling
    //          //std::cerr << "ERROR: send data." << std::endl;
    //          }
    //        }
    //      }
    //    } // for (iter)
    //  } // for (cliter)

    ////---------------------------------------------------------------------------
    //// Slice Driven by Locator
    //if (node && strcmp(node->GetID(), this->LocatorDriverNodeID.c_str()) == 0)
    //  {
    //  vtkMatrix4x4* transform = NULL;
    //  for (int i = 0; i < 3; i ++)
    //    {
    //    if (this->SliceDriver[i] == SLICE_DRIVER_LOCATOR)
    //      {
    //      if (!transform)
    //        {
    //        vtkMRMLLinearTransformNode* transNode =
    //          vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->LocatorDriverNodeID));
    //        if (transNode)
    //          {
    //          transform = transNode->GetMatrixTransformToParent();
    //          }
    //        }
    //      if (transform)
    //        {
    //        UpdateSliceNode(i, transform);
    //        }
    //      }
    //    }
    //  }
    //
    //
    ////---------------------------------------------------------------------------
    //// Slice Driven by Real-time image
    //if (strcmp(node->GetID(), this->RealTimeImageSourceNodeID.c_str()) == 0)
    //  {
    //  for (int i = 0; i < 3; i ++)
    //    {
    //    if (this->SliceDriver[i] == SLICE_DRIVER_RTIMAGE)
    //      {
    //      UpdateSliceNodeByImage(i);
    //      }
    //    }
    //  }
    //}
}


//
//
////----------------------------------------------------------------------------
//void vtkSlicerOpenIGTLinkIFLogic::ProcessLogicEvents(vtkObject *vtkNotUsed(caller),
//                                            unsigned long event,
//                                            void *callData)
//{
//  if (event ==  vtkCommand::ProgressEvent)
//    {
//    this->InvokeEvent(vtkCommand::ProgressEvent,callData);
//    }
//}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::UpdateSliceNode(int sliceNodeNumber, vtkMatrix4x4* transform)
{
  // NOTES: In Slicer3 ver. 3.2 and higher, a slice orientation information in
  // a slice nodes classes are automaticall set to "Reformat", whenever
  // SetSliceToRASByNTP() function is called.
  // The OpenIGTLinkIF module saves the slice orientations in SliceOrientation[]
  // before the slice nodes update slice orientation information.

  if (this->FreezePlane)
    {
    return;
    }

  CheckSliceNode();

  float tx = transform->Element[0][0];
  float ty = transform->Element[1][0];
  float tz = transform->Element[2][0];
  /*
  float sx = transform->GetElement(0, 1);
  float sy = transform->GetElement(1, 1);
  float sz = transform->GetElement(2, 1);
  */
  float nx = transform->Element[0][2];
  float ny = transform->Element[1][2];
  float nz = transform->Element[2][2];
  float px = transform->Element[0][3];
  float py = transform->Element[1][3];
  float pz = transform->Element[2][3];

  if (strcmp(this->SliceNode[sliceNodeNumber]->GetOrientationString(), "Axial") == 0)
    {
    if (this->EnableOblique) // perpendicular
      {
      //this->SliceOrientation[sliceNodeNumber] = SLICE_RTIMAGE_PERP;
      this->SliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeNumber);
      }
    else
      {
      this->SliceNode[sliceNodeNumber]->SetOrientationToAxial();
      this->SliceNode[sliceNodeNumber]->JumpSlice(px, py, pz);
      }
    }
  else if (strcmp(this->SliceNode[sliceNodeNumber]->GetOrientationString(), "Sagittal") == 0)
    {
    if (this->EnableOblique) // In-Plane
      {
      //this->SliceOrientation[sliceNodeNumber] = SLICE_RTIMAGE_INPLANE;
      this->SliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeNumber);
      }
    else
      {
      this->SliceNode[sliceNodeNumber]->SetOrientationToSagittal();
      this->SliceNode[sliceNodeNumber]->JumpSlice(px, py, pz);
      }
    }
  else if (strcmp(this->SliceNode[sliceNodeNumber]->GetOrientationString(), "Coronal") == 0)
    {
    if (this->EnableOblique)  // In-Plane 90
      {
       //this->SliceOrientation[sliceNodeNumber] = SLICE_RTIMAGE_INPLANE90;
      this->SliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeNumber);
      }
    else
      {
      this->SliceNode[sliceNodeNumber]->SetOrientationToCoronal();
      this->SliceNode[sliceNodeNumber]->JumpSlice(px, py, pz);
      }
    }

  else if (strcmp(this->SliceNode[sliceNodeNumber]->GetOrientationString(), "Reformat") == 0)
    {
    if (this->EnableOblique)  // In-Plane 90
      {
      if (this->SliceOrientation[sliceNodeNumber] == SLICE_RTIMAGE_PERP)
        {
        this->SliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 0);
        }
      else if (this->SliceOrientation[sliceNodeNumber] == SLICE_RTIMAGE_INPLANE)
        {
        this->SliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 1);
        }
      else if (this->SliceOrientation[sliceNodeNumber] == SLICE_RTIMAGE_INPLANE90)
        {
        this->SliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 2);
        }
      }
    else
      {
      if (this->SliceOrientation[sliceNodeNumber] == SLICE_RTIMAGE_PERP)
        {
        this->SliceNode[sliceNodeNumber]->SetOrientationToAxial();
        }
      else if (this->SliceOrientation[sliceNodeNumber] == SLICE_RTIMAGE_INPLANE)
        {
        this->SliceNode[sliceNodeNumber]->SetOrientationToSagittal();
        }
      else if (this->SliceOrientation[sliceNodeNumber] == SLICE_RTIMAGE_INPLANE90)
        {
        this->SliceNode[sliceNodeNumber]->SetOrientationToCoronal();
        }

      this->SliceNode[sliceNodeNumber]->JumpSlice(px, py, pz);
      }
    }

  this->SliceNode[sliceNodeNumber]->UpdateMatrices();
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::UpdateSliceNodeByImage(int sliceNodeNumber)
{

  vtkMRMLVolumeNode* volumeNode =
    vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->RealTimeImageSourceNodeID));

  if (volumeNode == NULL)
    {
    return;
    }

  vtkMatrix4x4* rtimgTransform = vtkMatrix4x4::New();
  //volumeNode->GetRASToIJKMatrix(rtimgTransform);
  volumeNode->GetIJKToRASMatrix(rtimgTransform);
  //rtimgTransform->Invert();

  float tx = rtimgTransform->GetElement(0, 0);
  float ty = rtimgTransform->GetElement(1, 0);
  float tz = rtimgTransform->GetElement(2, 0);
  float sx = rtimgTransform->GetElement(0, 1);
  float sy = rtimgTransform->GetElement(1, 1);
  float sz = rtimgTransform->GetElement(2, 1);
  float nx = rtimgTransform->GetElement(0, 2);
  float ny = rtimgTransform->GetElement(1, 2);
  float nz = rtimgTransform->GetElement(2, 2);
  float px = rtimgTransform->GetElement(0, 3);
  float py = rtimgTransform->GetElement(1, 3);
  float pz = rtimgTransform->GetElement(2, 3);

  vtkImageData* imageData;
  imageData = volumeNode->GetImageData();
  int size[3];
  imageData->GetDimensions(size);

  // normalize
  float psi = sqrt(tx*tx + ty*ty + tz*tz);
  float psj = sqrt(sx*sx + sy*sy + sz*sz);
  float psk = sqrt(nx*nx + ny*ny + nz*nz);
  float ntx = tx / psi;
  float nty = ty / psi;
  float ntz = tz / psi;
  float nsx = sx / psj;
  float nsy = sy / psj;
  float nsz = sz / psj;
  float nnx = nx / psk;
  float nny = ny / psk;
  float nnz = nz / psk;

  // Shift the center
  // NOTE: The center of the image should be shifted due to different
  // definitions of image origin between VTK (Slicer) and OpenIGTLink;
  // OpenIGTLink image has its origin at the center, while VTK image
  // has one at the corner.

  float hfovi = psi * size[0] / 2.0;
  float hfovj = psj * size[1] / 2.0;
  //float hfovk = psk * imgheader->size[2] / 2.0;
  float hfovk = 0;

  float cx = ntx * hfovi + nsx * hfovj + nnx * hfovk;
  float cy = nty * hfovi + nsy * hfovj + nny * hfovk;
  float cz = ntz * hfovi + nsz * hfovj + nnz * hfovk;

  rtimgTransform->SetElement(0, 0, ntx);
  rtimgTransform->SetElement(1, 0, nty);
  rtimgTransform->SetElement(2, 0, ntz);
  rtimgTransform->SetElement(0, 1, nsx);
  rtimgTransform->SetElement(1, 1, nsy);
  rtimgTransform->SetElement(2, 1, nsz);
  rtimgTransform->SetElement(0, 2, nnx);
  rtimgTransform->SetElement(1, 2, nny);
  rtimgTransform->SetElement(2, 2, nnz);
  rtimgTransform->SetElement(0, 3, px + cx);
  rtimgTransform->SetElement(1, 3, py + cy);
  rtimgTransform->SetElement(2, 3, pz + cz);

  UpdateSliceNode(sliceNodeNumber, rtimgTransform);

  rtimgTransform->Delete();
  //volumeNode->Delete();
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::CheckSliceNode()
{

  /*
  if (this->SliceNode[0] == NULL)
    {
    this->SliceNode[0] = this->GetApplicationLogic()
      ->GetSliceLogic("Red")->GetSliceNode();
    }
  if (this->SliceNode[1] == NULL)
    {
    this->SliceNode[1] = this->GetApplicationLogic()
      ->GetSliceLogic("Yellow")->GetSliceNode();
    }
  if (this->SliceNode[2] == NULL)
    {
    this->SliceNode[2] = this->GetApplicationLogic()
      ->GetSliceLogic("Green")->GetSliceNode();
    }
  */
}


//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::ProcCommand(const char* vtkNotUsed(nodeName), int vtkNotUsed(size), unsigned char* vtkNotUsed(data))
{
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::GetDeviceNamesFromMrml(IGTLMrmlNodeListType &list)
{

  list.clear();

  MessageConverterListType::iterator mcliter;
  for (mcliter = this->MessageConverterList.begin();
       mcliter != this->MessageConverterList.end();
       mcliter ++)
    {
    if ((*mcliter)->GetMRMLName())
      {
      std::string className = this->GetMRMLScene()->GetClassNameByTag((*mcliter)->GetMRMLName());
      std::string deviceTypeName;
      if ((*mcliter)->GetIGTLName() != NULL)
        {
        deviceTypeName = (*mcliter)->GetIGTLName();
        }
      else
        {
        deviceTypeName = (*mcliter)->GetMRMLName();
        }
      std::vector<vtkMRMLNode*> nodes;
      this->GetApplicationLogic()->GetMRMLScene()->GetNodesByClass(className.c_str(), nodes);
      std::vector<vtkMRMLNode*>::iterator iter;
      for (iter = nodes.begin(); iter != nodes.end(); iter ++)
        {
        IGTLMrmlNodeInfoType nodeInfo;
        nodeInfo.name   = (*iter)->GetName();
        nodeInfo.type   = deviceTypeName.c_str();
        nodeInfo.io     = vtkMRMLIGTLConnectorNode::IO_UNSPECIFIED;
        nodeInfo.nodeID = (*iter)->GetID();
        list.push_back(nodeInfo);
        }
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkIFLogic::GetDeviceNamesFromMrml(IGTLMrmlNodeListType &list, const char* mrmlTagName)
{

  list.clear();

  MessageConverterListType::iterator mcliter;
  for (mcliter = this->MessageConverterList.begin();
       mcliter != this->MessageConverterList.end();
       mcliter ++)
    {
    if ((*mcliter)->GetMRMLName() && strcmp(mrmlTagName, (*mcliter)->GetMRMLName()) == 0)
      {
      const char* className = this->GetMRMLScene()->GetClassNameByTag(mrmlTagName);
      const char* deviceTypeName = (*mcliter)->GetIGTLName();
      std::vector<vtkMRMLNode*> nodes;
      this->GetMRMLScene()->GetNodesByClass(className, nodes);
      std::vector<vtkMRMLNode*>::iterator iter;
      for (iter = nodes.begin(); iter != nodes.end(); iter ++)
        {
        IGTLMrmlNodeInfoType nodeInfo;
        nodeInfo.name = (*iter)->GetName();
        nodeInfo.type = deviceTypeName;
        nodeInfo.io   = vtkMRMLIGTLConnectorNode::IO_UNSPECIFIED;
        nodeInfo.nodeID = (*iter)->GetID();
        list.push_back(nodeInfo);
        }
      }
    }
}

////---------------------------------------------------------------------------
//const char* vtkSlicerOpenIGTLinkIFLogic::MRMLTagToIGTLName(const char* mrmlTagName);
//{
//
//}
