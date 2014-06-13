/*==========================================================================

  Portions (c) Copyright 2008-2014 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

==========================================================================*/

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLStatus.h"

// OpenIGTLink includes
#include <igtlStatusMessage.h>
#include <igtlWin32Header.h>

// MRML includes
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLIGTLStatusNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLToMRMLStatus);

//---------------------------------------------------------------------------
vtkIGTLToMRMLStatus::vtkIGTLToMRMLStatus()
{
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLStatus::~vtkIGTLToMRMLStatus()
{
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLStatus::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLStatus::CreateNewNode(vtkMRMLScene* scene, const char* name)
{
  vtkMRMLIGTLStatusNode* statusNode;

  statusNode = vtkMRMLIGTLStatusNode::New();
  statusNode->SetName(name);
  statusNode->SetDescription("Received by OpenIGTLink");

  vtkMRMLNode* n = scene->AddNode(statusNode);
  statusNode->Delete();

  return n;
}

//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLStatus::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLIGTLStatusNode::StatusModifiedEvent);

  return events;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLStatus::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
  vtkIGTLToMRMLBase::IGTLToMRML(buffer, node);

  // Create a message buffer to receive transform data
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->Copy(buffer);  // !! TODO: copy makes performance issue.

  // Deserialize the transform data
  // If CheckCRC==0, CRC check is skipped.
  int c = statusMsg->Unpack(this->CheckCRC);

  if (!(c & igtl::MessageHeader::UNPACK_BODY)) // if CRC check fails
    {
    // TODO: error handling
    return 0;
    }

  if (node == NULL)
    {
    return 0;
    }

  vtkMRMLIGTLStatusNode* statusNode =
    vtkMRMLIGTLStatusNode::SafeDownCast(node);

  statusNode->SetStatus(statusMsg->GetCode(), statusMsg->GetSubCode(),
                        statusMsg->GetErrorName(), statusMsg->GetStatusString());

  return 1;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLStatus::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (mrmlNode && event == vtkMRMLIGTLStatusNode::StatusModifiedEvent)
    {
    vtkMRMLIGTLStatusNode* statusNode =
      vtkMRMLIGTLStatusNode::SafeDownCast(mrmlNode);

    if (!statusNode)
      {
      return 0;
      }

    //igtl::StatusMessage::Pointer OutStatusMsg;
    if (this->OutStatusMsg.IsNull())
      {
      this->OutStatusMsg = igtl::StatusMessage::New();
      }

    this->OutStatusMsg->SetDeviceName(statusNode->GetName());
    this->OutStatusMsg->SetCode(statusNode->GetCode());
    this->OutStatusMsg->SetSubCode(statusNode->GetSubCode());
    this->OutStatusMsg->SetErrorName(statusNode->GetErrorName());
    this->OutStatusMsg->SetStatusString(statusNode->GetStatusString());
    this->OutStatusMsg->Pack();

    *size = this->OutStatusMsg->GetPackSize();
    *igtlMsg = (void*)this->OutStatusMsg->GetPackPointer();

    return 1;
    }

  return 0;
}


