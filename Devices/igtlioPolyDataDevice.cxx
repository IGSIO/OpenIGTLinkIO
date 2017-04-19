
#include "igtlioPolyDataDevice.h"
#include <vtkObjectFactory.h>

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
std::string PolyDataDevice::GetDeviceType() const
{
  return PolyDataConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
int PolyDataDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (PolyDataConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC))
 {
   this->Modified();
   this->InvokeEvent(ReceiveEvent);
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

 if (!PolyDataConverter::toIGTL(HeaderData, Content, &this->OutMessage))
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

