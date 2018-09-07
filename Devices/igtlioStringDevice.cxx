#include "igtlioStringDevice.h"
#include <vtkObjectFactory.h>
#include "igtlStringMessage.h"

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioStringDeviceCreator::Create(std::string device_name)
{
 igtlioStringDevicePointer retval = igtlioStringDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string igtlioStringDeviceCreator::GetDeviceType() const
{
 return igtlioStringConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioStringDeviceCreator);




//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioStringDevice);
//---------------------------------------------------------------------------
igtlioStringDevice::igtlioStringDevice()
{
}

//---------------------------------------------------------------------------
igtlioStringDevice::~igtlioStringDevice()
{
}

//---------------------------------------------------------------------------
std::string igtlioStringDevice::GetDeviceType() const
{
  return igtlioStringConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
unsigned int igtlioStringDevice::GetDeviceContentModifiedEvent() const
{
  return StringModifiedEvent;
}

//---------------------------------------------------------------------------
int igtlioStringDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  int success = igtlioStringConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo);
  if (success)
  {
    this->Modified();
    this->InvokeEvent(StringModifiedEvent, this);
  }
  return success;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioStringDevice::GetIGTLMessage()
{

 if (!igtlioStringConverter::toIGTL(HeaderData, Content, &this->OutMessage, this->metaInfo))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioStringDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioStringDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

void igtlioStringDevice::SetContent(igtlioStringConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(StringModifiedEvent, this);
}

igtlioStringConverter::ContentData igtlioStringDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void igtlioStringDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);

  os << indent << "Encoding:\t" << Content.encoding << "\n";
  os << indent << "String:\t" << Content.string_msg << "\n";
}
