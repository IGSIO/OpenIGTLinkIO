
// OpenIGTLinkIO includes
#include "igtlioImageMetaDevice.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioImageMetaDeviceCreator);
vtkStandardNewMacro(igtlioImageMetaDevice);

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioImageMetaDeviceCreator::Create(std::string device_name)
{
  igtlioImageMetaDevicePointer retval = igtlioImageMetaDevicePointer::New();
  retval->SetDeviceName(device_name);
  return retval;
}

//---------------------------------------------------------------------------
std::string igtlioImageMetaDeviceCreator::GetDeviceType() const
{
  return igtlioImageMetaConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
igtlioImageMetaDevice::igtlioImageMetaDevice()
{
}

//---------------------------------------------------------------------------
igtlioImageMetaDevice::~igtlioImageMetaDevice()
{
}

//---------------------------------------------------------------------------
std::string igtlioImageMetaDevice::GetDeviceType() const
{
  return igtlioImageMetaConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
unsigned int igtlioImageMetaDevice::GetDeviceContentModifiedEvent() const
{
  return ImageMetaModifiedEvent;
}

//---------------------------------------------------------------------------
int igtlioImageMetaDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  int success = igtlioImageMetaConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo);
  if (success)
  {
    this->Modified();
    this->InvokeEvent(this->GetDeviceContentModifiedEvent(), this);
  }
  return success;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioImageMetaDevice::GetIGTLMessage()
{
  if (!igtlioImageMetaConverter::toIGTL(HeaderData, Content, &this->OutMessage, this->metaInfo))
  {
    return 0;
  }

  return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioImageMetaDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
  if (prefix == MESSAGE_PREFIX_NOT_DEFINED)
  {
    return this->GetIGTLMessage();
  }
  else if (prefix == MESSAGE_PREFIX_GET)
  {
    igtl::GetImageMetaMessage::Pointer getImageMetaMessage = igtl::GetImageMetaMessage::New();
    getImageMetaMessage->SetDeviceName(this->GetDeviceName());
    getImageMetaMessage->Pack();
    return getImageMetaMessage;
  }

  return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioImageMetaDevice::GetSupportedMessagePrefixes() const
{
  std::set<MESSAGE_PREFIX> retval;
  retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
  retval.insert(MESSAGE_PREFIX_GET);
  return retval;
}

//---------------------------------------------------------------------------
void igtlioImageMetaDevice::SetContent(igtlioImageMetaConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(this->GetDeviceContentModifiedEvent(), this);
}

//---------------------------------------------------------------------------
igtlioImageMetaConverter::ContentData igtlioImageMetaDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void igtlioImageMetaDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);

  os << indent << "ImageMetaElements:" << std::endl;
  int i = 0;
  for (igtlioImageMetaConverter::ImageMetaDataList::const_iterator imageMetaDataIt = this->Content.ImageMetaDataElements.begin();
    imageMetaDataIt != Content.ImageMetaDataElements.end(); ++imageMetaDataIt)
  {
    os << indent << indent << "ImageMetaData #" << i << std::endl;
    igtl::ImageMetaElement::Pointer imageMetaElement = igtl::ImageMetaElement::New();
    os << indent << indent << "Name: " << imageMetaDataIt->Name << std::endl;
    os << indent << indent << "DeviceName: " << imageMetaDataIt->DeviceName << std::endl;
    os << indent << indent << "Modality: " << imageMetaDataIt->Modality << std::endl;
    os << indent << indent << "PatientName: " << imageMetaDataIt->PatientName << std::endl;
    os << indent << indent << "PatientID: " << imageMetaDataIt->PatientID << std::endl;
    os << indent << indent << "Timestamp: " << imageMetaDataIt->Timestamp << std::endl;
    os << indent << indent << "Size: " << imageMetaDataIt->Size[0] << ", " << imageMetaDataIt->Size[1] << ", " <<
      imageMetaDataIt->Size[2] << std::endl;
    os << indent << indent << "ScalarType: " << imageMetaDataIt->ScalarType << std::endl;
  }

}
