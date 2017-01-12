
// OpenIGTLinkIF MRML includes
#include "vtkIGTLIOStatusDevice.h"

// igtl support includes
#include <igtl_util.h>
#include <igtlImageMessage.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>
#include "igtlImageConverter.h"

namespace igtlio
{

//---------------------------------------------------------------------------
vtkSmartPointer<vtkIGTLIODevice> vtkIGTLIOStatusDeviceCreator::Create(std::string device_name)
{
 vtkSmartPointer<vtkIGTLIOStatusDevice> retval = vtkSmartPointer<vtkIGTLIOStatusDevice>::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOStatusDeviceCreator::GetDeviceType() const
{
 return igtlio::StatusConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOStatusDeviceCreator);




//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOStatusDevice);
//---------------------------------------------------------------------------
vtkIGTLIOStatusDevice::vtkIGTLIOStatusDevice()
{
}

//---------------------------------------------------------------------------
vtkIGTLIOStatusDevice::~vtkIGTLIOStatusDevice()
{
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOStatusDevice::GetDeviceType() const
{
  return igtlio::StatusConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
int vtkIGTLIOStatusDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
 if (igtlio::StatusConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC))
   {
   this->Modified();
   return 1;
   }

 return 0;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkIGTLIOStatusDevice::GetIGTLMessage()
{
 // cannot send a non-existent status (?)
 if (Content.errorname.empty())
  {
  return 0;
  }

 if (!igtlio::StatusConverter::toIGTL(HeaderData, Content, &this->OutMessage))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkIGTLIOStatusDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
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
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<vtkIGTLIODevice::MESSAGE_PREFIX> vtkIGTLIOStatusDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_GET);
 return retval;
}

void vtkIGTLIOStatusDevice::SetContent(igtlio::StatusConverter::ContentData content)
{
  Content = content;
  this->Modified();
}

igtlio::StatusConverter::ContentData vtkIGTLIOStatusDevice::GetContent()
{
  return Content;
}

//---------------------------------------------------------------------------
void vtkIGTLIOStatusDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkIGTLIODevice::PrintSelf(os, indent);

  os << indent << "ErrorCode:\t" << Content.code << "\n";
  os << indent << "ErrorSubCode:\t" << Content.subcode << "\n";
  os << indent << "ErrorName:\t" << Content.errorname << "\n";
  os << indent << "StatusString:\t" << Content.statusstring << "\n";
}

} //namespace igtlio
