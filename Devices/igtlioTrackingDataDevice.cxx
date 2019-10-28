/*==========================================================================

  Portions (c) Copyright 2019 Robarts Research Institute All Rights Reserved.

==========================================================================*/

// Local includes
#include "igtlioTrackingDataDevice.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioTrackingDataDeviceCreator::Create(std::string device_name)
{
  igtlioTrackingDataDevicePointer retval = igtlioTrackingDataDevicePointer::New();
  retval->SetDeviceName(device_name);
  return retval;
}

//---------------------------------------------------------------------------
std::string igtlioTrackingDataDeviceCreator::GetDeviceType() const
{
  return igtlioTrackingDataConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioTrackingDataDeviceCreator);


//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioTrackingDataDevice);
//---------------------------------------------------------------------------
igtlioTrackingDataDevice::igtlioTrackingDataDevice()
{
}

//---------------------------------------------------------------------------
igtlioTrackingDataDevice::~igtlioTrackingDataDevice()
{
}

//---------------------------------------------------------------------------
unsigned int igtlioTrackingDataDevice::GetDeviceContentModifiedEvent() const
{
  return TDATAModifiedEvent;
}

//---------------------------------------------------------------------------
std::string igtlioTrackingDataDevice::GetDeviceType() const
{
  return igtlioTrackingDataConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
void igtlioTrackingDataDevice::SetContent(igtlioTrackingDataConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(TDATAModifiedEvent, this);
}

//---------------------------------------------------------------------------
igtlioTrackingDataConverter::ContentData igtlioTrackingDataDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
int igtlioTrackingDataDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  int success = igtlioTrackingDataConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo);
  if (success)
  {
    this->Modified();
    this->InvokeEvent(TDATAModifiedEvent, this);
  }
  return success;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioTrackingDataDevice::GetIGTLMessage()
{
  // cannot send a non-existent image
  if (Content.trackingDataElements.empty())
  {
    return 0;
  }

  if (!igtlioTrackingDataConverter::toIGTL(HeaderData, Content, &this->OutTDATAMessage, this->metaInfo))
  {
    return 0;
  }

  return dynamic_pointer_cast<igtl::MessageBase>(this->OutTDATAMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioTrackingDataDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
  if (prefix == MESSAGE_PREFIX_NOT_DEFINED)
  {
    return this->GetIGTLMessage();
  }
  else if (prefix == igtlioDevice::MESSAGE_PREFIX_START)
  {
    if (this->StartTDATAMessage.IsNull())
    {
      this->StartTDATAMessage = igtl::StartTrackingDataMessage::New();
    }
    this->StartTDATAMessage->SetDeviceName(HeaderData.deviceName.c_str());
    this->StartTDATAMessage->Pack();
    return dynamic_pointer_cast<igtl::MessageBase>(this->StartTDATAMessage);
  }
  else if (prefix == igtlioDevice::MESSAGE_PREFIX_STOP)
  {
    if (this->StopTDATAMessage.IsNull())
    {
      this->StopTDATAMessage = igtl::StopTrackingDataMessage::New();
    }
    this->StopTDATAMessage->SetDeviceName(HeaderData.deviceName.c_str());
    this->StopTDATAMessage->Pack();
    return dynamic_pointer_cast<igtl::MessageBase>(this->StopTDATAMessage);
  }

  return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioTrackingDataDevice::GetSupportedMessagePrefixes() const
{
  std::set<MESSAGE_PREFIX> retval;
  retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
  retval.insert(MESSAGE_PREFIX_START);
  retval.insert(MESSAGE_PREFIX_STOP);
  return retval;
}

//---------------------------------------------------------------------------
void igtlioTrackingDataDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);
}
