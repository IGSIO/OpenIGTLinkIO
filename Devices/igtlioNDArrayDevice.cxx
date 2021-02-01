#include "igtlioNDArrayDevice.h"
#include <vtkDataArray.h>

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioNDArrayDeviceCreator::Create(std::string device_name)
{
 igtlioNDArrayDevicePointer retval = igtlioNDArrayDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string igtlioNDArrayDeviceCreator::GetDeviceType() const
{
 return igtlioNDArrayConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioNDArrayDeviceCreator);




//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioNDArrayDevice);
//---------------------------------------------------------------------------
igtlioNDArrayDevice::igtlioNDArrayDevice()
{
}

//---------------------------------------------------------------------------
igtlioNDArrayDevice::~igtlioNDArrayDevice()
{
}

//---------------------------------------------------------------------------
std::string igtlioNDArrayDevice::GetDeviceType() const
{
  return igtlioNDArrayConverter::GetIGTLTypeName();
}

int igtlioNDArrayDevice::ReceiveIGTLMessage(igtl::NDArrayMessage::Pointer buffer, bool checkCRC)
{
  int success = igtlioNDArrayConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo);
  if (success)
  {
    this->Modified();
    this->InvokeEvent(ArrayModifiedEvent, this);
  }
  return success;
}
//---------------------------------------------------------------------------
unsigned int igtlioNDArrayDevice::GetDeviceContentModifiedEvent() const
{
  return ArrayModifiedEvent;
}

//---------------------------------------------------------------------------
int igtlioNDArrayDevice::ReceiveIGTLMessage(igtl::NDArrayMessage::Pointer buffer, bool checkCRC)
{
  int success = igtlioNDArrayConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo);
  if (success)
  {
    this->Modified();
    this->InvokeEvent(ArrayModifiedEvent, this);
  }
  return success;
}

//---------------------------------------------------------------------------
igtl::NDArrayMessage::Pointer igtlioNDArrayDevice::GetIGTLMessage()
{

 if (!igtlioNDArrayConverter::toIGTL(HeaderData, Content, &this->OutMessage, this->metaInfo))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::NDArrayMessage>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::NDArrayMessage::Pointer igtlioNDArrayDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::NDArrayMessage::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioNDArrayDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

void igtlioNDArrayDevice::SetContent(igtlioNDArrayConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(ArrayModifiedEvent, this);
}

igtlioNDArrayConverter::ContentData igtlioNDArrayDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void igtlioNDArrayDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);

  os << indent << "Array:\t" << Content.NDArray_msg << "\n";
}
