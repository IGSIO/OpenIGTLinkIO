
// OpenIGTLInkIO includes
#include "igtlioLabelMetaDevice.h"

// OpenIGTLink includes
#include "igtlLabelMetaMessage.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioLabelMetaDeviceCreator);
vtkStandardNewMacro(igtlioLabelMetaDevice);

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioLabelMetaDeviceCreator::Create(std::string device_name)
{
  igtlioLabelMetaDevicePointer retval = igtlioLabelMetaDevicePointer::New();
  retval->SetDeviceName(device_name);
  return retval;
}

//---------------------------------------------------------------------------
std::string igtlioLabelMetaDeviceCreator::GetDeviceType() const
{
  return igtlioLabelMetaConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
igtlioLabelMetaDevice::igtlioLabelMetaDevice()
{
}

//---------------------------------------------------------------------------
igtlioLabelMetaDevice::~igtlioLabelMetaDevice()
{
}

//---------------------------------------------------------------------------
std::string igtlioLabelMetaDevice::GetDeviceType() const
{
  return igtlioLabelMetaConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
unsigned int igtlioLabelMetaDevice::GetDeviceContentModifiedEvent() const
{
  return LabelMetaModifiedEvent;
}

//---------------------------------------------------------------------------
int igtlioLabelMetaDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  int success = igtlioLabelMetaConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo);
  if (success)
  {
    this->Modified();
    this->InvokeEvent(this->GetDeviceContentModifiedEvent(), this);
  }
  return success;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioLabelMetaDevice::GetIGTLMessage()
{
  if (!igtlioLabelMetaConverter::toIGTL(HeaderData, Content, &this->OutMessage, this->metaInfo))
  {
    return 0;
  }

  return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioLabelMetaDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
  if (prefix == MESSAGE_PREFIX_NOT_DEFINED)
  {
    return this->GetIGTLMessage();
  }
  else if (prefix == MESSAGE_PREFIX_GET)
  {
    igtl::GetLabelMetaMessage::Pointer getLabelMetaMessage = igtl::GetLabelMetaMessage::New();
    getLabelMetaMessage->SetDeviceName(this->GetDeviceName());
    getLabelMetaMessage->Pack();
    return dynamic_pointer_cast<igtl::MessageBase>(getLabelMetaMessage);
  }

  return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioLabelMetaDevice::GetSupportedMessagePrefixes() const
{
  std::set<MESSAGE_PREFIX> retval;
  retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
  retval.insert(MESSAGE_PREFIX_GET);
  return retval;
}

//---------------------------------------------------------------------------
void igtlioLabelMetaDevice::SetContent(igtlioLabelMetaConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(this->GetDeviceContentModifiedEvent(), this);
}

//---------------------------------------------------------------------------
igtlioLabelMetaConverter::ContentData igtlioLabelMetaDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void igtlioLabelMetaDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);

  os << indent << "LabelMetaElements:" << std::endl;
  int i = 0;
  for (igtlioLabelMetaConverter::LabelMetaDataList::const_iterator labelMetaDataIt = this->Content.LabelMetaDataElements.begin();
    labelMetaDataIt != Content.LabelMetaDataElements.end(); ++labelMetaDataIt)
  {
    os << indent << indent << "LabelMetaData #" << i << std::endl;
    igtl::LabelMetaElement::Pointer labelMetaElement = igtl::LabelMetaElement::New();
    os << indent << indent << "Name: " << labelMetaDataIt->Name << std::endl;
    os << indent << indent << "DeviceName: " << labelMetaDataIt->DeviceName << std::endl;
    os << indent << indent << "Label: " << labelMetaDataIt->Label << std::endl;
    os << indent << indent << "RGBA: " << labelMetaDataIt->RGBA[0] << ", " << labelMetaDataIt->RGBA[1] << ", "
      << labelMetaDataIt->RGBA[2] << ", " << labelMetaDataIt->RGBA[3] << std::endl;
    os << indent << indent << "Size: " << labelMetaDataIt->Size[0] << ", " << labelMetaDataIt->Size[1] << ", "
      << labelMetaDataIt->Size[2] << std::endl;
    os << indent << indent << "Owner: " << labelMetaDataIt->Owner << std::endl;
  }

}
