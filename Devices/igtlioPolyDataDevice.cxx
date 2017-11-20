/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: https://github.com/openigtlink/OpenIGTLinkIF/blob/master/MRML/vtkIGTLToMRMLPolyData.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

#include "igtlioPolyDataDevice.h"
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>

namespace igtlio
{

//---------------------------------------------------------------------------
DevicePointer PolyDataDeviceCreator::Create(std::string device_name)
{
 PolyDataDevicePointer retval = PolyDataDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string PolyDataDeviceCreator::GetDeviceType() const
{
 return PolyDataConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(PolyDataDeviceCreator);




//---------------------------------------------------------------------------
vtkStandardNewMacro(PolyDataDevice);
//---------------------------------------------------------------------------
PolyDataDevice::PolyDataDevice()
{
}

//---------------------------------------------------------------------------
PolyDataDevice::~PolyDataDevice()
{
}

//---------------------------------------------------------------------------
unsigned int PolyDataDevice::GetDeviceContentModifiedEvent() const
{
  return PolyDataModifiedEvent;
}

//---------------------------------------------------------------------------
std::string PolyDataDevice::GetDeviceType() const
{
  return PolyDataConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
int PolyDataDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (PolyDataConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, &this->metaInfo))
 {
   this->Modified();
   this->InvokeEvent(ReceiveEvent);
   this->InvokeEvent(PolyDataModifiedEvent, this);
   return 1;
 }

 return 0;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer PolyDataDevice::GetIGTLMessage()
{
    /*
 // cannot send a non-existent status (?)
 if (Content.errorname.empty())
  {
  return 0;
  }
  */

 if (!PolyDataConverter::toIGTL(HeaderData, Content, &this->OutMessage, &this->metaInfo))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer PolyDataDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{

 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<Device::MESSAGE_PREFIX> PolyDataDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

void PolyDataDevice::SetContent(PolyDataConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(PolyDataModifiedEvent, this);
}

PolyDataConverter::ContentData PolyDataDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void PolyDataDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  Device::PrintSelf(os, indent);

  os << indent << "deviceName:\t" << Content.deviceName << "\n";
}

} //namespace igtlio

