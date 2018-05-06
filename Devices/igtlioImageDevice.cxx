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

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioImageDeviceCreator::Create(std::string device_name)
{
 igtlioImageDevicePointer retval = igtlioImageDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string igtlioImageDeviceCreator::GetDeviceType() const
{
  return igtlioImageConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioImageDeviceCreator);


//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioImageDevice);
//---------------------------------------------------------------------------
igtlioImageDevice::igtlioImageDevice()
{
}

//---------------------------------------------------------------------------
igtlioImageDevice::~igtlioImageDevice()
{
}

//---------------------------------------------------------------------------
unsigned int igtlioImageDevice::GetDeviceContentModifiedEvent() const
{
  return ImageModifiedEvent;
}

//---------------------------------------------------------------------------
std::string igtlioImageDevice::GetDeviceType() const
{
  return igtlioImageConverter::GetIGTLTypeName();
}

void igtlioImageDevice::SetContent(igtlioImageConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(ImageModifiedEvent, this);
}

igtlioImageConverter::ContentData igtlioImageDevice::GetContent()
{
  return Content;
}


//---------------------------------------------------------------------------
int igtlioImageDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (igtlioImageConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo))
   {
   this->Modified();
   this->InvokeEvent(ImageModifiedEvent, this);
   this->InvokeEvent(ReceiveEvent, NULL);
   return 1;
   }

 return 0;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioImageDevice::GetIGTLMessage()
{
 if (!Content.image)
  {
  vtkWarningMacro("Image is NULL, message not generated.")
  return 0;
  }

 if (!igtlioImageConverter::toIGTL(HeaderData, Content, &this->OutImageMessage, this->metaInfo))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutImageMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioImageDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
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
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioImageDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

//---------------------------------------------------------------------------
void igtlioImageDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);

  os << indent << "Image:\t" <<"\n";
  Content.image->PrintSelf(os, indent.GetNextIndent());
  os << indent << "Transform:\t" << "\n";
  Content.transform->PrintSelf(os, indent.GetNextIndent());
}
