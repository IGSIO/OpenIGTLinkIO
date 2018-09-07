/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLIOTransformDevice.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

#include "igtlioTransformDevice.h"

#include <vtkObjectFactory.h>
#include "vtkMatrix4x4.h"

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioTransformDeviceCreator::Create(std::string device_name)
{
 igtlioTransformDevicePointer retval = igtlioTransformDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string igtlioTransformDeviceCreator::GetDeviceType() const
{
  return igtlioTransformConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioTransformDeviceCreator);


//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioTransformDevice);
//---------------------------------------------------------------------------
igtlioTransformDevice::igtlioTransformDevice()
{
}

//---------------------------------------------------------------------------
igtlioTransformDevice::~igtlioTransformDevice()
{
}

//---------------------------------------------------------------------------
unsigned int igtlioTransformDevice::GetDeviceContentModifiedEvent() const
{
  return TransformModifiedEvent;
}

//---------------------------------------------------------------------------
std::string igtlioTransformDevice::GetDeviceType() const
{
  return igtlioTransformConverter::GetIGTLTypeName();
}

void igtlioTransformDevice::SetContent(igtlioTransformConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(TransformModifiedEvent, this);
}

igtlioTransformConverter::ContentData igtlioTransformDevice::GetContent()
{
  return Content;
}


//---------------------------------------------------------------------------
int igtlioTransformDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  int success = igtlioTransformConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo);
  if (success)
  {
    this->Modified();
    this->InvokeEvent(TransformModifiedEvent, this);
  }
 return success;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioTransformDevice::GetIGTLMessage()
{
  // cannot send a non-existent image
  if (!Content.transform)
  {
  return 0;
  }

 if (!igtlioTransformConverter::toIGTL(HeaderData, Content, &this->OutTransformMessage, this->metaInfo))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutTransformMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioTransformDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{

    /*
  if (prefix==MESSAGE_PREFIX_GET)
  {
   if (this->GetTransformMessage.IsNull())
     {
     this->GetTransformMessage = igtl::GetTransformMessage::New();
     }
   this->GetTransformMessage->SetDeviceName(HeaderData.deviceName.c_str());
   this->GetTransformMessage->Pack();
   return dynamic_pointer_cast<igtl::MessageBase>(this->GetTransformMessage);
  }
  */
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioTransformDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

//---------------------------------------------------------------------------
void igtlioTransformDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);

    // not sure what to do here.
  //os << indent << "CommandID:\t" <<"\n";
  //Content.transform->PrintSelf(os, indent.GetNextIndent());
  //os << indent << "CommandName:\t" << "\n";
  //Content.transform->PrintSelf(os, indent.GetNextIndent());
}
