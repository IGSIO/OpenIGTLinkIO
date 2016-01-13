/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLImage.h"
#include "vtkMRMLIGTLQueryNode.h"

// igtl support includes
#include <igtl_util.h>
#include <igtlImageMessage.h>

// Slicer includes
//#include <vtkSlicerColorLogic.h>
#include <vtkMRMLColorLogic.h>
#include <vtkMRMLColorTableNode.h>

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLVectorVolumeDisplayNode.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>
#include "igtlImageConverter.h"

#include "vtkSlicerOpenIGTLinkIFLogic.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLToMRMLImage);
//---------------------------------------------------------------------------
vtkIGTLToMRMLImage::vtkIGTLToMRMLImage()
{
   Codec = igtl::ImageConverter::New();
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLImage::~vtkIGTLToMRMLImage()
{
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLImage::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLImage::SetDefaultDisplayNode(vtkMRMLVolumeNode *volumeNode, int numberOfComponents)
{
  if (volumeNode==NULL)
    {
    vtkWarningMacro("Failed to create display node for volume node");
    return;
    }

  vtkMRMLScene* scene=volumeNode->GetScene();
  if (scene==NULL)
    {
    vtkWarningMacro("Failed to create display node for "<<(volumeNode->GetID()?volumeNode->GetID():"unknown volume node")<<": scene is invalid");
    return;
    }
  if (numberOfComponents!=1 && numberOfComponents!=3)
    {
    vtkWarningMacro("Failed to create display node for "<<(volumeNode->GetID()?volumeNode->GetID():"unknown volume node")
      <<": only 1 or 3 scalar components are supported, received: "<<numberOfComponents);
    return;
    }

  // If the input is a 3-component image then we assume it is a color image,
  // and we display it in true color. For true color display we need to use
  // a vtkMRMLVectorVolumeDisplayNode.
  bool scalarDisplayNodeRequired = (numberOfComponents==1);
  vtkSmartPointer<vtkMRMLVolumeDisplayNode> displayNode;
  if (scalarDisplayNodeRequired)
    {
    displayNode = vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode>::New();
    }
  else
    {
    displayNode = vtkSmartPointer<vtkMRMLVectorVolumeDisplayNode>::New();
    }

  scene->AddNode(displayNode);

  if (scalarDisplayNodeRequired)
    {
    const char* colorTableId = vtkMRMLColorLogic::GetColorTableNodeID(vtkMRMLColorTableNode::Grey);
    displayNode->SetAndObserveColorNodeID(colorTableId);
    }
  else
    {
    displayNode->SetDefaultColorMap();
    }

  volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());

  vtkDebugMacro("Display node "<<displayNode->GetClassName());
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLImage::CreateNewNodeWithMessage(vtkMRMLScene* scene, const char* name, igtl::MessageBase::Pointer incomingImageMessage)
{
  igtl::MessageBase* innerPtr = incomingImageMessage.GetPointer();
  if( innerPtr == NULL )
    {
    vtkErrorMacro("Unable to create MRML node from incoming IMAGE message: incomingImageMessage is invalid");
    return 0;
    }

  igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
  // As this method is only called when the first image message is received,
  // (it is called when receiving each image message) the performance impact of the
  // additional Copy() and Unpack() method calls are negligible.
  imgMsg->Copy(incomingImageMessage.GetPointer());
  // Deserialize the transform data
  // If CheckCRC==0, CRC check is skipped.
  int c = imgMsg->Unpack(this->CheckCRC);
  if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
    {
    vtkErrorMacro("Unable to create MRML node from incoming IMAGE message. Failed to unpack the message");
    return 0;
    }

  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
  int numberOfComponents=imgMsg->GetNumComponents();
  //float fov = 256.0;
  image->SetDimensions(256, 256, 1);
  image->SetExtent(0, 255, 0, 255, 0, 0 );
  //image->SetSpacing( fov/256, fov/256, 10 );
  image->SetSpacing(1.0, 1.0, 1.0);
  //image->SetOrigin( fov/2, -fov/2, -0.0 );
  image->SetOrigin(0.0, 0.0, 0.0);
#if (VTK_MAJOR_VERSION <= 5)
  image->SetNumberOfScalarComponents(numberOfComponents);
  image->SetScalarTypeToShort();
  image->AllocateScalars();
#else
  image->AllocateScalars(VTK_SHORT, numberOfComponents);
#endif

  short* dest = (short*) image->GetScalarPointer();
  if (dest)
    {
    memset(dest, 0x00, 256*256*sizeof(short));
#if (VTK_MAJOR_VERSION <= 5)
    image->Update();
#endif
    }

  vtkSmartPointer<vtkMRMLVolumeNode> volumeNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
  volumeNode->SetAndObserveImageData(image);
  volumeNode->SetName(name);

  scene->SaveStateForUndo();

  vtkDebugMacro("Setting scene info");
  volumeNode->SetScene(scene);
  volumeNode->SetDescription("Received by OpenIGTLink");

  ///double range[2];
  vtkDebugMacro("Set basic display info");
  //volumeNode->GetImageData()->GetScalarRange(range);
  //range[0] = 0.0;
  //range[1] = 256.0;
  //displayNode->SetLowerThreshold(range[0]);
  //displayNode->SetUpperThreshold(range[1]);
  //displayNode->SetWindow(range[1] - range[0]);
  //displayNode->SetLevel(0.5 * (range[1] + range[0]) );

  vtkDebugMacro("Name vol node "<<volumeNode->GetClassName());
  vtkMRMLNode* n = scene->AddNode(volumeNode);

  this->SetDefaultDisplayNode(volumeNode, numberOfComponents);

  vtkDebugMacro("Node added to scene");

  this->CenterImage(volumeNode);

  return n;
}

//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLImage::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent);

  return events;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLImage::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
  vtkMRMLVolumeNode* volumeNode = vtkMRMLVolumeNode::SafeDownCast(node);
  if (volumeNode==NULL)
    {
    vtkErrorMacro("vtkIGTLToMRMLImage::IGTLToMRML failed: invalid node");
    return 0;
    }

  igtl::ImageConverter::MessageContent content;
  content.image = volumeNode->GetImageData(); // reuse existing imagedata if structure (dims, type, components) is identical

  if (Codec->IGTLToVTK(buffer, &content, this->CheckCRC) == 0)
    {
    return 0;
    }

  vtkSmartPointer<vtkImageData> nodeImageData = volumeNode->GetImageData();

  int numComponentsInNode = 0;
  int numComponents = 0;

  if (nodeImageData)
    numComponentsInNode = nodeImageData->GetNumberOfScalarComponents();
  numComponents = content.image->GetNumberOfScalarComponents();

  if (numComponentsInNode != numComponents)
    {
    // number of components changed, so we need to remove the incompatible
    // dispay nodes and create a default display node if no compatible display
    // node remains

    bool scalarDisplayNodeRequired = (numComponents==1);
    bool mayNeedToRemoveDisplayNodes=false;
    do
      {
      mayNeedToRemoveDisplayNodes=false;
      for (int i=0; i<volumeNode->GetNumberOfDisplayNodes(); i++)
        {
        vtkMRMLVolumeDisplayNode* currentDisplayNode = vtkMRMLVolumeDisplayNode::SafeDownCast(volumeNode->GetNthDisplayNode(i));
        bool currentDisplayNodeIsScalar = (vtkMRMLVectorVolumeDisplayNode::SafeDownCast(currentDisplayNode)==NULL);
        if (scalarDisplayNodeRequired!=currentDisplayNodeIsScalar)
          {
          // incompatible display node, remove it
          //volumeNode->RemoveNthDisplayNodeID(i);
          volumeNode->GetScene()->RemoveNode(currentDisplayNode);
          mayNeedToRemoveDisplayNodes=true;
          }
        }
      }
    while (mayNeedToRemoveDisplayNodes);

    if (volumeNode->GetNumberOfDisplayNodes()==0)
      {
      // the new default display node may be incompatible with the current image data,
      // so clear it to make sure no display is attempted with incompatible image data
      volumeNode->SetAndObserveImageData(NULL);
      SetDefaultDisplayNode(volumeNode, numComponents);
      }
    }

  // insert igtl transform and image into MRML:
  volumeNode->SetIJKToRASMatrix(content.transform);
  volumeNode->SetAndObserveImageData(content.image);
  volumeNode->Modified();

  return 1;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLImage::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (!mrmlNode)
    {
    return 0;
    }

  // If mrmlNode is Image node
  if (event == vtkMRMLVolumeNode::ImageDataModifiedEvent && strcmp(mrmlNode->GetNodeTagName(), "Volume") == 0)
    {
    vtkMRMLScalarVolumeNode* volumeNode =
        vtkMRMLScalarVolumeNode::SafeDownCast(mrmlNode);

    if (!volumeNode)
      {
      return 0;
      }

    igtl::ImageConverter::MessageContent content;
    content.image = volumeNode->GetImageData();

    if (Codec->VTKToIGTL(content, &this->OutImageMessage) == 0)
      {
      return 0;
      }

    *size = this->OutImageMessage->GetPackSize();
    *igtlMsg = (void*)this->OutImageMessage->GetPackPointer();

    return 1;
    }
  else if (strcmp(mrmlNode->GetNodeTagName(), "IGTLQuery") == 0)   // If mrmlNode is query node
    {
    vtkMRMLIGTLQueryNode* qnode = vtkMRMLIGTLQueryNode::SafeDownCast(mrmlNode);
    if (qnode)
      {
      if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_GET)
        {
        if (this->GetImageMessage.IsNull())
          {
          this->GetImageMessage = igtl::GetImageMessage::New();
          }
        this->GetImageMessage->SetDeviceName(qnode->GetIGTLDeviceName());
        this->GetImageMessage->Pack();
        *size = this->GetImageMessage->GetPackSize();
        *igtlMsg = this->GetImageMessage->GetPackPointer();
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

//---------------------------------------------------------------------------
void vtkIGTLToMRMLImage::CenterImage(vtkMRMLVolumeNode *volumeNode)
{
  if ( volumeNode )
    {
    vtkImageData *image = volumeNode->GetImageData();
    if (image)
      {
      vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
      volumeNode->GetIJKToRASMatrix(ijkToRAS);

      double dimsH[4];
      double rasCorner[4];
      int *dims = image->GetDimensions();
      dimsH[0] = dims[0] - 1;
      dimsH[1] = dims[1] - 1;
      dimsH[2] = dims[2] - 1;
      dimsH[3] = 0.;
      ijkToRAS->MultiplyPoint(dimsH, rasCorner);

      double origin[3];
      int i;
      for (i = 0; i < 3; i++)
        {
        origin[i] = -0.5 * rasCorner[i];
        }
      volumeNode->SetDisableModifiedEvent(1);
      volumeNode->SetOrigin(origin);
      volumeNode->SetDisableModifiedEvent(0);
      volumeNode->InvokePendingModifiedEvent();

      ijkToRAS->Delete();
      }
    }
}
