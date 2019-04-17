
// OpenIGTLInkIO includes
#include "igtlioPointDevice.h"

// OpenIGTLink includes
#include "igtlPointMessage.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioPointDeviceCreator);
vtkStandardNewMacro(igtlioPointDevice);

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioPointDeviceCreator::Create(std::string device_name)
{
  igtlioPointDevicePointer retval = igtlioPointDevicePointer::New();
  retval->SetDeviceName(device_name);
  return retval;
}

//---------------------------------------------------------------------------
std::string igtlioPointDeviceCreator::GetDeviceType() const
{
  return igtlioPointConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
igtlioPointDevice::igtlioPointDevice()
{
}

//---------------------------------------------------------------------------
igtlioPointDevice::~igtlioPointDevice()
{
}

//---------------------------------------------------------------------------
std::string igtlioPointDevice::GetDeviceType() const
{
  return igtlioPointConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
unsigned int igtlioPointDevice::GetDeviceContentModifiedEvent() const
{
  return PointModifiedEvent;
}

//---------------------------------------------------------------------------
int igtlioPointDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  int success = igtlioPointConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo);
  if (success)
  {
    this->Modified();
    this->InvokeEvent(this->GetDeviceContentModifiedEvent(), this);
  }
  return success;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioPointDevice::GetIGTLMessage()
{
  if (!igtlioPointConverter::toIGTL(HeaderData, Content, &this->OutMessage, this->metaInfo))
  {
    return 0;
  }

  return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioPointDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
  if (prefix == MESSAGE_PREFIX_NOT_DEFINED)
  {
    return this->GetIGTLMessage();
  }
  else if (prefix == MESSAGE_PREFIX_GET)
  {
    igtl::GetPointMessage::Pointer getPointMessage = igtl::GetPointMessage::New();
    getPointMessage->SetDeviceName(this->GetDeviceName());
    getPointMessage->Pack();
    return dynamic_pointer_cast<igtl::MessageBase>(getPointMessage);
  }

  return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioPointDevice::GetSupportedMessagePrefixes() const
{
  std::set<MESSAGE_PREFIX> retval;
  retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
  retval.insert(MESSAGE_PREFIX_GET);
  return retval;
}

//---------------------------------------------------------------------------
void igtlioPointDevice::SetContent(igtlioPointConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(this->GetDeviceContentModifiedEvent(), this);
}

//---------------------------------------------------------------------------
igtlioPointConverter::ContentData igtlioPointDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void igtlioPointDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);
  os << indent << "Points:" << std::endl;
  int i = 0;
  for (igtlioPointConverter::PointList::const_iterator pointIt = this->Content.PointElements.begin();
    pointIt != Content.PointElements.end(); ++pointIt)
  {
    os << indent << indent << "Point #" << i << std::endl;
    igtl::PointElement::Pointer pointElement = igtl::PointElement::New();
    os << indent << indent << "Name: " << pointIt->Name << std::endl;
    os << indent << indent << "GroupName: " << pointIt->GroupName << std::endl;
    os << indent << indent << "Position: " << pointIt->Position[0] << ", " << pointIt->Position[1] << ", "
      << pointIt->Position[2] << std::endl;
    os << indent << indent << "Radius: " << pointIt->Radius << std::endl;
    os << indent << indent << "Owner: " << pointIt->Owner << std::endl;
    ++i;
  }
}
