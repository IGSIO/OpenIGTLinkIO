/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: https://github.com/openigtlink/OpenIGTLink/blob/master/Source/igtlPolyDataMessage.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

#include "igtlioPolyDataDevice.h"

#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include "vtkMatrix4x4.h"

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
vtkIntArray* PolyDataDevice::GetDeviceContentModifiedEvent() const
{
  vtkIntArray* events;
  events = vtkIntArray::New();
  events->InsertNextValue(PolyDataModifiedEvent);
  return events;
}
  
  
//---------------------------------------------------------------------------
std::string PolyDataDevice::GetDeviceType() const
{
  return PolyDataConverter::GetIGTLTypeName();
}

void PolyDataDevice::SetContent(PolyDataConverter::MessageContent content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(PolyDataModifiedEvent, this);
}

PolyDataConverter::MessageContent PolyDataDevice::GetContent()
{
  return Content;
}


//---------------------------------------------------------------------------
int PolyDataDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (PolyDataConverter::IGTLToVTK(buffer, &Content, checkCRC))
   {
   this->Modified();
   this->InvokeEvent(PolyDataModifiedEvent, this);
   return 1;
   }

 return 0;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer PolyDataDevice::GetIGTLMessage()
{
 if (!Content.polydata)
  {
  vtkWarningMacro("PolyData is NULL, message not generated.")
  return 0;
  }

 if (!PolyDataConverter::VTKToIGTL(Content, &this->OutPolyDataMessage))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutPolyDataMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer PolyDataDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
 if (prefix==MESSAGE_PREFIX_GET)
  {
   if (this->GetPolyDataMessage.IsNull())
     {
     this->GetPolyDataMessage = igtl::GetPolyDataMessage::New();
     }
   this->GetPolyDataMessage->SetDeviceName(HeaderData.deviceName.c_str());
   this->GetPolyDataMessage->Pack();
   return dynamic_pointer_cast<igtl::MessageBase>(this->GetPolyDataMessage);
  }
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
 retval.insert(MESSAGE_PREFIX_GET);
 return retval;
}

//---------------------------------------------------------------------------
void PolyDataDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  Device::PrintSelf(os, indent);

  os << indent << "PolyData:\t" <<"\n";
  Content.polydata->PrintSelf(os, indent.GetNextIndent());
}
} // namespace igtlio

