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

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioPolyDataDeviceCreator::Create(std::string device_name)
{
  igtlioPolyDataDevicePointer retval = igtlioPolyDataDevicePointer::New();
  retval->SetDeviceName(device_name);
  return retval;
}

//---------------------------------------------------------------------------
std::string igtlioPolyDataDeviceCreator::GetDeviceType() const
{
  return igtlioPolyDataConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioPolyDataDeviceCreator);




//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioPolyDataDevice);
//---------------------------------------------------------------------------
igtlioPolyDataDevice::igtlioPolyDataDevice()
{
}

//---------------------------------------------------------------------------
igtlioPolyDataDevice::~igtlioPolyDataDevice()
{
}

//---------------------------------------------------------------------------
unsigned int igtlioPolyDataDevice::GetDeviceContentModifiedEvent() const
{
  return PolyDataModifiedEvent;
}

//---------------------------------------------------------------------------
std::string igtlioPolyDataDevice::GetDeviceType() const
{
  return igtlioPolyDataConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
int igtlioPolyDataDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (igtlioPolyDataConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo))
 {
   this->Modified();
   this->InvokeEvent(ReceiveEvent);
   this->InvokeEvent(PolyDataModifiedEvent, this);
   return 1;
 }

 return 0;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioPolyDataDevice::GetIGTLMessage()
{
    /*
 // cannot send a non-existent status (?)
 if (Content.errorname.empty())
  {
  return 0;
  }
  */

  if (!igtlioPolyDataConverter::toIGTL(HeaderData, Content, &this->OutMessage, this->metaInfo))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioPolyDataDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{

 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioPolyDataDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

void igtlioPolyDataDevice::SetContent(igtlioPolyDataConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(PolyDataModifiedEvent, this);
}

igtlioPolyDataConverter::ContentData igtlioPolyDataDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void igtlioPolyDataDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);

  os << indent << "deviceName:\t" << Content.deviceName << "\n";
}
