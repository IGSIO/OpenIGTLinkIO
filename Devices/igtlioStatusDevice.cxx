
#include "igtlioStatusDevice.h"
#include <vtkObjectFactory.h>

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioStatusDeviceCreator::Create(std::string device_name)
{
 igtlioStatusDevicePointer retval = igtlioStatusDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string igtlioStatusDeviceCreator::GetDeviceType() const
{
 return igtlioStatusConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioStatusDeviceCreator);




//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioStatusDevice);
//---------------------------------------------------------------------------
igtlioStatusDevice::igtlioStatusDevice()
{
}

//---------------------------------------------------------------------------
igtlioStatusDevice::~igtlioStatusDevice()
{
}

//---------------------------------------------------------------------------
unsigned int igtlioStatusDevice::GetDeviceContentModifiedEvent() const
{
  return StatusModifiedEvent;
}

//---------------------------------------------------------------------------
std::string igtlioStatusDevice::GetDeviceType() const
{
  return igtlioStatusConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
int igtlioStatusDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  int success = igtlioStatusConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, this->metaInfo);
  if (success)
  {
    this->Modified();
    this->InvokeEvent(StatusModifiedEvent, this);
  }
  return success;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioStatusDevice::GetIGTLMessage()
{
    /*
 // cannot send a non-existent status (?)
 if (Content.errorname.empty())
  {
  return 0;
  }
  */

  if (!igtlioStatusConverter::toIGTL(HeaderData, Content, &this->OutMessage, this->metaInfo))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioStatusDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
    /*
 if (prefix==MESSAGE_PREFIX_GET)
  {
   if (this->GetMessage.IsNull())
     {
     this->GetMessage = igtl::GetStatusMessage::New();
     }
   this->GetMessage->SetDeviceName(HeaderData.deviceName.c_str());
   this->GetMessage->Pack();
   return dynamic_pointer_cast<igtl::MessageBase>(this->GetMessage);
  }
  */
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioStatusDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

void igtlioStatusDevice::SetContent(igtlioStatusConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(StatusModifiedEvent, this);
}

igtlioStatusConverter::ContentData igtlioStatusDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void igtlioStatusDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);

  os << indent << "ErrorCode:\t" << Content.code << "\n";
  os << indent << "ErrorSubCode:\t" << Content.subcode << "\n";
  os << indent << "ErrorName:\t" << Content.errorname << "\n";
  os << indent << "StatusString:\t" << Content.statusstring << "\n";
}
