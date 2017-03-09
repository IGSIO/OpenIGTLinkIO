#include "igtlioStringDevice.h"
#include <vtkObjectFactory.h>
#include "igtlStringMessage.h"

namespace igtlio
{

//---------------------------------------------------------------------------
DevicePointer StringDeviceCreator::Create(std::string device_name)
{
 StringDevicePointer retval = StringDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string StringDeviceCreator::GetDeviceType() const
{
 return StringConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(StringDeviceCreator);




//---------------------------------------------------------------------------
vtkStandardNewMacro(StringDevice);
//---------------------------------------------------------------------------
StringDevice::StringDevice()
{
}

//---------------------------------------------------------------------------
StringDevice::~StringDevice()
{
}

//---------------------------------------------------------------------------
std::string StringDevice::GetDeviceType() const
{
  return StringConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
int StringDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (StringConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC))
   {
   this->Modified();
   return 1;
   }

 return 0;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer StringDevice::GetIGTLMessage()
{

 if (!StringConverter::toIGTL(HeaderData, Content, &this->OutMessage))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer StringDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<Device::MESSAGE_PREFIX> StringDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

void StringDevice::SetContent(StringConverter::ContentData content)
{
  Content = content;
  this->Modified();
}

StringConverter::ContentData StringDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void StringDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  Device::PrintSelf(os, indent);

  os << indent << "Encoding:\t" << Content.encoding << "\n";
  os << indent << "String:\t" << Content.string_msg << "\n";
}

} //namespace igtlio
