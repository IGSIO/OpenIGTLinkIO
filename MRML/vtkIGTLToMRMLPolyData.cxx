/*==========================================================================

  Portions (c) Copyright 2008-2014 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    vtkIGTLToMRMLPolyData.cxx

==========================================================================*/

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLPolyData.h"
#include "vtkMRMLIGTLQueryNode.h"

// OpenIGTLink includes
#include <igtl_util.h>
#include <igtlPolyDataMessage.h>

// Slicer includes
//#include <vtkSlicerColorLogic.h>
#include <vtkMRMLColorLogic.h>
#include <vtkMRMLColorTableNode.h>

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLVectorVolumeDisplayNode.h>

// VTK includes
#include <vtkPolyData.h>
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkVertex.h>
#include <vtkCellArray.h>
#include <vtkPolyLine.h>
#include <vtkPolygon.h>
#include <vtkTriangleStrip.h>
#include <vtkFloatArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkPointData.h>
#include <vtkCellData.h>


// VTKSYS includes
#include <vtksys/SystemTools.hxx>

#include "vtkSlicerOpenIGTLinkIFLogic.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLToMRMLPolyData);
//---------------------------------------------------------------------------
vtkIGTLToMRMLPolyData::vtkIGTLToMRMLPolyData()
{
  Converter = igtl::PolyDataConverter::New();
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLPolyData::~vtkIGTLToMRMLPolyData()
{
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLPolyData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}


//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLPolyData::CreateNewNodeWithMessage(vtkMRMLScene* scene, const char* name, igtl::MessageBase::Pointer incomingPolyDataMessage)
{
  igtl::MessageBase* innerPtr = incomingPolyDataMessage.GetPointer();
  if( innerPtr == NULL )
    {
    vtkErrorMacro("Unable to create MRML node from incoming POLYDATA message: incoming PolyDataMessage is invalid");
    return 0;
    }


  // Create a node
  vtkSmartPointer<vtkMRMLModelNode> modelNode = vtkSmartPointer<vtkMRMLModelNode>::New();
  modelNode->SetName(name);

  scene->SaveStateForUndo();

  vtkDebugMacro("Setting scene info");
  modelNode->SetScene(scene);
  modelNode->SetDescription("Received by OpenIGTLink");

  // Display Node
  vtkSmartPointer< vtkMRMLModelDisplayNode > displayNode = vtkSmartPointer< vtkMRMLModelDisplayNode >::New();

  double color[3];
  color[0] = 0.5;
  color[1] = 0.5;
  color[2] = 1.0;
  displayNode->SetColor(color);
  displayNode->SetOpacity(1.0);

  displayNode->SliceIntersectionVisibilityOn();  
  displayNode->VisibilityOn();

  scene->SaveStateForUndo();
  scene->AddNode(modelNode);
  scene->AddNode(displayNode);

  displayNode->SetScene(scene);
  modelNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  modelNode->SetHideFromEditors(0);
  
  return modelNode;
}


//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLPolyData::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLModelNode::PolyDataModifiedEvent);

  return events;
}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLPolyData::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{

  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(node);

  if (modelNode==NULL)
    {
    vtkErrorMacro("vtkIGTLToMRMLPolyData::IGTLToMRML failed: invalid node");
    return 0;
    }

  igtl::PolyDataConverter::MessageContent content;
  if (Converter->IGTLToVTK(buffer, &content, this->CheckCRC) == 0)
    {
    return 0;
    }

  modelNode->SetAndObservePolyData(content.polydata);
  modelNode->Modified();

  return 1;
}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLPolyData::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (!mrmlNode)
    {
    return 0;
    }

  // If mrmlNode is PolyData node
  if (event == vtkMRMLModelNode::PolyDataModifiedEvent && strcmp(mrmlNode->GetNodeTagName(), "Model") == 0)
    {
    vtkMRMLModelNode* modelNode =
      vtkMRMLModelNode::SafeDownCast(mrmlNode);

    if (!modelNode)
      {
      // TODO: the node not found
      return 0;
      }
    
    igtl::PolyDataConverter::MessageContent content;
    content.polydata = modelNode->GetPolyData();
    content.deviceName = modelNode->GetName();

    if (Converter->VTKToIGTL(content, &this->OutPolyDataMessage) == 0)
      {
      return 0;
      }

    *size = this->OutPolyDataMessage->GetPackSize();
    *igtlMsg = (void*)this->OutPolyDataMessage->GetPackPointer();

    return 1;
    }
  else if (strcmp(mrmlNode->GetNodeTagName(), "IGTLQuery") == 0)   // If mrmlNode is query node
    {
    vtkMRMLIGTLQueryNode* qnode = vtkMRMLIGTLQueryNode::SafeDownCast(mrmlNode);
    if (qnode)
      {
      if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_GET)
        {
        if (this->GetPolyDataMessage.IsNull())
          {
          this->GetPolyDataMessage = igtl::GetPolyDataMessage::New();
          }
        this->GetPolyDataMessage->SetDeviceName(qnode->GetIGTLDeviceName());
        this->GetPolyDataMessage->Pack();
        *size = this->GetPolyDataMessage->GetPackSize();
        *igtlMsg = this->GetPolyDataMessage->GetPackPointer();
        return 1;
        }
      /*
      else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_START)
        {
        *size = 0;
        return 0;
        }
      else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_STOP)
        {
        *size = 0;
        return 0;
        }
      */
      return 0;
      }
    }
  else
    {
    return 0;
    }

  return 0;
}



