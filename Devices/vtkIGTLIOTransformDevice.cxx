/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLIOTransformDevice.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

#include "vtkIGTLIOTransformDevice.h"

// igtl support includes
#include <igtl_util.h>
#include <igtlTransformMessage.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include "vtkMatrix4x4.h"

// VTKSYS includes
#include <vtksys/SystemTools.hxx>
#include "igtlTransformConverter.h"

//---------------------------------------------------------------------------
vtkSmartPointer<vtkIGTLIODevice> vtkIGTLIOTransformDeviceCreator::Create(std::string device_name)
{
 vtkSmartPointer<vtkIGTLIOTransformDevice> retval = vtkSmartPointer<vtkIGTLIOTransformDevice>::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOTransformDeviceCreator::GetDeviceType() const
{
  return igtl::TransformConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOTransformDeviceCreator);


//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOTransformDevice);
//---------------------------------------------------------------------------
vtkIGTLIOTransformDevice::vtkIGTLIOTransformDevice()
{
   Converter = igtl::TransformConverter::New();
}

//---------------------------------------------------------------------------
vtkIGTLIOTransformDevice::~vtkIGTLIOTransformDevice()
{
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOTransformDevice::GetDeviceType() const
{
  return igtl::TransformConverter::GetIGTLTypeName();
}

void vtkIGTLIOTransformDevice::SetContent(igtl::TransformConverter::ContentData content)
{
  Content = content;
  this->Modified();
}

igtl::TransformConverter::ContentData vtkIGTLIOTransformDevice::GetContent()
{
  return Content;
}


//---------------------------------------------------------------------------
int vtkIGTLIOTransformDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (Converter->fromIGTL(buffer, &HeaderData, &Content, checkCRC))
   {
   this->Modified();
   return 1;
   }

 return 0;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkIGTLIOTransformDevice::GetIGTLMessage()
{
  // cannot send a non-existent image
  if (!Content.transform)
  {
  return 0;
  }

 if (!Converter->toIGTL(HeaderData, Content, &this->OutTransformMessage))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutTransformMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkIGTLIOTransformDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
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
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<vtkIGTLIODevice::MESSAGE_PREFIX> vtkIGTLIOTransformDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_GET);
 return retval;
}

//---------------------------------------------------------------------------
void vtkIGTLIOTransformDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkIGTLIODevice::PrintSelf(os, indent);

    // not sure what to do here.
  //os << indent << "CommandID:\t" <<"\n";
  //Content.transform->PrintSelf(os, indent.GetNextIndent());
  //os << indent << "CommandName:\t" << "\n";
  //Content.transform->PrintSelf(os, indent.GetNextIndent());
}

