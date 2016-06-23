/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLIOImageDevice.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

// OpenIGTLinkIF MRML includes
#include "vtkIGTLIOImageDevice.h"

// igtl support includes
#include <igtl_util.h>
#include <igtlImageMessage.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include "vtkMatrix4x4.h"

// VTKSYS includes
#include <vtksys/SystemTools.hxx>
#include "igtlImageConverter.h"

//---------------------------------------------------------------------------
vtkSmartPointer<vtkIGTLIODevice> vtkIGTLIOImageDeviceCreator::Create(std::string device_name)
{
 vtkSmartPointer<vtkIGTLIOImageDevice> retval = vtkSmartPointer<vtkIGTLIOImageDevice>::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOImageDeviceCreator::GetDeviceType() const
{
  return igtl::ImageConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOImageDeviceCreator);


//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOImageDevice);
//---------------------------------------------------------------------------
vtkIGTLIOImageDevice::vtkIGTLIOImageDevice()
{
   Converter = igtl::ImageConverter::New();
}

//---------------------------------------------------------------------------
vtkIGTLIOImageDevice::~vtkIGTLIOImageDevice()
{
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOImageDevice::GetDeviceType() const
{
  return igtl::ImageConverter::GetIGTLTypeName();
}

void vtkIGTLIOImageDevice::SetContent(igtl::ImageConverter::ContentData content)
{
  Content = content;
  this->Modified();
}

igtl::ImageConverter::ContentData vtkIGTLIOImageDevice::GetContent(igtl::ImageConverter::ContentData content)
{
  return Content;
}


//---------------------------------------------------------------------------
int vtkIGTLIOImageDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (Converter->fromIGTL(buffer, &HeaderData, &Content, checkCRC))
   {
   this->Modified();
   return 1;
   }

 return 0;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkIGTLIOImageDevice::GetIGTLMessage()
{
 if (!Content.image)
  {
  vtkWarningMacro("Image is NULL, message not generated.")
  return 0;
  }

 if (!Converter->toIGTL(HeaderData, Content, &this->OutImageMessage))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutImageMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkIGTLIOImageDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
 if (prefix==MESSAGE_PREFIX_GET)
  {
   if (this->GetImageMessage.IsNull())
     {
     this->GetImageMessage = igtl::GetImageMessage::New();
     }
   this->GetImageMessage->SetDeviceName(HeaderData.deviceName.c_str());
   this->GetImageMessage->Pack();
   return dynamic_pointer_cast<igtl::MessageBase>(this->GetImageMessage);
  }
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<vtkIGTLIODevice::MESSAGE_PREFIX> vtkIGTLIOImageDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_GET);
 return retval;
}

//---------------------------------------------------------------------------
void vtkIGTLIOImageDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkIGTLIODevice::PrintSelf(os, indent);

  os << indent << "Image:\t" <<"\n";
  Content.image->PrintSelf(os, indent.GetNextIndent());
  os << indent << "Transform:\t" << "\n";
  Content.transform->PrintSelf(os, indent.GetNextIndent());
}

