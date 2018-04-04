/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLIOImageDevice.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

#include "igtlioImageDevice.h"

#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include "vtkMatrix4x4.h"

namespace igtlio
{

//---------------------------------------------------------------------------
DevicePointer ImageDeviceCreator::Create(std::string device_name)
{
 ImageDevicePointer retval = ImageDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string ImageDeviceCreator::GetDeviceType() const
{
  return ImageConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(ImageDeviceCreator);


//---------------------------------------------------------------------------
vtkStandardNewMacro(ImageDevice);
//---------------------------------------------------------------------------
ImageDevice::ImageDevice()
{
}

//---------------------------------------------------------------------------
ImageDevice::~ImageDevice()
{
}

//---------------------------------------------------------------------------
unsigned int ImageDevice::GetDeviceContentModifiedEvent() const
{
  return ImageModifiedEvent;
}
  
  
//---------------------------------------------------------------------------
std::string ImageDevice::GetDeviceType() const
{
  return ImageConverter::GetIGTLTypeName();
}

void ImageDevice::SetContent(ImageConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(ImageModifiedEvent, this);
}

ImageConverter::ContentData ImageDevice::GetContent()
{
  return Content;
}


//---------------------------------------------------------------------------
int ImageDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (ImageConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo))
   {
   this->Modified();
   this->InvokeEvent(ImageModifiedEvent, this);
   this->InvokeEvent(ReceiveEvent, NULL);
   return 1;
   }

 return 0;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer ImageDevice::GetIGTLMessage()
{
 if (!Content.image)
  {
  vtkWarningMacro("Image is NULL, message not generated.")
  return 0;
  }

 if (!ImageConverter::toIGTL(HeaderData, Content, &this->OutImageMessage, this->metaInfo))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutImageMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer ImageDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
	/*
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
  */
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<Device::MESSAGE_PREFIX> ImageDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

//---------------------------------------------------------------------------
void ImageDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  Device::PrintSelf(os, indent);

  os << indent << "Image:\t" <<"\n";
  Content.image->PrintSelf(os, indent.GetNextIndent());
  os << indent << "Transform:\t" << "\n";
  Content.transform->PrintSelf(os, indent.GetNextIndent());
}
} // namespace igtlio

